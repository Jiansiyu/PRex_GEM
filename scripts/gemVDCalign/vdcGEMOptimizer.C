/*
 *
 *
 *
 *
 */

#include "vector"
#include "string"

#include "TH1F.h"
#include "TF1.h"
#include "TLatex.h"
#include "TString.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TChain.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TCanvas.h"

//_________________________________
bool  enableAngleCorr = false;      // enable angle correction in the optimizer
bool  enableTransformCorr = true;  // enable transform correction in the optimizer
//_________________________________

struct hrsTrack{
    hrsTrack(){};
    hrsTrack(std::map<Int_t, TVector3> gemPos,TVector3 vdcPos,TVector3 vdcSlop){
        for (auto iter = gemPos.begin(); iter != gemPos.end(); iter++){
            this->gemPos[iter->first] = iter->second;
        }
        this->vdcPos = vdcPos;
        this->vdcSlop = vdcSlop;
    };
    ~hrsTrack(){};
public:
    Bool_t Clear(){
        vdcPos.Clear();
        vdcSlop.Clear();
        gemPos.clear();
        return true;
    };

    ///
    /// \param gemID : the GEM detector
    /// \param corrPar, correction parameter for x, y, z, cosX, cosY, cosZ
    /// \return
    double_t  getCorrectedResidual(Int_t gemID,double_t *corrPar){
        if (gemPos.find(gemID) == gemPos.end()){
            std::vector<Int_t> chamberList;
            for (auto iter = gemPos.begin(); iter !=gemPos.end(); iter++) chamberList.push_back(iter->first);
            std::cout<<"[ERROR]:: Can not find GEM Chamber  "<<gemID<<std::endl;
            for(auto gemID : chamberList){
                std::cout<<"\t "<< gemID<<std::endl;
            }
            std::cout<<std::endl;
        }

        double x = gemPos[gemID].X();
        double y = gemPos[gemID].Y();
        double z = gemPos[gemID].Z();

        // apply correction on the X, Y . Z transform
        if (enableTransformCorr){
            x += corrPar[0];
            y += corrPar[1];
            z += corrPar[2];
        }
        if (enableAngleCorr){
            //take the following rulls
            // https://mathworld.wolfram.com/EulerAngles.html

            double cosX = corrPar[3];
            double cosY = corrPar[4];
            double cosZ = corrPar[5];
            double sinX = TMath::Sqrt(1-cosX*cosX);
            double sinY = TMath::Sqrt(1-cosY*cosY);
            double sinZ = TMath::Sqrt(1-cosZ*cosZ);

            // X -> theta
            // y -> phi
            // z -> psi
            double a11 = cosZ * cosY - cosX*sinX*sinZ;
            double a12 = cosZ * sinY + cosX*cosY*sinZ;
            double a13 = sinZ*sinX;

            double a21 = - sinZ*cosY - cosX*sinY*cosZ;
            double a22 = - sinZ*sinY + cosX*cosY*cosZ;
            double a23 = cosZ*sinX;

            double a31 = sinX*sinY;
            double a32 = -sinX*cosY;
            double a33 = cosX;

            x = a11 * x + a12 * y + a13 * z;
            y = a21 * x + a22 * y + a23 * z;
            z = a31 * x + a32 * y + a33 * z;
        }

        // calculate the residual
        double vdcProjectX = vdcPos.X() + z * vdcSlop.X();
        double vdcProjectY = vdcPos.Y() + z * vdcSlop.Y();

        // return residual values
        //TODO
        // whether need to optimized one by one
        return TMath::Sqrt((x-vdcProjectX)*(x-vdcProjectX) + (y-vdcProjectY)*(y-vdcProjectY));
    };

    void Print(){
        std::cout<<"-------------"<<std::endl;
        std::cout<<"GEM detector"<<std::endl;
        for (auto iter = gemPos.begin(); iter != gemPos.end(); iter++){
            std::cout<<"\t GEM "<<iter->first<<"  ("<<iter->second.X()<<", "<<iter->second.Y()<<", "<<iter->second.Z()<<")"<<std::endl;
        }
        std::cout<<"----"<<std::endl;
        std::cout<<"VDC Pos:"<<std::endl;
        std::cout<<"\t Pos: ("<< vdcPos.X()<<","<<vdcPos.Y()<<")  slop: ("<< vdcSlop.X()<<", "<<vdcSlop.Y()<<")"<<std::endl;
        std::cout<<"----"<<std::endl;
        std::cout<<"vdc Projected Pos "<<std::endl;
        for (auto iter = gemPos.begin(); iter != gemPos.end(); iter++){
            std::cout<<"\t GEM "<<iter->first<<"  ("<<vdcPos.X()+iter->second.Z()*vdcSlop.X()<<", "<<vdcPos.Y()+iter->second.Z()*vdcSlop.Y()<<")"<<std::endl;
        }
        std::cout<<"--------------"<<std::endl;

    }
private:
    TVector3 vdcPos;
    TVector3 vdcSlop;
    std::map<Int_t, TVector3> gemPos;


};

std::vector<hrsTrack> rawEvent;
Int_t optGEMID = 1;

void LoadRaw(TString fname,UInt_t maxEvent = 50000){
    TChain *chain = new TChain("T");
    chain->Add(fname.Data());
    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }
    std::vector<Int_t> chamberIDList;
    for (int i = 0; i <= 6; ++i) {
            if (chain->GetListOfBranches()->Contains(Form("%s.x%d.coord.pos",GEMProfix.Data(),i))) chamberIDList.push_back(i);
    }

    //________________________________________________
    double_t zpos[]={0.00,   0.9,   1.547,   1.77848,   2.311,   2.438,   2.56586};
    //________________________________________________

    //project the data to the buff
    Int_t           Ndata_GEM_X_coord_pos[7];
    Double_t        GEM_X_coord_pos[7][2];   //[Ndata.RGEM.rgems.x1.coord.pos]
    Int_t           Ndata_GEM_Y_coord_pos[7];
    Double_t        GEM_Y_coord_pos[7][2];   //[Ndata.RGEM.rgems.x1.coord.pos]

    Int_t           Ndata_GEM_X_coord_trkpos[7];
    Double_t        GEM_X_coord_trkpos[7][2];
    Int_t           Ndata_GEM_X_coord_trkslope[7];
    Double_t        GEM_X_coord_trkslope[7][2];   //[Ndata.RGEM.rgems.x1.coord.trkslope]

    Int_t           Ndata_GEM_Y_coord_trkpos[7];
    Double_t        GEM_Y_coord_trkpos[7][2];
    Int_t           Ndata_GEM_Y_coord_trkslope[7];
    Double_t        GEM_Y_coord_trkslope[7][2];   //[Ndata.RGEM.rgems.x1.coord.trkslope]
    for (auto chamberID: chamberIDList){
        TString  Ndata_GEM_X_coord_pos_str(Form("Ndata.%s.x%d.coord.pos",GEMProfix.Data(),chamberID));
        TString  GEM_X_coord_pos_str(Form("%s.x%d.coord.pos",GEMProfix.Data(),chamberID));   //[Ndata.RGEM.rgems.x1.coord.pos]
        TString  Ndata_GEM_Y_coord_pos_str(Form("Ndata.%s.y%d.coord.pos",GEMProfix.Data(),chamberID));
        TString  GEM_Y_coord_pos_str(Form("%s.y%d.coord.pos",GEMProfix.Data(),chamberID));   //[Ndata.RGEM.rgems.x1.coord.pos]

        chain ->SetBranchAddress(Ndata_GEM_X_coord_pos_str.Data(),&Ndata_GEM_X_coord_pos[chamberID]);
        chain ->SetBranchAddress(GEM_X_coord_pos_str.Data(),GEM_X_coord_pos[chamberID]);
        chain ->SetBranchAddress(Ndata_GEM_Y_coord_pos_str.Data(), &Ndata_GEM_Y_coord_pos[chamberID]);
        chain ->SetBranchAddress(GEM_Y_coord_pos_str,GEM_Y_coord_pos[chamberID]);

        TString Ndata_GEM_X_coord_trkpos_str(Form("Ndata.%s.x%d.coord.trkpos",GEMProfix.Data(),chamberID));
        TString GEM_X_coord_trkpos_str(Form("%s.x%d.coord.trkpos",GEMProfix.Data(),chamberID));
        TString Ndata_GEM_X_coord_trkslope_str(Form("Ndata.%s.x%d.coord.trkslope",GEMProfix.Data(),chamberID));
        TString GEM_X_coord_trkslope_str(Form("%s.x%d.coord.trkslope",GEMProfix.Data(),chamberID));

        chain->SetBranchAddress(Ndata_GEM_X_coord_trkpos_str.Data(),&Ndata_GEM_X_coord_trkpos[chamberID]);
        chain->SetBranchAddress(GEM_X_coord_trkpos_str.Data(),GEM_X_coord_trkpos[chamberID]);
        chain->SetBranchAddress(Ndata_GEM_X_coord_trkslope_str.Data(),&Ndata_GEM_X_coord_trkslope[chamberID]);
        chain->SetBranchAddress(GEM_X_coord_trkslope_str.Data(),GEM_X_coord_trkslope[chamberID]);

        TString Ndata_GEM_Y_coord_trkpos_str(Form("Ndata.%s.y%d.coord.trkpos",GEMProfix.Data(),chamberID));
        TString GEM_Y_coord_trkpos_str(Form("%s.y%d.coord.trkpos",GEMProfix.Data(),chamberID));
        TString Ndata_GEM_Y_coord_trkslope_str(Form("Ndata.%s.y%d.coord.trkslope",GEMProfix.Data(),chamberID));
        TString GEM_Y_coord_trkslope_str(Form("%s.y%d.coord.trkslope",GEMProfix.Data(),chamberID));

        chain->SetBranchAddress(Ndata_GEM_Y_coord_trkpos_str.Data(),&Ndata_GEM_Y_coord_trkpos[chamberID]);
        chain->SetBranchAddress(GEM_Y_coord_trkpos_str.Data(),GEM_Y_coord_trkpos[chamberID]);
        chain->SetBranchAddress(Ndata_GEM_Y_coord_trkslope_str.Data(),&Ndata_GEM_Y_coord_trkslope[chamberID]);
        chain->SetBranchAddress(GEM_Y_coord_trkslope_str.Data(),GEM_Y_coord_trkslope[chamberID]);
    }

    // get the vdc parameters
    std::string NDatavdcTrX_str(Form("Ndata.%s.tr.x",HRS.Data()));
    std::string NDatavdcTrY_str(Form("Ndata.%s.tr.y",HRS.Data()));
    std::string NDatavdcTrTH_str(Form("Ndata.%s.tr.th",HRS.Data()));
    std::string NDatavdcTrPH_str(Form("Ndata.%s.tr.ph",HRS.Data()));

    std::string vdcTrX_str(Form("%s.tr.x",HRS.Data()));
    std::string vdcTrY_str(Form("%s.tr.y",HRS.Data()));
    std::string vdcTrTH_str(Form("%s.tr.th",HRS.Data()));
    std::string vdcTrPH_str(Form("%s.tr.ph",HRS.Data()));

    Int_t NDatavdcTrX;
    Int_t NDatavdcTrY;
    Int_t NDatavdcTrTH;
    Int_t NDatavdcTrPH;

    double_t vdcTrX[2000];
    double_t vdcTrY[2000];
    double_t vdcTrPH[2000];
    double_t vdcTrTH[2000];

    if(chain->GetListOfBranches()->Contains(NDatavdcTrX_str.c_str())){
        chain->SetBranchAddress(NDatavdcTrX_str.c_str(),&NDatavdcTrX);
    }else{
        std::cout<<"[WORNING]:: "<< NDatavdcTrX_str.c_str()<<" cannot find!!"<<std::endl;
    }

    if(chain->GetListOfBranches()->Contains(NDatavdcTrY_str.c_str())){
        chain->SetBranchAddress(NDatavdcTrY_str.c_str(),&NDatavdcTrY);
    }else{
        std::cout<<"[WORNING]:: "<< NDatavdcTrY_str.c_str()<<" cannot find!!"<<std::endl;
    }

    if(chain->GetListOfBranches()->Contains(NDatavdcTrTH_str.c_str())){
        chain->SetBranchAddress(NDatavdcTrTH_str.c_str(),&NDatavdcTrTH);
    }else{
        std::cout<<"[WORNING]:: "<< NDatavdcTrTH_str.c_str()<<" cannot find!!"<<std::endl;
    }

    if(chain->GetListOfBranches()->Contains(NDatavdcTrPH_str.c_str())){
        chain->SetBranchAddress(NDatavdcTrPH_str.c_str(),&NDatavdcTrPH);
    }else{
        std::cout<<"[WORNING]:: "<< NDatavdcTrPH_str.c_str()<<" cannot find!!"<<std::endl;
    }

    // load the x y th ph value
    if (chain->GetListOfBranches()->Contains(vdcTrX_str.c_str())) {
        chain->SetBranchAddress(vdcTrX_str.c_str(), vdcTrX);
    } else {
        std::cout << "[WORNING]:: " << vdcTrX_str.c_str() << " cannot find!!"<< std::endl;
    }
    // load the x y th ph value
    if (chain->GetListOfBranches()->Contains(vdcTrY_str.c_str())) {
        chain->SetBranchAddress(vdcTrY_str.c_str(), vdcTrY);
    } else {
        std::cout << "[WORNING]:: " << vdcTrY_str.c_str() << " cannot find!!"<< std::endl;
    }

    if (chain->GetListOfBranches()->Contains(vdcTrPH_str.c_str())) {
        chain->SetBranchAddress(vdcTrPH_str.c_str(), vdcTrPH);
    } else {
        std::cout << "[WORNING]:: " << vdcTrPH_str.c_str() << " cannot find!!"<< std::endl;
    }
    // load the x y th ph value
    if (chain->GetListOfBranches()->Contains(vdcTrTH_str.c_str())) {
        chain->SetBranchAddress(vdcTrTH_str.c_str(), vdcTrTH);
    } else {
        std::cout << "[WORNING]:: " << vdcTrTH_str.c_str() << " cannot find!!"<< std::endl;
    }

    Long64_t entries = chain ->GetEntries();
//    Long64_t allentries = entries;
//    if (entries > maxEvent) entries = maxEvent;

    for (Long64_t entry = 0 ; entry < entries && entry < maxEvent; entry++){
        chain->GetEntry(entry);
        // progess bar
        if (entry%100==0 ) {
            MemInfo_t memInfo;
            CpuInfo_t cpuInfo;
            gSystem->GetMemInfo(&memInfo);
            std::cout<<"\x1B[s"<<Form("\tProgress %2.1f %% (%6zu/%6u %2.1f %%)",(double)entry*100/entries,rawEvent.size(),maxEvent,(double ) rawEvent.size()*100.0/maxEvent)<<" /"<<Form("RAM:%6d MB",memInfo.fMemUsed/1000)<<"\x1B[u" << std::flush;
        }
        //-----------------------------------------------------------------------
        //require all the GEM have trk pos which serve as a cut on the good event
        // Cut::
        //        vdc : gem have track
        //        GEMs: all the GEM have tracks
        //-----------------------------------------------------------------------

        bool goodEvtCut = true;
        // vdc cut
        if (NDatavdcTrX > 0 && NDatavdcTrY > 0 && NDatavdcTrTH > 0 && NDatavdcTrPH > 0){
            goodEvtCut = goodEvtCut && true;
        } else{
            goodEvtCut = goodEvtCut && false;
        }

        //GEM cut event
        for (auto chamberID: chamberIDList){
            if (Ndata_GEM_Y_coord_trkpos[chamberID] > 0 && Ndata_GEM_Y_coord_trkpos[chamberID] > 0 ){
                goodEvtCut = goodEvtCut && true;
            } else{
                goodEvtCut = goodEvtCut && false;
            }
        }

        if(!goodEvtCut) continue;

        std::map<Int_t, TVector3> gemPos;
        // load GEM data
        for (auto chamberID: chamberIDList){
            TVector3 gem(GEM_X_coord_trkpos[chamberID][0],GEM_Y_coord_trkpos[chamberID][0], zpos[chamberID]);
            gemPos[chamberID] = gem;
        }
        // load VDC data
        TVector3 vdcPos(vdcTrX[0],vdcTrY[0],zpos[0]);
        TVector3 vdcSlop(vdcTrTH[0],vdcTrPH[0],zpos[0]);

        // fill the data to the buffer
        hrsTrack trackVal(gemPos,vdcPos,vdcSlop);
        rawEvent.push_back(trackVal);
//        std::cout<<entry<<std::endl;
//        trackVal.Print();
//        getchar();
    }
}

void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
    Double_t chisq = 0.0;
    for (auto event : rawEvent){
        chisq += event.getCorrectedResidual(optGEMID,par);
    }
    f =chisq;
}


void OptimizeGEM(TString fname="/home/newdriver/PRex_GEM/PRex_replayed/prexRHRS_21282_-1*"){
    //TODO need to get the number of GEMs before do the Optimization
     LoadRaw(fname.Data());
    std::cout<<"----> Finish Loading file <----"<<std::endl;

    for (auto i = 1; i <= 1; i ++)
    {
        std::cout<<Form("********  Work On GEM %d ********",i)<<std::endl;
        Int_t optnPar = 0 ;
        if (enableAngleCorr) optnPar += 3;
        if (enableTransformCorr) optnPar += 3;
        TMinuit *gMinuit = new TMinuit(optnPar);
        gMinuit->SetFCN(fcn);

        Double_t arglist[10];//declare flags
        Int_t ierflg = 0;
        arglist[0] = 2;
//        gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);// just for information
//        gMinuit->mnexcm("SET STR", arglist ,1,ierflg);// just for information

        static Double_t vstart[3]={-0.0,    -0.0,   -0.0};
        static Double_t step[3]=  {0.05,   0.05,  0.2};
//mnparm( parNo, sname, initVal, initErr, lowerLimit, upperLimit, err);
        gMinuit->mnparm(0, "shiftX",    vstart[0], step[0], -0.1,0.1,ierflg);
        gMinuit->mnparm(1, "shiftY",    vstart[1], step[1], -0.1,0.1,ierflg);
        gMinuit->mnparm(2, "shiftZ",    vstart[2], step[2], -0.0,0.0,ierflg);
//        gMinuit->mnparm(3, "cos#Theta", vstart[3], step[3], 0,0,ierflg);
//        gMinuit->mnparm(4, "cos#Phi",   vstart[4], step[4], 0,0,ierflg);
//        gMinuit->mnparm(5, "cos#Psi",   vstart[5], step[5], 0,0,ierflg);

        // now ready for minimization step
        arglist[0]=50000; // 5000 step
        arglist[1]=0.0000001;    // stop when it reach a condition. If you want to do more change 1 to 0.1
        gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);//call MIGRAD to do optimization

        double OptimizedPar[optnPar];
        double OptimizedParErr[optnPar];
        for (auto i = 0 ; i < optnPar ; i++){
            gMinuit->GetParameter(i,OptimizedPar[i],OptimizedParErr[i]);
            std::cout<<i<<"  "<<OptimizedPar[i]<<std::endl;
        }


        // generate the diagnose plot
        TH1F *residualBeforeOpt = new TH1F(Form("Before Opt"),Form("Before Opt (%d)",optnPar),1000,0,0.05);
        TH1F *residualAfterOpt = new TH1F(Form("After Opt"),Form("After Opt (%d)",optnPar),1000,0,0.05);
        for (auto event : rawEvent){
            static  Double_t dommyOpt[6]={0.0,0.0,0.0,0.0,0.0,0.0};
            residualBeforeOpt->Fill(event.getCorrectedResidual(1,dommyOpt));
//            std::cout<<event.getCorrectedResidual(1,dommyOpt)<<std::endl;
            residualAfterOpt->Fill(event.getCorrectedResidual(1,OptimizedPar));
        }

        TCanvas *optCanv = new TCanvas(Form("Opt Result"),Form("Opt Result"),1960,1080);
        optCanv->Draw();
        optCanv->cd();
        residualBeforeOpt->SetLineColor(kBlue);
        residualBeforeOpt->Fit("gaus");
        residualBeforeOpt->Draw();
        residualAfterOpt->SetLineColor(kGreen);
        residualAfterOpt->Draw("same");
        optCanv->Update();
    }
}

void tester_LoadRaw(TString fname=""){
    LoadRaw("/home/newdriver/PRex_GEM/PRex_replayed/prexRHRS_21282_-1*");

}

void tester_GetGEMList(TString fname=""){


}


void tester_Optimizer(){
    OptimizeGEM();
}
#ifndef __CINT__
int main(int argc,char **argv){
    tester_Optimizer();
}
#endif
/*
 * GEM detector shift estimator based on the VDC detector
 *
 * Project the VDC to each GEM detector and canculate the shift of the GEM and the VDC projected position, take the difference to be the difference
 * Although the VDC can not give an accurate direction, but this shoild be enough used as pre-alignment for the SBSGEM_standalone
 */

#include "TCanvas.h"
#include "TH1F.h"
#include "TSystem.h"
#include "TString.h"
#include "map"
#include "iostream"
#include "istream"
#include "fstream"
#include "algorithm"
#include "TVector3.h"
#include "sstream"
#include "string"
#include "TChain.h"
#include "vector"
#include "TH1F.h"
#include "TMinuit.h"
#include "TVirtualFitter.h"
#include "TF1.h"
#include "TLine.h"
#include "TLatex.h"

std::map<std::string,TString> dbbuff;
std::map<int,TVector3> shiftbuffX;
std::map<int,TVector3> shiftbuffY;

struct PRexTrack{
public:
    PRexTrack(){};
    ~PRexTrack(){};
private:
    // transport coordination system
    double tr_th=0;
    double tr_ph=0;
    double tr_y=0;
    double tr_x=0;
    std::map<Int_t,TVector3> GEMPos;
};

void ReadDatabase(TString HRS = "R",TString DB_DIR="./"){

    std::cout<<"DB path "<< DB_DIR.Data()<<std::endl;

    dbbuff.clear();
    shiftbuffX.clear();
    shiftbuffY.clear();

    TString dbfname;
    if (HRS == "R"){
       dbfname =  Form("%s/db_RGEM.rgems.dat",DB_DIR.Data());
    } else{
        dbfname =  Form("%s/db_LGEM.lgems.dat",DB_DIR.Data());
    }

    if (gSystem->AccessPathName(dbfname.Data())){
        std::cout<<"[ERROR]:: "<<"CAN NOT FIND DB  \""<<dbfname.Data()<<"\""<<std::endl;
        exit(-1);
    }
    std::ifstream dbfileio(dbfname.Data(),std::ifstream::in);
    std::string line;
    while (getline(dbfileio,line)){
        if (line[0] == '#' || line.empty()) continue;
        auto delimiterPos = line.find("=");
        auto name = line.substr(0, delimiterPos);
        TString keyVal = name;
        if (keyVal.Contains("position")){
            auto value = line.substr(delimiterPos + 1);
            name.erase(std::remove_if(name.begin(),name.end(),::isspace),name.end());
            dbbuff[name] = value;
        }
    }

    // parser the strings
    for (int i = 1; i <= 6; i++){
        // check the x and Y, make sure they agree with each other
        // if not it will take the x value as the shift and throw and warning
        TString nameTemplateX,nameTemplateY;
        if (HRS == "R"){
            nameTemplateX = Form("RGEM.rgems.x%d.position",i);
            nameTemplateY = Form("RGEM.rgems.y%d.position",i);

        } else{
            nameTemplateX = Form("LGEM.lgems.x%d.position",i);
            nameTemplateY = Form("LGEM.lgems.y%d.position",i);
        }
        //paser x
        TVector3 x_shift(0,0,0),y_shift(0,0,0);
        if (dbbuff.find(nameTemplateX.Data())!=dbbuff.end()){
            std::istringstream value(dbbuff[nameTemplateX.Data()].Data());
            double x=0,y=0,z=0;
            value >> x;
            value >> y;
            value >> z;
            x_shift.SetXYZ(x,y,z);
        }
        if (dbbuff.find(nameTemplateY.Data())!=dbbuff.end()){
            std::istringstream value(dbbuff[nameTemplateY.Data()].Data());
            double x=0,y=0,z=0;
            value >> x;
            value >> y;
            value >> z;
            y_shift.SetXYZ(x,y,z);
        }
//        x_shift.Print();
//        y_shift.Print();
        shiftbuffX[i] = x_shift;
        shiftbuffY[i] = y_shift;
    }
}

void fcn_residual(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
    // firt ste, only take the tranlation into consideration
    Double_t chisq=0.0;

    f=chisq;
}


void Optimizer(Int_t chamberID){
    TMinuit *gMinuit = new TMinuit(3);


}


float gemEstimateZ(TString fname,Int_t GEMID, float start=1.0, float  end=3.0){
    TChain *chain= new TChain("T");
    chain->Add(fname.Data());

    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }

    // read the Podd database file
    ReadDatabase(HRS);

    // search for the chambers
    std::vector<Int_t> chamberIDList;
    for (int i = 0; i <= 6; ++i) {
        if (HRS == "R"){
            if (chain->GetListOfBranches()->Contains(Form("RGEM.rgems.x%d.coord.pos",i))) chamberIDList.push_back(i);
        }else{
            if (chain->GetListOfBranches()->Contains(Form("LGEM.lgems.x%d.coord.pos",i))) chamberIDList.push_back(i);
        }
    }

    std::cout<<"--------> Scan Root Tree for GEM chambers <------------"<<std::endl;
    for (auto chamberID : chamberIDList){
        std::cout<<"ID "<<chamberID<<"  shift in DB: ("<<shiftbuffX[chamberID].X()<<","<<shiftbuffX[chamberID].Y()<<","<<shiftbuffX[chamberID].Z()<<")"<<std::endl;
    }

    std::vector<TString> treeBrachCheckArray;
    {
        treeBrachCheckArray.push_back(Form("%s.tr.x",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.th",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.y",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.ph",HRS.Data()));
        for (int i = 1; i<= 6; i++){
            if (HRS=="R"){
                treeBrachCheckArray.push_back(Form("RGEM.rgems.x%d.coord.pos",i));
            } else{
                treeBrachCheckArray.push_back(Form("LGEM.lgems.x%d.coord.pos",i));
            }
        }
    }
    for (auto str : treeBrachCheckArray) {
        if (!chain->GetListOfBranches()->Contains(str.Data())){
            std::cout<<"[ERROR]:: can NOT find banch "<< str.Data()<<std::endl;
        }
    }

    // all united in the hall are in unit of meter
    TH1F *rmsXH = new TH1F(Form("PRex GEM X %d",runID),Form("PRex GEM X %d",runID),500,start,end);
    TH1F *rmsYH = new TH1F(Form("PRex GEM Y %d",runID),Form("PRex GEM Y %d",runID),500,start,end);
    TCanvas *canv = new TCanvas("All run","All run",1960,1080);
    canv->Divide(5,3);
    float zscan = start;
    Int_t counter = 0;
    for (;zscan < end;){
        std::cout<<"Working on "<<zscan<<std::endl;
        canv->cd(counter/10 +1);
        zscan = zscan + 0.005;

        TString formStrX = Form("%s.tr.x + %s.tr.th * %f - %s.x%d.coord.pos",HRS.Data(),HRS.Data(),zscan,GEMProfix.Data(),GEMID);
        TString formStrY = Form("%s.tr.y + %s.tr.ph * %f - %s.y%d.coord.pos",HRS.Data(),HRS.Data(),zscan,GEMProfix.Data(),GEMID);
        TH1F *residualX = new TH1F(Form("PRex%d_GEM%d_vdc_ProjResidual%f_X",runID,GEMID,zscan),Form("PRex%d_GEM%d_vdc_ProjResidual%f_X",runID,GEMID,zscan),1000,-0.1,0.1);
        TH1F *residualY = new TH1F(Form("PRex%d_GEM%d_vdc_ProjResidual%f_Y",runID,GEMID,zscan),Form("PRex%d_GEM%d_vdc_ProjResidual%f_Y",runID,GEMID,zscan),1000,-0.1,0.1);

        chain->Project(residualX->GetName(),formStrX.Data());
        chain->Project(residualY->GetName(),formStrY.Data());

        residualX->Fit("gaus","","");
        residualY->Fit("gaus","","");

        rmsXH->Fill(zscan,residualX->GetFunction("gaus")->GetParameter(2));
        rmsYH->Fill(zscan,residualY->GetFunction("gaus")->GetParameter(2));

        residualX->SetLineColor(random());
        if (counter %10==0){
            residualX->Draw();
        } else{
            residualX->Draw();
        }
        counter += 1;
        canv->Update();
    }
    TCanvas *resCanv = new TCanvas("Res Canv","Res Canv",1960,1080);
    resCanv->Draw();
    rmsXH->Fit("pol2");
    rmsXH->Draw();

    auto fitFunc = rmsXH->GetFunction("pol2");
    float centralXVal = -fitFunc->GetParameter(1)/(2.0*fitFunc->GetParameter(2));
    TLine *line1 = new TLine(centralXVal,0.000,centralXVal,0.006);
    line1->Draw("same");
    std::cout<<centralXVal<<std::endl;

    resCanv->Update();

    TCanvas *resYCanv = new TCanvas("Res Y Canv","Res Y Canv",1960,1080);
    resYCanv->Draw();
    rmsYH->Fit("pol2");
    rmsYH->Draw();
    auto fitFuncY = rmsYH->GetFunction("pol2");
    float centralYVal = -fitFuncY->GetParameter(1)/(2.0*fitFuncY->GetParameter(2));
    TLine *line2 = new TLine(centralYVal,0.000,centralYVal,0.006);
    line2->Draw("same");
    std::cout<<centralYVal<<std::endl;
    resYCanv->Update();

    std::cout<<"Average ::"<<(centralXVal+centralYVal)/2.0<<std::endl;
    return (centralXVal+centralYVal)/2.0;
}



float gemEsitimatGEM1(TString fname){
    TChain *chain= new TChain("T");
    chain->Add(fname.Data());

    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }


    // read the Podd database file
    ReadDatabase(HRS);

    // search for the chambers
    std::vector<Int_t> chamberIDList;
    for (int i = 0; i <= 6; ++i) {
        if (HRS == "R"){
            if (chain->GetListOfBranches()->Contains(Form("RGEM.rgems.x%d.coord.pos",i))) chamberIDList.push_back(i);
        }else{
            if (chain->GetListOfBranches()->Contains(Form("LGEM.lgems.x%d.coord.pos",i))) chamberIDList.push_back(i);
        }
    }

    //#ifdef __DEBUG__
    std::cout<<"--------> Scan Root Tree for GEM chambers <------------"<<std::endl;
    for (auto chamberID : chamberIDList){
        std::cout<<"ID "<<chamberID<<"  shift in DB: ("<<shiftbuffX[chamberID].X()<<","<<shiftbuffX[chamberID].Y()<<","<<shiftbuffX[chamberID].Z()<<")"<<std::endl;
    }

    std::vector<TString> treeBrachCheckArray;
    {
        treeBrachCheckArray.push_back(Form("%s.tr.x",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.th",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.y",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.ph",HRS.Data()));
        for (int i = 1; i<= 6; i++){
            if (HRS=="R"){
                treeBrachCheckArray.push_back(Form("RGEM.rgems.x%d.coord.pos",i));
            } else{
                treeBrachCheckArray.push_back(Form("LGEM.lgems.x%d.coord.pos",i));
            }
        }
    }
    for (auto str : treeBrachCheckArray) {
        if (!chain->GetListOfBranches()->Contains(str.Data())){
            std::cout<<"[ERROR]:: can NOT find banch "<< str.Data()<<std::endl;
        }
    }
//#endif

    // all united in the hall are in unit of meter
    TH1F *rmsXH = new TH1F(Form("PRex GEM X %d",runID),Form("PRex GEM X %d",runID),500,0.8,1.3);
    TH1F *rmsYH = new TH1F(Form("PRex GEM Y %d",runID),Form("PRex GEM Y %d",runID),500,0.8,1.3);
    TCanvas *canv = new TCanvas("All run","All run",1960,1080);
    canv->Divide(5,3);
    float zscan = 0.8;
    Int_t counter = 0;
    for (;zscan < 1.3;){
        std::cout<<"Working on "<<zscan<<std::endl;
        canv->cd(counter/10 +1);
        zscan = zscan + 0.005;

        TString formStrX = Form("%s.tr.x + %s.tr.th * %f - %s.x1.coord.pos",HRS.Data(),HRS.Data(),zscan,GEMProfix.Data());
        TString formStrY = Form("%s.tr.y + %s.tr.ph * %f - %s.y1.coord.pos",HRS.Data(),HRS.Data(),zscan,GEMProfix.Data());
        TH1F *residualX = new TH1F(Form("PRex%d_GEM1_vdc_ProjResidual%f_X",runID,zscan),Form("PRex%d_GEM1_vdc_ProjResidual%f_X",runID,zscan),1000,-0.1,0.1);
        TH1F *residualY = new TH1F(Form("PRex%d_GEM1_vdc_ProjResidual%f_Y",runID,zscan),Form("PRex%d_GEM1_vdc_ProjResidual%f_Y",runID,zscan),1000,-0.1,0.1);

        chain->Project(residualX->GetName(),formStrX.Data());
        chain->Project(residualY->GetName(),formStrY.Data());

        residualX->Fit("gaus","","");
        residualY->Fit("gaus","","");

        rmsXH->Fill(zscan,residualX->GetFunction("gaus")->GetParameter(2));
        rmsYH->Fill(zscan,residualY->GetFunction("gaus")->GetParameter(2));

        residualX->SetLineColor(random());
        if (counter %10==0){
            residualX->Draw();
        } else{
            residualX->Draw();
        }
        counter += 1;
        canv->Update();
    }
    TCanvas *resCanv = new TCanvas("Res Canv","Res Canv",1960,1080);
    resCanv->Draw();
    rmsXH->Fit("pol2");
    rmsXH->Draw();

    auto fitFunc = rmsXH->GetFunction("pol2");
    float centralXVal = -fitFunc->GetParameter(1)/(2.0*fitFunc->GetParameter(2));
    TLine *line1 = new TLine(centralXVal,0.000,centralXVal,0.006);
    line1->Draw("same");
    std::cout<<centralXVal<<std::endl;

    resCanv->Update();

    TCanvas *resYCanv = new TCanvas("Res Y Canv","Res Y Canv",1960,1080);
    resYCanv->Draw();
    rmsYH->Fit("pol2");
    rmsYH->Draw();
    auto fitFuncY = rmsYH->GetFunction("pol2");
    float centralYVal = -fitFuncY->GetParameter(1)/(2.0*fitFuncY->GetParameter(2));
    TLine *line2 = new TLine(centralYVal,0.000,centralYVal,0.006);
    line2->Draw("same");
    std::cout<<centralYVal<<std::endl;
    resYCanv->Update();

    std::cout<<"Average ::"<<(centralXVal+centralYVal)/2.0<<std::endl;
    return (centralXVal+centralYVal)/2.0;

}
void gemEstimator(TString fname){

    TString DBLoadPath="/home/newdriver/Storage/Research/PRex_GEM/PRex_Replay/PRex_Database/PRex_Database";
    float gem0Zpos=0.0;
            //gemEsitimatGEM1(fname);

    TChain *chain= new TChain("T");
    chain->Add(fname.Data());

    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }


    // read the Podd database file
    ReadDatabase(HRS,DBLoadPath.Data());

    // search for the chambers
    std::vector<Int_t> chamberIDList;
    for (int i = 0; i <= 6; ++i) {
        if (HRS == "R"){
            if (chain->GetListOfBranches()->Contains(Form("RGEM.rgems.x%d.coord.pos",i))) chamberIDList.push_back(i);
        }else{
            if (chain->GetListOfBranches()->Contains(Form("LGEM.lgems.x%d.coord.pos",i))) chamberIDList.push_back(i);
        }
    }
//#ifdef __DEBUG__
     std::cout<<"--------> Scan Root Tree for GEM chambers <------------"<<std::endl;
     for (auto chamberID : chamberIDList){
         std::cout<<"ID "<<chamberID<<"  shift in DB: ("<<shiftbuffX[chamberID].X()<<","<<shiftbuffX[chamberID].Y()<<","<<shiftbuffX[chamberID].Z()<<")"<<std::endl;
     }

     std::vector<TString> treeBrachCheckArray;
    {
        treeBrachCheckArray.push_back(Form("%s.tr.x",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.th",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.y",HRS.Data()));
        treeBrachCheckArray.push_back(Form("%s.tr.ph",HRS.Data()));
        for (int i = 1; i<= 6; i++){
            if (HRS=="R"){
                treeBrachCheckArray.push_back(Form("RGEM.rgems.x%d.coord.pos",i));
            } else{
                treeBrachCheckArray.push_back(Form("LGEM.lgems.x%d.coord.pos",i));
            }
        }
    }
    for (auto str : treeBrachCheckArray) {
        if (!chain->GetListOfBranches()->Contains(str.Data())){
            std::cout<<"[ERROR]:: can NOT find banch "<< str.Data()<<std::endl;
        }
    }
//#endif

    std::map<Int_t, TH1F *> gemResidualX;
    std::map<Int_t, TH1F *> gemResidualY;

    for (auto chamberID : chamberIDList){
        TH1F *residualX = new TH1F(Form("PRex%d_GEM%d_vdc_ProjResidual_X",runID,chamberID),Form("PRex%d_GEM%d_vdc_ProjResidual_X",runID,chamberID),1000,-0.1,0.1);
        TH1F *residualY = new TH1F(Form("PRex%d_GEM%d_vdc_ProjResidual_Y",runID,chamberID),Form("PRex%d_GEM%d_vdc_ProjResidual_Y",runID,chamberID),1000,-0.1,0.1);

        TString formStrX = Form("%s.tr.x + %s.tr.th * (%f+%f) - %s.x%d.coord.pos",HRS.Data(),HRS.Data(),shiftbuffX[chamberID].Z(),gem0Zpos,GEMProfix.Data(),chamberID);
        TString formStrY = Form("%s.tr.y + %s.tr.ph * (%f+%f) - %s.y%d.coord.pos",HRS.Data(),HRS.Data(),shiftbuffX[chamberID].Z(),gem0Zpos,GEMProfix.Data(),chamberID);

//        TString formStrX = Form("%s.x1.coord.pos + %s.tr.th * %f - %s.x%d.coord.pos",GEMProfix.Data(),HRS.Data(),shiftbuffX[chamberID].Z(),GEMProfix.Data(),chamberID);
//        TString formStrY = Form("%s.y1.coord.pos + %s.tr.ph * %f - %s.y%d.coord.pos",GEMProfix.Data(),HRS.Data(),shiftbuffX[chamberID].Z(),GEMProfix.Data(),chamberID);

        std::cout<<__FUNCTION__ <<"("<<__LINE__<<") project histogram:"<< formStrX.Data()<<std::endl;
        std::cout<<__FUNCTION__ <<"("<<__LINE__<<") project histogram:"<< formStrY.Data()<<std::endl;

        chain->Project(residualX->GetName(),formStrX.Data(),Form("%s.tr.x + %s.tr.th * (%f+%f) - %s.x%d.coord.pos !=0",HRS.Data(),HRS.Data(),shiftbuffX[chamberID].Z(),gem0Zpos,GEMProfix.Data(),chamberID));
        chain->Project(residualY->GetName(),formStrY.Data());
        gemResidualX[chamberID] = residualX;
        gemResidualY[chamberID] = residualY;
    }

    std::map<Int_t,TVector3> residualRes;
    for (auto chamberID : chamberIDList) {
        residualRes[chamberID] = TVector3(0.0,0.0,0.0);
    }
    TCanvas *ResidualXcanv = new TCanvas(Form("PRex/CRex X %sHRS %d",HRS.Data(),runID),Form("PRex/CRex X %sHRS %d",HRS.Data(),runID),1960,1080);
    ResidualXcanv->Divide(3,2);
    ResidualXcanv->Draw();
    for (auto chamberID : chamberIDList){
        ResidualXcanv->cd(chamberID);
        if (gemResidualX.find(chamberID) != gemResidualX.end()){
            gemResidualX[chamberID]->Draw();
//            if (chamberID != 1)
            {
                auto mean = gemResidualX[chamberID]->GetBinCenter(gemResidualX[chamberID]->GetMaximumBin());
                auto rms = gemResidualX[chamberID]->GetRMS();
                gemResidualX[chamberID]->GetXaxis()->SetRangeUser(mean - 10 * rms, mean + 10 * rms);
                gemResidualX[chamberID]->Fit("gaus", "", "", mean - 3 * rms, mean + 3 * rms);
                auto fitFunc = gemResidualX[chamberID]->GetFunction("gaus");
                TLine *line = new TLine(fitFunc->GetParameter(1),0,fitFunc->GetParameter(1),fitFunc->GetParameter(0));
                line->SetLineColor(kRed);
                line->SetLineWidth(2);
                line->Draw("same");
                TLatex *txt = new TLatex(fitFunc->GetParameter(1),fitFunc->GetParameter(0),Form("Mean : %f",fitFunc->GetParameter(1)));
                txt->Draw("same");
                residualRes[chamberID].SetX(shiftbuffX[chamberID].X()+fitFunc->GetParameter(1));
            }

        }
    }
    ResidualXcanv->Update();

    TCanvas *ResidualYcanv = new TCanvas(Form("PRex/CRex Y %sHRS %d",HRS.Data(),runID),Form("PRex/CRex Y %sHRS %d",HRS.Data(),runID),1960,1080);
    ResidualYcanv->Divide(3,2);
    ResidualYcanv->Draw();
    for (auto chamberID : chamberIDList){
        ResidualYcanv->cd(chamberID);
        if (gemResidualY.find(chamberID) != gemResidualY.end()){
            gemResidualY[chamberID]->Draw();
//            if (chamberID != 1)
            {
                auto mean = gemResidualY[chamberID]->GetBinCenter(gemResidualY[chamberID]->GetMaximumBin());
                auto rms = gemResidualY[chamberID]->GetRMS();
                gemResidualY[chamberID]->GetXaxis()->SetRangeUser(mean - 10 * rms, mean + 10 * rms);
                gemResidualY[chamberID]->Fit("gaus", "", "", mean - 3 * rms, mean + 3 * rms);

                auto fitFunc = gemResidualY[chamberID]->GetFunction("gaus");
                TLine *line = new TLine(fitFunc->GetParameter(1),0,fitFunc->GetParameter(1),fitFunc->GetParameter(0));
                line->SetLineColor(kRed);
                line->SetLineWidth(2);
                line->Draw("same");
                TLatex *txt = new TLatex(fitFunc->GetParameter(1),fitFunc->GetParameter(0),Form("Mean : %f",fitFunc->GetParameter(1)));
                txt ->Draw("same");
                residualRes[chamberID].SetY(shiftbuffX[chamberID].Y()+fitFunc->GetParameter(1));
            }
        }
    }
    ResidualYcanv->Update();

    for (auto  chamberID : chamberIDList){
        residualRes[chamberID].SetZ(shiftbuffX[chamberID].Z()+gem0Zpos);
    }

    std::cout<<"------------------------------------------------------"<<std::endl;
    for (auto  chamberID: chamberIDList){
        std::cout<<"Chamber ID :"<<chamberID <<"  ("<<residualRes[chamberID].X()<<"   "<<residualRes[chamberID].Y()<<"   "<<residualRes[chamberID].Z()<<")"<<std::endl;
    }
    std::cout<<"Variable \tGEM1 \tGEM1 \tGEM2 \tGEM3 \tGEM4 \tGEM5 \tGEM6"<<std::endl;
    std::cout<<"X";
    for (auto  chamberID: chamberIDList){
        std::cout<<",   "<<residualRes[chamberID].X();
//        std::cout<<"Chamber ID :"<<chamberID <<"  ("<<residualRes[chamberID].X()<<"   "<<residualRes[chamberID].Y()<<"   "<<residualRes[chamberID].Z()<<")"<<std::endl;
    }
    std::cout<<std::endl;
    std::cout<<"Y";
    for (auto  chamberID: chamberIDList){
        std::cout<<",   "<<residualRes[chamberID].Y();
//        std::cout<<"Chamber ID :"<<chamberID <<"  ("<<residualRes[chamberID].X()<<"   "<<residualRes[chamberID].Y()<<"   "<<residualRes[chamberID].Z()<<")"<<std::endl;
    }
    std::cout<<std::endl;
    std::cout<<"Z";
    for (auto  chamberID: chamberIDList){
        std::cout<<",   "<<residualRes[chamberID].Z();
//        std::cout<<"Chamber ID :"<<chamberID <<"  ("<<residualRes[chamberID].X()<<"   "<<residualRes[chamberID].Y()<<"   "<<residualRes[chamberID].Z()<<")"<<std::endl;
    }
    std::cout<<std::endl;

}


void  tester_ReadGEMMap(){
    ReadDatabase("L");
}

void tester_main(){
    gemEstimator("/home/newdriver/PRex_GEM/PRex_replayed/prexLHRS_145*");
}

void testGetGEM1(){
    gemEsitimatGEM1("/home/newdriver/PRex_GEM/PRex_replayed/prexLHRS_145*");
}

void testGetGEMZ(){
    gemEstimateZ("/home/newdriver/PRex_GEM/PRex_replayed/prexLHRS_145*",1,1.8,3.0);

}

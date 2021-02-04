/*
 * rootReader2.C
 *
 *  Created on: Aug 4, 2019
 *      Author: newdriver
 */

#include <TString.h>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMatrix.h>
#include <vector>
#include <string>
#include <TCanvas.h>
#include <TLine.h>
#include <string>
#include <stdlib.h>
#include <cctype>
#include <numeric>
#include "TVector3.h"
#include "TSystem.h"
#include "istream"
#include "fstream"
#include "TChain.h"

#pragma cling load("libTreeSearch-GEM.so");
//#pragma cling load("/adaqfs/home/a-onl/siyu/PRex_MPD/libprexCounting.so"); // load
#pragma cling load("/home/newdriver/Storage/Research/PRex_GEM/PRex_Replay/PRexAnalyzer/libprexCounting.so"); // load

#define _ROOTREADER_MAX_GEM_CHAMBER 7
#define _ROOTREADER_MAX_TSAMPLE 3


#define _ROOTREADER_MAX_GEM_CHAMBER 7
#define _ROOTREADER_MAX_TSAMPLE 3


TString DBLoadFolder = "/home/newdriver/Storage/Research/PRex_GEM/PRex_Replay/PRex_Database/PRex_Database";

struct HitStruct{
public:
	HitStruct(int8_t detID, double_t hitX, double_t hitY, double_t hitZ, double_t hitTh=0.0, double_t hitPh=0.0){
		this->detectorID=detID;
		this->x=hitX;
		this->y=hitY;
		this->z=hitZ;
		this->theta=hitTh;
		this->phi=hitPh;
	};

	~HitStruct(){};

	inline int8_t GetDetectorID(){return detectorID;};
	inline double_t GetX(){return x;};
	inline double_t GetY(){return y;};
	inline double_t GetZ(){return z;};
	inline double_t GetTheta(){return theta;};
	inline double_t GetPhi(){return phi;};



	void Print(){
		std::cout<<"==> ("<<(int)detectorID<<")"<<std::endl;
		std::cout<<"	x    :"<<x<<std::endl
				 <<"	y    :"<<y<<std::endl
				 <<"	z    :"<<z<<std::endl
				 <<"	theta:"<<theta<<std::endl
				 <<"	phi  :"<<phi<<std::endl;};

	inline bool operator == (const HitStruct &y){
		return this->detectorID==y.detectorID;}
private:
	int8_t detectorID;
	double_t x;
	double_t y;
	double_t z;
	double_t theta;   // x'
	double_t phi;     // y'

};

std::vector<std::vector<Int_t>> splitCluster(std::vector<Int_t> StripsVec,Int_t clusterSizeCut=2){
	std::sort(StripsVec.begin(),StripsVec.end());
	std::vector<std::vector<Int_t>> result;

	if(StripsVec.size()!=0){
		std::vector<Int_t> buff;
		for(auto value=StripsVec.begin();value!=StripsVec.end();value++){
			if((value+1)!=StripsVec.end()){
				//std::cout<<*value<<"  next:"<<*value+1<<"  next in buff:"<< *(value+1);

				if((*value+1)==*(value+1)){
					//std::cout<<"  Match"<<std::endl;
					buff.push_back(*value);
				}else{
					buff.push_back(*value);
					if(buff.size()>=clusterSizeCut)
					result.push_back(buff);
					buff.clear();
				}
			}else{
				buff.push_back(*value);
				if(buff.size()>=clusterSizeCut)
				result.push_back(buff);
				buff.clear();
			}
		}

	}
	return result;
}


std::map<int,TVector3> ReadDatabase(TString HRS = "R",TString DB_DIR="./"){

    std::cout<<"DB path "<< DB_DIR.Data()<<std::endl;

    std::map<std::string,TString> dbbuff;
    std::map<int,TVector3> shiftbuffX;
    std::map<int,TVector3> shiftbuffY;

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
    return shiftbuffX;
}


void rootReader2(TString fname,Bool_t save2pdf= false){

    if (fname.IsNull()){
        std::cout<<"[ERROR]:: can NOT find root file "<<fname.Data()<<std::endl;
        exit(-1);
    }

    TChain *chain = new TChain("T");
    chain->Add(fname.Data());
    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }

    TLine *detPlaneline[6];

    // search for the chambers
    std::vector<Int_t> chamberIDList;
    std::map<int16_t,std::vector<int16_t>> TsampleLit;

    for (int i = 0; i <= 6; ++i) {
        if (chain->GetListOfBranches()->Contains(Form("%s.x%d.coord.pos",GEMProfix.Data(),i))) chamberIDList.push_back(i);
    }

    // initialize the buffers
    for (auto chamberID : chamberIDList){
        std::cout<<"Reading chamber :"<<chamberID<<"\n	sample:";
        for (int adc_sample =0; adc_sample <_ROOTREADER_MAX_TSAMPLE; adc_sample++){
            if(chain->GetListOfBranches()->Contains(Form("%s.x%d.adc%d",GEMProfix.Data(),chamberID,adc_sample))){
                std::cout<<adc_sample<<"  ";
                TsampleLit[chamberID].push_back(adc_sample);
            }
        }
        std::cout<<std::endl;
    }
    std::cout<<"******************  Reading Done  *********************"<<std::endl;

    // LINK THE LIST
    Int_t fEvtNum=0;
    Int_t fRun=0;
    Int_t fvdcXNum=0;
    double_t fvdcX[100];
    Int_t fvdcYNum=0;
    double_t fvdcY[100];
    double_t fvdc_th[10];
    double_t fvdc_ph[10];

    std::string fEvtNumForm("fEvtHdr.fEvtNum");
    if (chain->GetListOfBranches()->Contains(fEvtNumForm.c_str())){
        chain->SetBranchAddress(fEvtNumForm.c_str(),&fEvtNum);
    }else{
        std::cout<<"[Warning]:: fEvtNum data did not find in the replay resuly, skip it"<<std::endl;
    }
    std::string fRunForm("Event_Branch/fEvtHdr/fEvtHdr.fRun");
    if(chain->GetListOfBranches()->Contains(fRunForm.c_str())){
        chain->SetBranchAddress(fRunForm.c_str(),&fRun);
    }else{
        std::cout<<"[Warning]:: fRun data did not find in the replay resuly, skip it"<<std::endl;
    }

    std::string fvdcXNumForm(Form("Ndata.%s.tr.x",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(fvdcXNumForm.c_str())){
        chain->SetBranchAddress(fvdcXNumForm.c_str(),&fvdcXNum);
    }else{
        std::cout<<"[Warning]:: VDC data did not find in the replay resuly"<<std::endl;
    }

    std::string fvdcXForm(Form("%s.tr.x",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(fvdcXForm.c_str())){
        chain->SetBranchAddress(fvdcXForm.c_str(),fvdcX);
    }else{
        std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
    }

    std::string fvdc_th_Form(Form("%s.tr.th",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(fvdc_th_Form.c_str())){
        chain->SetBranchAddress(fvdc_th_Form.c_str(),fvdc_th);
    }else{
        std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
    }

    std::string fvdc_ph_Form(Form("%s.tr.ph",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(fvdc_ph_Form.c_str())){
        chain->SetBranchAddress(fvdc_ph_Form.c_str(),fvdc_ph);
    }else{
        std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
    }

    std::string fvdcYNumForm(Form("Ndata.%s.tr.y",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(fvdcYNumForm.c_str())){
        chain->SetBranchAddress(fvdcYNumForm.c_str(),&fvdcYNum);
    }else{
        std::cout<<"[Warning]:: VDC data did not find in the replay resuly"<<std::endl;
    }

    std::string fvdcYForm(Form("%s.tr.y",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(fvdcYForm.c_str())){
        chain->SetBranchAddress(fvdcYForm.c_str(),fvdcY);
    }else{
        std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
    }

    double_t DetectorZpos[7]={0.0,  0.9, 1.5982485,  1.8558464, 2.3839658, 2.5141378, 2.6395974};

    auto databasePos= ReadDatabase(HRS.Data(),DBLoadFolder.Data());
    for (int i = 1; i <= 6; ++i) {
        if (databasePos.find(i)!=databasePos.end()){
            DetectorZpos[i] = databasePos[i].Z();
        }
    }

    for (int chamberID=0 ; chamberID<=6 ; chamberID++){
        if(chamberID <=3){
            detPlaneline[chamberID]= new TLine(-0.1,DetectorZpos[chamberID],0.1,DetectorZpos[chamberID]);
            //detPlaneline[chamberID]->Draw("same");
        }else{
            detPlaneline[chamberID]= new TLine(-0.3,DetectorZpos[chamberID],0.3,DetectorZpos[chamberID]);
            //detPlaneline[chamberID]->Draw("same");
        }
    }

    std::map<Int_t,Int_t> GEMNDetX;
    std::map<Int_t,Int_t> GEMNDetY;
    std::map<Int_t,double_t  *> GEMDetX;
    std::map<Int_t,double_t  *> GEMDetY;


    // read out the coord parameters
    //  coord.pos parameters
    std::map<Int_t, Int_t> GEM_NCoordPosX;
    std::map<Int_t, Int_t> GEM_NCoordPosY;
    std::map<Int_t, double *>	GEM_CoordPosX;
    std::map<Int_t, double *>	GEM_CoordPosY;

    //trkPos
    std::map<Int_t, Int_t> GEM_NCoordTrackPosX;
    std::map<Int_t, Int_t> GEM_NCoordTrackPosY;
    std::map<Int_t, double *>	GEM_CoordTrackPosX;
    std::map<Int_t, double *>	GEM_CoordTrackPosY;

    // load the GEM th-ph result

    Int_t NGEMTracktheta=0;
    Int_t NGEMTrackphi=0;
    double	GEMTracktheta[50];
    double  GEMTrackphi[50];

    // load the data for the theta-phi for GEM detectors
    std::string NGEMTracktheta_str(Form("Ndata.%sGEM.tr.th",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(NGEMTracktheta_str.c_str())){
        chain->SetBranchAddress(NGEMTracktheta_str.c_str(), &NGEMTracktheta);
    } else{
        std::cout<<"[ERROR]::Can not find "<<NGEMTracktheta_str.c_str()<<std::endl;
    }
    std::string GEMTracktheta_str(Form("%sGEM.tr.th",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(GEMTracktheta_str.c_str())){
        chain->SetBranchAddress(GEMTracktheta_str.c_str(), GEMTracktheta);
    } else{
        std::cout<<"[ERROR]::Can not find "<<GEMTracktheta_str.c_str()<<std::endl;
    }

    std::string NGEMTrackphi_str(Form("Ndata.%sGEM.tr.ph",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(NGEMTrackphi_str.c_str())){
        chain->SetBranchAddress(NGEMTrackphi_str.c_str(),&NGEMTrackphi);
    } else{
        std::cout<<"[ERROR]::Can not find "<<NGEMTrackphi_str.c_str()<<std::endl;
    }
    std::string GEMTrackphi_str(Form("%sGEM.tr.ph",HRS.Data()));
    if(chain->GetListOfBranches()->Contains(GEMTrackphi_str.c_str())){
        chain->SetBranchAddress(GEMTrackphi_str.c_str(), GEMTrackphi);
    } else{
        std::cout<<"[ERROR]::Can not find "<<GEMTrackphi_str.c_str()<<std::endl;
    }

    // connect the chain to the GEM chamber
    for (auto chamberID:chamberIDList){
        std::cout<<chamberID<<std::endl;

        double_t detX[20];
        double_t detY[20];
        Int_t NdetX=0;
        Int_t NdetY=0;
        double_t detZ=DetectorZpos[chamberID];

        std::string NdetX_str(Form("Ndata.%s.x%d.hit.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(NdetX_str.c_str())){
            chain->SetBranchAddress(NdetX_str.c_str(),&GEMNDetX[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }
        // load the data
        GEMDetX[chamberID]=new double_t [100];
        std::string detX_str(Form("%s.x%d.hit.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(detX_str.c_str())){
            chain->SetBranchAddress(detX_str.c_str(),GEMDetX[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }

        // load the Y dimension
        std::string NdetY_str(Form("Ndata.%s.y%d.hit.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(NdetY_str.c_str())){
            chain->SetBranchAddress(NdetY_str.c_str(),&GEMNDetY[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }
        GEMDetY[chamberID]=new double_t [100];
        std::string detY_str(Form("%s.y%d.hit.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(detY_str.c_str())){
            chain->SetBranchAddress(detY_str.c_str(),GEMDetY[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }


        //finish load the data

        // start load the coordi parameter in the root file

        std::string NCoordPosX_str(Form("Ndata.%s.x%d.coord.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(NCoordPosX_str.c_str())){
            chain->SetBranchAddress(NCoordPosX_str.c_str(), &GEM_NCoordPosX[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }
        GEM_CoordPosX[chamberID]=new double_t [100];
        std::string CoordPosX_str(Form("%s.x%d.coord.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(CoordPosX_str.c_str())){
            chain->SetBranchAddress(CoordPosX_str.c_str(), GEM_CoordPosX[chamberID]);
        }else{
            std::cout<<"[Warning]:: GEM_CoordPosX data did not find in the replay resuly, skip it"<<std::endl;
        }

        std::string NCoordPosY_str(Form("Ndata.%s.y%d.coord.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(NCoordPosY_str.c_str())){
            chain->SetBranchAddress(NCoordPosY_str.c_str(), &GEM_NCoordPosY[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }

        GEM_CoordPosY[chamberID]=new double_t [100];
        std::string CoordPosY_str(Form("%s.y%d.coord.pos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(CoordPosY_str.c_str())){
            chain->SetBranchAddress(CoordPosY_str.c_str(), GEM_CoordPosY[chamberID]);
        }else{
            std::cout<<"[Warning]:: GEM_CoordPosY data did not find in the replay resuly, skip it"<<std::endl;
        }

        // start load the coordi parameter in the root file
        std::string NCoordTrackPosX_str(Form("Ndata.%s.x%d.coord.trkpos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(NCoordTrackPosX_str.c_str())){
            chain->SetBranchAddress(NCoordTrackPosX_str.c_str(), &GEM_NCoordTrackPosX[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it "<<__LINE__<<std::endl;
        }

        GEM_CoordTrackPosX[chamberID]=new double_t [100];
        std::string CoordTrackPosX_str(Form("%s.x%d.coord.trkpos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(CoordTrackPosX_str.c_str())){
            chain->SetBranchAddress(CoordTrackPosX_str.c_str(), GEM_CoordTrackPosX[chamberID]);
        }else{
            std::cout<<"[Warning]:: GEM_CoordPosX data did not find in the replay resuly, skip it"<<std::endl;
        }

        std::string NCoordTrackPosY_str(Form("Ndata.%s.y%d.coord.trkpos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(NCoordTrackPosY_str.c_str())){
            chain->SetBranchAddress(NCoordTrackPosY_str.c_str(), &GEM_NCoordTrackPosY[chamberID]);
        }else{
            std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
        }

        GEM_CoordTrackPosY[chamberID]=new double_t [100];
        std::string CoordTrackPosY_str(Form("%s.y%d.coord.trkpos",GEMProfix.Data(),chamberID));
        if(chain->GetListOfBranches()->Contains(CoordTrackPosY_str.c_str())){
            chain->SetBranchAddress(CoordTrackPosY_str.c_str(), GEM_CoordTrackPosY[chamberID]);
        }else{
            std::cout<<"[Warning]:: GEM_CoordPosY data did not find in the replay resuly, skip it"<<std::endl;
        }
    }

    // loop on the data
    std::vector<std::vector<HitStruct>> DetEventBuff;
    std::vector<HitStruct> DetHitArr;
    std::vector<HitStruct> DetHitArrX;
    std::vector<HitStruct> DetHitArrY;
    std::cout<<"Total Entries:"<<chain->GetEntries()<<std::endl;

    TH2F *DetHist2DXZ;//= new TH2F("XZ","XZ",2000,-0.4,0.4,1000,-0,3.0);
    TH2F *DetHist2DYZ;
    TH2F *DetHist2DXZCorr;//= new TH2F("XZ","XZ",2000,-0.4,0.4,1000,-0,3.0);
    TH2F *DetHist2DYZCorr;

    //used for plot the coord positions
    TH2F *DetCoordPosXZ;
    TH2F *DetCoordPosYZ;
    TH2F *DetCoordTrackPosXZ;
    TH2F *DetCoordTrackPosYZ;

    TCanvas *eventCanvas=new TCanvas("CanvasDisplay","CanvasDisplay",1000,1000);
    eventCanvas->Draw();
    eventCanvas->Print(Form("EvtDisp_%d.pdf(",runID),"pdf");
    for(auto entry=1;entry<(chain->GetEntries()) && entry<100;entry++){
        eventCanvas->Clear();
        eventCanvas->Divide(1,2);
        eventCanvas->cd(1)->Divide(3,1);
        eventCanvas->cd(2)->Divide(3,1);

        chain->GetEntry(entry);
        //PRex_GEM_tree->GetEntry(1);
        DetHitArr.clear();
        DetHitArrX.clear();
        DetHitArrY.clear();
        // load the data to data buff
        for (auto chamberID : chamberIDList){
            if (GEMNDetX.find(chamberID) != GEMNDetX.end()) {
                for (int i = 0; i < GEMNDetX[chamberID]; i++) {
                    HitStruct hittemp(chamberID, GEMDetX[chamberID][i], 0.0,
                                      DetectorZpos[chamberID]);
                    DetHitArrX.push_back(hittemp);
                }
            }

            // load the Y dimension
            if (GEMNDetY.find(chamberID) != GEMNDetY.end()) {
                for (int i = 0; i < GEMNDetY[chamberID]; i++) {
                    double_t y = GEMDetY[chamberID][i];
//					if(chamberID ==6) y+=0.05;
                    HitStruct hittemp(chamberID, 0.0, y,
                                      DetectorZpos[chamberID]);
                    DetHitArrY.push_back(hittemp);
                }
            }

        }
        // write all the good event to the buff
        // check the VDC
        Bool_t goodHitFlag = kTRUE;
        if((fvdcYNum==1)&&(fvdcXNum==1)){
            HitStruct hit(0,fvdcX[0],fvdcY[0],0.0,fvdc_th[0],fvdc_ph[0]);
            DetHitArr.push_back(hit);
        }else{
            goodHitFlag = kFALSE;
        }


        for (auto chamberID : chamberIDList) {
            if ((GEMNDetX.find(chamberID) != GEMNDetX.end())
                && (GEMNDetY.find(chamberID) != GEMNDetY.end())) {
                if ((GEMNDetX[chamberID] == 1)
                    && (GEMNDetY[chamberID] == 1)) {

                } else {
                    goodHitFlag = kFALSE;
                }
            } else {
                goodHitFlag = kFALSE;
            }
        }
        if(goodHitFlag){
            for (auto chamberID : chamberIDList) {
                HitStruct hit(chamberID,GEMDetX[chamberID][0],GEMDetY[chamberID][0],DetectorZpos[chamberID]);
                DetHitArr.push_back(hit);
            }
        }
        if(goodHitFlag){
            DetEventBuff.push_back(DetHitArr);
        }

        for (auto hit : DetHitArrX) hit.Print();
        for (auto hit : DetHitArrY) hit.Print();
        // finish load the data
        // load the data for display
        double_t CorrectionMatrix[]={
                0.0,
                0.0,
                0.0,

                0.00,
                0.00,
                0.00,

                0.00,
                0.00,
                0.00,

                0.00,
                0.00,
                0.00,

                0.00,
                0.00,
                0.00,

                0.00,
                0.00,
                0.00};
        DetHist2DXZ= new TH2F(Form("XZ_Evt%d",entry),Form("XZ_Evt%d",entry),2000,-0.4,0.4,1000,-0,3.0);
        DetHist2DYZ= new TH2F(Form("YZ_Evt%d",entry),Form("YZ_Evt%d",entry),2000,-0.4,0.4,1000,-0,3.0);

        DetHist2DXZCorr= new TH2F(Form("XZ_CEvt%d",entry),Form("XZ_CEvt%d",entry),2000,-0.4,0.4,1000,-0,3.0);
        DetHist2DYZCorr= new TH2F(Form("YZ_CEvt%d",entry),Form("YZ_CEvt%d",entry),2000,-0.4,0.4,1000,-0,3.0);

        DetHist2DXZ->SetMarkerSize(1);
        DetHist2DXZ->SetMarkerColor(2);
        DetHist2DXZ->SetMarkerStyle(20);

        DetHist2DYZ->SetMarkerSize(1);
        DetHist2DYZ->SetMarkerColor(2);
        DetHist2DYZ->SetMarkerStyle(20);


        DetHist2DXZCorr->SetMarkerSize(1);
        DetHist2DXZCorr->SetMarkerColor(3);
        DetHist2DXZCorr->SetMarkerStyle(20);

        DetHist2DYZCorr->SetMarkerSize(1);
        DetHist2DYZCorr->SetMarkerColor(3);
        DetHist2DYZCorr->SetMarkerStyle(20);

        std::cout<<"--------------Hit Pos----------------"<<std::endl;
        std::cout<<"====> Hit Position "<<std::endl;
        for(auto Hit : DetHitArrX){
            DetHist2DXZ->Fill(Hit.GetX(),Hit.GetZ());
            DetHist2DXZCorr->Fill(Hit.GetX()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)],Hit.GetZ()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)+2]);
            Hit.Print();
        }

        for (auto Hit : DetHitArrY){
            DetHist2DYZ->Fill(Hit.GetY(),Hit.GetZ());
            DetHist2DYZCorr->Fill(Hit.GetY()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)+1],Hit.GetZ()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)+2]);
        }

        // plot the data
        DetCoordPosXZ= new TH2F(Form("XZ_Pos%d",entry),Form("XZ_Pos%d",entry),2000,-0.4,0.4,1000,-0,3.0);
        DetCoordPosYZ= new TH2F(Form("YZ_Pos%d",entry),Form("YZ_Pos%d",entry),2000,-0.4,0.4,1000,-0,3.0);

        DetCoordTrackPosXZ= new TH2F(Form("XZ_CTRackPos%d",entry),Form("XZ_CTrackPos%d",entry),2000,-0.4,0.4,1000,-0,3.0);
        DetCoordTrackPosYZ= new TH2F(Form("YZ_CTrackPos%d",entry),Form("YZ_CTrackPos%d",entry),2000,-0.4,0.4,1000,-0,3.0);


        DetCoordPosXZ->SetMarkerSize(1);
        DetCoordPosXZ->SetMarkerColor(4);
        DetCoordPosXZ->SetMarkerStyle(20);

        DetCoordPosYZ->SetMarkerSize(1);
        DetCoordPosYZ->SetMarkerColor(4);
        DetCoordPosYZ->SetMarkerStyle(20);


        DetCoordTrackPosXZ->SetMarkerSize(1);
        DetCoordTrackPosXZ->SetMarkerColor(4);
        DetCoordTrackPosXZ->SetMarkerStyle(20);

        DetCoordTrackPosYZ->SetMarkerSize(1);
        DetCoordTrackPosYZ->SetMarkerColor(4);
        DetCoordTrackPosYZ->SetMarkerStyle(20);
        std::cout<<"--------------Coord Pos----------------"<<std::endl;
        for (auto chamberID : chamberIDList){

            std::cout<<"====>"<<chamberID<<std::endl;
            if((GEM_NCoordPosX.find(chamberID)!=GEM_NCoordPosX.end())){
                for(int i = 0; i < GEM_NCoordPosX[chamberID]; i++){

                    std::cout<< i<<std::endl;
                    DetCoordPosXZ->Fill(GEM_CoordPosX[chamberID][i],DetectorZpos[chamberID]);
                    std::cout<<"  xzPos: ("<<GEM_CoordPosX[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
                }
            }
            if((GEM_NCoordPosY.find(chamberID)!=GEM_NCoordPosY.end())){
                for(int i = 0; i < GEM_NCoordPosY[chamberID]; i++){
                    DetCoordPosYZ->Fill(GEM_CoordPosY[chamberID][i],DetectorZpos[chamberID]);
                    std::cout<<"  yzPos: ("<<GEM_CoordPosY[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
                }
            }

            if((GEM_NCoordTrackPosX.find(chamberID)!=GEM_NCoordTrackPosX.end())){
                for(int i = 0; i < GEM_NCoordTrackPosX[chamberID]; i++){
                    DetCoordTrackPosXZ->Fill(GEM_CoordTrackPosX[chamberID][i],DetectorZpos[chamberID]);
                    std::cout<<"  xzTrackPos: ("<<GEM_CoordTrackPosX[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
                }
            }
            if((GEM_NCoordTrackPosY.find(chamberID)!=GEM_NCoordTrackPosY.end())){
                for(int i = 0; i < GEM_NCoordTrackPosY[chamberID]; i++){
                    DetCoordTrackPosYZ->Fill(GEM_CoordTrackPosY[chamberID][i],DetectorZpos[chamberID]);
                    std::cout<<"  yzTrackPos: ("<<GEM_CoordTrackPosY[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
                }
            }
            std::cout<<std::endl;
        }

        eventCanvas->cd(1);


        eventCanvas->cd(1)->cd(1);
        DetHist2DXZ->Draw();
        for(int i = 0 ; i <=6 ; i++){
            detPlaneline[i]->Draw("same");
        }


        eventCanvas->cd(1)->cd(2);
        DetHist2DXZ->Draw();
        DetCoordPosXZ->Draw("same");
        for(int i = 0 ; i <=6 ; i++){
            detPlaneline[i]->Draw("same");
        }

        eventCanvas->cd(1)->cd(3);
        DetHist2DXZ->Draw();
        DetCoordTrackPosXZ->Draw("same");
        for(int i = 0 ; i <=6 ; i++){
            detPlaneline[i]->Draw("same");
        }

        {
            TLine *xztrack=new TLine(fvdcX[0],0.0,fvdcX[0]+fvdc_th[0]*2.7,2.7);
            xztrack->Draw("same");
        }

        eventCanvas->cd(2)->cd(1);
        DetHist2DYZ->Draw();
//		{
//			TLine *yztrack=new TLine(fvdcY[0],0.0,fvdcY[0]+fvdc_ph[0]*2.7,2.7);
//			yztrack->Draw("same");
//		}
        for(int i = 0 ; i <=6 ; i++){
            detPlaneline[i]->Draw("same");
        }

        eventCanvas->cd(2)->cd(2);
        DetHist2DYZ->Draw();
        DetCoordPosYZ->Draw("same");
        for(int i = 0 ; i <=6 ; i++){
            detPlaneline[i]->Draw("same");
        }

        eventCanvas->cd(2)->cd(3);
        DetHist2DYZ->Draw();
        DetCoordTrackPosYZ->Draw("same");
        for(int i = 0 ; i <=6 ; i++){
            detPlaneline[i]->Draw("same");
        }

        {
            TLine *yztrack=new TLine(fvdcY[0],0.0,fvdcY[0]+fvdc_ph[0]*2.7,2.7);
            yztrack->Draw("same");
        }

        eventCanvas->Update();
        eventCanvas->Print(Form("EvtDisp_%d.pdf",runID),"pdf");
        //-------------------------------------
        std::cout<<"------------ Located the reconstructed angles -------------------"<<std::endl;
        std::cout<<"===> "<<std::endl;
        std::cout<<"vdc    ::  ";
        for(int i =0 ; i < fvdcXNum; i ++){
            std::cout<<" ("<<fvdc_th[i]<<",  "<<fvdc_ph[i]<<"),   ";
        }
        std::cout<<std::endl;

        // load the GEM detectors
        std::cout<<"===> "<<std::endl;
        std::cout<<"GEM    ::  theta-> ";
        for(int i = 0 ; i < NGEMTracktheta; i ++){
            std::cout<<"  "<< GEMTracktheta[i]<<",  ";
        }
        std::cout<<std::endl;

        std::cout<<"       ::  phi-> ";

        for (int i = 0 ; i < NGEMTrackphi; i++){
            std::cout<<"  "<<GEMTrackphi[i]<<",   ";
        }
        //if(DetHitArrX.size()<=4)
        if (! save2pdf)getchar();

        DetHist2DYZ->Delete();
        DetHist2DXZ->Delete();
        DetCoordPosXZ->Delete();
        DetCoordPosYZ->Delete();
    }
    eventCanvas->Print(Form("EvtDisp_%d.pdf)",runID),"pdf");

    FILE *trackXYZ=fopen("trackxyz.txt","w");
    // write the data to file
    for (auto Event : DetEventBuff){
        std::string line;
        for(auto Hit : Event){
            if(Hit.GetDetectorID()==0){
                line.append(Form("%d, %f, %f, %f, %f, %f, ",Hit.GetDetectorID(),Hit.GetTheta(),Hit.GetPhi(), Hit.GetX(),Hit.GetY(),Hit.GetZ() ));
            }else{
                line.append(Form("%d, %f, %f, %f,",Hit.GetDetectorID(), Hit.GetX(),Hit.GetY(),Hit.GetZ() ));
            }
        }
        line.append("\n");
        fprintf(trackXYZ,"%s",line.c_str());
        line.clear();
    }
    fclose(trackXYZ);

}

/// add the code for the                                     // RGEM or left GEM
void rootReader(TString fname="test_20532.root", std::string HRSarm="RGEM.rgems"){

    TChain *chain = new TChain("T");
    chain->Add(fname.Data());
    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }


	TCanvas *eventCanvas=new TCanvas("CanvasDisplay","CanvasDisplay",1000,1000);
	eventCanvas->Divide(1,2);
    eventCanvas->cd(1)->Divide(3,1);
	eventCanvas->cd(2)->Divide(3,1);
	TLine *detPlaneline[6];

	if(fname.IsNull()){
		std::cout<<"Please input the file name"<<std::endl;
	}

	// read the left HRS or the right HRS
	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	TTree *PRex_GEM_tree;
	fileio->GetObject("T",PRex_GEM_tree);
	if(PRex_GEM_tree->IsZombie()){
		std::cout<<"[Error]: can not find tree in the file !!!"<<std::endl;
	}else{
		std::cout<<"Total Entries in the file:"<< (PRex_GEM_tree->GetEntries())<<std::endl;
	}

	// check the detector are listed in the tree
	std::vector<int16_t> chamberList;
	std::map<int16_t,std::vector<int16_t>> TsampleLit;

	std::cout<<"List of Chambers:"<<std::endl;
	for(int16_t chambercount=0; chambercount<=_ROOTREADER_MAX_GEM_CHAMBER;chambercount++){
		if(PRex_GEM_tree->GetListOfBranches()->Contains(Form("%s.x%d.adc1",HRSarm.c_str(),chambercount))){
			std::cout<<"	->"<< chambercount<<std::endl;
			chamberList.push_back(chambercount);
		}
	}

	// initialize the buffers
	for (auto chamberID : chamberList){

		std::cout<<"Reading chamber :"<<chamberID<<"\n	sample:";
		//std::vector<int16_t> TsampleLit;
		for (int adc_sample =0; adc_sample <_ROOTREADER_MAX_TSAMPLE; adc_sample++){
			std::string getstring;
			if(PRex_GEM_tree->GetListOfBranches()->Contains(Form("%s.x%d.adc%d",HRSarm.c_str(),chamberID,adc_sample))){
				std::cout<<adc_sample<<"  ";
				TsampleLit[chamberID].push_back(adc_sample);
			}
		}
		std::cout<<std::endl;
	}

	std::cout<<"******************  Reading Done  *********************"<<std::endl;

	// loop on the chamber and read out the data for the position

	std::string HRSarmTag="R";
	if(HRSarm=="LGEM.lgems"){
		HRSarmTag="L";
	}

	//----------------------------------------------------------------------------
	// search for the vdcs
	Int_t fEvtNum=0;
	Int_t fRun=0;
	Int_t fvdcXNum=0;
	double_t fvdcX[100];
	Int_t fvdcYNum=0;
	double_t fvdcY[100];
	double_t fvdc_th[10];
	double_t fvdc_ph[10];

	std::string fEvtNumForm("fEvtHdr.fEvtNum");
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fEvtNumForm.c_str())){
		PRex_GEM_tree->SetBranchAddress(fEvtNumForm.c_str(),&fEvtNum);
	}else{
		std::cout<<"[Warning]:: fEvtNum data did not find in the replay resuly, skip it"<<std::endl;
	}
	std::string fRunForm("Event_Branch/fEvtHdr/fEvtHdr.fRun");
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fRunForm.c_str())){
		PRex_GEM_tree->SetBranchAddress(fRunForm.c_str(),&fRun);
	}else{
		std::cout<<"[Warning]:: fRun data did not find in the replay resuly, skip it"<<std::endl;
	}

	std::string fvdcXNumForm(Form("Ndata.R.tr.x"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fvdcXNumForm.c_str())){
		PRex_GEM_tree->SetBranchAddress(fvdcXNumForm.c_str(),&fvdcXNum);
	}else{
		std::cout<<"[Warning]:: VDC data did not find in the replay resuly"<<std::endl;
	}

	std::string fvdcXForm(Form("R.tr.x"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fvdcXForm.c_str())){
		PRex_GEM_tree->SetBranchAddress(fvdcXForm.c_str(),fvdcX);
	}else{
		std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
	}

	std::string fvdc_th_Form(Form("R.tr.th"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fvdc_th_Form.c_str())){
		PRex_GEM_tree->SetBranchAddress(fvdc_th_Form.c_str(),fvdc_th);
	}else{
		std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
	}

	std::string fvdc_ph_Form(Form("R.tr.ph"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fvdc_ph_Form.c_str())){
		PRex_GEM_tree->SetBranchAddress(fvdc_ph_Form.c_str(),fvdc_ph);
	}else{
		std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
	}

	std::string fvdcYNumForm(Form("Ndata.R.tr.y"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fvdcYNumForm.c_str())){
		PRex_GEM_tree->SetBranchAddress(fvdcYNumForm.c_str(),&fvdcYNum);
	}else{
		std::cout<<"[Warning]:: VDC data did not find in the replay resuly"<<std::endl;
	}

	std::string fvdcYForm(Form("R.tr.y"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(fvdcYForm.c_str())){
		PRex_GEM_tree->SetBranchAddress(fvdcYForm.c_str(),fvdcY);
	}else{
		std::cout<<"[Warning]:: VDC data did not find in the replay result"<<std::endl;
	}

	// get the detector position from the root file

	// load the data from  root file

	// load the Z position
    double_t DetectorZpos[7]={0.0,  0.9, 1.5982485,  1.8558464, 2.3839658, 2.5141378, 2.6395974};
    auto databasePos= ReadDatabase(HRSarmTag.c_str(),DBLoadFolder.Data());
    for (int i = 1; i <= 6; ++i) {
        if (databasePos.find(i)!=databasePos.end()){
            DetectorZpos[i] = databasePos[i].Z();
        }
    }

	for (int chamberID=0 ; chamberID<=6 ; chamberID++){
		if(chamberID <=3){
			detPlaneline[chamberID]= new TLine(-0.1,DetectorZpos[chamberID],0.1,DetectorZpos[chamberID]);
			//detPlaneline[chamberID]->Draw("same");
		}else{
			detPlaneline[chamberID]= new TLine(-0.3,DetectorZpos[chamberID],0.3,DetectorZpos[chamberID]);
			//detPlaneline[chamberID]->Draw("same");
		}
	}

	//
	std::map<Int_t,Int_t> GEMNDetX;
	std::map<Int_t,Int_t> GEMNDetY;
	std::map<Int_t,double_t  *> GEMDetX;
	std::map<Int_t,double_t  *> GEMDetY;


	// read out the coord parameters
	//  coord.pos parameters
	std::map<Int_t, Int_t> GEM_NCoordPosX;
	std::map<Int_t, Int_t> GEM_NCoordPosY;
	std::map<Int_t, double *>	GEM_CoordPosX;
	std::map<Int_t, double *>	GEM_CoordPosY;

	//trkPos
	std::map<Int_t, Int_t> GEM_NCoordTrackPosX;
	std::map<Int_t, Int_t> GEM_NCoordTrackPosY;
	std::map<Int_t, double *>	GEM_CoordTrackPosX;
	std::map<Int_t, double *>	GEM_CoordTrackPosY;

	// load the GEM th-ph result

	Int_t NGEMTracktheta=0;
	Int_t NGEMTrackphi=0;
	double	GEMTracktheta[50];
	double  GEMTrackphi[50];

	// load the data for the theta-phi for GEM detectors
	std::string NGEMTracktheta_str(Form("Ndata.RGEM.tr.th"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(NGEMTracktheta_str.c_str())){
		PRex_GEM_tree->SetBranchAddress(NGEMTracktheta_str.c_str(), &NGEMTracktheta);
	}
	std::string GEMTracktheta_str(Form("RGEM.tr.th"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(GEMTracktheta_str.c_str())){
		PRex_GEM_tree->SetBranchAddress(GEMTracktheta_str.c_str(), GEMTracktheta);
	}

	std::string NGEMTrackphi_str(Form("Ndata.RGEM.tr.ph"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(NGEMTrackphi_str.c_str())){
		PRex_GEM_tree->SetBranchAddress(NGEMTrackphi_str.c_str(),&NGEMTrackphi);
	}
	std::string GEMTrackphi_str(Form("RGEM.tr.ph"));
	if(PRex_GEM_tree->GetListOfBranches()->Contains(GEMTrackphi_str.c_str())){
		PRex_GEM_tree->SetBranchAddress(GEMTrackphi_str.c_str(), GEMTrackphi);
	}

	// loop on the chamber list
	for (auto chamberID : chamberList){
		std::cout<<chamberID<<std::endl;

		double_t detX[20];
		double_t detY[20];
		Int_t NdetX=0;
		Int_t NdetY=0;
		double_t detZ=DetectorZpos[chamberID];

		// load the branch data
		// load the number of the Size X
		std::string NdetX_str(Form("Ndata.RGEM.rgems.x%d.hit.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(NdetX_str.c_str())){
				PRex_GEM_tree->SetBranchAddress(NdetX_str.c_str(),&GEMNDetX[chamberID]);
		}else{
				std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
			}

		// load the data
		GEMDetX[chamberID]=new double_t [100];
		std::string detX_str(Form("RGEM.rgems.x%d.hit.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(detX_str.c_str())){
				PRex_GEM_tree->SetBranchAddress(detX_str.c_str(),GEMDetX[chamberID]);
		}else{
				std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
			}

		// load the Y dimension
		std::string NdetY_str(Form("Ndata.RGEM.rgems.y%d.hit.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(NdetY_str.c_str())){
				PRex_GEM_tree->SetBranchAddress(NdetY_str.c_str(),&GEMNDetY[chamberID]);
		}else{
				std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
			}
		GEMDetY[chamberID]=new double_t [100];
		std::string detY_str(Form("RGEM.rgems.y%d.hit.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(detY_str.c_str())){
				PRex_GEM_tree->SetBranchAddress(detY_str.c_str(),GEMDetY[chamberID]);
		}else{
				std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
			}

		//finish load the data

		// start load the coordi parameter in the root file

		std::string NCoordPosX_str(Form("Ndata.RGEM.rgems.x%d.coord.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(NCoordPosX_str.c_str())){
			PRex_GEM_tree->SetBranchAddress(NCoordPosX_str.c_str(), &GEM_NCoordPosX[chamberID]);
		}else{
			std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
		}
		GEM_CoordPosX[chamberID]=new double_t [100];
		std::string CoordPosX_str(Form("RGEM.rgems.x%d.coord.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(CoordPosX_str.c_str())){
			PRex_GEM_tree->SetBranchAddress(CoordPosX_str.c_str(), GEM_CoordPosX[chamberID]);
		}else{
			std::cout<<"[Warning]:: GEM_CoordPosX data did not find in the replay resuly, skip it"<<std::endl;
		}

		std::string NCoordPosY_str(Form("Ndata.RGEM.rgems.y%d.coord.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(NCoordPosY_str.c_str())){
				PRex_GEM_tree->SetBranchAddress(NCoordPosY_str.c_str(), &GEM_NCoordPosY[chamberID]);
		}else{
				std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
		}

		GEM_CoordPosY[chamberID]=new double_t [100];
		std::string CoordPosY_str(Form("RGEM.rgems.y%d.coord.pos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(CoordPosY_str.c_str())){
			PRex_GEM_tree->SetBranchAddress(CoordPosY_str.c_str(), GEM_CoordPosY[chamberID]);
		}else{
			std::cout<<"[Warning]:: GEM_CoordPosY data did not find in the replay resuly, skip it"<<std::endl;
		}


		// start load the coordi parameter in the root file
		std::string NCoordTrackPosX_str(Form("Ndata.RGEM.rgems.x%d.coord.trkpos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(NCoordTrackPosX_str.c_str())){
			PRex_GEM_tree->SetBranchAddress(NCoordTrackPosX_str.c_str(), &GEM_NCoordTrackPosX[chamberID]);
		}else{
			std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it "<<__LINE__<<std::endl;
		}

		GEM_CoordTrackPosX[chamberID]=new double_t [100];
		std::string CoordTrackPosX_str(Form("RGEM.rgems.x%d.coord.trkpos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(CoordTrackPosX_str.c_str())){
			PRex_GEM_tree->SetBranchAddress(CoordTrackPosX_str.c_str(), GEM_CoordTrackPosX[chamberID]);
		}else{
			std::cout<<"[Warning]:: GEM_CoordPosX data did not find in the replay resuly, skip it"<<std::endl;
		}

		std::string NCoordTrackPosY_str(Form("Ndata.RGEM.rgems.y%d.coord.trkpos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(NCoordTrackPosY_str.c_str())){
				PRex_GEM_tree->SetBranchAddress(NCoordTrackPosY_str.c_str(), &GEM_NCoordTrackPosY[chamberID]);
		}else{
				std::cout<<"[Warning]:: NdetX data did not find in the replay resuly, skip it"<<std::endl;
		}

		GEM_CoordTrackPosY[chamberID]=new double_t [100];
		std::string CoordTrackPosY_str(Form("RGEM.rgems.y%d.coord.trkpos",chamberID));
		if(PRex_GEM_tree->GetListOfBranches()->Contains(CoordTrackPosY_str.c_str())){
			PRex_GEM_tree->SetBranchAddress(CoordTrackPosY_str.c_str(), GEM_CoordTrackPosY[chamberID]);
		}else{
			std::cout<<"[Warning]:: GEM_CoordPosY data did not find in the replay resuly, skip it"<<std::endl;
		}

		// load the theta-phi result
	}

	// loop on the data
    std::vector<std::vector<HitStruct>> DetEventBuff;
	std::vector<HitStruct> DetHitArr;
	std::vector<HitStruct> DetHitArrX;
	std::vector<HitStruct> DetHitArrY;
	std::cout<<"Total Entries:"<<PRex_GEM_tree->GetEntries()<<std::endl;

	TH2F *DetHist2DXZ;//= new TH2F("XZ","XZ",2000,-0.4,0.4,1000,-0,3.0);
	TH2F *DetHist2DYZ;
	TH2F *DetHist2DXZCorr;//= new TH2F("XZ","XZ",2000,-0.4,0.4,1000,-0,3.0);
	TH2F *DetHist2DYZCorr;

	//used for plot the coord positions
	TH2F *DetCoordPosXZ;
	TH2F *DetCoordPosYZ;
	TH2F *DetCoordTrackPosXZ;
	TH2F *DetCoordTrackPosYZ;


	for(auto entry=1;entry<(PRex_GEM_tree->GetEntries()) && entry<2000;entry++)
	{

		PRex_GEM_tree->GetEntry(entry);
		//PRex_GEM_tree->GetEntry(1);
		DetHitArr.clear();
		DetHitArrX.clear();
		DetHitArrY.clear();

		// load the data to data buff
		for (auto chamberID : chamberList){
			if (GEMNDetX.find(chamberID) != GEMNDetX.end()) {
				for (int i = 0; i < GEMNDetX[chamberID]; i++) {
					HitStruct hittemp(chamberID, GEMDetX[chamberID][i], 0.0,
							DetectorZpos[chamberID]);
					DetHitArrX.push_back(hittemp);
				}
			}

			// load the Y dimension
			if (GEMNDetY.find(chamberID) != GEMNDetY.end()) {
				for (int i = 0; i < GEMNDetY[chamberID]; i++) {
					double_t y = GEMDetY[chamberID][i];
//					if(chamberID ==6) y+=0.05;
					HitStruct hittemp(chamberID, 0.0, y,
							DetectorZpos[chamberID]);
					DetHitArrY.push_back(hittemp);
				}
			}

		}
		// write all the good event to the buff
		// check the VDC
		Bool_t goodHitFlag = kTRUE;
		if((fvdcYNum==1)&&(fvdcXNum==1)){
			HitStruct hit(0,fvdcX[0],fvdcY[0],0.0,fvdc_th[0],fvdc_ph[0]);
			DetHitArr.push_back(hit);
		}else{
			goodHitFlag = kFALSE;
		}


		for (auto chamberID : chamberList) {
			if ((GEMNDetX.find(chamberID) != GEMNDetX.end())
					&& (GEMNDetY.find(chamberID) != GEMNDetY.end())) {
				if ((GEMNDetX[chamberID] == 1)
						&& (GEMNDetY[chamberID] == 1)) {

				} else {
					goodHitFlag = kFALSE;
				}
			} else {
				goodHitFlag = kFALSE;
			}
		}
		if(goodHitFlag){
			for (auto chamberID : chamberList) {
				HitStruct hit(chamberID,GEMDetX[chamberID][0],GEMDetY[chamberID][0],DetectorZpos[chamberID]);
				DetHitArr.push_back(hit);
			}
		}
		if(goodHitFlag){
			DetEventBuff.push_back(DetHitArr);
		}



		for (auto hit : DetHitArrX) hit.Print();
		for (auto hit : DetHitArrY) hit.Print();


		// finish load the data
		// load the data for display
		double_t CorrectionMatrix[]={
		0.0,
		0.0,
		0.0,

		0.00,
		0.00,
		0.00,

		0.00,
		0.00,
		0.00,

		0.00,
		0.00,
		0.00,

		0.00,
		0.00,
		0.00,

		0.00,
		0.00,
		0.00};


		DetHist2DXZ= new TH2F(Form("XZ_Evt%d",entry),Form("XZ_Evt%d",entry),2000,-0.4,0.4,1000,-0,3.0);
		DetHist2DYZ= new TH2F(Form("YZ_Evt%d",entry),Form("YZ_Evt%d",entry),2000,-0.4,0.4,1000,-0,3.0);

		DetHist2DXZCorr= new TH2F(Form("XZ_CEvt%d",entry),Form("XZ_CEvt%d",entry),2000,-0.4,0.4,1000,-0,3.0);
		DetHist2DYZCorr= new TH2F(Form("YZ_CEvt%d",entry),Form("YZ_CEvt%d",entry),2000,-0.4,0.4,1000,-0,3.0);

		DetHist2DXZ->SetMarkerSize(1);
		DetHist2DXZ->SetMarkerColor(2);
		DetHist2DXZ->SetMarkerStyle(20);

		DetHist2DYZ->SetMarkerSize(1);
		DetHist2DYZ->SetMarkerColor(2);
		DetHist2DYZ->SetMarkerStyle(20);


		DetHist2DXZCorr->SetMarkerSize(1);
		DetHist2DXZCorr->SetMarkerColor(3);
		DetHist2DXZCorr->SetMarkerStyle(20);

		DetHist2DYZCorr->SetMarkerSize(1);
		DetHist2DYZCorr->SetMarkerColor(3);
		DetHist2DYZCorr->SetMarkerStyle(20);


		std::cout<<"--------------Hit Pos----------------"<<std::endl;
		std::cout<<"====> Hit Position "<<std::endl;
		for(auto Hit : DetHitArrX){
			DetHist2DXZ->Fill(Hit.GetX(),Hit.GetZ());
			DetHist2DXZCorr->Fill(Hit.GetX()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)],Hit.GetZ()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)+2]);
			Hit.Print();
		}

		for (auto Hit : DetHitArrY){
			DetHist2DYZ->Fill(Hit.GetY(),Hit.GetZ());
			DetHist2DYZCorr->Fill(Hit.GetY()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)+1],Hit.GetZ()+CorrectionMatrix[3*(Hit.GetDetectorID()-1)+2]);
		}

		// plot the data
		DetCoordPosXZ= new TH2F(Form("XZ_Pos%d",entry),Form("XZ_Pos%d",entry),2000,-0.4,0.4,1000,-0,3.0);
		DetCoordPosYZ= new TH2F(Form("YZ_Pos%d",entry),Form("YZ_Pos%d",entry),2000,-0.4,0.4,1000,-0,3.0);

		DetCoordTrackPosXZ= new TH2F(Form("XZ_CTRackPos%d",entry),Form("XZ_CTrackPos%d",entry),2000,-0.4,0.4,1000,-0,3.0);
		DetCoordTrackPosYZ= new TH2F(Form("YZ_CTrackPos%d",entry),Form("YZ_CTrackPos%d",entry),2000,-0.4,0.4,1000,-0,3.0);


		DetCoordPosXZ->SetMarkerSize(1);
		DetCoordPosXZ->SetMarkerColor(4);
		DetCoordPosXZ->SetMarkerStyle(20);

		DetCoordPosYZ->SetMarkerSize(1);
		DetCoordPosYZ->SetMarkerColor(4);
		DetCoordPosYZ->SetMarkerStyle(20);


		DetCoordTrackPosXZ->SetMarkerSize(1);
		DetCoordTrackPosXZ->SetMarkerColor(4);
		DetCoordTrackPosXZ->SetMarkerStyle(20);

        DetCoordTrackPosYZ->SetMarkerSize(1);
		DetCoordTrackPosYZ->SetMarkerColor(4);
		DetCoordTrackPosYZ->SetMarkerStyle(20);

		std::cout<<"--------------Coord Pos----------------"<<std::endl;
		for (auto chamberID : chamberList){

			std::cout<<"====>"<<chamberID<<std::endl;
			if((GEM_NCoordPosX.find(chamberID)!=GEM_NCoordPosX.end())){
				for(int i = 0; i < GEM_NCoordPosX[chamberID]; i++){

				    std::cout<< i<<std::endl;
					DetCoordPosXZ->Fill(GEM_CoordPosX[chamberID][i],DetectorZpos[chamberID]);
					std::cout<<"  xzPos: ("<<GEM_CoordPosX[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
				}
			}
			if((GEM_NCoordPosY.find(chamberID)!=GEM_NCoordPosY.end())){
				for(int i = 0; i < GEM_NCoordPosY[chamberID]; i++){
					DetCoordPosYZ->Fill(GEM_CoordPosY[chamberID][i],DetectorZpos[chamberID]);
					std::cout<<"  yzPos: ("<<GEM_CoordPosY[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
				}
			}

			if((GEM_NCoordTrackPosX.find(chamberID)!=GEM_NCoordTrackPosX.end())){
				for(int i = 0; i < GEM_NCoordTrackPosX[chamberID]; i++){
					DetCoordTrackPosXZ->Fill(GEM_CoordTrackPosX[chamberID][i],DetectorZpos[chamberID]);
					std::cout<<"  xzTrackPos: ("<<GEM_CoordTrackPosX[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
				}
			}
			if((GEM_NCoordTrackPosY.find(chamberID)!=GEM_NCoordTrackPosY.end())){
				for(int i = 0; i < GEM_NCoordTrackPosY[chamberID]; i++){
					DetCoordTrackPosYZ->Fill(GEM_CoordTrackPosY[chamberID][i],DetectorZpos[chamberID]);
					std::cout<<"  yzTrackPos: ("<<GEM_CoordTrackPosY[chamberID][i]<<",  "<<DetectorZpos[chamberID]<<");   ";
				}
			}
		std::cout<<std::endl;
		}



		eventCanvas->cd(1);


		eventCanvas->cd(1)->cd(1);
		DetHist2DXZ->Draw();
//		{
//			TLine *yztrack=new //TLine(DetHitArrX.front().GetX(),DetHitArrX.front().GetZ(),DetHitArrX.back().GetX(),DetHitArrX.back().GetZ());
//			yztrack->Draw("same");
//		}

		// draw the plane pannel
		for(int i = 0 ; i <=6 ; i++){
			detPlaneline[i]->Draw("same");
		}


		eventCanvas->cd(1)->cd(2);
		DetHist2DXZ->Draw();
		DetCoordPosXZ->Draw("same");
		for(int i = 0 ; i <=6 ; i++){
			detPlaneline[i]->Draw("same");
		}

        eventCanvas->cd(1)->cd(3);
		DetHist2DXZ->Draw();
		DetCoordTrackPosXZ->Draw("same");
		for(int i = 0 ; i <=6 ; i++){
			detPlaneline[i]->Draw("same");
		}

		{
			TLine *xztrack=new TLine(fvdcX[0],0.0,fvdcX[0]+fvdc_th[0]*2.7,2.7);
			xztrack->Draw("same");
		}

		eventCanvas->cd(2)->cd(1);
		DetHist2DYZ->Draw();
//		{
//			TLine *yztrack=new TLine(fvdcY[0],0.0,fvdcY[0]+fvdc_ph[0]*2.7,2.7);
//			yztrack->Draw("same");
//		}
		for(int i = 0 ; i <=6 ; i++){
			detPlaneline[i]->Draw("same");
		}

		eventCanvas->cd(2)->cd(2);
		DetHist2DYZ->Draw();
		DetCoordPosYZ->Draw("same");
		for(int i = 0 ; i <=6 ; i++){
			detPlaneline[i]->Draw("same");
		}

		eventCanvas->cd(2)->cd(3);
		DetHist2DYZ->Draw();
        DetCoordTrackPosYZ->Draw("same");
		for(int i = 0 ; i <=6 ; i++){
			detPlaneline[i]->Draw("same");
		}
		
		{
			TLine *yztrack=new TLine(fvdcY[0],0.0,fvdcY[0]+fvdc_ph[0]*2.7,2.7);
			yztrack->Draw("same");
		}

		eventCanvas->Update();
		//if(goodHitFlag)


		//-------------------------------------
		std::cout<<"------------ Located the reconstructed angles -------------------"<<std::endl;
		std::cout<<"===> "<<std::endl;
		std::cout<<"vdc    ::  ";
		for(int i =0 ; i < fvdcXNum; i ++){
			std::cout<<" ("<<fvdc_th[i]<<",  "<<fvdc_ph[i]<<"),   ";
		}
		std::cout<<std::endl;

		// load the GEM detectors
        std::cout<<"===> "<<std::endl;
		std::cout<<"GEM    ::  theta-> ";
		for(int i = 0 ; i < NGEMTracktheta; i ++){
			std::cout<<"  "<< GEMTracktheta[i]<<",  ";
		}
		std::cout<<std::endl;

		std::cout<<"       ::  phi-> ";

		for (int i = 0 ; i < NGEMTrackphi; i++){
			std::cout<<"  "<<GEMTrackphi[i]<<",   ";
		}
		std::cout<<std::endl;
		//if(DetHitArrX.size()<=4)
//		eventCanvas->SaveAs(Form("result/HitEvt%d.jpg",entry));
		getchar();

		DetHist2DYZ->Delete();
		DetHist2DXZ->Delete();
		DetCoordPosXZ->Delete();
		DetCoordPosYZ->Delete();
	}
	FILE *trackXYZ=fopen("trackxyz.txt","w");
	// write the data to file
	for (auto Event : DetEventBuff){
		std::string line;
		for(auto Hit : Event){
			if(Hit.GetDetectorID()==0){
				line.append(Form("%d, %f, %f, %f, %f, %f, ",Hit.GetDetectorID(),Hit.GetTheta(),Hit.GetPhi(), Hit.GetX(),Hit.GetY(),Hit.GetZ() ));
			}else{
				line.append(Form("%d, %f, %f, %f,",Hit.GetDetectorID(), Hit.GetX(),Hit.GetY(),Hit.GetZ() ));
			}
		}
		line.append("\n");
		fprintf(trackXYZ,"%s",line.c_str());
		line.clear();
	}
	fclose(trackXYZ);
}

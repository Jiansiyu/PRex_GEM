/**
 * User for generate GEM detector Performance Plot
 *
 * author : Siyu Jian
 * email  : jiansiyu@gmail.com
 *
 **/

#include <string>
#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <vector>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <map>
#include <TPaveText.h>
#include <TStyle.h>
#include "TChain.h"
#include "TLatex.h"

TTree *tree;
// check the existance of the branch and attach to the data
// the data size need to initilize before load to the data
template <class T>
Bool_t AttachBranch(std::string branchName, TTree *tree, T data) {

	if(tree->GetListOfBranches()->Contains(branchName.c_str())){
		tree->SetBranchAddress(branchName.c_str(), &data);
		return true;
	}else{
		std::cout<<"[WORNING]:: Can NOT find \'"<<branchName.c_str()<<" in "<< tree->GetName()<<std::endl;
		return false;
	}
}





Bool_t IsBranchExist(std::string branchName){
	return tree->GetListOfBranches()->Contains(branchName.c_str());
}

void LoadFiles(std::vector<TString> fnameList){

	if(fnameList.size()==0){
		std::cout<<"[ERROR]:: the input file name is empty"<<std::endl;
		exit(0);
	}



}

// load the root file
void gemPerformance(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_21289_00_test.root") {

	if (fname.IsNull()) {
		std::cout<<"Please input the file name"<<std::endl;
	}

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	fileio->GetObject("T",tree);
	fileio->Print();


}

std::vector<Int_t> gemGetDetectorList(TString fname){
	//
	//
	std::vector<Int_t> DetList;
	DetList.push_back(1);
	DetList.push_back(2);
	DetList.push_back(3);
	DetList.push_back(4);
	DetList.push_back(5);
	DetList.push_back(6);
	return DetList;
}

void gemNoiseSignalLevel(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_20862_00_test.root",std::string HRS="RHRS"){
	if (fname.IsNull()) {
		std::cout<<"Please input the file name"<<std::endl;
	}

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	fileio->GetObject("T",tree);
	fileio->Print();

	// read out the noise and signal level
	std::string noisePattern_x;
	std::string noisePattern_y;
	std::string signalPattern_x;
	std::string signalPattern_y;

	// check which HRS it is

	if (HRS == "RHRS"){
		noisePattern_x="RGEM.rgems.x%d.strip.adcnoise";
		noisePattern_y="RGEM.rgems.y%d.strip.adcnoise";
		signalPattern_x="RGEM.rgems.x%d.strip.adc";
		signalPattern_y="RGEM.rgems.y%d.strip.adc";

	}else if ((HRS == "LHRS")) {
		noisePattern_x="LGEM.lgems.x%d.strip.adcnoise";
		noisePattern_y="LGEM.lgems.y%d.strip.adcnoise";
		signalPattern_x="LGEM.lgems.x%d.strip.adc";
		signalPattern_y="LGEM.lgems.y%d.strip.adc";
	}else{
		std::cout<<"[ERROR]:: only take RHRS/LHRS"<<std::endl;
	    exit(-1);
	}


	TCanvas *canvas_x=new TCanvas("x_dist","x_dist",1000,1000);
	TCanvas *canvas_y=new TCanvas("y_dist","y_dist",1000,1000);
	canvas_x->SetLogy();   //set the log scale for the
	if(gemGetDetectorList(fname).size()<=3){
		canvas_x->Divide(1,gemGetDetectorList(fname).size());
	}else{
		int8_t split_H=(int8_t) std::sqrt(gemGetDetectorList(fname).size())+1;
		int8_t split_V=(int8_t) std::sqrt(gemGetDetectorList(fname).size());
		canvas_x->Divide(split_H,split_V);
	}
	canvas_y->SetLogy();   //set the log scale for the
	if(gemGetDetectorList(fname).size()<=3){
		canvas_y->Divide(1,gemGetDetectorList(fname).size());
	}else{
		int8_t split_H=(int8_t) std::sqrt(gemGetDetectorList(fname).size())+1;
		int8_t split_V=(int8_t) std::sqrt(gemGetDetectorList(fname).size());
		canvas_y->Divide(split_H,split_V);
	}

	// generate the histogram
	std::map<Int_t,TH1F *> noiseHist_x;
	std::map<Int_t,TH1F *> noiseHist_y;
	std::map<Int_t,TH1F *> signalHist_x;
	std::map<Int_t,TH1F *> signalHist_y;

	for (auto ChamberID : gemGetDetectorList(fname)){
		if(noiseHist_x.find(ChamberID)==noiseHist_x.end()){
			noiseHist_x[ChamberID]=new TH1F(Form("%s_Chamber%d_X",HRS.c_str(),ChamberID),Form("%s_Chamber%d_X",HRS.c_str(),ChamberID),2000,-1000,4000);
		}
		if(noiseHist_y.find(ChamberID)==noiseHist_y.end()){
			noiseHist_y[ChamberID]=new TH1F(Form("%s_Chamber%d_Y",HRS.c_str(),ChamberID),Form("%s_Chamber%d_Y",HRS.c_str(),ChamberID),2000,-1000,4000);
		}

		if(signalHist_x.find(ChamberID)==signalHist_x.end()){
			signalHist_x[ChamberID]=new TH1F(Form("%s_Chamber%d_sigLevel_X",HRS.c_str(),ChamberID),Form("%s_Chamber%d_sigLevel_X",HRS.c_str(),ChamberID),2000,-1000,4000);
			signalHist_x[ChamberID]->SetLineColor(2);
		}
		if(signalHist_y.find(ChamberID)==signalHist_y.end()){
			signalHist_y[ChamberID]=new TH1F(Form("%s_Chamber%d_sigLevel_Y",HRS.c_str(),ChamberID),Form("%s_Chamber%d_sigLevel_Y",HRS.c_str(),ChamberID),2000,-1000,4000);
			signalHist_y[ChamberID]->SetLineColor(2);
		}

		//  plot the X-dimension
		if(IsBranchExist(Form(noisePattern_x.c_str(),ChamberID))){
			std::cout<<(noiseHist_x[ChamberID]->GetName())<<std::endl;
			tree->Project((noiseHist_x[ChamberID]->GetName()),Form(noisePattern_x.c_str(),ChamberID));
			std::string cut=std::string(Form(signalPattern_x.c_str(),ChamberID))+" !=0";
			tree->Project((signalHist_x[ChamberID]->GetName()),Form(signalPattern_x.c_str(),ChamberID),cut.c_str());
			canvas_x->cd(ChamberID);
			canvas_x->cd(ChamberID)->SetLogy();
			noiseHist_x[ChamberID]->SetXTitle("ADC(Tsample Average)");
			noiseHist_x[ChamberID]->SetYTitle("Count");

			noiseHist_x[ChamberID]->Draw();
			signalHist_x[ChamberID]->Draw("][same");
		}
		//  plot the Y-dimension
		if(IsBranchExist(Form(noisePattern_y.c_str(),ChamberID))){
			std::cout<<(noiseHist_y[ChamberID]->GetName())<<std::endl;
			tree->Project(noiseHist_y[ChamberID]->GetName(),Form(noisePattern_y.c_str(),ChamberID));
			std::string cut=std::string(Form(signalPattern_y.c_str(),ChamberID))+" !=0";
			tree->Project(signalHist_y[ChamberID]->GetName(),Form(signalPattern_y.c_str(),ChamberID),cut.c_str());
			canvas_y->cd(ChamberID);
			canvas_y->cd(ChamberID)->SetLogy();
			noiseHist_y[ChamberID]->SetXTitle("ADC(Tsample Average)");
			noiseHist_y[ChamberID]->SetYTitle("Count");
			noiseHist_y[ChamberID]->Draw();
			signalHist_y[ChamberID]->Draw("][same");
		}
	}

}



//-------------------------------------------------------
// Load the Pedestal Root file, and generate the Pedestal distribution for all the APVs
//UVa Pedestal Format
//
//
//-------------------------------------------------------
void gemPedestal(TString fname="/home/newdriver/Storage/Research/PRex_Experiment/PRex_Script/PRex_pedestal_generator/Pedestal/pedestal_RHRS_20512.root"){

	std::string pedestalMeanPattern("PedestalMean(offset)_mpd_%d_ch_%d");
	std::string pedestalRMSPattern("PedestalRMS_mpd_%d_ch_%d");

	// load the pedestal and attach to branch
	Int_t mpdMin=0;
	Int_t mpdMax=32;
	// buffers the list of MPDs
	std::map<Int_t,std::vector<Int_t>> mpdAPVList;

	std::map<Int_t,std::map<Int_t,TH1F *>> pedestalHist;
	std::map<Int_t,std::map<Int_t,TH1F *>> rmsHist;

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);

	// check MPD  APV list
	for(Int_t mpdID=mpdMin; mpdID<=mpdMax; mpdID++){
		std::vector<Int_t> apvList;
		apvList.clear();
		for(Int_t apvID=0; apvID<32;apvID++){
			if(fileio->GetListOfKeys()->Contains(Form(pedestalMeanPattern.c_str(),mpdID,apvID))){
				apvList.push_back(apvID);
			}
		}
		if(apvList.size()!=0){
			mpdAPVList[mpdID]=apvList;
		}
	}

	for(auto mpdIter=mpdAPVList.begin();mpdIter!=mpdAPVList.end();mpdIter++){
		std::cout<<"|========= > MPD "<< (mpdIter->first)<<" (APV "<<(mpdIter->second.size())<<")"<<std::endl;
		std::cout<<"	(";
		for(auto apvID : mpdIter->second){
			std::cout<<" "<<apvID<<" ";
		}
		std::cout<<")"<<std::endl<<std::endl;
	}

	// load the pedestal data

	for(auto mpdIter=mpdAPVList.begin();mpdIter!=mpdAPVList.end();mpdIter++){
		Int_t mpdID=mpdIter->first;
		for(auto apvID : mpdIter->second){
			pedestalHist[mpdID][apvID]=(TH1F *) fileio->Get(Form(pedestalMeanPattern.c_str(),mpdID,apvID));
			rmsHist[mpdID][apvID]=(TH1F *) fileio->Get(Form(pedestalRMSPattern.c_str(),mpdID,apvID));
		}
	}

	// global APV Pedestal Distribution
	TH1F *PedestalMeanDist= new TH1F ("Pedestal_Mean","Pedestal_Mean",200,-100,100);
	TH1F *PedestalRMSDist= new TH1F ("Pedestal_rms","Pedestal_rms",200,-100,100);

	for(auto mpdIter=mpdAPVList.begin();mpdIter!=mpdAPVList.end();mpdIter++){
			Int_t mpdID=mpdIter->first;
			for(auto apvID : mpdIter->second){
				for(int i =1 ; i<=128; i ++ ){
					PedestalMeanDist->Fill(pedestalHist[mpdID][apvID]->GetBinContent(i));
					PedestalRMSDist->Fill(rmsHist[mpdID][apvID]->GetBinContent(i));
				}
			}
		}

	TH2F *PedestalRMS2DDist=new TH2F("RMS distribution","RMS distribution",90,0,90,200,0,200);
	TH1F *PedestalRMSAverageDist=new TH1F("RMS aver distribution","RMS aver distribution",120,0,120);
	Int_t APV_counter =0;
	TH1F *rmsDist_temp;
	for(auto mpdIter=mpdAPVList.begin();mpdIter!=mpdAPVList.end();mpdIter++){
			Int_t mpdID=mpdIter->first;
			for(auto apvID : mpdIter->second){
				double_t rmsSum=0;
				rmsDist_temp=new TH1F("rms","rms",200,0,200);
				for(int i =1 ; i<=128; i ++ ){
					PedestalRMS2DDist->Fill(APV_counter,rmsHist[mpdID][apvID]->GetBinContent(i));
					rmsDist_temp->Fill(rmsHist[mpdID][apvID]->GetBinContent(i));
				}
				PedestalRMSAverageDist->Fill(APV_counter,rmsDist_temp->GetMean());
				PedestalRMSAverageDist->SetBinError(APV_counter,rmsDist_temp->GetRMS());
				APV_counter++;
				rmsDist_temp->Delete();
			}
		}

	// TODO
	// need to create canvas and plot the data to canvas
	TCanvas *canvas = new TCanvas("Pedestal","Pedestal",600,600);
	canvas->cd();
	PedestalRMS2DDist->Draw("zcol");
	PedestalRMSAverageDist->SetMarkerSize(10);
	PedestalRMSAverageDist->SetLineWidth(2);
	PedestalRMSAverageDist->SetLineColor(2);
	PedestalRMS2DDist->GetYaxis()->SetTitle("RMS");
	PedestalRMS2DDist->GetXaxis()->SetTitle("APV Cards");
	PedestalRMSAverageDist->GetXaxis()->SetTitle("APV cards");

//	PedestalRMSAverageDist->SetLineColor(3);
	PedestalRMSAverageDist->Draw("same");
}


//-------------------------------------------------------
// This will load the ADC value above the sigma-cut for each strips
// This represent the gain uniformity and the efficiency of each strip
//-------------------------------------------------------
void gemStripGain(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_20862_00_test.root",std::string HRS="RHRS"){

	// Get how many strips are fired in each event, read the strip number and the ADC value
    // Get the sum of the three time sample
    // save to histogram

	if (fname.IsNull()) {
		std::cout<<"Please input the file name"<<std::endl;
	}

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	fileio->GetObject("T",tree);


	std::string stripNumberPattern_x;
	std::string stripADCPattern_x;
	std::string stripADCSumPattern_x;

	std::string stripNumberPattern_y;
	std::string stripADCPattern_y;
	std::string stripADCSumPattern_y;
	// check which HRS it is

	if (HRS == "RHRS"){
		stripNumberPattern_x="RGEM.rgems.x%d.strip.number";
		stripNumberPattern_y="RGEM.rgems.y%d.strip.number";
		stripADCPattern_x="RGEM.rgems.x%d.adc%d";
		stripADCPattern_y="RGEM.rgems.y%d.adc%d";
        stripADCSumPattern_x="RGEM.rgems.x%d.strip.adc";
        stripADCSumPattern_y="RGEM.rgems.y%d.strip.adc";
	}else if ((HRS == "LHRS")) {
		stripNumberPattern_x="LGEM.lgems.x%d.strip.number";
		stripNumberPattern_y="LGEM.lgems.y%d.strip.number";
		stripADCPattern_x="LGEM.lgems.x%d.adc%d";
		stripADCPattern_y="LGEM.lgems.y%d.adc%d";
        stripADCSumPattern_x="LGEM.lgems.x%d.strip.adc";
        stripADCSumPattern_y="LGEM.lgems.y%d.strip.adc";
	}else{
		std::cout<<"[ERROR]:: only take RHRS/LHRS"<<std::endl;
	    exit(-1);
	}

	// Set the Max Timesample
	Int_t MaxTSample=3;
	std::cout<<"[Run Infor]:: Max Time Sample set to "<< MaxTSample<<std::endl;

	std::map<Int_t, Int_t> NDataStripNumber_x;
	std::map<Int_t,double_t *> StripNumber_x;
	std::map<Int_t,double_t *> StripADCSum_x;
	std::map<Int_t,std::map<Int_t,double_t *>> StripADC_x;

	std::map<Int_t, Int_t> NDataStripNumber_y;
	std::map<Int_t,double_t *> StripNumber_y;
	std::map<Int_t,double_t *> StripADCSum_y;
	std::map<Int_t,std::map<Int_t,double_t *>> StripADC_y;


	// Assign the branch to the data buffer
	Int_t error_counter=0;
	for(auto chamberID : gemGetDetectorList(fname)){

		std::string branchPatter;
		branchPatter.clear();

		branchPatter = "Ndata."+(std::string)Form(stripNumberPattern_x.c_str(),chamberID);
		if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
			std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
			tree->SetBranchAddress(branchPatter.c_str(),&(NDataStripNumber_x[chamberID]));

		}else{
			std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
			error_counter++;
		}

		branchPatter.clear();
		StripNumber_x[chamberID]=new double_t[2000];
		branchPatter=Form(stripNumberPattern_x.c_str(),chamberID);
		if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
			std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
			tree->SetBranchAddress(branchPatter.c_str(),StripNumber_x[chamberID]);

		}else{
			std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
			error_counter++;
		}

		branchPatter.clear();
		StripADCSum_x[chamberID]=new double_t[3000];
		branchPatter=Form(stripADCSumPattern_x.c_str(),chamberID);
		if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
			std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
			tree->SetBranchAddress(branchPatter.c_str(),(StripADCSum_x[chamberID]));

		}else{
			std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
			error_counter++;
		}

		for (Int_t tsample = 0 ; tsample < MaxTSample; tsample++){
			StripADC_x[chamberID][tsample]=new double_t[2000];
			branchPatter.clear();
			branchPatter=Form(stripADCPattern_x.c_str(),chamberID,tsample);
			if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
				std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
				tree->SetBranchAddress(branchPatter.c_str(),(StripADC_x[chamberID][tsample]));
			}else{
				std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
				error_counter++;
			}

		}

		// allocate Y-dimension
		branchPatter.clear();
		branchPatter= "Ndata."+(std::string)Form(stripNumberPattern_y.c_str(),chamberID);
		if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
			std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
			tree->SetBranchAddress(branchPatter.c_str(),&(NDataStripNumber_y[chamberID]));

		}else{
			std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
			error_counter++;
		}

		branchPatter.clear();
		StripNumber_y[chamberID]=new double_t[3000];
		branchPatter=Form(stripNumberPattern_y.c_str(),chamberID);
		if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
			std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
			tree->SetBranchAddress(branchPatter.c_str(),StripNumber_y[chamberID]);

		}else{
			std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
			error_counter++;
		}

		branchPatter.clear();
		StripADCSum_y[chamberID]=new double_t[3000];
		branchPatter=Form(stripADCSumPattern_y.c_str(),chamberID);
		if(tree->GetListOfBranches()->Contains(branchPatter.c_str())){
			std::cout<<"	... allocate "<< branchPatter.c_str()<< std::endl;
			tree->SetBranchAddress(branchPatter.c_str(),(StripADCSum_y[chamberID]));

		}else{
			std::cout<<"[WORNING]::"<<__LINE__<<"  branch \'"<<branchPatter.c_str()<<"\' did not find in tree "<<tree->GetName()<<std::endl;
			error_counter++;
		}
	}

	std::cout<<"[Run Infor]:: allocate buffer done !"<<std::endl;
	std::cout<<"		>>  | "<<error_counter<<" load error !|"<<std::endl;


	std::map<Int_t, TH2F *> StripADC2D_x;
	std::map<Int_t, TH2F *> StripADC2D_y;

	// attach the data to buffer, and loop on the entrys
	Int_t Entries=tree->GetEntries();


	for (Int_t entry = 1; entry<Entries; entry++)
	{
		// read the data and save to buffer

		// Test purpose
		tree->GetEntry(entry);
	for(auto chamberID : gemGetDetectorList(fname)){


			// write the X-dimension
			if(NDataStripNumber_x[chamberID]!=0){
				for (int stripCounter = 0; stripCounter < NDataStripNumber_x[chamberID]; stripCounter ++){
					auto stripID=(Int_t)StripNumber_x[chamberID][stripCounter];
					auto stripADC=StripADCSum_x[chamberID][(Int_t)(StripNumber_x[chamberID][stripCounter])];
					if(StripADC2D_x.find(chamberID)==StripADC2D_x.end()){
						if(chamberID<=3){
							StripADC2D_x[chamberID]= new TH2F(Form("chamber%d_x_strip_ADC",chamberID),Form("chamber%d_x_strip_ADC",chamberID),520,-1,519,2000,0,8000);
						}else{
							StripADC2D_x[chamberID]= new TH2F(Form("chamber%d_x_strip_ADC",chamberID),Form("chamber%d_x_strip_ADC",chamberID),1290,-1,1289,2000,0,8000);
						}
					}
					StripADC2D_x[chamberID]->Fill(stripID,stripADC);
				}
			}
			// write the Y-dimension
			if(NDataStripNumber_y[chamberID]!=0){
				for(int stripCounter = 0; stripCounter < NDataStripNumber_y[chamberID]; stripCounter ++){
					auto stripID=(Int_t)StripNumber_y[chamberID][stripCounter];
					auto stripADC=StripADCSum_y[chamberID][(Int_t)(StripNumber_y[chamberID][stripCounter])];
					if(StripADC2D_y.find(chamberID)==StripADC2D_y.end()){
						if(chamberID<=3){
							StripADC2D_y[chamberID]= new TH2F(Form("chamber%d_y_strip_ADC",chamberID),Form("chamber%d_y_strip_ADC",chamberID),257,-1,256,2000,0,8000);
						}else{
							StripADC2D_y[chamberID]= new TH2F(Form("chamber%d_y_strip_ADC",chamberID),Form("chamber%d_y_strip_ADC",chamberID),1290,-1,1289,2000,0,8000);
						}
					}
				    StripADC2D_y[chamberID]->Fill(stripID,stripADC);
				}
			}

		}

	}

	// output to canvas
	TCanvas *canvas_x=new TCanvas("x_dist","x_dist",1000,1000);
	TCanvas *canvas_y=new TCanvas("y_dist","y_dist",1000,1000);
	if(gemGetDetectorList(fname).size()<=3){
		canvas_x->Divide(1,gemGetDetectorList(fname).size());
	}else{
		int8_t split_H=(int8_t) std::sqrt(gemGetDetectorList(fname).size())+1;
		int8_t split_V=(int8_t) std::sqrt(gemGetDetectorList(fname).size());
		canvas_x->Divide(split_H,split_V);
	}

	if(gemGetDetectorList(fname).size()<=3){
		canvas_y->Divide(1,gemGetDetectorList(fname).size());
	}else{
		int8_t split_H=(int8_t) std::sqrt(gemGetDetectorList(fname).size())+1;
		int8_t split_V=(int8_t) std::sqrt(gemGetDetectorList(fname).size());
		canvas_y->Divide(split_H,split_V);
	}
	// write to canvas
	for(auto iter=StripADC2D_x.begin(); iter!=StripADC2D_x.end();iter++){
		canvas_x->cd(iter->first);
		iter->second->SetXTitle("Strip ID");
		iter->second->SetYTitle("ADC (Tsample Sum)");
		gStyle->SetOptStat("e");

		iter->second->Draw("zcol");
	}

	for(auto iter = StripADC2D_y.begin(); iter!=StripADC2D_y.end();iter++){
		canvas_y->cd(iter->first);
		iter->second->SetXTitle("Strip ID");
		iter->second->SetYTitle("ADC (Tsample Sum)");
		//iter->second->GetYaxis()->SetTitleSize(0.07);
		gStyle->SetOptStat("e");
		iter->second->Draw("zcol");
	}
}

// Project the VDC to each GEM plane and calculate the efficiency of each GEM
// Calculate the sigma, and set the searching area to be 5 sigma
void gemTrackEfficiency2(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_20862_00_test.root"){

	// global efficiency
	// efficiency heat map
	if (fname.IsNull()) {
		std::cout<<"Please input the file name"<<std::endl;
	}

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	TTree *tree;
	fileio->GetObject("T",tree);
	if(tree->IsZombie()){
		std::cout<<"[Error]: can not find tree in the file !!!"<<std::endl;
	}else{
		std::cout<<"Total Entries in the file:"<< (tree->GetEntries())<<std::endl;
	}
	// for run 20862, the Z-distance is {1.161, 1.7979800, 2.0902131, 2.7165651, 2.8749137, 2.9976041}
	double_t zpos[]=   {0.0,1.161, 1.7979800, 2.0902131, 2.7165651, 2.8749137, 2.9976041};
	double_t xMinCut[]={0.0,  -0.0594110320,  -0.0549617020,  -0.0534276410,   -0.46155132,  -0.46307054,  -0.46354599  };
	double_t xMaxCut[]={0.0,   0.040588968,    0.045038298,    0.046572359,     0.13844868,   0.13692946,   0.13645401  };
	double_t yMinCut[]={0.0,  -0.096555290,   -0.091156590,   -0.089864033,    -0.27825711,  -0.27738908,  -0.22982879 };
	double_t yMaxCut[]={0.0,   0.1034447100,   0.10884341,     0.11013597,      0.22174289,   0.22261092,   0.27017121 };

	// searching area (square)
	double_t xCut[]={0.02,  0.02,   0.02,  0.02,  0.10,  0.10,  0.10};
	double_t yCut[]={0.02,  0.02,   0.02,  0.02,  0.10,  0.10,  0.10};

	//load the variable and create the buffer

	//buffer the data

	// VDC hit in Rotation Transport Coordinatio

	std::string NDatavdcTrX_str("Ndata.R.tr.x");
	std::string NDatavdcTrY_str("Ndata.R.tr.y");
	std::string NDatavdcTrTH_str("Ndata.R.tr.th");
	std::string NDatavdcTrPH_str("Ndata.R.tr.ph");

	std::string vdcTrX_str("R.tr.x");
	std::string vdcTrY_str("R.tr.y");
	std::string vdcTrTH_str("R.tr.th");
	std::string vdcTrPH_str("R.tr.ph");

	Int_t NDatavdcTrX;
	Int_t NDatavdcTrY;
	Int_t NDatavdcTrTH;
	Int_t NDatavdcTrPH;

	double_t vdcTrX[2000];
	double_t vdcTrY[2000];
	double_t vdcTrPH[2000];
	double_t vdcTrTH[2000];

	if(tree->GetListOfBranches()->Contains(NDatavdcTrX_str.c_str())){
		tree->SetBranchAddress(NDatavdcTrX_str.c_str(),&NDatavdcTrX);
	}else{
		std::cout<<"[WORNING]:: "<< NDatavdcTrX_str.c_str()<<" cannot find!!"<<std::endl;
	}

	if(tree->GetListOfBranches()->Contains(NDatavdcTrY_str.c_str())){
		tree->SetBranchAddress(NDatavdcTrY_str.c_str(),&NDatavdcTrY);
	}else{
		std::cout<<"[WORNING]:: "<< NDatavdcTrY_str.c_str()<<" cannot find!!"<<std::endl;
	}

	if(tree->GetListOfBranches()->Contains(NDatavdcTrTH_str.c_str())){
		tree->SetBranchAddress(NDatavdcTrTH_str.c_str(),&NDatavdcTrTH);
	}else{
		std::cout<<"[WORNING]:: "<< NDatavdcTrTH_str.c_str()<<" cannot find!!"<<std::endl;
	}

	if(tree->GetListOfBranches()->Contains(NDatavdcTrPH_str.c_str())){
		tree->SetBranchAddress(NDatavdcTrPH_str.c_str(),&NDatavdcTrPH);
	}else{
		std::cout<<"[WORNING]:: "<< NDatavdcTrPH_str.c_str()<<" cannot find!!"<<std::endl;
	}

	// load the x y th ph value
	if (tree->GetListOfBranches()->Contains(vdcTrX_str.c_str())) {
		tree->SetBranchAddress(vdcTrX_str.c_str(), vdcTrX);
	} else {
		std::cout << "[WORNING]:: " << vdcTrX_str.c_str() << " cannot find!!"<< std::endl;
	}
	// load the x y th ph value
	if (tree->GetListOfBranches()->Contains(vdcTrY_str.c_str())) {
		tree->SetBranchAddress(vdcTrY_str.c_str(), vdcTrY);
	} else {
		std::cout << "[WORNING]:: " << vdcTrY_str.c_str() << " cannot find!!"<< std::endl;
	}

	if (tree->GetListOfBranches()->Contains(vdcTrPH_str.c_str())) {
		tree->SetBranchAddress(vdcTrPH_str.c_str(), vdcTrPH);
	} else {
		std::cout << "[WORNING]:: " << vdcTrPH_str.c_str() << " cannot find!!"<< std::endl;
	}
	// load the x y th ph value
	if (tree->GetListOfBranches()->Contains(vdcTrTH_str.c_str())) {
		tree->SetBranchAddress(vdcTrTH_str.c_str(), vdcTrTH);
	} else {
		std::cout << "[WORNING]:: " << vdcTrTH_str.c_str() << " cannot find!!"<< std::endl;
	}


	// GEM detector
	std::map<Int_t, std::string> NdataGEMHitPosX_str;
	std::map<Int_t, std::string> NdataGEMHitPosY_str;
	std::map<Int_t, std::string> GEMHitPosX_str;
	std::map<Int_t, std::string> GEMHitPosY_str;

	std::map<Int_t, Int_t> 	NdataGEMHitPosX;
	std::map<Int_t, Int_t> 	NdataGEMHitPosY;

	std::map<Int_t, double_t *> GEMHitPosX;
	std::map<Int_t, double_t *> GEMHitPosY;

	//
	for (auto chamberID : gemGetDetectorList(fname.Data())){
		std::string treeName;
		treeName.clear();

		// locate the Ndata
		treeName=Form("Ndata.RGEM.rgems.x%d.hit.pos",chamberID);
		NdataGEMHitPosX_str[chamberID]=treeName;
		if(tree->GetListOfBranches()->Contains(treeName.c_str())){
			tree->SetBranchAddress(treeName.c_str(),&NdataGEMHitPosX[chamberID]);
		}else{
			std::cout<<"[WORNING]:: "<<treeName.c_str()<<"  can NOT find in tree "<<(tree->GetName())<<std::endl;
		}

		treeName.clear();
		treeName=Form("Ndata.RGEM.rgems.y%d.hit.pos",chamberID);
		NdataGEMHitPosY_str[chamberID]=treeName;
		if(tree->GetListOfBranches()->Contains(treeName.c_str())){
			tree->SetBranchAddress(treeName.c_str(),&NdataGEMHitPosY[chamberID]);
		}else{
			std::cout<<"[WORNING]:: "<<treeName.c_str()<<"  can NOT find in tree "<<(tree->GetName())<<std::endl;
		}

		// initialize and attach the buff
		treeName=Form("RGEM.rgems.x%d.hit.pos",chamberID);
		//GEMHitPosX

	}

	for(auto entry=1; entry < (tree->GetEntries());entry++){
		tree->GetEntry(entry);
		std::cout<<"data set:"<<NDatavdcTrX<<", "<<NDatavdcTrY<<",  "<<NDatavdcTrTH<<",  "<<NDatavdcTrPH<<std::endl;
	}
}

/// Project the VDC to each GEM and get the efficiency compare with the VDC
/// \param fname
void gemVDCTrackEfficiency(TString fname ="/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_20862_00_test.root"){
    TChain *chain = new TChain("T");
    chain -> Add(fname.Data());
    Int_t runID = chain->GetMaximum("fEvtHdr.fRun");

    TString HRS = "L";
    TString GEMProfix = "LGEM.lgems";
    if (runID>20000) {
        HRS = "R";
        GEMProfix = "RGEM.rgems";
    }

    // search for the chambers
    std::vector<Int_t> chamberIDList;
    for (int i = 0; i <= 6; ++i) {
        if(chain->GetListOfBranches()->Contains(Form("%s.x%d.coord.pos",GEMProfix.Data(),i))){
            chamberIDList.push_back(i);
        }
    }
#ifdef __DEBUG__
    assert(chamberIDList.size() > 0);
    std::vector<TString>  treeBranchCheckArray;
    {
        treeBranchCheckArray.push_back(Form("%s.tr.x", HRS.Data()));
        treeBranchCheckArray.push_back(Form("%s.tr.th", HRS.Data()));
        treeBranchCheckArray.push_back(Form("%s.tr.y", HRS.Data()));
        treeBranchCheckArray.push_back(Form("%s.tr.ph", HRS.Data()));

        std::cout<<"------------------------------------------"<<std::endl;
        for (auto chamberID : chamberIDList)
            treeBranchCheckArray.push_back(Form("%s.x%d.coord.pos", GEMProfix.Data(), chamberID));

        for (auto item : treeBranchCheckArray) {
            if (!chain->GetListOfBranches()->Contains(item.Data())) {
                std::cout << "[ERROR]:: can NOT find banch " << item.Data() << std::endl;
                exit(-1);
            } else {
                std::cout << "Find Branch: " << item.Data() << std::endl;
            }
        }
        std::cout<<"--------------------------end of branch check"<<std::endl;
    }
#endif

    double_t zpos[]={0,1.07093,   1.75673,   1.95993,   2.40013,   2.51523,   2.61068};
    double_t xMinCut[]={0.0,  -0.0594110320,  -0.0549617020,  -0.0534276410,   -0.46155132,  -0.46307054,  -0.46354599  };
    double_t xMaxCut[]={0.0,   0.040588968,    0.045038298,    0.046572359,     0.13844868,   0.13692946,   0.13645401  };
    double_t yMinCut[]={0.0,  -0.096555290,   -0.091156590,   -0.089864033,    -0.27825711,  -0.27738908,  -0.22982879 };
    double_t yMaxCut[]={0.0,   0.1034447100,   0.10884341,     0.11013597,      0.22174289,   0.22261092,   0.27017121 };

    // searching area (square)
    double_t xCut[]={0.02,  0.02,   0.02,  0.02,  0.10,  0.10,  0.10};
    double_t yCut[]={0.02,  0.02,   0.02,  0.02,  0.10,  0.10,  0.10};


    std::map<Int_t, TH2F *> vdcPredicted2D;
    std::map<Int_t, TH2F *> gemDetected2D;
    std::map<Int_t, TH2F *> DetectedEfficiency2D;

    std::string vdcPredicted2DPattern;
    std::string gemDetected2DPattern;
    //initialized the histogram
    for (auto chamberID : chamberIDList){
        vdcPredicted2DPattern.clear();
        gemDetected2DPattern.clear();
        if(vdcPredicted2D.find(chamberID)== vdcPredicted2D.end()){
            if(chamberID<=3){
                vdcPredicted2D[chamberID]=new TH2F(Form("vdcProjected_ch%d",chamberID),Form("vdcProjected_ch%d",chamberID),20,-0.05,0.05,40,-0.1,0.1);
                gemDetected2D[chamberID]=new TH2F(Form("gemDetected_ch%d",chamberID),Form("gemDetected_ch%d",chamberID),20,-0.05,0.05,40,-0.1,0.1);

            }else{
                vdcPredicted2D[chamberID]=new TH2F(Form("vdcPredicted_ch%d",chamberID),Form("vdcPredicted_ch%d",chamberID),120,-0.3,0.3,100,-0.25,0.25);
                gemDetected2D[chamberID]=new TH2F(Form("gemDetected_ch%d",chamberID),Form("gemDetected_ch%d",chamberID),120,-0.3,0.3,100,-0.25,0.25);
            }
        }
        std::string boundaryCut =
                Form(  "%s.tr.x + %s.tr.th * %f >= %f && %s.tr.x + %s.tr.th * %f <= %f && %s.tr.y + %s.tr.ph * %f >= %f && %s.tr.y + %s.tr.ph * %f <= %f ",
                       HRS.Data(),HRS.Data(),zpos[chamberID], xMinCut[chamberID],HRS.Data(),HRS.Data(),zpos[chamberID],xMaxCut[chamberID],
                       HRS.Data(),HRS.Data(),zpos[chamberID], yMinCut[chamberID], HRS.Data(),HRS.Data(),zpos[chamberID], yMaxCut[chamberID]);
        std::string gemVDCMatchCut =
                Form("%s && abs(%s.tr.x + %s.tr.th * %f-%s.x%d.coord.pos)<=%f && abs(%s.tr.y + %s.tr.ph * %f-%s.y%d.coord.pos)<=%f",
                     boundaryCut.c_str(), HRS.Data(),HRS.Data(),zpos[chamberID], GEMProfix.Data(),chamberID,xCut[chamberID],
                                           HRS.Data(),HRS.Data(),zpos[chamberID],GEMProfix.Data(),chamberID,yCut[chamberID]);

        vdcPredicted2DPattern=Form("%s.tr.y + %s.tr.ph * %f : %s.tr.x + %s.tr.th * %f", HRS.Data(),HRS.Data(),zpos[chamberID],HRS.Data(),HRS.Data(),zpos[chamberID]);
        gemDetected2DPattern =Form("%s.tr.y + %s.tr.ph * %f : %s.tr.x + %s.tr.th * %f", HRS.Data(),HRS.Data(),zpos[chamberID],HRS.Data(),HRS.Data(),zpos[chamberID]);
        std::cout<<"===>"<<chamberID<<"\n	Plot:: "<<vdcPredicted2DPattern.c_str()<<std::endl<<"	Boundary Cut:: "<<boundaryCut.c_str()<<std::endl;
        chain->Project(vdcPredicted2D[chamberID]->GetName(),vdcPredicted2DPattern.c_str(), boundaryCut.c_str());
        chain->Project(gemDetected2D[chamberID]->GetName(),gemDetected2DPattern.c_str(), gemVDCMatchCut.c_str());
        DetectedEfficiency2D[chamberID]=(TH2F *) gemDetected2D[chamberID]->Clone(Form("gem_predict_ratio_%d",chamberID));
    }
    // apply filter on the Histogram

    // the efficiency distribution of the detectors
    std::map<Int_t,TH1F *> detEfficiencyDish;
    std::map<Int_t,Int_t> gemEventCountList;
    std::map<Int_t,Int_t> vdcProjEventCountList;

    for (auto chamberID : chamberIDList){
        Int_t gemEvent = 0;
        Int_t vdcProjEvent = 0;
        for (int i = 0; i < gemDetected2D[chamberID]->GetXaxis()->GetNbins();i++){
            for (int j = 0; j < gemDetected2D[chamberID]->GetYaxis()->GetNbins(); j++){
                if (gemDetected2D[chamberID]->GetBinContent(i,j) < 30){
                    gemDetected2D[chamberID]->SetBinContent(i,j,0);
                    vdcPredicted2D[chamberID]->SetBinContent(i,j,0);
                } else{
                    gemEvent += gemDetected2D[chamberID]->GetBinContent(i,j);
                    vdcProjEvent += vdcPredicted2D[chamberID]->GetBinContent(i,j);

                }
            }
        }
        gemEventCountList[chamberID]=gemEvent;
        vdcProjEventCountList[chamberID] = vdcProjEvent;
        std::cout<<"ChamberID::"<<chamberID<<"   ->"<<float (float (gemEvent)/vdcProjEvent) <<" with ("<<gemEvent<<","<<vdcProjEvent<<")"<<std::endl;
    }

    std::map<Int_t, TCanvas *> gemCanvas;

    TCanvas *canvas=new TCanvas("GEM efficiency","GEM efficiency",600,600);
    canvas->Divide(3,6);
    TPaveText *pt;
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetOptStat("e");
    gStyle->SetStatFontSize(0.1);
    gStyle->SetStatW(0.1);
    gStyle->SetStatX(0.9);
    gStyle->SetStatY(0.9);

    for(auto chamberID : gemGetDetectorList(fname)){
        canvas->cd((chamberID-1)*3+1);
        gemDetected2D[chamberID]->GetXaxis()->SetLabelSize(0.1);
        gemDetected2D[chamberID]->GetYaxis()->SetLabelSize(0.1);
        gemDetected2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
        gemDetected2D[chamberID]->Draw("zcol");
        canvas->cd((chamberID-1)*3+2);
        vdcPredicted2D[chamberID]->GetXaxis()->SetLabelSize(0.1);
        vdcPredicted2D[chamberID]->GetYaxis()->SetLabelSize(0.1);
        vdcPredicted2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
        vdcPredicted2D[chamberID]->Draw("zcol");
        canvas->cd((chamberID-1)*3+3);
        DetectedEfficiency2D[chamberID]->GetXaxis()->SetLabelSize(0.1);
        DetectedEfficiency2D[chamberID]->GetYaxis()->SetLabelSize(0.1);
        DetectedEfficiency2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
        DetectedEfficiency2D[chamberID]->Divide(vdcPredicted2D[chamberID]);
        DetectedEfficiency2D[chamberID]->Draw("zcol");
        DetectedEfficiency2D[chamberID]->GetZaxis()->SetRangeUser(0.7,1.0);
        float txtXpos = DetectedEfficiency2D[chamberID]->GetXaxis()->GetXmin() +0.1 *(DetectedEfficiency2D[chamberID]->GetXaxis()->GetXmax()-DetectedEfficiency2D[chamberID]->GetXaxis()->GetXmin());
        float txtYpos = DetectedEfficiency2D[chamberID]->GetYaxis()->GetXmin() + 0.8 *(DetectedEfficiency2D[chamberID]->GetYaxis()->GetXmax()-DetectedEfficiency2D[chamberID]->GetYaxis()->GetXmin());
        float overAlleff = (double_t)(float (gemEventCountList[chamberID])/float (vdcProjEventCountList[chamberID]));
        TLatex *pt = new TLatex(txtXpos,txtYpos,Form("GEM%d Eff=%1.2f%%",chamberID,overAlleff));
        pt->SetTextSize(0.08);
        pt->Draw("same");
        canvas->cd((chamberID-1)*3+3)->SetRightMargin(0.13);

        if(gemCanvas.find(chamberID)==gemCanvas.end()){
            gemCanvas[chamberID]=new TCanvas(Form("gem%d",chamberID),Form("gem%d",chamberID),600,600);
            gemCanvas[chamberID]->Divide(3,1);
            gStyle->SetOptStat("e");
        }
        gemCanvas[chamberID]->cd(1);
        gemDetected2D[chamberID]->GetXaxis()->SetLabelSize(0.03);
        gemDetected2D[chamberID]->GetYaxis()->SetLabelSize(0.03);
        gemDetected2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
        gemDetected2D[chamberID]->Draw("zcol");
        gemCanvas[chamberID]->cd(2);
        vdcPredicted2D[chamberID]->GetXaxis()->SetLabelSize(0.03);
        vdcPredicted2D[chamberID]->GetYaxis()->SetLabelSize(0.03);
        vdcPredicted2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
        vdcPredicted2D[chamberID]->Draw("zcol");
        gemCanvas[chamberID]->cd(3);
        DetectedEfficiency2D[chamberID]->GetXaxis()->SetLabelSize(0.03);
        DetectedEfficiency2D[chamberID]->GetYaxis()->SetLabelSize(0.03);
        DetectedEfficiency2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
        DetectedEfficiency2D[chamberID]->Draw("zcol");
        pt->Draw();
    }

}

///
/// \param fname
void gemOnlyTrackEfficiency(TString fname ){

}


// Project the VDC to each GEM plane and calculate the efficiency of each GEM
// Calculate the sigma, and set the searching area to be 5 sigma
//
void gemTrackEfficiency(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_20862_00_test.root"){
	// global efficiency
	// efficiency heat map
	if (fname.IsNull()) {
		std::cout<<"Please input the file name"<<std::endl;
	}

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	fileio->GetObject("T",tree);
	// for run 20862, the Z-distance is {1.161, 1.7979800, 2.0902131, 2.7165651, 2.8749137, 2.9976041}
	double_t zpos[]={0,1.07093,   1.75673,   1.95993,   2.40013,   2.51523,   2.61068};
	double_t xMinCut[]={0.0,  -0.0594110320,  -0.0549617020,  -0.0534276410,   -0.46155132,  -0.46307054,  -0.46354599  };
	double_t xMaxCut[]={0.0,   0.040588968,    0.045038298,    0.046572359,     0.13844868,   0.13692946,   0.13645401  };
	double_t yMinCut[]={0.0,  -0.096555290,   -0.091156590,   -0.089864033,    -0.27825711,  -0.27738908,  -0.22982879 };
	double_t yMaxCut[]={0.0,   0.1034447100,   0.10884341,     0.11013597,      0.22174289,   0.22261092,   0.27017121 };

	// searching area (square)
	double_t xCut[]={0.02,  0.02,   0.02,  0.02,  0.10,  0.10,  0.10};
	double_t yCut[]={0.02,  0.02,   0.02,  0.02,  0.10,  0.10,  0.10};

	std::map<Int_t, TH2F *> vdcPredicted2D;
	std::map<Int_t, TH2F *> gemDetected2D;
	std::map<Int_t, TH2F *> DetectedEfficiency2D;

	std::string vdcPredicted2DPattern;
	std::string gemDetected2DPattern;
	for (auto chamberID : gemGetDetectorList(fname)){
		vdcPredicted2DPattern.clear();
		gemDetected2DPattern.clear();
		//initialize the Histogram
		if(vdcPredicted2D.find(chamberID)== vdcPredicted2D.end()){
			if(chamberID<=3){
				vdcPredicted2D[chamberID]=new TH2F(Form("vdcProjected_ch%d",chamberID),Form("vdcProjected_ch%d",chamberID),20,-0.05,0.05,40,-0.1,0.1);
				gemDetected2D[chamberID]=new TH2F(Form("gemDetected_ch%d",chamberID),Form("gemDetected_ch%d",chamberID),20,-0.05,0.05,40,-0.1,0.1);

			}else{
				vdcPredicted2D[chamberID]=new TH2F(Form("vdcPredicted_ch%d",chamberID),Form("vdcPredicted_ch%d",chamberID),120,-0.3,0.3,100,-0.25,0.25);
				gemDetected2D[chamberID]=new TH2F(Form("gemDetected_ch%d",chamberID),Form("gemDetected_ch%d",chamberID),120,-0.3,0.3,100,-0.25,0.25);
			}
		}
		//T->Draw("RGEM.rgems.y1.coord.pos:RGEM.rgems.x1.coord.pos >>gemResidX1(1000,-0.05,0.05)","abs(R.tr.x + R.tr.th * 1.161-RGEM.rgems.x1.coord.pos)<0.01 && abs(R.tr.y + R.tr.ph * 1.161-RGEM.rgems.y1.coord.pos)<0.01")
		std::string boundaryCut =
				Form(  "R.tr.x + R.tr.th * %f >= %f && R.tr.x + R.tr.th * %f <= %f && R.tr.y + R.tr.ph * %f >= %f && R.tr.y + R.tr.ph * %f <= %f ",
						zpos[chamberID], xMinCut[chamberID], zpos[chamberID],xMaxCut[chamberID],
						zpos[chamberID], yMinCut[chamberID], zpos[chamberID], yMaxCut[chamberID]);
		std::string gemVDCMatchCut =
				Form("%s && abs(R.tr.x + R.tr.th * %f-RGEM.rgems.x%d.coord.pos)<=%f && abs(R.tr.y + R.tr.ph * %f-RGEM.rgems.y%d.coord.pos)<=%f",
						boundaryCut.c_str(), zpos[chamberID], chamberID,xCut[chamberID], zpos[chamberID],chamberID,yCut[chamberID]);

		vdcPredicted2DPattern=Form("R.tr.y + R.tr.ph * %f : R.tr.x + R.tr.th * %f", zpos[chamberID],zpos[chamberID]);
		gemDetected2DPattern=Form("R.tr.y + R.tr.ph * %f : R.tr.x + R.tr.th * %f", zpos[chamberID],zpos[chamberID]);
		std::cout<<"===>"<<chamberID<<"\n	Plot:: "<<vdcPredicted2DPattern.c_str()<<std::endl<<"	Boundary Cut:: "<<boundaryCut.c_str()<<std::endl;
		tree->Project(vdcPredicted2D[chamberID]->GetName(),vdcPredicted2DPattern.c_str(), boundaryCut.c_str());
		tree->Project(gemDetected2D[chamberID]->GetName(),gemDetected2DPattern.c_str(), gemVDCMatchCut.c_str());
		DetectedEfficiency2D[chamberID]=(TH2F *) gemDetected2D[chamberID]->Clone(Form("gem_predict_ratio_%d",chamberID));
	}

	std::map<Int_t, TCanvas *> gemCanvas;

	TCanvas *canvas=new TCanvas("GEM efficiency","GEM efficiency",600,600);
	canvas->Divide(3,6);
	TPaveText *pt;
	gStyle->SetTitleFontSize(0.1);
	gStyle->SetOptStat("e");
	gStyle->SetStatFontSize(0.1);
	gStyle->SetStatW(0.1);
	gStyle->SetStatX(0.9);
	gStyle->SetStatY(0.9);

	for(auto chamberID : gemGetDetectorList(fname)){
		canvas->cd((chamberID-1)*3+1);
		gemDetected2D[chamberID]->GetXaxis()->SetLabelSize(0.1);
		gemDetected2D[chamberID]->GetYaxis()->SetLabelSize(0.1);
		gemDetected2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
		gemDetected2D[chamberID]->Draw("zcol");
		canvas->cd((chamberID-1)*3+2);
		vdcPredicted2D[chamberID]->GetXaxis()->SetLabelSize(0.1);
		vdcPredicted2D[chamberID]->GetYaxis()->SetLabelSize(0.1);
		vdcPredicted2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
		vdcPredicted2D[chamberID]->Draw("zcol");
		canvas->cd((chamberID-1)*3+3);
		DetectedEfficiency2D[chamberID]->GetXaxis()->SetLabelSize(0.1);
		DetectedEfficiency2D[chamberID]->GetYaxis()->SetLabelSize(0.1);
		DetectedEfficiency2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
		DetectedEfficiency2D[chamberID]->Divide(vdcPredicted2D[chamberID]);
		DetectedEfficiency2D[chamberID]->Draw("zcol");

		pt = new TPaveText(0.2,0.7,0.4,0.85,"NDC");
		pt->AddText(Form("Efficiency=%f",(double_t)(gemDetected2D[chamberID]->GetEntries())/(double_t)(vdcPredicted2D[chamberID]->GetEntries())));
		pt->Draw();

		if(gemCanvas.find(chamberID)==gemCanvas.end()){
			gemCanvas[chamberID]=new TCanvas(Form("gem%d",chamberID),Form("gem%d",chamberID),600,600);
			gemCanvas[chamberID]->Divide(3,1);
			gStyle->SetOptStat("e");
		}
		gemCanvas[chamberID]->cd(1);
		gemDetected2D[chamberID]->GetXaxis()->SetLabelSize(0.03);
		gemDetected2D[chamberID]->GetYaxis()->SetLabelSize(0.03);
		gemDetected2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
		gemDetected2D[chamberID]->Draw("zcol");
		gemCanvas[chamberID]->cd(2);
		vdcPredicted2D[chamberID]->GetXaxis()->SetLabelSize(0.03);
		vdcPredicted2D[chamberID]->GetYaxis()->SetLabelSize(0.03);
		vdcPredicted2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
		vdcPredicted2D[chamberID]->Draw("zcol");
		gemCanvas[chamberID]->cd(3);
		DetectedEfficiency2D[chamberID]->GetXaxis()->SetLabelSize(0.03);
		DetectedEfficiency2D[chamberID]->GetYaxis()->SetLabelSize(0.03);
		DetectedEfficiency2D[chamberID]->GetZaxis()->SetLabelSize(0.07);
		DetectedEfficiency2D[chamberID]->Draw("zcol");
		pt->Draw();
	}
}

void gemNoiseLevel(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_20862_00_test.root",std::string HRS="RHRS"){
	if (fname.IsNull()) {
		std::cout<<"Please input the file name"<<std::endl;
	}

	TFile *fileio=TFile::Open(fname.Data());
	assert(fileio);
	fileio->GetObject("T",tree);
	fileio->Print();

	// read out the noise and signal level
	std::string noisePattern_x;
	std::string noisePattern_y;
	std::string signalPattern_x;
	std::string signalPattern_y;

	// check which HRS it is

	if (HRS == "RHRS"){
		noisePattern_x="RGEM.rgems.x%d.strip.adcnoise";
		noisePattern_y="RGEM.rgems.y%d.strip.adcnoise";
		signalPattern_x="RGEM.rgems.x%d.strip.adc";
		signalPattern_y="RGEM.rgems.y%d.strip.adc";

	}else if ((HRS == "LHRS")) {
		noisePattern_x="LGEM.lgems.x%d.strip.adcnoise";
		noisePattern_y="LGEM.lgems.y%d.strip.adcnoise";
		signalPattern_x="LGEM.lgems.x%d.strip.adc";
		signalPattern_y="LGEM.lgems.y%d.strip.adc";
	}else{
		std::cout<<"[ERROR]:: only take RHRS/LHRS"<<std::endl;
	    exit(-1);
	}


	TCanvas *canvas_x=new TCanvas("x_dist","x_dist",1000,1000);
	TCanvas *canvas_y=new TCanvas("y_dist","y_dist",1000,1000);
	canvas_x->SetLogy();   //set the log scale for the
	if(gemGetDetectorList(fname).size()<=3){
		canvas_x->Divide(1,gemGetDetectorList(fname).size());
	}else{
		int8_t split_H=(int8_t) std::sqrt(gemGetDetectorList(fname).size())+1;
		int8_t split_V=(int8_t) std::sqrt(gemGetDetectorList(fname).size());
		canvas_x->Divide(split_H,split_V);
	}
	canvas_y->SetLogy();   //set the log scale for the
	if(gemGetDetectorList(fname).size()<=3){
		canvas_y->Divide(1,gemGetDetectorList(fname).size());
	}else{
		int8_t split_H=(int8_t) std::sqrt(gemGetDetectorList(fname).size())+1;
		int8_t split_V=(int8_t) std::sqrt(gemGetDetectorList(fname).size());
		canvas_y->Divide(split_H,split_V);
	}

	// generate the histogram
	std::map<Int_t,TH1F *> noiseHist_x;
	std::map<Int_t,TH1F *> noiseHist_y;
	std::map<Int_t,TH1F *> signalHist_x;
	std::map<Int_t,TH1F *> signalHist_y;

	for (auto ChamberID : gemGetDetectorList(fname)){
		if(noiseHist_x.find(ChamberID)==noiseHist_x.end()){
			noiseHist_x[ChamberID]=new TH1F(Form("%s_Chamber%d_X",HRS.c_str(),ChamberID),Form("%s_Chamber%d_X_noiseLevel",HRS.c_str(),ChamberID),1000,-500,500);
		}
		if(noiseHist_y.find(ChamberID)==noiseHist_y.end()){
			noiseHist_y[ChamberID]=new TH1F(Form("%s_Chamber%d_Y",HRS.c_str(),ChamberID),Form("%s_Chamber%d_Y_noiseLevel",HRS.c_str(),ChamberID),1000,-500,500);
		}

		if(signalHist_x.find(ChamberID)==signalHist_x.end()){
			signalHist_x[ChamberID]=new TH1F(Form("%s_Chamber%d_sigLevel_X",HRS.c_str(),ChamberID),Form("%s_Chamber%d_sigLevel_X",HRS.c_str(),ChamberID),2000,-2000,8000);
			signalHist_x[ChamberID]->SetLineColor(2);
		}
		if(signalHist_y.find(ChamberID)==signalHist_y.end()){
			signalHist_y[ChamberID]=new TH1F(Form("%s_Chamber%d_sigLevel_Y",HRS.c_str(),ChamberID),Form("%s_Chamber%d_sigLevel_Y",HRS.c_str(),ChamberID),2000,-2000,8000);
			signalHist_y[ChamberID]->SetLineColor(2);
		}

		TPaveText *pt;
		//  plot the X-dimension
		if(IsBranchExist(Form(noisePattern_x.c_str(),ChamberID))){
			std::cout<<(noiseHist_x[ChamberID]->GetName())<<std::endl;
			tree->Project((noiseHist_x[ChamberID]->GetName()),Form(noisePattern_x.c_str(),ChamberID));
			std::string cut=std::string(Form(signalPattern_x.c_str(),ChamberID))+" !=0";
			tree->Project((signalHist_x[ChamberID]->GetName()),Form(signalPattern_x.c_str(),ChamberID),cut.c_str()); // need to take the average
			canvas_x->cd(ChamberID);
			//canvas_x->cd(ChamberID)->SetLogy();
			noiseHist_x[ChamberID]->SetXTitle("ADC(Tsample Average)");
			noiseHist_x[ChamberID]->SetYTitle("Count");
			noiseHist_x[ChamberID]->Fit("gaus","","",-110,110);
			noiseHist_x[ChamberID]->Draw();

			pt = new TPaveText(0.2,0.7,0.4,0.85,"NDC");
			pt->AddText(Form("Fit Sigma: %5.2f",((TF1 *)(noiseHist_x[ChamberID]->GetListOfFunctions()->FindObject("gaus")))->GetParameter(2)));
			pt->SetLineColor(0);
			pt->SetFillColor(0);
			pt->SetShadowColor(0);
			pt->SetTextSize(0.04);
			pt->Draw();
			//signalHist_x[ChamberID]->Draw("][same");
		}
		//  plot the Y-dimension
		if(IsBranchExist(Form(noisePattern_y.c_str(),ChamberID))){
			std::cout<<(noiseHist_y[ChamberID]->GetName())<<std::endl;
			tree->Project(noiseHist_y[ChamberID]->GetName(),Form(noisePattern_y.c_str(),ChamberID));
			std::string cut=std::string(Form(signalPattern_y.c_str(),ChamberID))+" !=0";
			tree->Project(signalHist_y[ChamberID]->GetName(),Form(signalPattern_y.c_str(),ChamberID),cut.c_str());
			canvas_y->cd(ChamberID);
			//canvas_y->cd(ChamberID)->SetLogy();
			noiseHist_y[ChamberID]->SetXTitle("ADC(Tsample Average)");
			noiseHist_y[ChamberID]->SetYTitle("Count");
			noiseHist_y[ChamberID]->Fit("gaus","","",-150,150);
//			noiseHist_y[ChamberID]->Scale(0.3);
			noiseHist_y[ChamberID]->Draw();
			//signalHist_y[ChamberID]->Draw("][same");
			pt = new TPaveText(0.2,0.7,0.4,0.85,"NDC");
			pt->AddText(Form("Fit Sigma: %5.2f",((TF1 *)(noiseHist_y[ChamberID]->GetListOfFunctions()->FindObject("gaus")))->GetParameter(2)));
			pt->SetLineColor(0);
			pt->SetFillColor(0);
			pt->SetShadowColor(0);
			pt->SetTextSize(0.04);
			pt->Draw();

		}
	}

}

// Used for check the X-Y hit match
//
//
void gemHitMacth(TString fname = "/home/newdriver/PRex/PRex_Data/GEMRootFile/prexRHRS_21289_00_test.root"){
	// initial one should use RGEM.rgems.y5.hit.pos
	// Final One should use    RGEM.rgems.y5.coord.pos

}

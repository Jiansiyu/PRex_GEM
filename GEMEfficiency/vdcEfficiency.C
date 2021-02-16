/*
 * vdc Profile scripts
 * author : Siyu Jian
 * email  : sj9va@virginia.edu
 *
 */

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
#include <TProcPool.h>
#include "TChain.h"
#include "TLatex.h"
#include "TVector3.h"
#include "TSystem.h"
#include "istream"
#include "fstream"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TBenchmark.h"
#include "TException.h"
#include "TCut.h"

/// Get the VDC profile
/// \param fname
double  getVDCProfile(TString fname){
    if (fname.IsNull()) std::cout<<"Please input the file name"<<std::endl;
    TChain *chain  = new TChain("T");
    chain->Add(fname.Data());

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

    TString vdcCut = Form("Ndata.%s.tr.d_ph > 0 && Ndata.%s.tr.d_th > 0",HRS.Data(),HRS.Data());
    TString gemCut = "1>0";
    for (auto chamberID : chamberIDList)gemCut = gemCut + Form("&& Ndata.%s.x%d.coord.trkpos >0 && Ndata.%s.y%d.coord.trkpos > 0",GEMProfix.Data(),chamberID, GEMProfix.Data(),chamberID);

    auto vdcEffEntries = chain->GetEntries(Form("%s&&%s",vdcCut.Data(),gemCut.Data()));
    auto vdcTotalEntries = chain->GetEntries(gemCut.Data());

    std::cout<<"---------------->"<<std::endl<<"\t"<<fname.Data()<<std::endl;
    std::cout<<"\tvdc Efficiency:: "<< double (vdcEffEntries)/vdcTotalEntries<< "    "<< vdcEffEntries<<"/"<<vdcTotalEntries<<std::endl;

    return double (vdcEffEntries)/vdcTotalEntries;//,0.01*double (vdcEffEntries)/vdcTotalEntries};
}

///
/// \param fname
/// \return
Int_t vdcEfficiency(){
    //       runID, Rate
    std::map<Int_t, Int_t> runRateMap;
    {
        runRateMap[21281] = 45;
        runRateMap[21282] = 110;
        runRateMap[21283] = 200;
        runRateMap[21284] = 510;
    }

    std::map<Int_t, double> vdcEfficiencyMap;

    for (auto iter = runRateMap.begin(); iter != runRateMap.end(); iter ++){
        Int_t runID = iter->first;
        Int_t runRate = iter->second;
        std::cout<<"Run ID :"<< runID <<" --> "<< runRate<<std::endl;
        auto res = getVDCProfile(Form("/home/newdriver/PRex_GEM/PRex_replayed/sigma4/prexRHRS_%d_-1_*",runID));
        std::cout<<res<<std::endl;
    }

    return  1;
}
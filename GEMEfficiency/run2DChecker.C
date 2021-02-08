#include "TCanvas.h"
#include "TH2F.h"
#include "TChain.h"
#include "vector"
#include "map"
#include "TSystem.h"
#include "iostream"
#include "TROOT.h"
#include "TSeqCollection.h"
#include "TProcPool.h"
#include "TStyle.h"
#include "TLegend.h"
#include "algorithm"
#include "TFile.h"
#include "TRandom.h"

Int_t MAXTSample = 3;
Int_t MinStripInCluster = 2;

std::map<Int_t, TH2F *> getPlot(TString fname){
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
        if(chain->GetListOfBranches()->Contains(Form("%s.x%d.coord.pos",GEMProfix.Data(),i))){
            chamberIDList.push_back(i);
        }
    }

    // project the chamberID
    std::map<Int_t, TH2F *> gemClusterDisthh;
    for (auto chamberID: chamberIDList){
        gemClusterDisthh[chamberID] = new TH2F(Form("GEM_Track_run%d_%d",runID,chamberID),Form("GEM_Track_run%d_%d",runID,chamberID), 600,-0.15,0.15,600,-0.15,0.15);
    }

    for (auto  chamberID : chamberIDList){
        TString projectionCom = Form("%s.y%d.coord.trkpos:%s.x%d.coord.trkpos",GEMProfix.Data(),chamberID,GEMProfix.Data(),chamberID);
        chain->Project(gemClusterDisthh[chamberID]->GetName(),projectionCom.Data());
    }
    return gemClusterDisthh;
}



void run2DChecker(){
    // lload the files and create the canvas
    std::vector<TString> runList;
    TString rawPathDir = "/home/newdriver/PRex_GEM/PRex_replayed";
    {
        for (auto i = 21281; i< 21293; i ++){
            TString filename =Form("%s/prexRHRS_%d_01_gem.root",rawPathDir.Data(),i);
            if (!gSystem->AccessPathName(filename.Data()))
                runList.push_back(filename.Data());
        }
    }

    Int_t runNumber = runList.size();
    TCanvas *canv =new TCanvas("Canv","Canv",1960,1080);
    canv->Divide(3,2);
    canv->Draw();

    for (auto i = 0; i < runList.size() ; i ++){
        auto chamberhh = getPlot(runList.at(i).Data());

        canv->Clear();
        canv->Divide(3,2);
        for (auto plot = chamberhh.begin(); plot != chamberhh.end(); plot ++){
            canv->cd(plot->first);
            canv->cd(plot->first)->SetGridx();
            canv->cd(plot->first)->SetGridy();
            plot->second->Draw("zcol");
        }
        canv->Update();
        if (i == 0){
            canv->Print(Form("gemhistCheck.pdf("),"pdf");
        } else{
            canv->Print(Form("gemhistCheck.pdf"),"pdf");
        }
    }
    canv->Print(Form("gemhistCheck.pdf)"),"pdf");
    canv->Update();
}

struct gemProfileStru{
    std::map<Int_t,TH1F *> gemCluster;
    std::map<Int_t,TH1F *> gemNHit;

};


// gem chamber performance function
std::map<Int_t,std::map<Int_t,TH1F *>> getGEMCluster(TString fname="/home/newdriver/PRex_GEM/PRex_replayed/prexRHRS_21281_-1_1.root"){

    ROOT::EnableImplicitMT(8);

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
        if(chain->GetListOfBranches()->Contains(Form("%s.x%d.coord.pos",GEMProfix.Data(),i))){
            chamberIDList.push_back(i);
        }
    }

    //GEM detector strip ID
    Int_t Ndata_GEM_x_strip_number[7];//Ndata.RGEM.rgems.x3.strip.number
    Int_t  Ndata_GEM_y_strip_number[7];//Ndata.RGEM.rgems.x3.strip.number
    double_t GEM_x_strip_number[7][2000];//RGEM.rgems.x3.strip.number
    double_t GEM_y_strip_number[7][2000];//RGEM.rgems.x3.strip.number

    // GEM detector ADC values
    Int_t Ndata_GEM_x_adc[7][7];// 6 chamber, 3 timesamples, Ndata_RGEM_rgems_x1_adc0
    Int_t Ndata_GEM_y_adc[7][7];// 6 chamber, 3 timesamples, Ndata_RGEM_rgems_x1_adc0
    Double_t  GEM_x_adc[7][7][100];   //[Ndata.RGEM.rgems.x1.adc0]
    Double_t  GEM_y_adc[7][7][100];   //[Ndata.RGEM.rgems.x1.adc0]


    //project the data to the chamber
    for (auto  chamberID :chamberIDList){
        TString Ndata_GEM_x_strip_numberStr =Form("Ndata.%s.x%d.strip.number",GEMProfix.Data(),chamberID);
        chain->SetBranchAddress(Ndata_GEM_x_strip_numberStr.Data(),&Ndata_GEM_x_strip_number[chamberID]);

        TString GEM_x_strip_numberStr = Form("%s.x%d.strip.number",GEMProfix.Data(),chamberID);
        chain->SetBranchAddress(GEM_x_strip_numberStr.Data(),GEM_x_strip_number[chamberID]);

        TString Ndata_GEM_y_strip_numberStr =Form("Ndata.%s.y%d.strip.number",GEMProfix.Data(),chamberID);
        chain->SetBranchAddress(Ndata_GEM_y_strip_numberStr.Data(),&Ndata_GEM_y_strip_number[chamberID]);

        TString GEM_y_strip_numberStr = Form("%s.y%d.strip.number",GEMProfix.Data(),chamberID);
        chain->SetBranchAddress(GEM_y_strip_numberStr.Data(),GEM_y_strip_number[chamberID]);

        //project the ADC values, for here need to project the adc as well
        for (auto adcID = 0 ; adcID < MAXTSample ; adcID ++){
            TString Ndata_GEM_x_adcStr= Form("Ndata.%s.x%d.adc%d",GEMProfix.Data(),chamberID,adcID);
            chain->SetBranchAddress(Ndata_GEM_x_adcStr.Data(),&Ndata_GEM_x_adc[chamberID][adcID]);

            TString Ndata_GEM_y_adcStr= Form("Ndata.%s.y%d.adc%d",GEMProfix.Data(),chamberID,adcID);
            chain->SetBranchAddress(Ndata_GEM_y_adcStr.Data(),&Ndata_GEM_y_adc[chamberID][adcID]);

            TString GEM_x_adcStr = Form("%s.x%d.adc%d",GEMProfix.Data(),chamberID,adcID);
            chain->SetBranchAddress(GEM_x_adcStr.Data(),GEM_x_adc[chamberID][adcID]);

            TString GEM_y_adcStr = Form("%s.y%d.adc%d",GEMProfix.Data(),chamberID,adcID);
            chain->SetBranchAddress(GEM_y_adcStr.Data(),GEM_y_adc[chamberID][adcID]);
        }
    }

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


    // Result Histograme
    std::map<Int_t,TH1F *> gemClusterXSizeList;
    std::map<Int_t,TH1F *> gemNClustersXList;
    std::map<Int_t,TH1F *> gemClusterXADCList;
    for (auto chamberID : chamberIDList){
        gemClusterXSizeList[chamberID] = new TH1F(Form("PRex%d_GEM%d_X_ClusterSize",runID,chamberID),Form("PRex%d_GEM%d_X_ClusterSize",runID,chamberID),10,0,10);
        gemNClustersXList[chamberID] = new TH1F(Form("PRex%d_GEM%d_X_#_Cluster",runID,chamberID),Form("PRex%d_GEM%d_X_#_Cluster",runID,chamberID),10,0,10);
        gemClusterXADCList[chamberID] = new TH1F(Form("PRex%d_GEM%d_X_ClusterADC(max Strip ADC)",runID,chamberID),Form("PRex%d_GEM%d_X_ClusterADC(max Strip ADC)",runID,chamberID),400,0,4000);
    }

    Long64_t entries = chain->GetEntriesFast();
    for (Long64_t entry : ROOT::TSeqUL(entries)){
        chain->GetEntry(entry);
        if (entry%100==0 ) {
            MemInfo_t memInfo;
            CpuInfo_t cpuInfo;
            gSystem->GetMemInfo(&memInfo);
            std::cout<<"\x1B[s"<<Form("\tProgress %2.1f",(double)entry*100/entries)<<"%" <<" /"<<Form("RAM:%6d KB",memInfo.fMemUsed)<<"\x1B[u" << std::flush;
        }

#ifdef __DEBUG__
        // check the data, should be the same as the scan parameters
        std::cout<<"\n----------> Evt:"<<entry<<std::endl;
        for (auto chamberID : chamberIDList){
            std::cout<<"\t Chamber :"<<chamberID<<std::endl;
            std::cout<<"\t\tStrip Number ("<< Ndata_GEM_x_strip_number[chamberID]<<")  :: ";
            for (auto i = 0 ; i < Ndata_GEM_x_strip_number[chamberID]; i++){
                std::cout <<GEM_x_strip_number[chamberID][i]<<"\t";
            }
            std::cout<<std::endl;

            std::cout<<"\t\tADC Value 0 ("<< Ndata_GEM_x_adc[chamberID][0]<<")  :: ";
            for (auto i = 0 ; i < Ndata_GEM_x_adc[chamberID][0]; i++){
                std::cout <<GEM_x_adc[chamberID][0][i]<<"\t";
            }
            std::cout<<std::endl;

            std::cout<<"\t\tADC Value 1 ("<< Ndata_GEM_x_adc[chamberID][1]<<")  :: ";
            for (auto i = 0 ; i < Ndata_GEM_x_adc[chamberID][1]; i++){
                std::cout <<GEM_x_adc[chamberID][1][i]<<"\t";
            }
            std::cout<<std::endl;

            std::cout<<"\t\tADC Value 2 ("<< Ndata_GEM_x_adc[chamberID][2]<<")  :: ";
            for (auto i = 0 ; i < Ndata_GEM_x_adc[chamberID][2]; i++){
                std::cout <<GEM_x_adc[chamberID][2][i]<<"\t";
            }
            std::cout<<std::endl;
        }

#endif

        Double_t GEM_x_adc_sum[7][100];   // chamber, ADC sum of the three time sample
        Double_t GEM_y_adc_sum[7][100];   // chamberID, ADC sum of the three time sample
        for (auto chamberID : chamberIDList){
            // calculate the X dimension
            for (auto i =0; i < Ndata_GEM_x_strip_number[chamberID]; i++){
                //TODO Hard coded, for PRex there is no problem, for others, maybe there is a issue.
                GEM_x_adc_sum[chamberID][i] = GEM_x_adc[chamberID][0][i] + GEM_x_adc[chamberID][1][i] + GEM_x_adc[chamberID][2][i];
            }
            for (auto i = 0 ; i<Ndata_GEM_y_strip_number[chamberID]; i++){
                GEM_y_adc_sum[chamberID][i] = GEM_y_adc[chamberID][0][i] + GEM_y_adc[chamberID][1][i] + GEM_y_adc[chamberID][2][i];
            }
        }


#ifdef __DEBUG__
        std::cout<<"\t ---> ADC sum of the 3 time sample"<<std::endl;
        for (auto chamberID : chamberIDList){
            std::cout<<"\t Chamber :"<<chamberID<<std::endl;
            std::cout<<"\t\tStSum Val  ("<< Ndata_GEM_x_strip_number[chamberID]<<")  :: ";
            for (auto i =0; i < Ndata_GEM_x_strip_number[chamberID]; i++){
                //TODO Hard coded, for PRex there is no problem, for others, maybe there is a issue.
                std::cout<<GEM_x_adc_sum[chamberID][i] <<"\t";
            }
            std::cout<<std::endl;
        }
#endif

        //TODO match the track and the hit position
        std::map<Int_t,std::map<Int_t, Int_t>>StripIDMapX; // strip number, index
        std::map<Int_t,std::vector<std::vector<Int_t>>> clusterArrayX; // chop the strip ID

        std::map<Int_t,std::map<Int_t, Int_t>>StripIDMapY; // strip number, index
        std::map<Int_t,std::vector<std::vector<Int_t>>> clusterArrayY; // chop the strip ID

        // Map the Strip Number and the index in the data array
        for (auto chamberID: chamberIDList){
            std::map<Int_t,Int_t> chamberStripX;
            chamberStripX.clear();
            for (auto i = 0; i <Ndata_GEM_x_strip_number[chamberID]; i++){
                Int_t stripID = GEM_x_strip_number[chamberID][i];
                chamberStripX[stripID] = i;
            }
            StripIDMapX[chamberID] = chamberStripX;

            std::map<Int_t,Int_t> chamberStripY;
            chamberStripY.clear();
            for (auto i = 0; i <Ndata_GEM_y_strip_number[chamberID]; i++){
                Int_t  stripID =GEM_y_strip_number[chamberID][i];
                chamberStripY[stripID] = i;
            }
            StripIDMapY[chamberID] = chamberStripY;
        }

        // chop the strip, form cluster
        for (auto chamberID : chamberIDList){
            // chop the X dimension
            std::vector<Int_t> singleCluster;
            std::vector<std::vector<Int_t>> chamberClusterX;
            singleCluster.clear();
            chamberClusterX.clear();

            for (auto i = 0; i <Ndata_GEM_x_strip_number[chamberID]; i++){
                Int_t stripID = GEM_x_strip_number[chamberID][i];
                if (i+1 < Ndata_GEM_x_strip_number[chamberID] && (stripID +1 == GEM_x_strip_number[chamberID][i+1])){
                    singleCluster.push_back(stripID);
                } else{
                    singleCluster.push_back(stripID);
                    chamberClusterX.push_back(singleCluster);
                    singleCluster.clear();
                }
            }

            // chop the Y dimension
            std::vector<std::vector<Int_t>> chamberClusterY;
            singleCluster.clear();
            for (auto i = 0 ;  i< Ndata_GEM_y_strip_number[chamberID]; i++){
                Int_t  stripID = GEM_y_strip_number[chamberID][i];
                if ( i + 1 < GEM_y_strip_number[chamberID][i] && (stripID+1 == GEM_y_strip_number[chamberID][i+1])){
                    singleCluster.push_back(stripID);
                }else{
                    singleCluster.push_back(stripID);
                    chamberClusterY.push_back(singleCluster);
                    singleCluster.clear();
                }
            }

            clusterArrayX[chamberID] = chamberClusterX;
            clusterArrayY[chamberID] = chamberClusterY;
        }

#ifdef __DEBUG__
        std::cout<<"\t---> ADC sum of the 3 time sample"<<std::endl;
        for (auto chamberID:chamberIDList){
            if(Ndata_GEM_X_coord_trkpos[chamberID] > 0 && Ndata_GEM_Y_coord_trkpos[chamberID] > 0){
                std::cout<<"\t\t Chamber "<< chamberID << "  find cluster :"<<clusterArrayX[chamberID].size()<<"   (***) Find Track (***)"<<std::endl;
            }else{
                std::cout<<"\t\t Chamber "<< chamberID << "  find cluster :"<<clusterArrayX.size()<<std::endl;
            }
            for (auto cluster : clusterArrayX[chamberID]){
                std::cout<<"\t\t\t ==> Cluster :";
                for (auto strip : cluster){
                    std::cout<<strip<<"\t";
                }
                std::cout<<std::endl;
            }
        }
        getchar();
#endif
        // Finish chop the clusters, apply cut and fill the histogram
        //TODO need to polish the cut
        for (auto chamberID : chamberIDList){
            // very strick cut on the plot
            gemNClustersXList[chamberID]->Fill(clusterArrayX[chamberID].size());
//            if (clusterArrayX[chamberID].size() >= 1)
            if (Ndata_GEM_X_coord_trkpos[chamberID] > 0 && Ndata_GEM_Y_coord_trkpos[chamberID] > 0 && (clusterArrayX[chamberID].size() == 1))
            {
                for (auto clusters : clusterArrayX[chamberID]){
                    if (clusters.size()<=1) continue;
                    Int_t stripADC = 0;
                    Int_t stripIndexID = 0;
                    Int_t stripMaxADC= 0;
                    for (auto strip : clusters){
                        auto currStripADC = GEM_x_adc_sum[chamberID][StripIDMapX[chamberID][strip]];
                        if (stripADC < currStripADC){
                            stripADC = currStripADC;
                            stripIndexID = StripIDMapX[chamberID][strip];
                            //get the maximum of the three time samples
                        }
                        // find the maximum of the three time sample
                        //GEM_x_adc[chamberID][0][i] + GEM_x_adc[chamberID][1][i] + GEM_x_adc[chamberID][2][i];
                        for (int adcID = 0 ; adcID < MAXTSample ; adcID ++){
                            if (stripMaxADC < GEM_x_adc[chamberID][adcID][stripIndexID]){
                                stripMaxADC = GEM_x_adc[chamberID][adcID][stripIndexID];
                            }
                        }
                    }
                    gemClusterXADCList[chamberID]->Fill(stripMaxADC);
                    gemClusterXSizeList[chamberID]->Fill(clusters.size());
//                    std::cout<<"** \tChamber "<<chamberID<<"  maxADC:"<<stripADC<<"  cluster Size"<<clusters.size()<<std::endl;
                }
            }

        }
    }

    gStyle->SetStatH(0.4);

    TCanvas *canv = new TCanvas(Form("GEM Cluster Infor %d",runID),Form("GEM Cluster Infor %d",runID),1960,4000);
    canv->Divide(3,6);
    for (auto chamberID : chamberIDList){
        canv->cd((chamberID-1) *3 + 1);
        gemClusterXSizeList[chamberID]->Draw();
        canv->cd((chamberID-1) *3 + 2);
        gemClusterXADCList[chamberID]->Draw();
        canv->cd((chamberID-1) *3 + 3);
        gemNClustersXList[chamberID]->Draw();
    }
    canv->Update();

    std::map<Int_t,std::map<Int_t,TH1F *>> res;
    res[1] = gemNClustersXList;
    res[2] = gemClusterXSizeList;
    res[3] = gemClusterXADCList;

    return  res;
}

void getGEMClusterRunner(TString pdfSaveName="gemClusterInfor.pdf"){
//    gStyle->SetPalette(1);
//    gStyle->SetOptStat(0);
    TFile resultTFile(Form("%s_Res.root",__FUNCTION__ ),"recreate");

    TString rawPathDir = "/home/newdriver/PRex_GEM/PRex_replayed";
    std::map<Int_t,TString> runList;
//    {
////        runList[20862]=(Form("%s/prexRHRS_20862_01_gem.root",rawPathDir.Data()));
//        for (auto i = 21281; i< 21295; i ++){
//            TString filename =Form("%s/prexRHRS_%d_-1.root",rawPathDir.Data(),i);
//            if (!gSystem->AccessPathName(filename.Data()))
//                runList[i]=(filename.Data());
//        }
//    }

    {
//        runList.push_back(Form("%s/prexRHRS_20862_-1.root",rawPathDir.Data()));
        for (auto i = 2139; i< 2151; i ++){
            TString filename =Form("%s/prexLHRS_%d_01_gem.root",rawPathDir.Data(),i);
            if (!gSystem->AccessPathName(filename.Data()))
                runList[i]=(filename.Data());
        }
    }

    std::map<Int_t,TH1F *>AverClusterSizeh;
    std::map<Int_t,TH1F *>AverClusterADCh;

    for (int chamberID = 0; chamberID <= 6; ++chamberID) {
        AverClusterSizeh[chamberID]=new TH1F(Form("Average Cluster Size GEM%d",chamberID),Form("Average Cluster Size GEM%d",chamberID),15,21280,21295);
//        AverClusterSizeh[chamberID]->GetYaxis()->SetRangeUser(1.6,3.5);
        AverClusterADCh[chamberID] = new TH1F(Form("Average Cluster ADC GEM%d",chamberID),Form("Average Cluster ADC GEM%d",chamberID),15,21280,21295);
//        AverClusterADCh[chamberID]->GetYaxis()->SetRangeUser()
    }


    TCanvas *canv = new TCanvas("Canv","Canv",3960,2160);
    canv->Draw();
    canv->Print(Form("%s(",pdfSaveName.Data()),"pdf");

    std::map<Int_t,std::map<Int_t,std::map<Int_t,TH1F *>>> allRunResultHistoBuff;

    for (auto iter =runList.begin(); iter!=runList.end();iter++){
        auto fname = iter->second;
        auto runID = iter->first;

        canv->Clear();
        canv->Divide(3,6);
        canv->SetTitle(Form("run%d_clusterInfor",runID));
        canv->SetName(Form("run%d_clusterInfor",runID));
        auto runhisto = getGEMCluster(fname.Data());
        allRunResultHistoBuff[runID] = runhisto;
        // print first element
        for (auto plotIndex = runhisto[1].begin();plotIndex !=runhisto[1].end(); plotIndex ++){
            auto chamberID = plotIndex->first;
//            if (chamberID <4) continue;
            canv->cd((chamberID-1)*3+1);
            plotIndex->second->Draw();
            plotIndex->second->Write();
        }
        for (auto plotIndex = runhisto[2].begin();plotIndex !=runhisto[2].end(); plotIndex ++){
            auto chamberID = plotIndex->first;
//            if (chamberID<4) continue;
            canv->cd((chamberID-1)*3+2);
            plotIndex->second->Draw();
            plotIndex->second->Write();
            AverClusterSizeh[chamberID]->Fill(runID,plotIndex->second->GetMean());
            AverClusterSizeh[chamberID]->Fill(runID,plotIndex->second->GetMeanError());
        }

        for (auto plotIndex = runhisto[3].begin();plotIndex !=runhisto[3].end(); plotIndex ++){
            auto chamberID = plotIndex->first;
//            if (chamberID<4) continue;
            canv->cd((chamberID-1)*3+3);
            plotIndex->second->Draw();
            plotIndex->second->Write();
            AverClusterADCh[chamberID]->Fill(runID,plotIndex->second->GetMean());
            AverClusterADCh[chamberID]->SetBinError(runID,plotIndex->second->GetMeanError());
        }
        canv->Update();
        canv->Write();
        canv->Print(Form("%s",pdfSaveName.Data()),"pdf");
    }

    // plot the histogram
    canv->Clear();
    canv->Divide(2,6);
    canv->SetTitle(Form("cluster_allrun_summary"));
    canv->SetName(Form("cluster_allrun_summary"));
    for (Int_t i = 1; i<= 6; i ++){
        canv->cd((i-1)*2+1);
        double RangeMax =AverClusterSizeh[i]->GetBinContent(AverClusterSizeh[i]->GetMaximumBin())+0.2;
        double RangeMin = RangeMax-1.2;
        AverClusterSizeh[i]->SetMarkerStyle(20);
        AverClusterSizeh[i]->SetMarkerSize(2);
        AverClusterSizeh[i]->GetYaxis()->SetRangeUser(RangeMin,RangeMax);
        AverClusterSizeh[i]->Draw("e1");
        canv->cd((i-1)*2+2);
        RangeMax  =AverClusterADCh[i]->GetBinContent(AverClusterADCh[i]->GetMaximumBin()) + 100;
        RangeMin  = RangeMax - 800;
        AverClusterADCh[i]->GetYaxis()->SetRangeUser(RangeMin,RangeMax);
        AverClusterADCh[i]->SetMarkerStyle(20);
        AverClusterADCh[i]->SetMarkerSize(2);
        AverClusterADCh[i]->Draw("e1");
    }
    canv->Write();
    canv->Print(Form("%s",pdfSaveName.Data()),"pdf");

//    gStyle->SetOptStat("n");
    canv->Clear();
    canv->Divide(2,1);
    canv->SetTitle(Form("cluster_allrun_summary_vs.run"));
    canv->SetName(Form("cluster_allrun_summary_vs.run"));
    // plot all canvas on the same plot
    canv->cd(1);
    auto legendClusterSize = new TLegend(0.1,0.7,0.48,0.9);
    for (Int_t chamberID =1; chamberID <=6; chamberID++){
        AverClusterSizeh[chamberID]->SetMarkerColor(chamberID);
        AverClusterSizeh[chamberID]->GetXaxis()->SetTitle("runID");
        AverClusterSizeh[chamberID]->GetYaxis()->SetTitle("Cluster Size");
//        AverClusterSizeh[chamberID]->SetLineColor(chamberID);
        if (chamberID == 1){
            AverClusterSizeh[chamberID]->Draw();
        } else{
            AverClusterSizeh[chamberID]->Draw("same");
        }
        legendClusterSize->AddEntry(AverClusterSizeh[chamberID],AverClusterSizeh[chamberID]->GetName());
    }
    legendClusterSize->Draw("same");

    canv->cd(2);
    auto legendClusterADC = new TLegend(0.1,0.7,0.48,0.9);
    for (Int_t chamberID =1; chamberID <=6; chamberID++){
        AverClusterADCh[chamberID]->SetMarkerColor(chamberID);
        //AverClusterADCh[chamberID]->SetLineColor(chamberID);
        AverClusterADCh[chamberID]->GetXaxis()->SetTitle("runID");
        AverClusterADCh[chamberID]->GetYaxis()->SetTitle("Average ADC");
        if (chamberID == 1){
            AverClusterADCh[chamberID]->Draw();
        } else{
            AverClusterADCh[chamberID]->Draw("same");
        }
        legendClusterADC->AddEntry(AverClusterADCh[chamberID],AverClusterADCh[chamberID]->GetName());
    }
    legendClusterADC->Draw("same");
    canv->Write();
    canv->SaveAs(Form("adcdistributionAllRun.png"));
    canv->Print(Form("%s)",pdfSaveName.Data()),"pdf");

    //plot all the the six GEM chamber in canv

    for (auto chamberID = 1; chamberID <=6 ; chamberID ++){
        canv->Clear();
        canv->cd();
        canv->SetTitle(Form("cluster_chamber%d_result",chamberID));
        canv->SetName(Form("cluster_chamber%d_result",chamberID));
        auto legendADCrun = new TLegend(0.1,0.7,0.48,0.9);
        Int_t counter = 1;
        for (auto iter  = allRunResultHistoBuff.begin(); iter!=allRunResultHistoBuff.end();iter++){
            Int_t  runID= iter->first;
            if(runID > 21286) break;
            auto chamberPlot = iter->second[3][chamberID];
            chamberPlot->SetLineColor(counter++);
            chamberPlot->SetLineWidth(3);
            chamberPlot->Scale(1.0/chamberPlot->GetEntries());
            chamberPlot->SetTitle(Form("GEM%d Cluster ADC",chamberID));
            chamberPlot->GetXaxis()->SetTitle("ADC");
            chamberPlot->GetYaxis()->SetTitle("Count");
            if (iter == allRunResultHistoBuff.begin()){
                chamberPlot->Draw();
            }else{
                chamberPlot->Draw("same");
            }
            legendADCrun->AddEntry(chamberPlot,chamberPlot->GetName());
        }
        legendADCrun->Draw("same");
        canv->Update();
        canv->Write();
        canv->Print(Form("%s)",pdfSaveName.Data()),"pdf");
    }


//    canv->Clear();
//    canv->cd(1);
//    canv->SetTitle(Form("cluster_chamber6_result"));
//    canv->SetName(Form("cluster_chamber6_result"));
//    auto legendADCrun = new TLegend(0.1,0.7,0.48,0.9);
//    //runID, plot index, chamberID, TH1F *
//    Int_t counter = 1;
//    TRandom *r = new TRandom();
//    for (auto iter = allRunResultHistoBuff.begin();iter!=allRunResultHistoBuff.end();iter ++){
//        Int_t  runID =iter->first;
//
//        if (runID > 21286) break;
//
//        auto chamberPlot = iter->second[3][6];
//        int color = (int) ((113-51)*r->Rndm()+51);
//        chamberPlot->SetLineColor(counter++);
//        chamberPlot->SetLineWidth(3);
//        chamberPlot->Scale(1.0/chamberPlot->GetEntries());
//        chamberPlot->SetTitle("GEM Cluster ADC");
//        if (iter == allRunResultHistoBuff.begin()){
//            chamberPlot->Draw();
//        }else{
//            chamberPlot->Draw("same");
//        }
//        legendADCrun->AddEntry(chamberPlot,chamberPlot->GetName());
//    }
//    legendADCrun->Draw("same");
//    canv->Update();
//    canv->Write();

    resultTFile.Write();
    resultTFile.Close();
}

void testgemCluster(){

}


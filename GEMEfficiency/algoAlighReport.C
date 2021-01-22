#include "TCanvas.h"
#include "TSystem.h"
#include "TChain.h"
#include "TString.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TF1.h"
#include "map"
#include "iostream"
#include "TLatex.h"
void algoAlighReport(TString fname="/home/newdriver/Storage/Research/PRex_Workspace/PREX-MPDGEM/PRexScripts/TrackAlignment/SBSAlignAlgo/Result/ResultTest2.root"){
    gROOT->ProcessLine(".x ~/rootlogon.C");

    gStyle->SetOptStat(0);

    gStyle->SetTitleSize( .05, "XYZ");
    gStyle->SetLabelSize( .05, "XYZ");
    gStyle->SetNdivisions(505,"XYZ");

    gStyle->SetStatW(0.25);
    //gStyle->SetStatH(0.45);

    TGaxis::SetMaxDigits(3);

    gROOT->ForceStyle();

    TCanvas *canvas =new TCanvas("Rec Report","Rec Report",1960,1080);
    canvas->Divide(3,2);

    TChain *chain = new TChain("Tout");
    chain->Add(fname);
    std::map<Int_t,TH1F *> hitResid;
    for (int i = 0 ; i < 6 ; i ++){
        gStyle->SetOptFit();
        hitResid[i] = new TH1F(Form("Residual_GEM%d",i+1),Form("Residual_GEM%d",i+1),1000,-2,2);
        auto chi2cut =50.0;
        chain->Project(hitResid[i]->GetName(),"HitXresid",Form("HitModule==%d && TrackChi2NDF<%g",i,chi2cut));

        auto binmax = hitResid[i]->GetMaximumBin();
        auto binlow = binmax,binhigh = binmax;
        while( hitResid[i]->GetBinContent(binlow--)>0.5*hitResid[i]->GetBinContent(binmax) ){};
        while( hitResid[i]->GetBinContent(binhigh++)>0.5*hitResid[i]->GetBinContent(binmax) ){};
        canvas->cd(i+1);

        hitResid[i]->Fit("gaus","","",hitResid[i]->GetBinCenter(binlow),hitResid[i]->GetBinCenter(binhigh));
        hitResid[i]->SetXTitle(Form("Track X excl. resid (mm), #chi^{2}/dof<%5.3g",chi2cut));


        hitResid[i]->Draw();


    }

    gStyle->SetNdivisions(510,"XYZ");
    gStyle->SetLabelSize(.06,"XYZ");
    gStyle->SetTitleSize(.06,"XYZ");

    gROOT->ForceStyle();

}
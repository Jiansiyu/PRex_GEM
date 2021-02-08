R__LOAD_LIBRARY(../bobParityData/libParity.so)
R__LOAD_LIBRARY(libTreeSearch-GEM.so)
R__LOAD_LIBRARY(../PRexAnalyzer/libprexCounting.so)

//-------------------------------------------------------------
const TString rawfolder = "/home/newdriver/PRex/PRex_Data/Raw";
const TString rootSaveFolder = "./Result";
const TString ThisDIR = ".";
//--------------------------------------------------------------

using namespace std;
void gemrun(Int_t runNo=0, Int_t lastevt=-1, Int_t split_in=1){
  //  R. Michaels, May 2014
  //  Steering script for Hall A analyzer

  char infile[300];
  //static const char* replay_dir_prefix = "./%s";
  THaApparatus* HRSR = new THaHRS("R","Right arm HRS");
  gHaApps->Add( HRSR );

  THaApparatus* HRSL = new THaHRS("L","Left arm HRS");
  gHaApps->Add( HRSL );
 //   HRSL->AddDetector( new TriFadcXscin("s0","s0 scintillator",kFALSE) );
 //   HRSL->AddDetector( new THaVDC("vdc", "Vertical Drift Chamber"));
  
  // add detectors that are not in the default config  
  //  HRSL->AddDetector( new THaScintillator("s0","Scintillator S0"));


  // done in rootlogon.C:  gSystem->Load("libParity.so");
  gHaApps->Add(new ParityData("P","HAPPEX Data"));

  if(runNo>20000){
  PREXStand *rprex= new PREXStand("RGEM", "GEM stand Right Arm");
  MPDGEMTracker *rgems = new MPDGEMTracker("rgems", "Collection of GEMs in Right arm");
  rprex->AddDetector(rgems);
  gHaApps->Add(rprex);
  gHaPhysics->Add( new THaGoldenTrack( "RGEM.gold", "RGEM Golden Track", "RGEM" ));
  }
  else
  {
  PREXStand *lprex= new PREXStand("LGEM", "GEM stand Right Arm");
  MPDGEMTracker *lgems = new MPDGEMTracker("lgems", "Collection of GEMs in Right arm");
  lprex->AddDetector(lgems);
  gHaApps->Add(lprex);
  gHaPhysics->Add( new THaGoldenTrack( "LGEM.gold", "LGEM Golden Track", "LGEM" ));
  }
  // e-p scattering
  Double_t amu = 931.494*1.e-3;  // amu to GeV
  Double_t pb208 = 208*amu;
  
  Double_t mass_tg  = pb208; // Helium

  // Electron kinematics
  THaPhysicsModule* EK_r = new THaElectronKine("EK_R",
					       "Electron kinematics in HRS-R",
					       "R",mass_tg);
  THaPhysicsModule* EK_l = new THaElectronKine("EK_L",
					       "Electron kinematics in HRS-L",
					       "L",mass_tg);
  
  gHaPhysics->Add( EK_r );
  gHaPhysics->Add( EK_l );

  gHaPhysics->Add( new THaGoldenTrack( "R.gold", "HRS-R Golden Track", "R" ));
  gHaPhysics->Add( new THaGoldenTrack( "L.gold", "HRS-L Golden Track", "L" ));


  THaScalerEvtHandler *lscaler = new THaScalerEvtHandler("Left","HA scaler event type 140");
  lscaler->SetDebugFile("./log/LeftScaler.txt");
  gHaEvtHandlers->Add (lscaler);

  THaScalerEvtHandler *rscaler = new THaScalerEvtHandler("Right","HA scaler event type 140");
  rscaler->SetDebugFile("./log/RightScaler.txt");
  gHaEvtHandlers->Add (rscaler);


  THaAnalyzer* analyzer = new THaAnalyzer;
  
  THaEvent* event = new THaEvent;
  for (Int_t nsplit=0;nsplit<1;nsplit++){  
  if(runNo>20000)
  //sprintf(infile,"/adaq1/data1/prexRHRS_%d.dat.%d",runNo,split_in);
  sprintf(infile,"%s/prexRHRS_%d.dat.%d",rawfolder.Data(),runNo,nsplit);
  else
  //sprintf(infile,"/adaq1/data1/prexLHRS_%d.dat.%d",runNo,split_in);
  sprintf(infile,"%s/prexLHRS_%d.dat.%d",rawfolder.Data(),runNo,nsplit);
 
  cout<<"replay: Try file "<<infile<<endl;
  THaRun *run;
  run = new THaRun(infile);
 // run->SetDataRequired( THaRunBase::kDate );
  TDatime now;
  run->SetDate( now );
  run->SetDataRequired( 0 );  // it was: ( THaRunBase::kDate );

  analyzer->EnableBenchmarks();
  analyzer->SetEvent( event );
  char outname[300];
  if(runNo>20000)
  sprintf(outname,"%s/prexRHRS_%d_%02d_gem.root",rootSaveFolder.Data(),runNo, split_in);
  else
  sprintf(outname,"%s/prexLHRS_%d_%02d_gem.root",rootSaveFolder.Data(),runNo, split_in);
  analyzer->SetOutFile( outname );
  analyzer->SetCutFile("./cuts/onlana.cuts");
 if(runNo>20000)
  analyzer->SetOdefFile("./outputDef/output_R.def");
 else  analyzer->SetOdefFile("./outputDef/output_L.def");
   analyzer->SetSummaryFile("./log/summary_example.log"); // optional
//
  run->SetLastEvent(lastevt);   // Number of events to process
  analyzer->Process(*run);
  }
}

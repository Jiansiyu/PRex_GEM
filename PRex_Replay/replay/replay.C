/*
 * PRex/CRex GEM Analaysis Scripts
 *
 */

//#include "iostream"
//#include "cstdlib"
//#include "THaApparatus.h"
//#include "TApplication.h"
//#include "fstream"
//#include "TString.h"
//#include "THaHRS.h"
//#include "THaAnalyzer.h"
//#include "THaPhysicsModule.h"
//#include "THaElectronKine.h"
//#include "THaGoldenTrack.h"
//#include "THaScalerEvtHandler.h"
//#include "THaRun.h"
//#include "THaEvent.h"

R__LOAD_LIBRARY(../bobParityData/libParity.so)
R__LOAD_LIBRARY(libTreeSearch-GEM.so)
R__LOAD_LIBRARY(../PRexAnalyzer/libprexCounting.so)
R__LOAD_LIBRARY(../FadcRasteredBeam/libTriFadcRasteredBeam.so)

using namespace std;

//-------------------------------------------------------------
const TString rawfolder = "/home/newdriver/PRex/PRex_Data/Raw";
const TString rootSaveFolder = "./Result";
const TString ThisDIR = ".";
//--------------------------------------------------------------

Bool_t IsFileExist(const Char_t * fname);

///
/// \param runNo
/// \param lastevt
/// \param target
void replay(Int_t runNo=0, Int_t lastevt=-1, TString target=""){

    TString HRS = "L";
    if (runNo > 20000) HRS = "R";

    THaApparatus *HRSApp;
    if (HRS == "R"){
        HRSApp = new THaHRS("R","Right arm HRS");
    } else{
        HRSApp = new THaHRS("L","Left arm HRS");
    }
    gHaApps->Add(HRSApp);
    gHaApps->Add(new ParityData("P","HAPPEX Data"));

    // add the gem detector to the HRS
    if (HRS == "R"){
        PREXStand *rprex= new PREXStand("RGEM", "GEM stand Right Arm");
        MPDGEMTracker *rgems = new MPDGEMTracker("rgems", "Collection of GEMs in Right arm");
        rprex->AddDetector(rgems);
        gHaApps->Add(rprex);
        gHaPhysics->Add( new THaGoldenTrack( "RGEM.gold", "RGEM Golden Track", "RGEM" ));
    } else{
        PREXStand *lprex= new PREXStand("LGEM", "GEM stand Right Arm");
        MPDGEMTracker *lgems = new MPDGEMTracker("lgems", "Collection of GEMs in Right arm");
        lprex->AddDetector(lgems);
        gHaApps->Add(lprex);
        gHaPhysics->Add( new THaGoldenTrack( "LGEM.gold", "LGEM Golden Track", "LGEM" ));
    }

    // code  used for decode the BPM and the raster current
    THaApparatus *Lrb = new TriFadcRasteredBeam(Form("%srb",HRS.Data()),"Raster Beamline for FADC ");
    gHaApps->Add(Lrb);

    // e-p scattering
    Double_t amu = 931.494028*1.e-3;  // amu to GeV
    Double_t he4 = 4*amu;
    Double_t pb208 = 208*amu;
    Double_t c12 = 12*amu-.511e-3*6;
    Double_t ca40= 40*amu;
    Double_t ca48= 48*amu;

    Double_t mass_tg  = pb208; // Helium

    // Electron kinematics
    if (HRS == "R"){
        THaPhysicsModule* EK_r = new THaElectronKine("EK_R",
                                                     "Electron kinematics in HRS-R",
                                                     "R",mass_tg);
        gHaPhysics->Add( EK_r );
        gHaPhysics->Add( new THaGoldenTrack( "R.gold", "HRS-R Golden Track", "R" ));

        THaScalerEvtHandler *rscaler = new THaScalerEvtHandler("Right","HA scaler event type 140");
        rscaler->SetDebugFile(Form("%s/log/RightScaler.txt",ThisDIR.Data()));
        gHaEvtHandlers->Add (rscaler);
    } else{
        THaPhysicsModule* EK_l = new THaElectronKine("EK_L",
                                                     "Electron kinematics in HRS-L",
                                                     "L",mass_tg);
        gHaPhysics->Add( EK_l );
        gHaPhysics->Add( new THaGoldenTrack( "L.gold", "HRS-L Golden Track", "L" ));

        THaScalerEvtHandler *lscaler = new THaScalerEvtHandler("Left","HA scaler event type 140");
        lscaler->SetDebugFile(Form("%s/log/LeftScaler.txt",ThisDIR.Data()));
        gHaEvtHandlers->Add (lscaler);
    }

    THaAnalyzer* analyzer = new THaAnalyzer;
    TString outname = Form("%s/prex%sHRS_%d_%d.root",rootSaveFolder.Data(),HRS.Data(),runNo,lastevt);
    analyzer->SetOutFile(outname.Data());
    analyzer->SetCutFile(Form("%s/cuts/onlana_raster_%s.cuts",ThisDIR.Data(),HRS.Data()));
    analyzer->SetOdefFile(Form("%s/outputDef/output_%s.def",ThisDIR.Data(),HRS.Data()));
    analyzer->SetSummaryFile("./log/summary_example.log"); // optiona
    analyzer->EnableBenchmarks();

    // load the data
    TString oldfilename="";
    int found;
    Int_t FirstEventNum = 0;
    THaRun *oldrun=0, *run;
    Bool_t exit = false;
    if(lastevt>=0) lastevt+= FirstEventNum;
    THaEvent* event = new THaEvent;

    TString infile;
    for (Int_t nsplit=0;!exit; nsplit++){
        found = 0 ;
        infile = Form("%s/prex%sHRS_%d.dat.%d",rawfolder.Data(),HRS.Data(),runNo,nsplit);
        if( IsFileExist(infile) )
            found = 1;

        if (! found || oldfilename == infile){
            exit = true;
        } else{
            oldfilename = infile;
            if (oldrun){
                run = new THaRun(*oldrun);
                run->SetFilename(infile);
            }
            else // if this is the first run
            {
                run = new THaRun(infile);
            }

            if(lastevt>=0) run->SetLastEvent(lastevt);
            run->SetFirstEvent(FirstEventNum);

            try{
                analyzer->Process(run);
            }
            catch( exception& e)
            {
                cerr << "Unhandled exception during replay: " << e.what() << endl;
                cerr << "Exiting." << endl;
                run->Close();
                break;
            }
            run->Close();
            if( !oldrun ) oldrun = run;
        }
    }

}


Bool_t IsFileExist(const Char_t * fname){
    fstream testfile;
    testfile.open(fname,ios_base::in);
    Bool_t isopen = testfile.is_open();
    testfile.close();
    return isopen;
}

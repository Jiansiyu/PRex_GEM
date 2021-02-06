/*
 * PRex CRex GEM run Varification function
 */
#include <iostream>
#include "fstream"
#include "ostream"
#include "TApplication.h"
#include "runVerifier.h"
#include "vector"
#include "TString.h"
#include "TSystem.h"
#include "unistd.h"
#include "limits.h"
int main(int argc, char* argv[]) {

    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);

    TString hostStr=std::string(hostname);

    TString pathTemplate("/home/newdriver/PRex/PRex_Data/Raw/");
    if (hostStr.Contains("jlab.org") || hostStr.Contains("aonl") ){
        pathTemplate="/cache/halla/happexsp/raw/";
        std::cout<<"Work on Ifarm System: "<<pathTemplate<<std::endl;
    } else{
        pathTemplate="/home/newdriver/PRex/PRex_Data/Raw/";
        std::cout<<"Work on local System: "<<pathTemplate<<std::endl;
    }

    TApplication theApp("runVerifier",&argc,argv);
    runVerifier verifier;
    if (theApp.Argc() < 2){
        return  -1;
    }

    std::vector<TString> fileList;
    for ( int i  = 1 ; i < argc; i ++){

        TString runIDStr(theApp.Argv()[i]);
        Int_t  runID = std::stoi(runIDStr.Data());

        TString HRS = "L";
        if (runID > 20000)HRS = "R";
        TString filename = Form("%s/prex%sHRS_%d.dat.0",pathTemplate.Data(),HRS.Data(),runID);

        if (verifier.isValidGEMRun(filename.Data())){
            std::ofstream txtfileio("./runVerifierResult.csv",std::ofstream::app);
            auto writeStr=Form("%d  %s",runID,"True");
            txtfileio<<writeStr<<std::endl;
            txtfileio.close();
            std::cout<<"Find Dataframe for run "<< runID<<std::endl;
        } else{
            std::ofstream txtfileio("./runVerifierResult.csv",std::ofstream::app);
            auto writeStr=Form("%d  %s",runID,"False");
            txtfileio<<writeStr<<std::endl;
            txtfileio.close();
            std::cout<<"CAN NOT FIND Data Frame "<<runID<<std::endl;
        }
    }
    return  0;
}

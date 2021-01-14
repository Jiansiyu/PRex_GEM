/*
 * PRex CRex GEM run Varification function
 */
#include <iostream>
#include "TApplication.h"
#include "runVerifier.h"
#include "vector"
#include "TString.h"
int main(int argc, char* argv[]) {

    TApplication theApp("runVerifier",&argc,argv);

    runVerifier verifier;
    if (theApp.Argc() < 2){
        return  -1;
    }

    if (theApp.Argc() == 2){
        return verifier.isValidGEMRun(theApp.Argv()[1]);
    }

    std::vector<TString> veriArray;
    for (int i = 1; i < theApp.Argc(); ++i) {
        veriArray.push_back(theApp.Argv()[i]);
    }
    verifier.isValidGEMRun(veriArray);
    return  0;
}

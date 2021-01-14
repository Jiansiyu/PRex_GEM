//
// Created by newdriver on 1/13/21.
//

#include "runVerifier.h"
#include "iostream"
#include "evio.h"
bool runVerifier::isValidGEMRun(TString f) {
    return true;
}

bool runVerifier::isValidGEMRun(std::vector<TString> fList) {
    for (auto item : fList){
        validRunBuff[item] = isValidGEMRun(item);
    }
    std::cout<<"------------------\n \t\t\t Summary"<<std::endl;
    for (auto iter = validRunBuff.begin();iter!= validRunBuff.end();iter++){
        std::cout<<"\t"<< iter->first.Data()<<"\t "<<iter->second<<std::endl;
    }
    std::cout<<"------------------End of the Report\n \n"<<std::endl;
    return true;
}

bool runVerifier::Save(TString f) {

}
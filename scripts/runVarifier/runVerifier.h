//
// Created by newdriver on 1/13/21.
//

#ifndef RUNVARIFIER_RUNVERIFIER_H
#define RUNVARIFIER_RUNVERIFIER_H

#include "TString.h"
#include "vector"
#include "map"
#include "vector"
class runVerifier {
public:
    runVerifier(){};
    ~runVerifier(){};
    bool isValidGEMRun(TString f);
    bool isValidGEMRun(std::vector<TString> fList);
    bool Save(TString f);
private:
    int ProcessSspBlock(const std::vector<uint32_t> &block_vec);
};


#endif //RUNVARIFIER_RUNVERIFIER_H

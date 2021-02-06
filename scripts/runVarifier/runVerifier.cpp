//
// Created by newdriver on 1/13/21.
//

#include "runVerifier.h"
#include "iostream"
#include "evio.h"
#include "evioUtil.hxx"
#include "evioFileChannel.hxx"
#include "map"


#include "TSystem.h"
bool runVerifier::isValidGEMRun(std::vector<TString> fList) {
    for (auto filename : fList){
        isValidGEMRun(filename.Data());
    }
    return true;
}

/// Read the first teen event and check the GEM dataframe
/// \param f
/// \return
bool runVerifier::isValidGEMRun(TString f) {

    const int mpd_tag=3561,fadc_tag=3,tdc_tag=6;
    Long64_t entry = 0;
    try {
        evio::evioFileChannel chan(f.Data(),"r");
        chan.open();
        // check the first ten event and search for the MPD dataframe
        while (chan.read() && entry < 100){
            std::map<int, Double_t> mTDC;
            std::map<int, Double_t> mFADC;
            evio::evioDOMTree event(chan);
            evio::evioDOMNodeListP mpdEventList = event.getNodeList( evio::isLeaf() );
            evio::evioDOMNodeList::iterator iter;
            for (iter = mpdEventList->begin(); iter != mpdEventList->end(); iter++){
                int banktag = (*iter)->tag;
//                std::cout<<"Bank Tag::"<< banktag<<std::endl;
                switch (banktag) {
                    case mpd_tag:
                        {
//                        std::cout<<"Find MPD Tag  ("<<chan.getFileName()<<std::endl;
                        vector<uint32_t> *block_vec = (*iter)->getVector<uint32_t>();
                        if(block_vec->size()!=33063) break;
                        entry+=ProcessSspBlock(*block_vec);
                        return true;
                        }
                        break;
                    default:
                        entry += 1;
                        break;
                }
            }
        }

    }catch (evio::evioException e){
        std::cout<<e.toString()<<std::endl;
    }
    return false;
}

int runVerifier::ProcessSspBlock(const std::vector<uint32_t> &block_vec) {
    int vec_size = block_vec.size();
    int NofEvtInBlk=0;
    NofEvtInBlk++;
//    ProcessSingleSspEvent(block_vec,0);
    return NofEvtInBlk;
}

bool runVerifier::Save(TString f) {

}
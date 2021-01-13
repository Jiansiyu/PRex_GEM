#ifndef __INPUT_HANDLER_H__
#define __INPUT_HANDLER_H__

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cassert>

#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <stdio.h> //for getchar()

//evio
#include "evioUtil.hxx"
#include "evioFileChannel.hxx"

#include "raw_decoder.h"

using namespace std;
using namespace evio;

class InputHandler
{
 private:
  //+++++++++++++++++++++Private Variables++++++++++++++++++++++//
  
  //################config file parameters############: 
  int nEvt;                   //Max Number of Events to Analyze
  string runtype;
  int raw_co,raw_row;
  string PedFiletoSAVE;
  string PedFiletoLOAD;
  string HitRootFiletoSAVE;
  string MappingFile;
  int NbFile;                 //Total Number of Files to Analyze
  
  //################Mapping###########################
  map<int,vector<int> > mMapping;
  /*
   *mMapping[mpd_id | adc_id][0]----GEMId
   *mMapping[mpd_id | adc_id][1]----Axis X(0) Y(1)
   *mMapping[mpd_id | adc_id][2]----Pos
   *mMapping[mpd_id | adc_id][3]----Invert(0) notInvert(1)
   */
  //#################rootFileMode-----pedestal rms##################
  map<int,vector<int> > mvPedestalMean; 
  map<int,vector<int> > mvPedestalRMS;
  //map[mpd_id | adc_id][stripid]
  
  //#################rootFileMode-----tree and branch##################
  TTree *Hit;
  //The only event ID tracker, increament only in routine "ProcessSingleSspEvent()"
  Int_t EvtID=0;
  //GEM branch
  Int_t nch=0,*Vstrip,*VdetID,*VplaneID,*adc0,*adc1,*adc2,*adc3,*adc4,*adc5;
  //FADC branch
  Int_t nfadc=0,*fCH,*ftiming,*fadc;
  //TDC branch
  Int_t ntdc=0, *tCH; 
  Double_t *ttiming;
  


  //+++++++++++++++++++++Private Routines++++++++++++++++++++++//
  //#################Initializing+++++++++++++++++//
  int Initialize();
  //load general config file
  int LoadConfigFile();
  int LoadMapping();
  //load for pedestal mode
  int InitPedestalToWrite();
  //load for root file mode
  int LoadPedestalToRead();
  int InitRootFile();

  //##################Event Processing+++++++++++++++++//
  int ProcessSspBlock(const vector<uint32_t> &block_vec);
  int ProcessSingleSspEvent(const vector<uint32_t> &block_vec, int istart);
  int CalSinglePedestal(const map<int, vector<int> > &mTsAdc);
  int FillRootTree(const map<int, vector<int> > &mmHit);

  //##################Event Summarizing+++++++++++++++++//
  int SummPedestal();  
  int SummRootFile();  
  
 public:
  
  InputHandler();
  ~InputHandler();
  int ProcessAllFiles();
  int Summary();


  //bool MPDEventChooser(const evioDOMNodeP pNode);


  string filenameList[1000];
  int NbofInputFile=0;

private:
 
  //----------------------------------------------
  map<int, TH1F*> mAPVRawHistos;//mpd+adc
  map<int, vector<int> > mAPVRawTSs;//mpd+adc


  map<int, vector<int> > mTsAdc;//adc for each strip after common mode subtraction//mpd+adc+strip


  map<int, TH1F* > mPedestalHisto;//mpd+adc+strip
  map<int, TH1F* > mPedestalMean;
  map<int, TH1F* > mPedestalRMS;

  //----------------------------------------------

  int GetMPD_ID(const int &hybridID){int MPD_ID=0;     MPD_ID=(hybridID&0xff000)>>12;  return MPD_ID;}//Get MPD ID from hybrid ID(0-255)
  int GetADC_ch(const int &hybridID){int ADC_ch=0;     ADC_ch=(hybridID&0x00f00)>>8;  return ADC_ch;}//Get ADC ch from hybrid ID(0-15)
  int GetStripNb(const int &hybridID){int StripNb=0;  StripNb=(hybridID&0x000ff);  return StripNb;}//Get Strip Nb from hybrid ID(0-255)

  int GetDet_ID(int hybridID){int Det_ID=0;       Det_ID=(hybridID&0xfe000)>>13;   return Det_ID;}         //0-128
  int GetPlane_ID(int hybridID){int Plane_ID=0; Plane_ID=(hybridID&0x01000)>>12;   return Plane_ID;} //0,1
  int GetStripPos(int hybridID){int StripPos=0; StripPos=(hybridID&0x00fff);   return StripPos;} //0-4096



  TFile *f;
  TCanvas *cRaw0,*cRaw1,*cRaw2,*cRaw3,*cRaw4,*cRaw5,*cRaw6,*cRaw7, *cRaw8,*cRaw9,*cRaw10,*cRaw11;
  TCanvas *cRaw;
  TCanvas *cHit;

  RawDecoder *fRawDecoder;
};

#endif

/*******************************************************************************************
 * Usage:
 *
 * Input:  1), A buffer contains purely SRS data, and the buffer size in [int] unit
 *
 *         2), Or, SRS data filled in a vector<int>
 *
 * Output: 1), map<mpd_id, map<ch_id, vector<int> > >, vector: adc values (all time samples)
 *                map<int, map<int, vector<int> > > GetDecoded();
 *
 *         2), map<mpd_id, map<ch_id, TH1F* > > , TH1F*: adc values filled in histograms
 *             No need to worry memory leakage, this class owns every object it produced.
 *                map<int, map<int, TH1F* > > GetAPVRawHisto();
 * *****************************************************************************************/

#ifndef __RAW_DECODER_H__
#define __RAW_DECODER_H__

#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>

#include <map>
#include <vector>
#include <stdio.h>
#include <TH1F.h>

using namespace std;

class RawDecoder
{
public:
  RawDecoder( const vector<uint32_t> &, int, int);
  RawDecoder( unsigned int *, int);
  ~RawDecoder();

private:
  unsigned int * buf;
  int fBuf;
  //  map<int, map<int, vector<int> > > mAPVRawSingleEvent;
  //  map<int, map<int, TH1F* > > mAPVRawHisto;

  map<int, vector<int> >  mAPVRawSingleEvent;//mpd+adc
  map<int, TH1F* >  mAPVRawHisto;//mpd+adc
  int GetMPD_ID(int hybridID){int MPD_ID=0;  MPD_ID=(hybridID&0xff000)>>12;  return MPD_ID;}//Get MPD ID from hybrid ID(0-255)
  int GetADC_ch(int hybridID){int ADC_ch=0;  ADC_ch=(hybridID&0x00f00)>>8;  return ADC_ch;}//Get ADC ch from hybrid ID(0-15)
  int GetStripNb(int hybridID){int StripNb=0;  StripNb=(hybridID&0x000ff);  return StripNb;}//Get Strip Nb from hybrid ID(0-255)


  TFile *f;
  //int ChNb[128] ={0, 32, 64, 96, 8, 40, 72, 104, 16, 48, 80, 112, 24, 56, 88, 120, 1, 33, 65, 97, 9, 41, 73, 105, 17, 49, 81, 113, 25, 57, 89, 121, 2, 34, 66, 98, 10, 42, 74, 106, 18, 50, 82, 114, 26, 58, 90, 122, 3, 35, 67, 99, 11, 43, 75, 107, 19, 51, 83, 115, 27, 59, 91, 123, 4, 36, 68, 100, 12, 44, 76, 108, 20, 52, 84, 116, 28, 60, 92, 124, 5, 37, 69, 101, 13, 45, 77, 109, 21, 53, 85, 117, 29, 61, 93, 125, 6, 38, 70, 102, 14, 46, 78, 110, 22, 54, 86, 118, 30, 62, 94, 126, 7, 39, 71, 103, 15, 47, 79, 111, 23, 55, 87, 119, 31, 63, 95, 127};
  int ChNb[128] ={1, 33, 65, 97, 9, 41, 73, 105, 17, 49, 81, 113, 25, 57, 89, 121, 3, 35, 67, 99, 11, 43, 75, 107, 19, 51, 83, 115, 27, 59, 91, 123, 5, 37, 69, 101, 13, 45, 77, 109, 21, 53, 85, 117, 29, 61, 93, 125, 7, 39, 71, 103, 15, 47, 79, 111, 23, 55, 87, 119, 31, 63, 95, 127, 0, 32, 64, 96, 8, 40, 72, 104, 16, 48, 80, 112, 24, 56, 88, 120, 2, 34, 66, 98, 10, 42, 74, 106, 18, 50, 82, 114, 26, 58, 90, 122, 4, 36, 68, 100, 12, 44, 76, 108, 20, 52, 84, 116, 28, 60, 92, 124, 6, 38, 70, 102, 14, 46, 78, 110, 22, 54, 86, 118, 30, 62, 94, 126};//this is original and believed to be the right one,Jan 09 2017

  



public:
  void Decode();
  map<int, vector<int> > GetStripTsAdcMap();
  map<int, vector<int> > ZeroSup(map<int,vector<int> > mMapping, map<int,vector<int> > mPedestalMean, map<int,vector<int> > mPedestalRMS);
  map<int, vector<int> > GetDecoded();
  //map<int, map<int, map<int, vector<int> > > > mPedestalTsAdc;
  //map<int, map<int, TH1F* > > GetAPVRawHisto();
  map<int, TH1F* > DrawRawHisto(TCanvas *c);
  //  map<int, TH1F* > DrawRawHisto(TCanvas *c0,TCanvas *c1,TCanvas *c2);
  //  map<int, TH1F* > DrawRawHisto(TCanvas *c0,TCanvas *c1,TCanvas *c2,TCanvas *c3, TCanvas *c4, TCanvas *c5, TCanvas *c6,TCanvas *c7,TCanvas *c8, TCanvas *c9, TCanvas *c10, TCanvas *c11);
  map<int, vector<int> > DrawHits(map<int,vector<int> > mMapping, map<int,vector<int> > mPedestalMean, map<int,vector<int> > mPedestalRMS, TCanvas *c);
  map<int, vector<int> > mPedestalTsAdc;
};

#endif

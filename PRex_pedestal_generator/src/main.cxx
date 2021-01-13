#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include <stdio.h> // for getchar()
#include <string>

#include <TH1F.h>
#include <TApplication.h>
#include <TCanvas.h>
//#include <TROOT.h>

#include "raw_decoder.h"
#include "input_handler.h"
#include "GEMConfigure.h"

#include "BenchMark.h"

using namespace std;

int main(int argc, char* argv[])
{
  TApplication theApp("app", &argc, argv);
  //gROOT->SetBatch();
  BenchMark timer;
  InputHandler inputHandler;
  int entries;
  entries = inputHandler.ProcessAllFiles();
  inputHandler.Summary();
  cout<<"Total Events: "<<entries<<endl;
  cout<< "time used: "<<timer.GetElapsedTime()<<" ms"<<endl;
  return 0;
}



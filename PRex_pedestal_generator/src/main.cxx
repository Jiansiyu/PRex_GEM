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

extern std::string gemRunConfigFname="config/gem.cfg";

using namespace std;

int main(int argc, char* argv[])
{
  if (argc == 2) {
      gemRunConfigFname = argv[1];
  }

  TApplication theApp("app", &argc, argv);

  std::cout<<"Using configure file \""<< gemRunConfigFname<<"\""<<std::endl;
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



//==================================================================//
//                                                                  //
// Xinzhan Bai                                                      //
// 03/20/2016                                                       //
// xb4zp@virginia.edu                                               //
//==================================================================//

#ifndef __GEMCONFIGURE_H__
#define __GEMCONFIGURE_H__

#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>

using namespace std;

class GEMConfigure
{
public:
    GEMConfigure(const char * file );
    GEMConfigure();
    ~GEMConfigure();

    std::string GetRunType();
    string GetMapping();
    string GetSavePedPath();
    string GetLoadPedPath();
    int GetNumEvt();
    int GetRawCo();
    int GetRawRow();

    void LoadConfigure();

    // make following variables public
    // input files to be analyzed...
    string fileList[1000];
    string fileHeader;
    int nFile;
    int evioStart;
    int evioEnd;

    // file to save physics analysis results
    string hitrootfile_path;

private:
    string configure_file;

    string runType;
    int raw_CO, raw_ROW;
    int maxCluster;
    int minCluster;
    int nEvent;

    string mapping;
    string save_pedestal;
    string load_pedestal;
};

#endif

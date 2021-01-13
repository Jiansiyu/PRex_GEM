#include "GEMConfigure.h"
#include <unistd.h>

using namespace std;

GEMConfigure::GEMConfigure()
{
    nFile = 0;
    configure_file = "config/gem.cfg";
    cout<< "GEMConfigure: input configure file: "<< configure_file << endl;
    //LoadConfigure();
}

GEMConfigure::GEMConfigure(const char * file)
{
    configure_file = file;
    //LoadConfigure();
}

GEMConfigure::~GEMConfigure()
{
}

void GEMConfigure::LoadConfigure()
{
    ifstream filestream;
    filestream.open( configure_file.c_str(), ifstream::in);
    if(!filestream)
    {
        cout<<"GEMConfigure: configure file open error..."
	    <<endl;
    }
    string line;
    int kk_file = 0;
    while( getline(filestream, line) )
    {
	line.erase( remove_if(line.begin(), line.end(), ::isspace), line.end() ); // strip spaces
	if(line.find("#") == 0) continue; //skip comments

	char* tokens = strtok( (char*)line.data(), " :,");
	while( tokens != NULL)
	{
	    string s = tokens;
//        cout<<"tokens = " << s << endl;
	    if(s == "RUNTYPE")
	    {
		tokens = strtok(NULL, " :,");
		runType = tokens;
//        cout<<"runType = " << tokens << endl;
		tokens = strtok(NULL, " :,");
		raw_CO = atoi(tokens);
//        cout<<"raw_CO = " << tokens << endl;
		tokens = strtok(NULL, " :,");
		raw_ROW= atoi(tokens);
//        cout<<"raw_ROW = " << tokens << endl;
	    }
	    else if(s == "MAPPING")
	    {
		tokens = strtok(NULL, " :,");
		mapping = tokens;
//        cout<<"mapping = " << tokens << endl;
	    }
	    else if(s=="SAVEPED")
	    {
		tokens = strtok(NULL, " :,");
		save_pedestal = tokens;
//        cout<<"save_pedestal = " << tokens << endl;
	    }
	    else if(s=="NEVENT")
	    {
		tokens = strtok(NULL, " :,");
		nEvent = atoi(tokens);
	    }
	    else if(s=="LOADPED")
	    {
		tokens = strtok(NULL, " :,");
		load_pedestal = tokens;
	    }
	    /* EC
	    else if(s=="NFILE")
	    {
		tokens = strtok(NULL, " :,");
		nFile = atoi(tokens);
	    }
	    */
	    else if(s=="INPUTFILE")
	    {
		tokens = strtok(NULL, " :,");
		fileHeader = tokens;
		tokens = strtok(NULL, " :,");
		evioStart = atoi(tokens);
		tokens = strtok(NULL, " :,");
		evioEnd = atoi(tokens);

		for(int i=evioStart; i<= evioEnd;i++)
		{
		  string fff = fileHeader + "." + to_string(i);
		  if (access( fff.c_str(), F_OK ) != -1) { // file exists   
		    fileList[kk_file++] = fff;
		    cout << "GEMConfigure: Input File Chunk " << fff << " added (total chucks " << kk_file << endl;
		  }
		}
	    }
	    else if(s == "HitRootFile")
	    {
		tokens = strtok(NULL, " :,");
		hitrootfile_path = tokens;
	    }

	    else
	    {
		tokens = strtok(NULL, " :,");
	    }
	}
    }

    nFile = kk_file; // EC
    assert(kk_file == nFile);
    cout << "GEMConfigure::LoadConfigure() done"<<endl;
    
}

string GEMConfigure::GetRunType()
{
    return runType;
}

string GEMConfigure::GetMapping()
{
    return mapping;
}

string GEMConfigure::GetSavePedPath()
{
    return save_pedestal;
}

string GEMConfigure::GetLoadPedPath()
{
    return load_pedestal;
}

int GEMConfigure::GetNumEvt()
{
    return nEvent;
}
int GEMConfigure::GetRawCo()
{
    return raw_CO;
}
int GEMConfigure::GetRawRow()
{
    return raw_ROW;
}

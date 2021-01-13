/* 
Prex Online dosplay class
Used for display the the hit on the GEM and the VDC
 */
#include <string>
#include <map>
#include <vector>
#include <TH1F.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TMatrixD.h>
class prex_online_display
{
private:
    /* data */
public:

    prex_online_display(std::string display_engine="root");
    ~prex_online_display();
                    // detector ID, dimesion, strip, value
    void Plot(std::map<int,std::map<int,std::map<int,int>>>);
private:
    TCanvas *a=new TCanvas("Raw_frame_Monitor","Raw Frame Monitor",0,0,1080, 1000);
    
    
};

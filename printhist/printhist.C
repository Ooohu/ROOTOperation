#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TDirectory.h"
#include <sys/stat.h>

using namespace std;

void printhist(){
	
	char file_name[] = "/uboone/app/users/klin/ubflux_2_sbnflux/testing_directory/sbnmakehist_tool/test_kminus.root";
	char hist_name[] = "numu_ms_kminus_Flux/hEnumu_kminus_Flux_ms_995";
	TFile* f1=TFile::Open(file_name); //open a root file

	TH1D* h1 = (TH1D*) f1->Get(hist_name);
	TCanvas c1("c1","A hist",10,10,700,900);
	h1->Draw("hist");
	c1.SaveAs("./test.png","png");
	

}

#include <sys/stat.h>

using namespace std;

void Hist2Hist(){
	
	char file_name[] = "../SampleRoot/test_kminus.root";
	char hist_name[] = "numu_ms_kminus_Flux/hEnumu_kminus_Flux_ms_995";
	TFile* f1=TFile::Open(file_name); //open a root file

	TH1D* h1 = (TH1D*) f1->Get(hist_name);
	TCanvas c1("c1","A hist",10,10,700,900);
	h1->Draw("hist");
	c1.SaveAs("./test.png","png");
}

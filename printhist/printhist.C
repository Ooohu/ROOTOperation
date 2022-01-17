#include <sys/stat.h>

using namespace std;

void printhist(){
	
//	char file_name[] = "/uboone/app/users/klin/ubflux_2_sbnflux/testing_directory/sbnmakehist_tool/test_kminus.root";
//	char hist_name[] = "numu_ms_kminus_Flux/hEnumu_kminus_Flux_ms_995";
	char file_name[] = "/uboone/app/users/klin/oct2021_flux_ub2sbn/testing/sbndmakehist_tool/tmp_test.root";
	char hist_name[] = "numu_ms_horncurrent_Flux/hEnumu_horncurrent_Flux_ms_995";

	
	TFile* f1=TFile::Open(file_name); //open a root file

	TH1D* h1 = (TH1D*) f1->Get(hist_name);
	TCanvas c1("c1","A hist",10,10,700,900);
	h1->Draw("hist");
	c1.SaveAs("./test.png","png");
	

}

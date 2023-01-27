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

void TTree2Hist(){

//Load files
	TFile* Prun1 = TFile::Open("/uboone/data/users/kmistry/work/MCC9/searchingfornues/ntuple_files_v7/neutrinoselection_filt_run1_overlay.root");
	TTree* Tree1 = 0;//create an empty object for new stuff.
	Prun1->GetObject("nuselection/NeutrinoSelectionFilter", Tree1);

//Prepare histograms
	Tree1->Draw("trk_len_v>>ev1(30,0,150)","1","hist");
	TH1* hist1;
	hist1 = (TH1*)gDirectory->Get("ev1");

	//want some confighurations
	hist1->SetStats(false);
	hist1->SetTitle("Run1 vs. Run4 PeLEE Ntuples");
	hist1->SetLineColor(kRed-9);
//Draw them & saved


	TCanvas c1("c1","A hist",10,10,700,900);
	hist1->Draw("hist");
	c1.SaveAs("./test.png","png");

}

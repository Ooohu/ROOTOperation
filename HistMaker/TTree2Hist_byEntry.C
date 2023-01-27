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

void TTree2Hist_byEntry(){

//Load files
	TFile* Prun1 = TFile::Open("/uboone/data/users/kmistry/work/MCC9/searchingfornues/ntuple_files_v7/neutrinoselection_filt_run1_overlay.root");

	TTree* Tree1 = (TTree*) Prun1->Get("nuselection/NeutrinoSelectionFilter");
	std::vector< float > ftrk_len_v;
	std::vector< float >* pftrk_len_v = &ftrk_len_v;
	Tree1->SetBranchAddress("trk_len_v", &pftrk_len_v);

//Prepare histograms
	TH1* hist1 = new TH1F("hist_name", "hist description",10,0,100);//bins,min,max

	int nEntries = Tree1->GetEntries();
	std::cout<<"TTree with entries number "<<nEntries<<std::endl;

	for( int index = 0; index < nEntries; index ++){
		Tree1->GetEntry(index);
		hist1->Fill(ftrk_len_v[0]);
	}

	//want some confighurations
	hist1->SetStats(false);
	hist1->SetTitle("Run1 vs. Run4 PeLEE Ntuples");
	hist1->SetLineColor(kRed-9);
//Draw them & saved

std::cout<<"Hist sum "<<hist1->Integral()<<std::endl;

//	TCanvas c1("c1","A hist",10,10,700,900);
//	hist1->Draw("hist");
//	c1.SaveAs("./test.png","png");

}

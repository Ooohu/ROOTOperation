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

void TwoTTrees2Hist(){

//Load files
	TFile* Prun1 = TFile::Open("/uboone/data/users/kmistry/work/MCC9/searchingfornues/ntuple_files_v7/neutrinoselection_filt_run1_overlay.root");
	TTree* Tree1 = 0;//create an empty object for new stuff.
	Prun1->GetObject("nuselection/NeutrinoSelectionFilter", Tree1);


	TFile* Prun4 = TFile::Open("/pnfs/uboone/scratch/users/klin/MCC9/v08_00_00_46/NuMI_Run4_Ntuple_PeLEE/ntuple0/v5/test.root");
	TTree* Tree4 = 0;//create an empty object for new stuff.
	Prun4->GetObject("nuselection/NeutrinoSelectionFilter", Tree4);

//Prepare histograms
	Tree1->Draw("trk_len_v>>ev1(30,0,150)","1","hist");
	TH1* hist1;
	hist1 = (TH1*)gDirectory->Get("ev1");

	Tree4->Draw("trk_len_v>>ev4(30,0,150)","1","hist");
	TH1* hist4;
	hist4 = (TH1*)gDirectory->Get("ev4");

	//want some confighurations
	hist1->SetStats(false);
	hist1->SetTitle("Run1 vs. Run4 PeLEE Ntuples");
	double evt_ratio = Tree4->GetEntries()*1.0/Tree1->GetEntries();
	std::cout<<"Event Ratio "<<evt_ratio<<" from "<<Tree4->GetEntries()<<" "<<Tree1->GetEntries()<<std::endl;
	hist1->Scale(evt_ratio);
	hist1->SetLineColor(kRed-9);
	hist4->SetLineColor(kBlue-9);
//Draw them & saved


	TCanvas c1("c1","A hist",10,10,700,900);
	hist1->Draw("hist");
	hist4->Draw("hist same");
	c1.SaveAs("./test.png","png");

}

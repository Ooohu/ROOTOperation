#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

void draw_axises(TH2F* hist, TString name){
	if(name.Length()>1) hist->SetTitle(name);
	hist->Draw("COLZ");
	hist->GetXaxis()->SetTitle("Lepton Energy [GeV]");
	hist->GetYaxis()->SetTitle("UZ");
}

void TwoDhistRatio(){

	TString dir = "../SampleRoot/";
	vector<TString> Inputfiles = {"Reading_Output.root","Reading_Output.root"};

	TFile* f0=TFile::Open(dir+Inputfiles[0],"READ"); //open a root file
	TTree* input_tree0 = (TTree*) f0->Get("TreeName");

	TFile* f1=TFile::Open(dir+Inputfiles[1],"READ"); //open a root file
	TTree* input_tree1 = (TTree*) f1->Get("TreeName");

	input_tree0->Draw("entry:entry>>ev0(10,0,20,10,0,20)","1","COLZ");
	input_tree1->Draw("entry:entry>>ev1(10,0,20,10,0,20)","1","COLZ");

	TH2F* twodhist0;
	TH2F* twodhist1;
	
	twodhist0 = (TH2F*) gDirectory->Get("ev0");
	twodhist1 = (TH2F*) gDirectory->Get("ev1");

	twodhist0->Divide(twodhist1);

//	TCanvas c1("c1","A hist",10,10,700,900);
//	twodhist0->Draw("COLZ");
//	draw_axises(twodhist0, "Ratio 2D Hist");
//	c1.SaveAs("./test.png","png");
}


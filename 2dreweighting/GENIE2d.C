#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

int GENIE2d(){
	
//	TString dir = "/uboone/data/users/moon/MCC8_MCinfo_ForKeng/";
//	vector<TString> files = {"MCC8_NueIntrinsic_MCInfo.root","MCC8_BnbOverlay_MCInfo.root"};
	TString dir = "/scratch/condor-tmp/klin/samples/MC/TeppeiMC/";
	vector<TString> files = {"mb_nue.root","mb_numu.root"};//made with GENIE v3.0.6
	vector<TString> tag = {"Nue", "Numu"};
	vector<TString> lepton = {"Electron", "Muon"};
	vector<TString> file_code = {"RECREATE","UPDATE"};
//	vector<int> target_pdg = {11,13};//electron, muon
//	vector<TString> target_pdg = {"qel","mec"};//electron, muon
	TString output_dir = "output_plots/";//im lazy, so just make one outside.
	bool debug_message = false;

	for(int hndex = 0; hndex < 2; ++hndex){
	//STEP 1: Load files, create 2D hist;
//	int e_type = 0;
//	vector<int>* e_pdg = {0};
//	vector<double>* e_px = {0};
//	vector<double>* e_py = {0};
//	vector<double>* e_pz = {0};
	
	TFile* nue_file = TFile::Open(dir+files[hndex],"READ");
	
//    TTree* e_tree = (TTree* )nue_file->Get("EventMCINFO_DL");
    TTree* e_tree = dynamic_cast <TTree* > (nue_file->Get("gst"));
	
	//get variables
//	e_tree->SetBranchAddress("interactionType",&e_type);//1001 - CCQE, 1000 - MEC
//	e_tree->SetBranchAddress("daughterPdg_v",&e_pdg);//11-e 13-mu
//	e_tree->SetBranchAddress("daughterPx_v",&e_px);
//	e_tree->SetBranchAddress("daughterPy_v",&e_py);
//	e_tree->SetBranchAddress("daughterPz_v",&e_pz);

//	Long64_t Nevents = e_tree->GetEntries();

	int nb1=90;
	int nle=0.0;
	int nhe=3.0;
	int nb2=40;
	int nlz=-1.0;
	int nhz= 1.0;
	//We want a TH2D Histogram
	TH2F* twodhist_CCQE = new TH2F(tag.at(hndex)+"CCQEhist", tag.at(hndex)+"CCQE_MB_GENIE306",nb1,nle,nhe,nb2,nlz,nhz);//binning for xnbins,xmin,xmax,ybins,ymin.ymax;
	TH2F* twodhist_MEC = new TH2F(tag.at(hndex)+"MEChist", tag.at(hndex)+"MEC_MB_GENIE306",nb1,nle,nhe,nb2,nlz,nhz);//binning for xnbins,xmin,xmax,ybins,ymin.ymax;

	e_tree->Project(tag.at(hndex)+"CCQEhist","pzl/pl:El","qel==1");
	e_tree->Project(tag.at(hndex)+"MEChist","pzl/pl:El","mec==1");
	
	
	if(debug_message){
	std::cout<<"Check non-scaled GENIE MEC"<<std::endl; 
	for(int index = 1; index<nb1+1 ; ++index){
		for(int jndex = 1; jndex<nb2+1 ; ++jndex){
			std::cout<<"Bin "<<index<<","<<jndex<<" has events "<<twodhist_MEC->GetBinContent(index,jndex) <<std::endl;
		}
	}
	std::cout<<" Total: "<<twodhist_CCQE->Integral()<<std::endl;
	}
	//save histograms as PDF and to a file;
	gROOT->SetBatch(kTRUE);//No drawing windows pop up
	TCanvas *c1 = new TCanvas("c1","",900,400);
	c1->cd();
	twodhist_CCQE->Draw("COLZ");
	twodhist_CCQE->GetXaxis()->SetTitle(lepton.at(hndex)+" Energy");
	twodhist_CCQE->GetYaxis()->SetTitle("UZ");
	c1->SaveAs(output_dir+"GENIE_"+tag.at(hndex)+"_2dCCQE_raw.pdf","pdf");

	c1->Clear();
	twodhist_MEC->Draw("COLZ");
	twodhist_MEC->GetXaxis()->SetTitle(lepton.at(hndex)+" Energy");
	twodhist_MEC->GetYaxis()->SetTitle("UZ");
	c1->SaveAs(output_dir+"GENIE_"+tag.at(hndex)+"_2dMEC_raw.pdf","pdf");

	TString outfilename = "GENIE_twodhist.root";
	TFile* output = TFile::Open(outfilename,file_code.at(hndex));
	output->cd();
	twodhist_CCQE->Write();
	twodhist_MEC->Write();
	
	nue_file->Close();
	output->Close();
	}

	return 0;
}


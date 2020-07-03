#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include <stdio.h>
#include <stdlib.h>

//Read the input root file and save the selected branches to the output file;
TFile *root2root(bool get=false, Int_t print=1){


	//configuration
	TString input  = "1.root";
	TString output  = "2.root";

	//fix variables; discard all the pointers, use vectors!
	std::map< TString, Float_t> iFvar;
	std::map< TString, Double_t> iIvar;//all pointers, so need another variable to tell the size of them;
	std::map< TString, Float_t> oFvar;
	std::map< TString, Double_t> oIvar;

	Double_t ientry = 0; 
	Double_t oentry = 0;


	//make a map for TSring & variable;
	iIvar["entry"] = ientry;
	oIvar["entry"] = oentry;
	
	
	//read files
	TFile* in_file = TFile::Open(input, "READ");
	TTree* in_tree = (TTree*) in_file->Get("T");

	TFile* out_file = TFile::Open(output, "RECREATE");
	TTree* out_tree = new TTree("T", "test");

	for(auto &x: iIvar){
		in_tree->SetBranchAddress(x.first, &x.second);
	}


	for(auto &x: oIvar){
		out_tree->Branch(x.first, &x.second,x.first+"/D");
	}

	//start writing trees by entries;

	int entries = in_tree->GetEntries();
	for(int index = 0; index < entries ; ++index){
		in_tree->GetEntry(index);

		(oIvar.begin())->second = (iIvar.begin())->second;
		std::cout<<"input "<<(iIvar.begin())->second<<std::endl;

		out_tree->Fill();
	}

	out_file->cd();
	out_tree->Write();
	in_file->Close();
	out_file->Close();

	return out_file;


/*
	double entry;

	TString filename = "Reading_Output.root";
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());

	
	dir.ReplaceAll("ReadFile.C","");
	dir.ReplaceAll("/./","/");

	FILE *fp = fopen(Form("%sevents.dat",dir.Data()),"r");//THIS FILE.dat

	TFile *hfile = 0;

	if (get) {
		// if the argument get =1 return the file "cernstaff.root"
		// if the file does not exist, it is created
		if (!gSystem->AccessPathName(dir+filename,kFileExists)) {
			hfile = TFile::Open(dir+filename); //in $ROOTSYS/tutorials/tree
			if (hfile) return hfile;
		}
		//otherwise try $PWD/cernstaff.root
		if (!gSystem->AccessPathName("cernstaff.root",kFileExists)) {
			hfile = TFile::Open("cernstaff.root"); //in current dir
			if (hfile) return hfile;
		}
	}
	//no cernstaff.root file found. Must generate it !
	//generate cernstaff.root in $ROOTSYS/tutorials/tree if we have write access
	if (!gSystem->AccessPathName(dir,kWritePermission)) {
		filename = dir+filename;
	} else if (!gSystem->AccessPathName(".",kWritePermission)) {
		//otherwise generate cernstaff.root in the current directory
	} else {
		printf("you must run the script in a directory with write access\n");
		return 0;
	}

	hfile = TFile::Open(filename,"RECREATE");

	TTree* t = new TTree("T","low_energy_bump");
	t->Branch("entry",&entry,"entry/D");
	t->Branch("run",&run,"run/D");
	t->Branch("subrun",&subrun,"subrun/D");
	t->Branch("label",&label,"label/D");
	t->Branch("event",&event,"event/D");
	t->Branch("vertex",&vertex,"vertex/D");
	t->Branch("vertey",&vertey,"vertey/D");
	t->Branch("vertez",&vertez,"vertez/D");
	t->Branch("reco_shower_energy_plane0",&reco_shower_energy_plane0,"reco_shower_energy_plane0/D");
	t->Branch("reco_shower_energy_plane1",&reco_shower_energy_plane1,"reco_shower_energy_plane1/D");
	t->Branch("reco_shower_energy_plane2",&reco_shower_energy_plane2,"reco_shower_energy_plane2/D");
	t->Branch("reco_shower_plane0_nhits",&reco_shower_plane0_nhits,"reco_shower_plane0_nhits/D");
	t->Branch("reco_shower_plane1_nhits",&reco_shower_plane1_nhits,"reco_shower_plane1_nhits/D");
	t->Branch("reco_shower_plane2_nhits",&reco_shower_plane2_nhits,"reco_shower_plane2_nhits/D");
	t->Branch("reco_shower_plane0_meanRMS",&reco_shower_plane0_meanRMS,"reco_shower_plane0_meanRMS/D");
	t->Branch("reco_shower_plane1_meanRMS",&reco_shower_plane1_meanRMS,"reco_shower_plane1_meanRMS/D");
	t->Branch("reco_shower_plane2_meanRMS",&reco_shower_plane2_meanRMS,"reco_shower_plane2_meanRMS/D");
	t->Branch("reco_shower_dEdx_plane0_median",&reco_shower_dEdx_plane0_median,"reco_shower_dEdx_plane0_median/D");
	t->Branch("reco_shower_dEdx_plane1_median",&reco_shower_dEdx_plane1_median,"reco_shower_dEdx_plane1_median/D");
	t->Branch("reco_shower_dEdx_plane2_median",&reco_shower_dEdx_plane2_median,"reco_shower_dEdx_plane2_median/D");
	t->Branch("reco_shower_dEdx_amalgamated",&reco_shower_dEdx_amalgamated,"reco_shower_dEdx_amalgamated/D");
	t->Branch("reco_shower_dQdx_plane0_median",&reco_shower_dQdx_plane0_median,"reco_shower_dQdx_plane0_median/D");
	t->Branch("reco_shower_dQdx_plane1_median",&reco_shower_dQdx_plane1_median,"reco_shower_dQdx_plane1_median/D");
	t->Branch("reco_shower_dQdx_plane2_median",&reco_shower_dQdx_plane2_median,"reco_shower_dQdx_plane2_median/D");
	t->Branch("reco_shower_dEdx_plane0_mean",&reco_shower_dEdx_plane0_mean,"reco_shower_dEdx_plane0_mean/D");
	t->Branch("reco_shower_dEdx_plane1_mean",&reco_shower_dEdx_plane1_mean,"reco_shower_dEdx_plane1_mean/D");
	t->Branch("reco_shower_dEdx_plane2_mean",&reco_shower_dEdx_plane2_mean,"reco_shower_dEdx_plane2_mean/D");
	t->Branch("reco_shower_dEdx_plane0_max",&reco_shower_dEdx_plane0_max,"reco_shower_dEdx_plane0_max/D");
	t->Branch("reco_shower_dEdx_plane1_max",&reco_shower_dEdx_plane1_max,"reco_shower_dEdx_plane1_max/D");
	t->Branch("reco_shower_dEdx_plane2_max",&reco_shower_dEdx_plane2_max,"reco_shower_dEdx_plane2_max/D");
	t->Branch("reco_shower_dEdx_plane0_min",&reco_shower_dEdx_plane0_min,"reco_shower_dEdx_plane0_min/D");
	t->Branch("reco_shower_dEdx_plane1_min",&reco_shower_dEdx_plane1_min,"reco_shower_dEdx_plane1_min/D");
	t->Branch("reco_shower_dEdx_plane2_min",&reco_shower_dEdx_plane2_min,"reco_shower_dEdx_plane2_min/D");
	t->Branch("reco_shower_dEdx_plane0_nhits",&reco_shower_dEdx_plane0_nhits,"reco_shower_dEdx_plane0_nhits/D");
	t->Branch("reco_shower_dEdx_plane1_nhits",&reco_shower_dEdx_plane1_nhits,"reco_shower_dEdx_plane1_nhits/D");
	t->Branch("reco_shower_dEdx_plane2_nhits",&reco_shower_dEdx_plane2_nhits,"reco_shower_dEdx_plane2_nhits/D");

	char line[2000];
	while (fgets(line,2000,fp)){//%lf for double!
		sscanf(&line[0],"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf"
				,&entry
				,&run
				,&subrun
				,&label
				,&event
				,&vertex
				,&vertey
				,&vertez
				,&reco_shower_energy_plane0
				,&reco_shower_energy_plane1
				,&reco_shower_energy_plane2
				,&reco_shower_plane0_nhits
				,&reco_shower_plane1_nhits
				,&reco_shower_plane2_nhits
				,&reco_shower_plane0_meanRMS
				,&reco_shower_plane1_meanRMS
				,&reco_shower_plane2_meanRMS
				,&reco_shower_dEdx_plane0_median
				,&reco_shower_dEdx_plane1_median
				,&reco_shower_dEdx_plane2_median
				,&reco_shower_dEdx_amalgamated
				,&reco_shower_dQdx_plane0_median
				,&reco_shower_dQdx_plane1_median
				,&reco_shower_dQdx_plane2_median
				,&reco_shower_dEdx_plane0_mean
				,&reco_shower_dEdx_plane1_mean
				,&reco_shower_dEdx_plane2_mean
				,&reco_shower_dEdx_plane0_max
				,&reco_shower_dEdx_plane1_max
				,&reco_shower_dEdx_plane2_max
				,&reco_shower_dEdx_plane0_min
				,&reco_shower_dEdx_plane1_min
				,&reco_shower_dEdx_plane2_min
				,&reco_shower_dEdx_plane0_nhits
				,&reco_shower_dEdx_plane1_nhits
				,&reco_shower_dEdx_plane2_nhits);

		t->Fill();
	}

	if (print) t->Print();
	   t->Write();
	   fclose(fp);
	   delete hfile;
	   if (get) {
		   //we come here when the script is executed outside $ROOTSYS/tutorials/tree
		   hfile = TFile::Open(filename);
		   return hfile;
	   }
	   return 0;
*/
}

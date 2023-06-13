#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include <stdio.h>
#include <stdlib.h>

TFile *ReadText(bool get=false, Int_t print=1){
	
	double entry;
	std::vector<double> location(2);

	TString filename = "Reading_Output.root";
	TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dir.ReplaceAll("ReadText.C","");
	dir.ReplaceAll("/./","/");
	FILE *fp = fopen(Form("%s/testInput/events.dat",dir.Data()),"r");//THIS FILE.dat

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

	TTree* t = new TTree("TreeName","Descriptions just for reading");
	t->Branch("entry",&entry,"entry/D");
	t->Branch("location",&location);

	char line[2000];
	while (fgets(line,2000,fp)){//%lf for double!
		sscanf(&line[0],"%lf %lf %lf"
				,&entry
				,&location[0]
				,&location[1]);

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

}

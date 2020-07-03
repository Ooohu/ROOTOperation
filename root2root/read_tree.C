#include "TTree.h"
#include "TBranch.h"
#include "TBasket.h"
#include <stdio.h>
#include <stdlib.h>

int read_tree(){

	TFile *file = new TFile("1.root","READ");
	TTree *tree =(TTree*) file->Get("T");

	Double_t run = 0;

	tree->SetBranchAddress("run",&run);

	Int_t events = tree->GetEntries();
	for(Int_t i=0; i<events; i++){

		tree->GetEvent(i);
		std::cout<<run<<std::endl;
	}
	return 0;

}

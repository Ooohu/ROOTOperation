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

//for the case of a pointer, it needs to malloc memory and disable irrelevant branches;
	tree->SetMakeClass(1);

	tree->SetBranchStatus("*",0);
	
	std::vector< TString > name = {"1","2"};
	int num_swvars = name.size();

	std::vector< Float_t * > swvars(num_swvars);·
		//To load Float_t []: https://arduino.stackexchange.com/questions/3774/how-can-i-declare-an-array-of-variable-size-globally
		for(size_t kndex = 0; kndex < num_swvars; ++ kndex){//get sw branches
			swvars[kndex] = (Float_t*) malloc(events*sizeof(Float_t*)*num_throws);//num_throws is the # of swvars[0][?]
			temptree->SetBranchStatus(name[kndex],1);
			temptree->SetBranchAddress(name[kndex], swvars[kndex]);
		}
}

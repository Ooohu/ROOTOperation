#include "TTree.h"
#include "TBranch.h"
#include "TBasket.h"
#include <stdio.h>
#include <stdlib.h>


void CopyTree(){

	//TTree TreeName
	char file_name[] = "../SampleRoot/Reading_Output.root";

	TFile oldfile(file_name);
	TTree *oldtree;
	oldfile.GetObject("TreeName",oldtree);

	const auto nentries = oldtree->GetEntries();
 
    Double_t var;
	oldtree->SetBranchAddress("entry", &var);

	TFile newfile("copyed_Reading_Output.root","recreate");
	auto newtree = oldtree->CloneTree(0);

	for (int i = 0; i < nentries; i ++){
		oldtree->GetEntry(i);
		std::cout<<"Evaluate "<<i<<" entry with value "<<var<<std::endl;
		if (var > 20){ 
			newtree->Fill();
			std::cout<<"Copy This Entry"<<std::endl;
		}
	}

	newtree->Print();
	newfile.Write();

}

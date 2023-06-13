#include "TTree.h"
#include "TBranch.h"
#include "TBasket.h"
#include <stdio.h>
#include <stdlib.h>


void FilterTree(){

	//TTree TreeName
	char file_name[] = "../SampleRoot/Reading_Output.root";

	TFile oldfile(file_name);
	TTree *oldtree;
	oldfile.GetObject("TreeName",oldtree);

	const auto nentries = oldtree->GetEntries();
 
    Double_t var;
	oldtree->SetBranchAddress("entry", &var);

	//need text input
	FILE *fp = fopen("testInput/FilterTree.dat","r");

	double ent;
	std::vector<double> ent_list;

	char line[2000];
	while (fgets(line,2000,fp)){//%lf for double!
		sscanf(&line[0],"%lf"
				,&ent);
		ent_list.push_back(ent);
	}



	TFile newfile("FilterTree_Output.root","recreate");

	auto newtree = oldtree->CloneTree(0);

	for (int i = 0; i < nentries; i ++){
		oldtree->GetEntry(i);
		std::cout<<"Evaluate "<<i<<" entry with value "<<var<<std::endl;
		if ( std::find(ent_list.begin(), ent_list.end(), var) != ent_list.end() ){ 
			newtree->Fill();
			std::cout<<"Copy This Entry"<<std::endl;
		}
	}

	newtree->Print();
	newfile.Write();

}

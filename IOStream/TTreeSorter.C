#include "TTree.h"
#include "TBranch.h"
#include "TBasket.h"
#include <stdio.h>
#include <stdlib.h>

//sort in terms of run:subrun:event #
void TTreeSorter(){

	//TTree TreeName
	char file_name2[] = "/pnfs/uboone/persistent/users/klin/MCC9/axions_localgpvm_storage/axion_prod_ma_0016-01123/ntuples/glee_ntuples_16600evt.root";

//gLEE
	TFile gleefile(file_name2);
	TTree *gleetree;
	gleefile.GetObject("singlephotonana/vertex_tree",gleetree);


    Int_t frun;
    Int_t fsubrun;
    Int_t fevent;
	gleetree->SetBranchAddress("run_number", &frun);
	gleetree->SetBranchAddress("subrun_number", &fsubrun);
	gleetree->SetBranchAddress("event_number", &fevent);

	const auto gnentries = gleetree->GetEntries();


	TFile newfile("output/sorted_glee.root","recreate");
	auto newtree = gleetree->CloneTree(0);


	gleetree->BuildIndex("run_number*1E10+subrun_number*1E5+event_number"); 
	TTreeIndex *index = (TTreeIndex*)gleetree->GetTreeIndex(); 
	for( int i = 0; i < index->GetN() ; ++i ) { 
		Long64_t local = gleetree->LoadTree( index->GetIndex()[i] ); 
		gleetree->GetEntry(local);
		std::cout<<"Run num "<< frun <<" Subrun "<<fsubrun<<" Event num "<< fevent<<std::endl;
	}

////	newtree->Print();
//	newfile.Write();

}

#define newroot_cxx
#include "newroot.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TClass.h>

using namespace std;

void newroot::Loop()
{
//   In a ROOT session, you can do:
//      root> .L newroot.C
//      root> newroot t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

   if (fChain == 0) return;



//configuration
	TString dir = "./output_root/";//output directory
	bool OM = true;//entry_marks[?] record what entries means new file;
	//CHANGE line 284 for the correct throws; 
	const int multithrows = 66;//66 = entry_marks.size() for OM; others, 1000
	const int num_vars = 27;// has to be constant for the array size;
	
	//set output info.
	std::vector<TString > list = {
		"07DetBkgMCSW.root",//0
		"07DetBkgMCCV.root",
		"07DISCBkgMC.root",
		"07QTCORRBkgMC.root",
		"07DirtSW.root",//4
		"07DirtCV.root",
		"06OM_BkgMC.root",
		"06OM_CV.root",
	};

	TString outputname = list[6];

	TString output = dir + outputname;
	std::cout<<" Create file "<<output<<std::endl;

	TFile* out_file = TFile::Open(output, "RECREATE");
	TTree* out_tree = new TTree("TTiming", "test");
	std::vector< int> marks;
	
	if(OM){ marks = entry_marks;} else{ marks = {0};};
	//varaibles that we need to store in the output file:
	//define, out_tree->Branch("name", &var, "name+/type");

	std::vector< TString> ovname = {
		"el.X",//0
		"el.Y",//1
		"el.Z",//2
		"el.UX",//3
		"el.UY",//4
		"el.UZ",//5
		"el.E",//6
		"el.F",//7
		"el.R",//8, Float_t*
		"el.EnuQE",//9
		"el.RtoWallB",//10
		"el.RtoWallF",//11
		"mu.F",//12
		"piFix.F",//13
		"piNoFix.M",//14
		//and systematic stuff
		"CV",//15
		"KpProd",//16, sw 0
		"KmProd",//17, sw 1
		"K0Prod",//18, sw 2
		"PipProd_RawAboutSW",//19, sw 3
		"PimProd_RawAboutSW",//20, sw 4
		"BeamUnisims",//21, sw 5
		"XSec",//22, sw 6
		"CCQECCpi",//23, sw 7
		"Hadronic",//24, sw 8
		"Pi0Yield",//25, sw 9
		"Dirt",//26, sw 10
		//additional variables
//		"R500cm",//27
//		"emuLLH",//28
//		"epiLLH",//29
	};

	
	//Set Branches for outputs;
	//a special one
	int nthrows = 0;
	out_tree->Branch("Nthrows", &nthrows, "Nthrows/I");

	Float_t ovars[num_vars][multithrows] = {0};

	for(int index = 0; index < num_vars; ++index){
		out_tree->Branch( ovname[index], ovars[index], ovname[index] + "[Nthrows]/F");
	}

	//Need a giant container to store values before feeding to the tree;
	Long64_t nentries = (OM)? std::accumulate(marks.begin(),marks.end(),0) : fChain->GetEntriesFast();
	int max_entry = (OM)? *max_element(marks.begin(), marks.end()) : nentries;
	std::vector< std::vector< std::vector< Float_t > > > container( max_entry , 
			std::vector< std::vector< Float_t > > ( num_vars, 
				std::vector< Float_t >( multithrows, 0
					)));



	int om_index = 0;//for OM, this keep track of the nth element of given vector;
	int om_entries = 0;//for OM, this keep track of the nth element of given vector;


	for (Long64_t jentry=0; jentry<nentries;jentry++) {//all entries
//		   std::cout<<"\r Looping "<<std::setw(5)<<jentry<<"/"<<setw(10)<<nentries<<" throws "<<om_index;
		  if(jentry == std::accumulate(marks.begin(),marks.begin()+om_index+1,0)){//nth entry of each files;
			om_index++;
			om_entries = 0;
//			std::cout<<"This "<<jentry<<std::endl;
		  }
		  std::cout<<"\r Looping "<<std::setw(5)<<jentry<<"/"<<setw(5)<<nentries<<" throws "<<std::stw(3)<<om_index<<" subentries: "<<setw(4)<<om_entries;

//		   std::cout<<" At entries at "<<jentry<<" reset om_entries "<<om_jentry+1 <<" and finish throw: "<<om_index<<std::endl;


		   gSystem->RedirectOutput("/dev/null");
		   Long64_t ientry = LoadTree( jentry);
		   fChain->GetEntry( jentry);
		   gSystem->RedirectOutput(0,0);

		   //------not important stuff;
		   if (ientry < 0) break;
		   //	   nb = fChain->GetEntry(jentry);   nbytes += nb;

		   if (TOneTrackChunk_ == 0) {
			   //cout<<"Skip No TOneTrackChunk"<<endl;
			   continue;
		   }

		   int track = 0;
		   for (; track < TOneTrackChunk_; ++track) {
			   // 1 for electron fit
			   // 2 for muon fit
			   // 3 for proton fit (not used)
			   // 4 for pionplus fit (not used)
			   if (TOneTrackChunk_data__trackType[track] == 1) {
				   break;
			   }
		   }

		   if (track == TOneTrackChunk_) {
			   //cout<<"Skip No TOneTrackChunk2"<<endl;
			   continue;
		   }

		   //------- not important above
//		   nthrows = multithrows;

		   GrabVars(container,om_entries++, om_index, OM);
//		   int index = (OM)? om_index: 0;
//		   while(index < multithrows){
//			   //		   test[index] = TOneTrackChunk_data__E[0];
//			   //basics
//			   ovar[0][index] = TOneTrackChunk_data__X[0];
//			   ovar[1][index] = TOneTrackChunk_data__Y[0];
//			   ovar[2][index] = TOneTrackChunk_data__Z[0];
//			   ovar[3][index] = TOneTrackChunk_data__UX[0];
//			   ovar[4][index] = TOneTrackChunk_data__UY[0];
//			   ovar[5][index] = TOneTrackChunk_data__UZ[0];
//			   ovar[6][index] = TOneTrackChunk_data__E[0];
//			   ovar[7][index] = TOneTrackChunk_data__F[0];
//
//			   //derivated
//			   ovar[8][index] = sqrt( pow(ovar[0][index],2) + pow(ovar[1][index],2) + pow(ovar[2][index],2)); 
//
//			   Float_t Mlep = 0.511;//electron mass
//			   Float_t Mp = 938.27;//proton mass
//			   Float_t Mn = 939.56;//neutron mass
//
//			   ovar[9][index] = 0.5*(2.0*Mn* (Mlep+ ovar[6][index] ) + pow(Mp,2) - pow(Mn,2) - pow(Mlep,2))/(Mn - (Mlep+ ovar[6][index] ) + ovar[5][index] * sqrt( pow(Mlep+ ovar[6][index] ,2) - pow(Mlep, 2) ));
//
//			   Float_t UdotR = ovar[0][index]*ovar[3][index] + ovar[1][index]*ovar[4][index] + ovar[2][index]*ovar[5][index] ;
//			   ovar[10][index] = (ovar[8][index]<548.17)? UdotR + sqrt(pow( 548.17,2) - (pow(ovar[8][index],2) + pow( UdotR, 2))) : -9999;//RBOONE, radius of MiniBooNE 548.17 cm
//			   ovar[11][index] = (ovar[8][index]<548.17)? - UdotR + sqrt(pow( 548.17,2) - (pow(ovar[8][index],2) + pow( UdotR, 2))): -9999;
//
//			   ovar[12][index] = TOneTrackChunk_data__F[1];
//
//			   int pionFixindex = (TTwoTrackChunk_data__fixedMass[0]< 10e-20)?  1:0;
//			   ovar[13][index] = TTwoTrackChunk_data__F[pionFixindex];
//
//			   int pionNoFixindex = (TTwoTrackChunk_data__fixedMass[0]< 10e-20)?  0:1;
//			   ovar[14][index] = TTwoTrackChunk_data__M[pionNoFixindex];
//
//			   ovar[15][index] = target_wgt;//TWeightDetailsChunk_data__totalweight[0];//CV
//
//			   for( int jndex  = 0; jndex < 11; ++jndex){ //different systematic weights
//				   ovar[16+jndex][index] = target_wgt;//TMulMatWeightsChunk_data__MultiWeight[jndex][index];
//			   }
//
//			   index++;
//			   if(OM) break;//just do it once for OM;
//		   }

   }//end of looping entries;
	

// vector<vector<int> > vec( n , vector<int> (m)); 
//  vector<int> vect(n, 10);

   cout<<"\nFinish the loop!"<<endl;

	FillVars(nthrows, multithrows, ovars, container, out_tree);

	out_file->cd(); 
	out_tree->Write();
	out_file->Close();

   //save them according to types:

	TFile* check_file = TFile::Open(output, "READ");
	TTree* check_tree = (TTree*) check_file->Get("TTiming");

	std::cout<<"Write entires:"<<check_tree->GetEntries()<<std::endl;
	check_file->Close();
}

/*
 * Update the container that stores values of variables.
 */

void newroot::GrabVars(std::vector<std::vector< std::vector< Float_t> >> & container, int hndex, int index, bool its_OM){


			   int target_wgt = 1;

			   container[hndex][0][index] = TOneTrackChunk_data__X[0];
			   container[hndex][1][index] = TOneTrackChunk_data__Y[0];
			   container[hndex][2][index] = TOneTrackChunk_data__Z[0];
			   container[hndex][3][index] = TOneTrackChunk_data__UX[0];
			   container[hndex][4][index] = TOneTrackChunk_data__UY[0];
			   container[hndex][5][index] = TOneTrackChunk_data__UZ[0];
			   container[hndex][6][index] = TOneTrackChunk_data__E[0];
			   container[hndex][7][index] = TOneTrackChunk_data__F[0];

			   //derivated
			   container[hndex][8][index] = sqrt( pow(container[hndex][0][index],2) + pow(container[hndex][1][index],2) + pow(container[hndex][2][index],2)); 

			   Float_t Mlep = 0.511;//electron mass
			   Float_t Mp = 938.27;//proton mass
			   Float_t Mn = 939.56;//neutron mass

			   container[hndex][9][index] = 0.5*(2.0*Mn* (Mlep+ container[hndex][6][index] ) + pow(Mp,2) - pow(Mn,2) - pow(Mlep,2))/(Mn - (Mlep+ container[hndex][6][index] ) + container[hndex][5][index] * sqrt( pow(Mlep+ container[hndex][6][index] ,2) - pow(Mlep, 2) ));

			   Float_t UdotR = container[hndex][0][index]*container[hndex][3][index] + container[hndex][1][index]*container[hndex][4][index] + container[hndex][2][index]*container[hndex][5][index] ;
			   container[hndex][10][index] = (container[hndex][8][index]<548.17)? UdotR + sqrt(pow( 548.17,2) - (pow(container[hndex][8][index],2) + pow( UdotR, 2))) : -9999;//RBOONE, radius of MiniBooNE 548.17 cm
			   container[hndex][11][index] = (container[hndex][8][index]<548.17)? - UdotR + sqrt(pow( 548.17,2) - (pow(container[hndex][8][index],2) + pow( UdotR, 2))): -9999;

			   container[hndex][12][index] = TOneTrackChunk_data__F[1];

			   int pionFixindex = (TTwoTrackChunk_data__fixedMass[0]< 10e-20)?  1:0;
			   container[hndex][13][index] = TTwoTrackChunk_data__F[pionFixindex];

			   int pionNoFixindex = (TTwoTrackChunk_data__fixedMass[0]< 10e-20)?  0:1;
			   container[hndex][14][index] = TTwoTrackChunk_data__M[pionNoFixindex];

			   container[hndex][15][index] = (its_OM)? target_wgt : TWeightDetailsChunk_data__totalweight[0];//CV

			   for( int jndex  = 0; jndex < 11; ++jndex){ //different systematic weights, 16~26
				   container[hndex][16+jndex][index] = (its_OM)? target_wgt : TMulMatWeightsChunk_data__MultiWeight[jndex][index];
			   }

}





/*
 * Fill Variables to branches
 */
void newroot::FillVars(int &nthrows, int multithrows, Float_t ovars[][66], std::vector< std::vector< std::vector< Float_t > > > container, TTree* out_tree){

	int num_entries = container.size();
	int num_vars = container[0].size();
	int length_var = (container[0][0]).size();

	for(int index = 0; index < num_entries; ++index){//each entry
		nthrows = multithrows;
		for(int jndex = 0; jndex < num_vars; ++jndex){//each variable
			for(int kndex = 0; kndex < length_var; ++kndex){//each element of the variable
				ovars[jndex][kndex] = container[index][jndex][kndex];
			}
		}
		std::cout<<"\r Filling "<<std::setw(5)<<index<<" entry ";

		out_tree->Fill();
	}
	std::cout<<std::endl;
}

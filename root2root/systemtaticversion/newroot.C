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
	bool OMwgt = true;//fill in weighs with 1
	bool Multiwgt = false;//read weights from TMultiWeight Chunk, if true;
	bool multifiles = true;//reset entry at some specific entries, if true;
	//CHANGE line 246 for the correct throws; 
	const int multithrows = 66;//66 = entry_marks.size() for OM; others, 1000
	const int num_vars = 35;// has to be constant for the array size;
	
	//set output info.
	std::vector<TString > list = {
		"07DetBkgMC.root",//0
		"07DISCBkgMC.root",
		"07QTCORRBkgMC.root",
		"07Dirt.root",//3
		"06OM_BkgMC.root",
		"06OM_CV.root",//5
	};

	TString outputname = list[4];

	TString output = dir + outputname;
	std::cout<<" Create file "<<output<<std::endl;

	TFile* out_file = TFile::Open(output, "RECREATE");
	TTree* out_tree = new TTree("TTiming", "test");
	std::vector< int> marks;
	
	if(multifiles){ marks = entry_marks;} else{ marks = {0};};
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
		"Dirt",//26, sw 0
		"Pi0Yield",//25, sw 9
		"Hadronic",//24, sw 8
		"CCQECCpi",//23, sw 7
		"XSec",//22, sw 6
		"BeamUnisims",//21, sw 5
		"KmProd",//17, sw 1
		"K0Prod",//18, sw 2
		"KpProd",//16, sw 0
		"PimProd_RawAboutSW",//20, sw 4
		"PipProd_RawAboutSW",//19, sw 10
		//additional variables
		"nuleng",
		"enugen",
		"inno",
		"parid",//30
		"isdirt",
		"ispi0",
		"nuchan",
		"backgroundType",
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
	Long64_t nentries = (multifiles)? std::accumulate(marks.begin(),marks.end(),0) : fChain->GetEntriesFast();
	int max_entry = (multifiles)? *max_element(marks.begin(), marks.end()) : nentries;
	std::vector< std::vector< std::vector< Float_t > > > container( max_entry , 
			std::vector< std::vector< Float_t > > ( num_vars, 
				std::vector< Float_t >( multithrows, 0
					)));



	int om_index = 0;//for OM, this keep track of the nth element of given vector;
	int om_entries = 0;//for OM, this keep track of the nth element of given vector;


	for (Long64_t jentry=0; jentry<nentries;jentry++) {//all entries
		  if(jentry == std::accumulate(marks.begin(),marks.begin()+om_index+1,0)&&multifiles){//nth entry of each files;
			om_index++;
			om_entries = 0;
//			std::cout<<"This "<<jentry<<std::endl;
		  }
		  std::cout<<"\r Looping "<<std::setw(5)<<jentry<<"/"<<setw(5)<<nentries<<" throws "<<std::setw(3)<<om_index<<" subentries: "<<setw(4)<<om_entries;

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
		   if(OMwgt){
			   GrabVars(container,om_entries, om_index, OMwgt, Multiwgt);
		   } else{
				for(int temp_index = 0; temp_index< multithrows; ++temp_index){
					GrabVars(container,om_entries, temp_index, OMwgt, Multiwgt);
			   }
		   }
		   om_entries++;
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

void newroot::GrabVars(std::vector<std::vector< std::vector< Float_t> >> & container, int hndex, int index, bool its_OM, bool its_Multisim){


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
				   container[hndex][16+jndex][index] = (its_OM)? target_wgt : ((its_Multisim)? TMulMatWeightsChunk_data__MultiWeight[jndex][index]: 0);
			   }
			

//		"nuleng"
//		"enugen"
//		"inno",
//		"nuchan",
//		"parid",
//		"isdirt",
//		"ispi0",
//		"backgroundType",
			   Float_t vrtX = TMonteCarloEVNTChunk_data__VRTX[0][0][0];
			   Float_t vrtY = TMonteCarloEVNTChunk_data__VRTX[0][0][1];
			   Float_t vrtZ = TMonteCarloEVNTChunk_data__VRTX[0][0][2];
			   Float_t ini_posX = TMonteCarloBEAMChunk_data__ini_pos[0][0][0]; 
			   Float_t ini_posY = TMonteCarloBEAMChunk_data__ini_pos[0][0][1]; 
			   Float_t ini_posZ = TMonteCarloBEAMChunk_data__ini_pos[0][0][2]; 

			   container[hndex][27][index] = sqrt( pow( vrtX + 0 - ini_posX, 2) +
													pow( vrtY + 189.614 - ini_posY, 2) +
													pow( vrtZ + 54173.1 - ini_posZ, 2) ) *1.0E-5; //nuleng

			   container[hndex][28][index] = TMonteCarloEVNTChunk_data__PNEU[0][3];//enugen
			   container[hndex][29][index] = TMonteCarloEVNTChunk_data__INNO[0];//inno
			   container[hndex][30][index] = TMonteCarloEVNTChunk_data__EVWT[0];//nuchan

			   //parid: parent id
			   int parent_npart = TMonteCarloBEAMChunk_data__npart[0] - 1;
			   container[hndex][31][index] = TMonteCarloBEAMChunk_data__id[0][parent_npart-1];
			   while(container[hndex][31][index] ==14 && parent_npart>=3){
				   parent_npart -= 1;
				   container[hndex][31][index] =  TMonteCarloBEAMChunk_data__id[0][parent_npart-1];
			   }
				
				//check if it is dirt, 
			   bool isdirt = false;
			   if( sqrt(pow(vrtX,2) + pow(vrtY,2) + pow(vrtZ,2)) > 610.6) isdirt = true;
			   container[hndex][32][index] = (isdirt)? 1: 0 ;//isdirt

			   //ispi0?
			   container[hndex][33][index] = (GetNpi0() >0)? 1 : 0 ;

			   //backgroundType
			   Float_t type = 11;
			   if(isdirt){ // dirt
				   type = 8;

			   }else if(container[hndex][33][index]> 0){//pi0
				   type = 9;

			   } else if( container[hndex][30][index]  == 94 ){//delta
				   type = 10;

			   } else if (container[hndex][29][index] == 3 || container[hndex][29][index] == 4){
					switch((int) container[hndex][31][index]){//parent id
					case 8:
						type = 1;//kPiPlus
						break;
					case 9:
						type = 2;//PiMinus
						break;
					case 11:
						type = 3;//KPlus
						break;
					case 12:
						type = 4;//KMinus
						break;
					case 10:
						type = 5;//K0L
						break;
					case 5:
						type = 6;//MuPlus
						break;
					case 6:
						type = 7;//MuMinus
						break;
					};//1~7 type, for parent id: 8~12,5,6,13
			   }

			   container[hndex][34][index] = type;

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

/*
 * determine background.
 */

//
//   npi0 = this->GetNpi0();
//   if(npi0>0)   ispi0 = true;
//
int newroot::GetNpi0(){
   // modified from Framework Utilities/PhysicsFunctions Pi0Details
   int  npi0=0, nnpi0=0;
   int  mg=0, ngammas=0;
   float mtemp, Pgamma[127][4] ;
   float mpi0 = 0.1349766;
   float me   = 0.511e-3;
   for(int i =0; i<TMonteCarloEVNTChunk_data__NFSP[0]; i++){
      int ipfs = TMonteCarloEVNTChunk_data__IPFS[0][i];
      if(ipfs==7) {//pi0
         npi0 += 1;
      }
     else if(ipfs == 1.){//photon
       ngammas += 1;
       Pgamma[ngammas-1][3] = TMonteCarloEVNTChunk_data__PFSP[0][i][3];
       for(int k=0; k<3; k++) Pgamma[ngammas-1][k] = Pgamma[ngammas-1][3] *TMonteCarloEVNTChunk_data__PFSP[0][i][k];
       if(Pgamma[ngammas-1][3]>0.025) mg = mg + 1;
     }
      if(ipfs == 2 || ipfs==3) {//e+e-
         for(int j = i+1; j<TMonteCarloEVNTChunk_data__NFSP[0]; j++){
            int ipfs2 = TMonteCarloEVNTChunk_data__IPFS[0][j];
            if( (ipfs2==2 || ipfs2==3) && (ipfs!=ipfs2)){
               ngammas += 1;
               Pgamma[ngammas-1][3] = sqrt( pow(TMonteCarloEVNTChunk_data__PFSP[0][i][3],2)+me*me) +
                                      sqrt( pow(TMonteCarloEVNTChunk_data__PFSP[0][j][3],2)+me*me);
               for(int k=0; k<3; k++){
                  Pgamma[ngammas-1][k] = 
                    TMonteCarloEVNTChunk_data__PFSP[0][i][3] * TMonteCarloEVNTChunk_data__PFSP[0][i][k] +
                    TMonteCarloEVNTChunk_data__PFSP[0][j][3] * TMonteCarloEVNTChunk_data__PFSP[0][j][k];
               }
               if(Pgamma[ngammas-1][3]>0.025) mg = mg + 1;
            } // if(ipfs2)
         } // for(j)
      } // if(ipfs==2 || ipfs==3)
   }
   nnpi0 = 0;
   for(int i = 0; i<ngammas-1; i++){
      for(int j =0; j<ngammas;j++){
         mtemp = sqrt(pow(Pgamma[i][3]+Pgamma[j][3],2) - pow(Pgamma[i][0]+Pgamma[j][0],2) -
                      pow(Pgamma[i][1]+Pgamma[j][1],2) - pow(Pgamma[i][2]+Pgamma[j][2],2));
         if(abs(mtemp-mpi0) < 0.000001) {
            nnpi0 += 1;
         } // if(abs(mtemp-mpi0))
      } // for(j)
   } // for(i)
   npi0 = max(npi0,nnpi0);
   return npi0;
}


#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

int applyweights(){
	//Working at MiniBooNE Server
	//because of  f2, we need apply2dreweight.C first
	//Output: root2_"+tags[hndex]+"_3_weights.root; with three weights for CCQE+MEC, CCQE, MEC weights;
	
    //STEP1: Prepare root file for weights
	//weights
	vector<Float_t> current_weights(3);//varaible for storing the weight, name is stored in weights_name;
//	Float_t weights;

	TString ratio_root = "calc_ratio_rw2dhist.root";
	vector<TString> weights_name = {"GENIE_CCQEMEC_wgt", "GENIE_CCQE_wgt","GENIE_MEC_wgt"};
//	vector<TString> mbfiles = {"MC_nu_all_1_16.root","MC_numu_all_00_15.root"};
	vector<TString> mbfiles = {"timing_MC_nue_may07_2.root","timing_MC_numu_may07_2.root"};
	vector<TString> file_code = {"RECREATE","UPDATE"};
//	vector< Int_t> ntp_type = {1,3};//MCBEAM.ntp: 1 - nue, 3 - numu
	vector< int> particle_type = {3,1};//DIFFERENT to MCBEAM.ntp; MCEVNT.INNO: 1 - numu, 3 - nue 
	vector< int> fin_par = {2,3,5,6};//final sate particle, CHECK to be determined, for electron/muon;
	vector<TString> tags = {"Nue","Numu"}; 
	vector<float> lep_mass = {0.511*0.001, 105.66*0.001};

	//record 2dhist for weights;
	for(int hndex = 0; hndex < 2; ++hndex){
	if(hndex==1) exit(0);

		std::cout<<"Apply weights for "<<tags[hndex]<<std::endl;
		//variables named that we need
		Int_t ibkgd = 0;
		Int_t INNO = 0;
		Int_t NFSP = 0;
		vector<int> IPFS = {0};
		vector<Float_t> PFSPZ = {0};//pz/p
		vector<Float_t> PFSPP = {0};//p in GeV
		Float_t E = 0;//Visible/Neutrino Energy
//		Float_t mcevent_weight = 0;

		//based on the old tree;
		TString outfilename = "weights_"+tags[hndex]+"_1699.root";
		TFile* output = TFile::Open(outfilename,file_code[0]);

		TTree* out_tree = new TTree("T","2dweights");

		//STEP2: read contents from the root file;
		TFile* f1=TFile::Open("/scratch/condor-tmp/klin/data_timing_root/"+mbfiles[hndex],"READ"); //open a root file
		TTree* input_tree = (TTree*) f1->Get("TTiming");
//		TTree* input_tree = dynamic_cast <TTree* > (f1->Get("TTiming"));

		input_tree->SetMakeClass(true);//YESSS, bug solved!!!!, https://root-forum.cern.ch/t/issues-with-getting-data-with-setbranchaddress/28722/3, otherwise all output becomes 0;

		input_tree->SetBranchAddress("MCEVNT.E",&E);//lepton energy would be this (neutrino/visible energy) + mass; same as enugen and MCEVNT.PNEUE
		input_tree->SetBranchAddress("MCEVNT.INNO",&INNO);//neutrino type, 1 - numu; 3 - nue
		input_tree->SetBranchAddress("MCEVNT.IPFS",&IPFS);//particle id, not identified yet CHECK
		input_tree->SetBranchAddress("MCEVNT.NFSP",&NFSP);//# of final state
		input_tree->SetBranchAddress("MCEVNT.IBKGD",&ibkgd);//1 is CCQE; same as ibkgd
		input_tree->SetBranchAddress("ibkgd",&ibkgd);//1 is CCQE;
		input_tree->SetBranchAddress("MCEVNT.PFSPZ",&PFSPZ);//UZ
		input_tree->SetBranchAddress("MCEVNT.PFSPP",&PFSPP);//Momentum in GeV?
		//	input_tree->SetBranchAddress("MCEVNT.PNEUE",&PNEUE);//same as E, but in GeV;
//		input_tree->SetBranchAddress("MCEVNT.wt",&mcevent_weight);//SET branch before using it!
		
		//STEP3: Get ratio histograms for three weights from `apply2dreweight.C`;
		TFile* f2 = TFile::Open(ratio_root,"READ");
		TH2F* twodhist_CCQEMEC = (TH2F*) f2->Get("weights");
		TH2F* twodhist_CCQE = (TH2F*) f2->Get(tags[hndex]+"CCQEhist");
		TH2F* twodhist_MEC = (TH2F*) f2->Get(tags[hndex]+"MEChist");

		//STEP3.4: Apply weight; do it by entries
		int Nevents = input_tree->GetEntries();
		std::cout<<"Calculating the weight"<<std::endl;

		vector<TH2F*> ref_hist = {twodhist_CCQEMEC,twodhist_CCQE,twodhist_MEC};

		for( size_t index = 0; index < weights_name.size(); ++index){
			std::cout<<"Extract weigths for "<<weights_name[index]<<std::endl;
			int which_index = 3*hndex+index;

//			out_tree->Branch(weights_name[index],&(weights[which_index]),weights_name[index]+"/F");//get branch for different wgts;
			auto new_branch = out_tree->Branch(weights_name[index],&current_weights[index],weights_name[index]+"/F");//get branch for different wgts;
			TH2F* weight_2dhist = ref_hist[index];//look at different histogram;

			int counts = 0;
			int multi_counts = 0;
			for(Long64_t jndex = 0; jndex<Nevents; ++jndex){//take action here;
				std::cout<<"\r"<<jndex+1<<" of "<< Nevents <<" finish";
				input_tree->GetEntry(jndex);

				vector<Float_t> many_weights;
				for(int kndex = 0; kndex < IPFS.size(); ++kndex){
					if(ibkgd ==1 && INNO == particle_type[hndex] && (IPFS[kndex]==fin_par[2*hndex]||IPFS[kndex]==fin_par[2*hndex+1])){//CCQE, nue/numu, e/mu
						TAxis *xaxis = weight_2dhist->GetXaxis();
						TAxis *yaxis = weight_2dhist->GetYaxis();
						Int_t xbin = xaxis->FindBin(sqrt(pow(PFSPP[kndex],2)+pow(lep_mass[hndex],2)));
						Int_t ybin = yaxis->FindBin(PFSPZ[kndex]);
						if(xbin > xaxis->GetLast() || xbin<1 || ybin<1 ||ybin >yaxis->GetLast()){ 
						std::cout<<" CHECK, entry "<<jndex<<" is outside the 2dhist "<<xbin<<","<<ybin<<std::endl;
						continue;
						}
						many_weights.push_back(weight_2dhist->GetBinContent(xbin,ybin));

						if(xbin==25 && ybin ==4 ){ 
							std::cout<<" Individual weight "<<many_weights.back();
							std::cout<<" at "<<xbin<<","<<ybin;
							std::cout<<" for "<<sqrt(pow(PFSPP[kndex],2)+pow(lep_mass[hndex],2))<<",";
							std::cout<<PFSPZ[kndex]<<std::endl;
						}
					}
				}
				if(many_weights.size()>1) multi_counts++;

				current_weights[index] = accumulate(many_weights.begin(),many_weights.end(),0.0);
//				if(current_weights[index] == 0) current_weights[index] = 1.0; //CHECK, why I set 0 as 1?
//
//				weights[which_index] = accumulate(many_weights.begin(),many_weights.end(),0.0);
//				if(weights[which_index] == 0) weights[which_index] = 1.0;//mcevent_weight;
//				if(counts>1304&&counts < 1306) std::cout<<" Get a weight "<<current_weights[index]<<std::endl;
				counts++;
				if(index==0){
					out_tree->Fill();//create entries
				}else{
					new_branch->Fill();//update each entries;
				}
			}
			output->cd();
			out_tree->Write("", TObject::kOverwrite);//save the udpated tree
//			out_tree->ResetBranchAddresses();

			std::cout<<"\nFinish writing "<< counts<<" non-zero weights. "<<multi_counts<<" entries have multiple final states."<<std::endl;
		}

		output->Close();	
		f1->Close();	
		f2->Close();	
	}

	return 0;
}


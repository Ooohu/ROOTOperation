#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

void draw_axises(TH2F* hist, TString name){
	if(name.Length()>1) hist->SetTitle(name);
	hist->Draw("COLZ");
	hist->GetXaxis()->SetTitle("Lepton Energy [GeV]");
	hist->GetYaxis()->SetTitle("UZ");
}

int calculate2dweights(){
	//Working at MiniBooNE Server
	
	bool  debug_message = false;
    //STEP1: Prepare root file for weights
	//weights
	vector<Float_t> weights(2);//varaible for storing the weight, name is stored in weigths_name;
//	vector<TString> weights_name = {"uBnue_CCQEMEC_wgt", "uBnumu_CCQEMEC_wgt"};
	TString GENIE_2dhist = "GENIE_twodhist.root";//read histograms from a root
	TString ratio_root = "calc_ratio_rw2dhist.root";
//	vector<TString> mbfiles = {"timing_MC_nue_may07_2.root","MC_numu_all_00_15.root"};
	vector<TString> mbfiles = {"MC_nu_all_1_16.root","MC_numu_all_00_15.root"};
	vector<TString> file_code = {"RECREATE","UPDATE"};
//	vector< Int_t> ntp_type = {1,3};//MCBEAM.ntp: 1 - nue, 3 - numu
	vector< int> particle_type = {3,1};//DIFFERENT to MCBEAM.ntp; MCEVNT.INNO: 1 - numu, 3 - nue 
	vector< int> fin_par = {2,3,5,6};//final sate particle, CHECK to be determined, for electron/muon;
	vector<TString> tags = {"Nue","Numu"}; 
	vector<TString> tag_particles = {"Electron","Muon"}; 
	vector<float> lep_mass = {0.511*0.001, 105.66*0.001};
	TString output_dir = "output_plots/";//im lazy, so just make one outside.

	//record 2dhist for weights;
	TFile* save_weights_file = TFile::Open(ratio_root,"RECREATE");

	int nb1=90;
	int nle=0.0;
	int nhe=3.0;
	int nb2=40;
	int nlz=-1.0;
	int nhz= 1.0;


	for(int hndex = 0; hndex < 2; ++hndex){
	if(hndex==1) exit(0);
		std::cout<<"Generating histograms for "<<tags[hndex]<<std::endl;
		//variables named that we need
		Int_t ibkgd = 0;
		Int_t INNO = 0;
		Int_t NFSP = 0;
		vector<int> IPFS = {0};
		vector<Float_t> PFSPZ = {0};//pz/p
		vector<Float_t> PFSPP = {0};//p in GeV
		Float_t E = 0;//Visible/Neutrino Energy
		Float_t mcevent_weight = 0;

		//based on the old tree;
//		TString outfilename = "root_"+tags[hndex]+"_mBtouB_weights.root";
//		TFile* output = TFile::Open(outfilename,file_code[0]);

//		TTree* out_tree = new TTree("T","2dweights");
//		out_tree->Branch(weights_name[hndex],&(weights[hndex]),weights_name[hndex]+"/F");//get branch for different wgts;


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
		input_tree->SetBranchAddress("MCEVNT.wt",&mcevent_weight);//SET branch before using it!

//		cout<<input_tree->GetEntries("ibkgd==1&&MCEVNT.INNO=="+particle_type[hndex])<<" CCQE entries"<<endl;	//only CCQE events
//		cout<<"MCEVNT.IPFS=="+fin_par[hndex]+" "<<input_tree->GetEntries("MCEVNT.IPFS=="+fin_par[hndex])<<" final state particles identified entries"<<endl;	//only CCQE events
//		exit(0);
//		cout<<input_tree->GetEntries("ibkgd==1&&MCEVNT.INNO=="+particle_type[hndex]+"&&MCEVNT.IPFS=="+fin_par[hndex])<<" CCQE entries"<<endl;	//only CCQE events

		//STEP3.1: get 2dhistogrm of mB CCQE events 
		TH2F* twodhist_CCQE_mb = new TH2F("CCQEhist", "CCQEhist",nb1,nle,nhe,nb2,nlz,nhz);//binning for xnbins,xmin,xmax,ybins,ymin.ymax;
		Long64_t Nevents = input_tree->GetEntries();
		int counts = 0;
		std::cout<<"Filling in MB CCQE events "<<std::endl;
		for(Long64_t index = 0; index<Nevents; ++index){//take action here;
			std::cout<<"\r"<<index<<" of "<< Nevents <<" finish";
			input_tree->GetEntry(index);
			for(int jndex = 0; jndex < NFSP; ++jndex){
//			if(ibkgd!= 1 || INNO!=particle_type[hndex]|| IPFS[jndex]!=fin_par[hndex]) continue; //if it is not CCQE or not nue/numu, skip;
			if(ibkgd ==1 && INNO == particle_type[hndex] && (IPFS[jndex]==fin_par[2*hndex]||IPFS[jndex]==fin_par[2*hndex+1])){
			counts++;
			twodhist_CCQE_mb->Fill(sqrt(pow(PFSPP[jndex],2)+pow(lep_mass[hndex],2)), PFSPZ[jndex], mcevent_weight);
			}
			}
		}


		//PFSPP is in GeV;
//		TString psquare = "MCEVNT.PFSPP*MCEVNT.PFSPP";
//		TString Lepton_E = "sqrt("+psquare+"+"+lep_mass_square[hndex]+")";

//		input_tree->Project("CCQEhist","MCEVNT.PFSPZ:"+Lepton_E,"1");//("ibkgd==1&&MCEVNT.INNO=="+std::to_string(particle_type[hndex])+"&&MCEVNT.IPFS=="+std::to_string(fin_par[hndex])).c_str());
//		std::cout<<"\nFinish filling "<< twodhist_CCQE_mb->Integral()<<" CCQE events"<<std::endl;
		std::cout<<"\nFinish filling "<< counts<<" NUANCE CCQE events"<<std::endl;
		if(debug_message){
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"Bin "<<index<<","<<jndex<<" has events "<<twodhist_CCQE_mb->GetBinContent(index,jndex) <<std::endl;
				}
			}
			std::cout<<" Total: "<<twodhist_CCQE_mb->Integral()<<std::endl;
		}
		//STEP3.2: normalize 2dhistogram of mB CCQE, sum(uB CCQE, ub MEC);
		TFile* f2 = TFile::Open(GENIE_2dhist,"READ");
		TH2F* twodhist_CCQE_new = (TH2F*) f2->Get(tags[hndex]+"CCQEhist");
		TH2F* twodhist_MEC_new = (TH2F*) f2->Get(tags[hndex]+"MEChist");

		TH2F* norm_CCQE_mb = (TH2F*) twodhist_CCQE_mb->Clone("norm_CCQE_mb");
		TH2F* CCQEMEC_new = (TH2F*) twodhist_CCQE_new->Clone("CCQEMEC_new");
		//make CCEMEC sample;
		CCQEMEC_new->Add(twodhist_MEC_new);


		TH2F* norm_CCQEMEC_new = (TH2F*) CCQEMEC_new->Clone("norm_CCQEMEC_new");
		//start to normalize them;
		bool cross_compare = true;
		if(cross_compare){//kill ub events that does not shown in mb;
			TH2F* temp_mb = (TH2F*) norm_CCQE_mb->Clone("temp_CCQE_mb");
			//		TH2F* temp_ub = (TH2F*) norm_CCQEMEC_new->Clone("temp_CCQEMEC_new");
			temp_mb->Divide(norm_CCQE_mb);//prepare two binary 2dhist, 0 or 1
			//		temp_ub->Divide(norm_CCQEMEC_new);

			//only keep cross-over slots;
			//		norm_CCQE_mb->Divide(temp_ub);
			norm_CCQEMEC_new->Divide(temp_mb);
			twodhist_CCQE_new->Divide(temp_mb);
			twodhist_MEC_new->Divide(temp_mb);

			std::cout<<tags[hndex]<<" CCQE 2dhist from ub, has been matched to CCQE mb 2dhist."<<std::endl;
		}
		norm_CCQE_mb->Scale(1./twodhist_CCQE_mb->Integral());//done
		std::cout<<tags[hndex]<<" CCQE 2dhist from mb is normalized"<<std::endl;
		norm_CCQEMEC_new->Scale(1./norm_CCQEMEC_new->Integral());//done
		std::cout<<tags[hndex]<<" CCQE+MEC 2dhist from ub is normalized"<<std::endl;

		if(debug_message){
		std::cout<<"Check normalized NUANCE CCQE"<<std::endl; 
		for(int index = 1; index<nb1+1 ; ++index){
			for(int jndex = 1; jndex<nb2+1 ; ++jndex){
				std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<norm_CCQE_mb->GetBinContent(index,jndex) <<std::endl;
			}
		}
		}

		//STEP3.3: calculate weight of each event based on (ub CCQE+ ub MEC)/(mb CCQE);
		TH2F* weight_2dhist = (TH2F*) norm_CCQEMEC_new->Clone("weights");

		weight_2dhist->Divide(norm_CCQE_mb);

		std::cout<<"Applying weight from normalized 2dhist: (CCQE+MEC)_new/(CCQE)_mb"<<std::endl;

		//Add CCQE, MEC ratio respecively;
		if(debug_message){
		std::cout<<"Check non-scaled GENIE CCQE"<<std::endl; 
		for(int index = 1; index<nb1+1 ; ++index){
			for(int jndex = 1; jndex<nb2+1 ; ++jndex){
				std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<twodhist_CCQE_new->GetBinContent(index,jndex) <<std::endl;
			}
		}
		}
		twodhist_CCQE_new->Scale(1./twodhist_CCQE_new->Integral());

		if(debug_message){
		std::cout<<"Check scaled GENIE CCQE"<<std::endl; 
		for(int index = 1; index<nb1+1 ; ++index){
			for(int jndex = 1; jndex<nb2+1 ; ++jndex){
				std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<twodhist_CCQE_new->GetBinContent(index,jndex) <<std::endl;
			}
		}
		}
		twodhist_CCQE_new->Divide(norm_CCQE_mb);
		twodhist_MEC_new->Scale(1./twodhist_MEC_new->Integral());
		twodhist_MEC_new->Divide(norm_CCQE_mb);

		if(debug_message){
			std::cout<<"Check weights for CCQE to CCQE"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<twodhist_CCQE_new->GetBinContent(index,jndex) <<std::endl;
				}
			}
		}

		if(debug_message){
			std::cout<<"Check weights for CCQE to MEC"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<twodhist_MEC_new->GetBinContent(index,jndex) <<std::endl;
				}
			}
		}
		//STEP3.4: Apply weight; take this out;

//		counts = 0;
//		std::cout<<"Calculating the weight"<<std::endl;
//		for(Long64_t index = 0; index<Nevents; ++index){//take action here;
//			std::cout<<"\r"<<index<<" of "<< Nevents <<" finish";
//			input_tree->GetEntry(index);
//
//			vector<float> many_weights;
//			for(int jndex = 0; jndex < NFSP; ++jndex){
//
//				if(ibkgd ==1 && INNO == particle_type[hndex] && (IPFS[jndex]==fin_par[2*hndex]||IPFS[jndex]==fin_par[2*hndex+1])){//CCQE, nue/numu, e/mu
//					TAxis *xaxis = weight_2dhist->GetXaxis();
//					TAxis *yaxis = weight_2dhist->GetYaxis();
//					Int_t xbin = xaxis->FindBin(sqrt(pow(PFSPP[jndex],2)+pow(lep_mass[hndex],2)));
//					Int_t ybin = yaxis->FindBin(PFSPZ[jndex]);
//					many_weights.push_back(weight_2dhist->GetBinContent(xbin,ybin));
//				}
//			}
//
//			weights[hndex] = accumulate(many_weights.begin(),many_weights.end(),0.0);
//			if(weights[hndex] == 0) weights[hndex] = 1.0;//mcevent_weight;
//			if(counts < 7) std::cout<<" Get a weight "<<weights[hndex]<<std::endl;
//			counts++;
//			out_tree->Fill();
//		}
//		output->cd();
//		out_tree->Write();

		std::cout<<"\nFinish writing "<< counts<<" non-zero weights."<<std::endl;

		//STEP 4: configure and save 2dhistograms;

		gROOT->SetBatch(kTRUE);//No drawing windows pop up
		TCanvas *c1 = new TCanvas("c1","",900,400);
		draw_axises(weight_2dhist,tags[hndex]+" (CCQE+MEC)_{GENIE3.0.6,normalized}/CCQE_{mb,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQEMECmbratio.pdf","pdf");


		c1->Clear();
		draw_axises(twodhist_CCQE_new,tags[hndex]+"CCQE_{GENIE3.0.6}/CCQE_{mb,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQEratio.pdf","pdf");

		c1->Clear();
		draw_axises(twodhist_MEC_new,tags[hndex]+"MEC_{GENIE3.0.6}/CCQE_{mb,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"MECratio.pdf","pdf");

		c1->Clear();
		draw_axises(twodhist_CCQE_mb,tags[hndex]+"CCQE_{mb}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"twodhist_CCQE.pdf","pdf");

		c1->Clear();
		draw_axises(norm_CCQE_mb,tags[hndex]+"Normalized CCQE_{mb}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_CCQE_mb.pdf","pdf");

		c1->Clear();
		draw_axises(CCQEMEC_new,tags[hndex]+" (CCQE+MEC)_{GENIE3.0.6}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQEMEC_genie3.pdf","pdf");

		c1->Clear();
		draw_axises(norm_CCQEMEC_new,tags[hndex]+"Normalized (CCQE+MEC)_{GENIE3.0.6}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_CCQEMEC_genie3.pdf","pdf");

		//draw weights in 1dhist;
//		c1->Clear();
//		out_tree->Draw(weights_name[hndex]+">>"+tags[hndex]+"(60,0.01,20)");
//		TH1* weight_hist = (TH1*)gDirectory->Get(tags[hndex]);//fill TH1*
//		weight_hist->GetXaxis()->SetTitle(tags[hndex]+" Weights");
//		weight_hist->GetYaxis()->SetTitle("Events");
//		c1->SaveAs(output_dir+"test_"+tags[hndex]+"1dweights.pdf","pdf");


//		std::cout<<"Finish preparing "<<tags[hndex]<<" weights"<<std::endl;

		//save the 2d ratio;
		save_weights_file->cd();
		weight_2dhist->Write();
		twodhist_CCQE_new->Write();
		twodhist_MEC_new->Write();

//		output->Close();	
		f1->Close();	
		f2->Close();	
	}
	save_weights_file->Close();

	return 0;
}


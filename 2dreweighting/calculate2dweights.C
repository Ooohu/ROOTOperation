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
//	vector<TString> NUANCEfiles = {"timing_MC_nue_may07_2.root","MC_numu_all_00_15.root"};
	vector<TString> NUANCEfiles = {"MC_nu_all_1_16.root","MC_numu_all_00_15.root"};
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
		TFile* f1=TFile::Open("/scratch/condor-tmp/klin/data_timing_root/"+NUANCEfiles[hndex],"READ"); //open a root file
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
		TH2F* CCQE_old = new TH2F("CCQEhist", "CCQEhist",nb1,nle,nhe,nb2,nlz,nhz);//binning for xnbins,xmin,xmax,ybins,ymin.ymax;
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
			CCQE_old->Fill(sqrt(pow(PFSPP[jndex],2)+pow(lep_mass[hndex],2)), PFSPZ[jndex], mcevent_weight);
			}
			}
		}


		//PFSPP is in GeV;
//		TString psquare = "MCEVNT.PFSPP*MCEVNT.PFSPP";
//		TString Lepton_E = "sqrt("+psquare+"+"+lep_mass_square[hndex]+")";

//		input_tree->Project("CCQEhist","MCEVNT.PFSPZ:"+Lepton_E,"1");//("ibkgd==1&&MCEVNT.INNO=="+std::to_string(particle_type[hndex])+"&&MCEVNT.IPFS=="+std::to_string(fin_par[hndex])).c_str());
//		std::cout<<"\nFinish filling "<< CCQE_old->Integral()<<" CCQE events"<<std::endl;
		std::cout<<"\nFinish filling "<< counts<<" NUANCE CCQE events"<<std::endl;
		if(debug_message){
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"Bin "<<index<<","<<jndex<<" has events "<<CCQE_old->GetBinContent(index,jndex) <<std::endl;
				}
			}
			std::cout<<" Total: "<<CCQE_old->Integral()<<std::endl;
		}

		//STEP3.2: normalize 2dhistogram of old CCQE, new CCQE, new MEC, and new CCQE+MEC;
		TFile* f2 = TFile::Open(GENIE_2dhist,"READ");
		TH2F* norm_CCQE_new = (TH2F*) f2->Get(tags[hndex]+"CCQEhist");//GENIE CCQE
		TH2F* norm_MEC_new = (TH2F*) f2->Get(tags[hndex]+"MEChist");//GENIE MEC

		TH2F* norm_CCQE_old_CCQE = (TH2F*) CCQE_old->Clone("norm_CCQE_old_CCQE");//NUANCE CCQE
		TH2F* norm_CCQE_old_MEC = (TH2F*) CCQE_old->Clone("norm_CCQE_old_MEC");//NUANCE CCQE
		TH2F* norm_CCQE_old_CCQEMEC = (TH2F*) CCQE_old->Clone("norm_CCQE_old_CCQEMEC");//NUANCE CCQE
		TH2F* CCQEMEC_new = (TH2F*) norm_CCQE_new->Clone("CCQEMEC_new");//GENIE CCQE+MEC
		
		//make CCQEMEC sample;
		CCQEMEC_new->Add(norm_MEC_new);


		TH2F* norm_CCQEMEC_new = (TH2F*) CCQEMEC_new->Clone("norm_CCQEMEC_new");
		//start to normalize them;
		bool cross_compare = true;
		if(cross_compare){//kill ub events that does not shown in NUANCE;
			TH2F* temp_old = (TH2F*) norm_CCQE_old_CCQE->Clone("temp_CCQE_NUANCE_CCQE");
			TH2F* temp_new = (TH2F*) norm_CCQE_new->Clone("temp_CCQE_new");
			TH2F* temp_new2 = (TH2F*) norm_MEC_new->Clone("temp_EMEC_new");
			TH2F* temp_new3 = (TH2F*) norm_CCQEMEC_new->Clone("temp_CCQEMEC_new");

			temp_old->Divide(norm_CCQE_old_CCQE);//prepare two binary 2dhist, 0 or 1
			temp_new->Divide(norm_CCQE_new);
			temp_new2->Divide(norm_MEC_new);
			temp_new3->Divide(norm_CCQEMEC_new);

			//only keep cross-over slots;
			norm_CCQE_old_CCQE->Divide(temp_new);
			norm_CCQE_old_MEC->Divide(temp_new2);
			norm_CCQE_old_CCQEMEC->Divide(temp_new3);
			norm_CCQE_new->Divide(temp_old);
			norm_MEC_new->Divide(temp_old);
			norm_CCQEMEC_new->Divide(temp_old);

			std::cout<<tags[hndex]<<" GENIE and NUANCE 2dhist are now 100\% overlap."<<std::endl;
		}

		norm_CCQE_old_CCQE->Scale(1./norm_CCQE_old_CCQE->Integral());//CCQE
		std::cout<<tags[hndex]<<" CCQE 2dhist from NUANCE is normalized check total sum:"<<norm_CCQE_old_CCQE->Integral()<<std::endl;
		norm_CCQE_old_MEC->Scale(1./norm_CCQE_old_MEC->Integral());//MEC
		std::cout<<tags[hndex]<<" CCQE 2dhist from NUANCE is normalized check total sum:"<<norm_CCQE_old_MEC->Integral()<<std::endl;
		norm_CCQE_old_CCQEMEC->Scale(1./norm_CCQE_old_CCQEMEC->Integral());//CCQEMEC
		std::cout<<tags[hndex]<<" CCQE 2dhist from NUANCE is normalized check total sum:"<<norm_CCQE_old_CCQEMEC->Integral()<<std::endl;

		norm_CCQEMEC_new->Scale(1./norm_CCQEMEC_new->Integral());//done
		std::cout<<tags[hndex]<<" CCQE+MEC 2dhist from GENIE is normalized check total sum:"<<norm_CCQEMEC_new->Integral()<<std::endl;

		norm_CCQE_new->Scale(1./norm_CCQE_new->Integral());
		std::cout<<tags[hndex]<<" CCQE 2dhist from GENIE is normalized check total sum:"<<norm_CCQE_new->Integral()<<std::endl;

		norm_MEC_new->Scale(1./norm_MEC_new->Integral());
		std::cout<<tags[hndex]<<" MEC 2dhist from GENIE is normalized check total sum:"<<norm_MEC_new->Integral()<<std::endl;

		if(debug_message){
			std::cout<<"Check normalized NUANCE CCQE for CCQE"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<norm_CCQE_old_CCQE->GetBinContent(index,jndex) <<std::endl;
				}
			}
			std::cout<<"Check normalized GENIE CCQE"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<norm_CCQE_new->GetBinContent(index,jndex) <<std::endl;
				}
			}

			std::cout<<"Check normalized GENIE MEC"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<norm_MEC_new->GetBinContent(index,jndex) <<std::endl;
				}
			}
		}

		//STEP3.3: calculate weight of each event based on (ub CCQE+ ub MEC)/(NUANCE CCQE);
		//Add CCQE, MEC ratio respecively;
		TH2F* CCQE_ratio = (TH2F*) norm_CCQE_new->Clone(tags[hndex]+"CCQE_ratio");//GENIE CCQE+MEC for ratio
		TH2F* MEC_ratio = (TH2F*) norm_MEC_new->Clone(tags[hndex]+"MEC_ratio");//GENIE CCQE+MEC for ratio
		TH2F* CCQEMEC_ratio = (TH2F*) norm_CCQEMEC_new->Clone(tags[hndex]+"CCQEMEC_ratio");//CCQEMEC ratio

		CCQE_ratio->Divide(norm_CCQE_old_CCQE);
		MEC_ratio->Divide(norm_CCQE_old_MEC);
		CCQEMEC_ratio->Divide(norm_CCQE_old_CCQEMEC);

		if(debug_message){
			std::cout<<"Check weights for CCQE to CCQE"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<CCQE_ratio->GetBinContent(index,jndex) <<std::endl;
				}
			}
			std::cout<<"Check weights for CCQE to MEC"<<std::endl; 
			for(int index = 1; index<nb1+1 ; ++index){
				for(int jndex = 1; jndex<nb2+1 ; ++jndex){
					std::cout<<"weights Bin "<<index<<","<<jndex<<" has events "<<MEC_ratio->GetBinContent(index,jndex) <<std::endl;
				}
			}
		}

		//STEP 4: configure and save 2dhistograms;

		gROOT->SetBatch(kTRUE);//No drawing windows pop up
		TCanvas *c1 = new TCanvas("c1","",900,400);
		
		//ratios
		draw_axises(CCQEMEC_ratio,tags[hndex]+" (CCQE+MEC)_{GENIE3.0.6,normalized}/CCQE_{NUANCE,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQEMECratio.pdf","pdf");

		c1->Clear();
		draw_axises(CCQE_ratio,tags[hndex]+" CCQE_{GENIE3.0.6,normalized}/CCQE_{NUANCE,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQEratio.pdf","pdf");

		c1->Clear();
		draw_axises(MEC_ratio,tags[hndex]+" MEC_{GENIE3.0.6,normalized}/CCQE_{NUANCE,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"MECratio.pdf","pdf");

		//2dhist of one sample
		c1->Clear();//NUANCE
		draw_axises(CCQE_old,tags[hndex]+" CCQE_{NUANCE}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQE_nuance.pdf","pdf");

		c1->Clear();
		draw_axises(norm_CCQE_old_CCQE,tags[hndex]+" CCQE_{NUANCE,normalized} for GENIE CCQE");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_CCQE_nuance_for_CCQE.pdf","pdf");

		c1->Clear();
		draw_axises(norm_CCQE_old_MEC,tags[hndex]+" CCQE_{NUANCE,normalized} for GENIE MEC");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_CCQE_nuance_for_MEC.pdf","pdf");

		c1->Clear();
		draw_axises(norm_CCQE_old_CCQEMEC,tags[hndex]+" CCQE_{NUANCE,normalized} for GENIE CCQE+MEC");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_CCQE_nuance_for_CCQEMEC.pdf","pdf");

		c1->Clear();//GENIE
		draw_axises(norm_CCQE_new,tags[hndex]+" CCQE_{GENIE3.0.6,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_CCQE_genie3.pdf","pdf");

		c1->Clear();
		draw_axises(norm_MEC_new,tags[hndex]+" MEC_{GENIE3.0.6,normalized}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"norm_MEC_genie3.pdf","pdf");

		c1->Clear();//CCQEMEC
		draw_axises(CCQEMEC_new,tags[hndex]+" (CCQE+MEC)_{GENIE3.0.6}");
		c1->SaveAs(output_dir+"calc_"+tags[hndex]+"CCQEMEC_genie3.pdf","pdf");

		c1->Clear();
		draw_axises(norm_CCQEMEC_new,tags[hndex]+" (CCQE+MEC)_{GENIE3.0.6,normalized}");
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
		CCQEMEC_ratio->Write();
		CCQE_ratio->Write();
		MEC_ratio->Write();

//		output->Close();	
		f1->Close();	
		f2->Close();	
	}
	save_weights_file->Close();

	return 0;
}


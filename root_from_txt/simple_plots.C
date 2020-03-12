#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TDirectory.h"
#include <sys/stat.h>
#include <math.h>
//#include "Gadget.h"
//#include "Tree.h"
/*
 *
 * This script draws comparison between samples with different definitions
 * | |   |   see contents[i][4]
 * look for 1gamma1p
 */
using namespace std;

//set plot conf. here:
void set_conf( TH1* & plot, int preset1, int preset2){
	
	switch(preset1){//styles
		case 0:
//			plot->SetFillStyle(0);
//			plot->SetFillStyle(3144);
//			plot->SetFillColor(kRed-9);
//			plot->SetFillColorAlpha(kRed-9,1);
			break;
		case 1:
//			plot->SetFillStyle(3344);
			plot->SetFillColor(kGreen-8);
			plot->SetLineColor(kGreen-8);
			break;
	}

	switch(preset2){//samples
		case 0:
			(preset1>0)?plot->SetFillStyle(3144):
			plot->SetLineColor(kRed-9);
//			plot->SetFillColor(kRed-9);
			break;
		case 1:
			(preset1>0)?plot->SetFillStyle(3305):
			plot->SetLineColor(kBlue-9);
//			plot->SetFillColor(kRed-9);
			break;
		case 2:
			(preset1>0)?plot->SetFillStyle(3014):
			plot->SetLineColor(kGreen-5);
//			plot->SetFillColor(kRed-9);
			break;
		case 3:
			(preset1>0)?plot->SetFillStyle(3436):
			plot->SetLineColor(kViolet-3);
//			plot->SetFillColor(kRed-9);
			break;
	}

}

void simple_plots(){
	bool show_constraint = true;
	//	string sig_name = "/pnfs/uboone/persistent/users/klin/mcc9BBvertex_studies/v08_00_00_19/vertex/deltaradv3.4_optimize2.root";

	//	string sig_name = "/pnfs/uboone/persistent/users/klin/mcc9BBvertex_studies/v08_00_00_19/vertex/ncpi0v3.2_optimize1.root";
	TString dir = "./";//"/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v26/";
	vector <TString> files = {"Reading_Output.root"};
	int nfiles	= files.size();
	vector<int> max_height(2);

	vector <TFile*> sample(nfiles); 
	vector <TTree*> tree(nfiles); 

	for(int nthfile = 0; nthfile < nfiles; ++nthfile){
		sample[nthfile] = TFile::Open( dir + files[nthfile] );
		tree[nthfile] = (TTree* ) sample[nthfile] ->Get("T");//CHECK, this is different
	}
	//prepare directory to save plots.
	string directory = "./save_histograms/";
	mkdir(directory.c_str(),0777);

	//variable
	vector<string> parameter =  {"parameter_dist_tt","parameter_dist_sx","parameter_dist_st","parameter_dist_sst"};


	//name
	vector<string> signal_name = {"Single Shower","Missing 2nd Shower","Tracks"};//put them in one plot;
	vector<string> signal_def = {"label==1","label==2","label==0"};

	int ntypes = signal_name.size();//# of legends
	
	vector<string> sample_name = {""};

	vector<string> plot_name = {"Three Types of Pandora Shower"};
	//name, binning
	vector<string> binning = {"(20,0,10)","(20,0,80000)","(10,0,500)","(20,0,30)"};
	vector<string> variables = {"reco_shower_energy_plane0",binning[2],
								"reco_shower_energy_plane1",binning[2],
								"reco_shower_energy_plane2",binning[2],
								"reco_shower_plane0_nhits",binning[2],
								"reco_shower_plane1_nhits",binning[2],
								"reco_shower_plane2_nhits",binning[2],
								"reco_shower_plane0_meanRMS",binning[0],
								"reco_shower_plane1_meanRMS",binning[0],
								"reco_shower_plane2_meanRMS",binning[0],
								"reco_shower_dEdx_plane0_median",binning[0],
								"reco_shower_dEdx_plane1_median",binning[0],
								"reco_shower_dEdx_plane2_median",binning[0],
								"reco_shower_dEdx_amalgamated",binning[0],
								"reco_shower_dQdx_plane0_median",binning[1],
								"reco_shower_dQdx_plane1_median",binning[1],
								"reco_shower_dQdx_plane2_median",binning[1],
								"reco_shower_dEdx_plane0_mean",binning[0],
								"reco_shower_dEdx_plane1_mean",binning[0],
								"reco_shower_dEdx_plane2_mean",binning[0],
								"reco_shower_dEdx_plane0_max",binning[0],
								"reco_shower_dEdx_plane1_max",binning[0],
								"reco_shower_dEdx_plane2_max",binning[0],
								"reco_shower_dEdx_plane0_min",binning[0],
								"reco_shower_dEdx_plane1_min",binning[0],
								"reco_shower_dEdx_plane2_min",binning[0],
								"reco_shower_dEdx_plane0_nhits",binning[3],
								"reco_shower_dEdx_plane1_nhits",binning[3],
								"reco_shower_dEdx_plane2_nhits",binning[3],
								"max(max(reco_shower_dEdx_plane0_max,reco_shower_dEdx_plane1_max),reco_shower_dEdx_plane2_max)",binning[0],
								"1*(reco_shower_dEdx_plane0_max+reco_shower_dEdx_plane1_max+reco_shower_dEdx_plane2_max-min(min(reco_shower_dEdx_plane0_max,reco_shower_dEdx_plane1_max),reco_shower_dEdx_plane2_max))/2",binning[0]
	};
	cout<<"# of variables to be examinzed "<<variables.size()/2<<endl;
	//pre_contents: {variables, binning, constraint,legend,  x-axis label}
	//pdf name is made from "legend + x-axis +plot title";
	vector<string> empty_vector(5);
	vector <vector <string> > pre_contents(variables.size()/2,empty_vector);

	for(int i = 0; i<variables.size()/2; i++){
		pre_contents[i][0] = variables[2*i];
		pre_contents[i][1] = variables[2*i+1];
		pre_contents[i][2] = signal_def[0];//constraint
		pre_contents[i][3] = signal_name[0];//legend name
		pre_contents[i][4] = variables[2*i];//x-axis label
	}

/*	vector <vector <string> > pre_contents = {
		{"reco_shower_opening_angle", binning[1],  true_sig[1] , true_sig[0], "Reco Shower OpenAng [rad]"}
	};
	*/

	//contents: {variables, binning, constraint,legend,  x-axis label, y-axis label, plot title}
	vector <vector <string> > contents(pre_contents.size()*ntypes);//duplicate elements according to # of ntypes
	
	//the following will swap definitions to make overlay plots
	for(int loop = 0; loop<pre_contents.size(); loop++){
		for(int repeat = 0; repeat < ntypes; repeat++){ 
			int new_index = ntypes*loop+repeat;

//			vector<string> new_str = signal_def;
//			if(repeat>0) new_str = true_sig2;

			contents[new_index] = pre_contents[loop];
			contents[new_index][2] = signal_def[repeat];
			contents[new_index][3] = signal_name[repeat];
			contents[new_index][4] = contents[new_index][0];//CHECK too lazy to change name;
			contents[new_index].push_back("Vertices");
			contents[new_index].push_back(plot_name[0]);
//			cout<<"record "<<new_str[0]<<endl;
		}
	}//at this point, each inpu has their own line;

	// 2 types x 4 samples; ntypes = 2, nfiles = 4
	int nplots = ntypes;//ntypes*nfiles;//i.e. 8

	for(int index = 0; index < contents.size()/ntypes; index++){//each index for bobby/pandora plots; and there are contents.size()/2 of them;
		cout<<"Drawing the "<< index+1<< " plots of "<<contents.size()/ntypes<<": "<<contents[index][0]<<endl;
		int i = ntypes*index;//i=0,2,4...
		TCanvas* c1 = new TCanvas("","",800,600);
		//		TPad* p = (TPad*)c1->cd();//Make the Log
		//		p->SetLogy();
		THStack * stacks = new THStack("hs","");

		vector< TH1* > histograms(nplots);//draw two TH1s for 2 types
		//		vector<int> events_num(nplots);

		//make stack
		TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
		l->SetLineColor(kWhite);
		l->SetFillStyle(0);
		l->SetNColumns(2);

		double max;
		double min;
		for(int job = 0; job < nplots; job++){//two histograms packed into a stack;
//			max_height[job] = 0;
			int nthplot = i+job;//current plot
			int k = 0;
//			tree[k]->Draw((contents[nthplot][0]+">>events"+to_string(job)+contents[nthplot][1]).c_str(), (binninga).c_str(),"goff");
			const unsigned his_points = tree[k]->Draw((contents[nthplot][0]+">>events0"+to_string(job)+contents[nthplot][1]).c_str(), (contents[nthplot][2]).c_str(),"goff");
			cout<<"Drawing "<<contents[nthplot][2]<<endl;

			if(job < 1){
				double* his_array = tree[k]->GetV1();
				max  = (TMath::Ceil(*std::max_element(his_array, his_array + his_points)));//(contents[nthplot][0]).c_str());
				min  = (TMath::Floor(*std::min_element(his_array, his_array + his_points)));//(contents[nthplot][0]).c_str());
			}
			string binninga = "(40,"+to_string((int)std::round(min))+","+to_string((int)std::round(max))+")";
			cout<<"The binning "<<binninga<<endl;

			if(contents[nthplot][1].size()>2){binninga =contents[nthplot][1]; }

			tree[k]->Draw((contents[nthplot][0]+">>events"+to_string(job)+binninga).c_str(), (contents[nthplot][2]).c_str(),"goff");
			histograms[job] = (TH1*)gDirectory->Get(("events"+to_string(job)).c_str());//fill TH1*

			set_conf(histograms[job], 1, job);
			stacks->Add(histograms[job]);
			cout<<"Add "<<job<<"th histogram ("<<job<<","<<k<<")"<<endl;
			//Legend
			l->AddEntry(histograms[job], (contents[nthplot][3]+sample_name[k]).c_str(),"lf");
			//follwing line is for determine the max. height of plot;
//			max_height[job]+=(histograms[job]->GetMaximum());
		};
			stacks->Draw("hist");//nue

			//Set the titles;
			stacks->SetTitle((contents[i][6]).c_str());
			//		stacks->SetStats(false);
			stacks->GetYaxis()->SetTitle(contents[i][5].c_str());
			stacks->GetYaxis()->SetTitleOffset(1.5);
			stacks->GetXaxis()->SetTitle(contents[i][4].c_str());
			stacks->SetMinimum(0);
			stacks->SetMaximum(( stacks->GetMaximum())*1.2 );

			l->Draw();

			//Get numbers and optimize it; 


			string safe_name = contents[i][0]+contents[i][4]+contents[i][6]+to_string(index);
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '('), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ')'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '\\'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '/'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '['), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ']'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '+'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '-'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '*'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '.'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ' '), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ','), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '|'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '#'), safe_name.end());

			string final_name = directory+safe_name;
			if(final_name.length()>60) final_name.erase(61,final_name.length());
			cout<<"Save files with name:"<<final_name<<endl;
			c1->SaveAs((final_name+".pdf").c_str(),"pdf");
			delete c1;

	}//next stacked hist.
}


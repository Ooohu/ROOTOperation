#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TDirectory.h"
#include <sys/stat.h>
#include "Gadget.h"
#include "Tree.h"
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
			(preset1>0)?plot->SetFillStyle(3244):
			plot->SetLineColor(kBlue-9);
//			plot->SetFillColor(kRed-9);
			break;
		case 2:
			(preset1>0)?plot->SetFillStyle(3344):
			plot->SetLineColor(kGreen-5);
//			plot->SetFillColor(kRed-9);
			break;
		case 3:
			(preset1>0)?plot->SetFillStyle(3444):
			plot->SetLineColor(kViolet-3);
//			plot->SetFillColor(kRed-9);
			break;
	}

}

void BBhistograms(){
		bool show_constraint = true;
//	string sig_name = "/pnfs/uboone/persistent/users/klin/mcc9BBvertex_studies/v08_00_00_19/vertex/deltaradv3.4_optimize2.root";
	
//	string sig_name = "/pnfs/uboone/persistent/users/klin/mcc9BBvertex_studies/v08_00_00_19/vertex/ncpi0v3.2_optimize1.root";
	TString dir = "/pnfs/uboone/persistent/users/klin/mcc9BBvertex_studies/v08_00_00_19/vertex/"; 
	vector <TString> files = {  "deltaradv4.5_optimize5.root",
								"ncpi0v4.5_optimize5.root",
								"bnbv4.5_optimize5.root"};
	int nfiles	= files.size();
	vector<int> max_height(2);
	
	vector <TFile*> sample(nfiles); 
	vector <TTree*> tree(nfiles); 

	for(int nthfile = 0; nthfile < nfiles; ++nthfile){
//		gROOT->ProcessLine(".L Tree.C");//Tree.C is obtained by vertex_tree->MakeClass("Tree")
		sample[nthfile] = TFile::Open( dir + files[nthfile] );
		tree[nthfile] = (TTree* ) sample[nthfile] ->Get("singlephoton/vertex_tree");
	}

//	TFile* f1=TFile::Open(sig_name.c_str()); //open a root file
//	TTree * Tree_name = (TTree* )f1->Get("singlephoton/vertex_tree");

//	TTree* readThis = tree[0];//create an empty object for new stuff.
	//call tree[i] for each files, i = 0,1,2,3; for 4 files;
//	f1->GetObject("singlephoton/vertex_tree", readThis);

//	readThis->Draw("mctruth_nu_E[0]>>events(17,0,0.9)","1*1","hist");
	string directory = "./save_histograms/";
	mkdir(directory.c_str(),0777);
	
	//variable
	vector<string> parameter =  {"parameter_dist_tt","parameter_dist_sx","parameter_dist_st","parameter_dist_sst"};
	
	//binning	
	vector<string> binning = {"(30,0,35)", "(70,0,200)"};

	//name
//	vector<string> signal_name = {"Pandora ", "Bobby "};//put them in one plot;
	vector<string> signal_name = {"Perfect ", "Fake "};//put them in one plot;
	int ntypes = signal_name.size();

	vector<string> sample_name = {"NC#Delta", "NCPi0", "BNBbkg"};//, "BNBExt"};//stacked this;

	vector<string> plot_name = {"Reco. Objects Info.from DeltaRad Sample",
								"Where does pandora 1#gamma1p goes?",
								"BB in the same slice or not?",
								"Look at the parameters"};
	//type, pandora, bobby
	vector<string> pandora_conf = {"reco_asso_tracks == 0&&reco_asso_showers ==1","reco_asso_tracks == 1&&reco_asso_showers ==2"};
	vector<string> bb_conf = {"reco_bobbytracksv == 0&&reco_bobbyshowersv ==1","reco_bobbytracksv == 1&&reco_bobbyshowersv ==2"};
	int topo_type = 1;
	
	//{type, pandora, bobby}
	vector<string> true_sig = {"2#gammaXp",
		"abs(sim_shower_parent_pdg[0]) == 111&&abs(sim_shower_parent_pdg[1])==111&&" 
		"abs(sim_shower_pdg[0]) == 22&& abs(sim_shower_pdg[1]) == 22&&"+pandora_conf[topo_type],
		delta_num_str(2)+"&& mctruth_bobbyphotonshowerv==2&&reco_bobbytracksv=="
		"(mctruth_bobbydeltaradppdaughterv+mctruth_bobbydeltaradpdaughterv+mctruth_bobbydeltaradmdaughterv+"
		"mctruth_bobbydeltarad0daughterv+mctruth_bobbyotherdaughterv+mctruth_bobbyoverlayv)&&"+bb_conf[topo_type]};
	vector<string> fake_sig = {"Fake 0#gammaXp",
		"abs(sim_shower_pdg[0]) != 22 && abs(sim_shower_pdg[1]) != 22&&"+pandora_conf[topo_type],
		delta_num_str(0)+"&& mctruth_bobbyphotonshowerv==1&& reco_bobbytracksv=="
		"(mctruth_bobbydeltaradppdaughterv+mctruth_bobbydeltaradpdaughterv+mctruth_bobbydeltaradmdaughterv+"
		"mctruth_bobbydeltarad0daughterv+mctruth_bobbyotherdaughterv+mctruth_bobbyoverlayv-2)&&"+bb_conf[topo_type]};


/*
//		pandora_conf[topo_type],
	vector<string> true_sig = {"1#gamma1p ",
		"sim_shower_pdg[0] == 22&&sim_track_pdg[0]==2212&& mctruth_is_delta_radiative == 1&&"+pandora_conf[topo_type],
		delta_num_str(2)+"&& mctruth_bobbyphotonshowerv==1&&mctruth_bobbyprotontrackv==1&&"+bb_conf[topo_type]};
	vector<string> fake_shower = {"Half-reco. 0#gamma1p ",
		"sim_shower_pdg[0] != 22&&sim_track_pdg[0]==2212&& mctruth_is_delta_radiative == 1&&"+ pandora_conf[topo_type],
		delta_num_str(1)+"&& mctruth_bobbyphotonshowerv==0&& mctruth_bobbyprotontrackv==1&&"+bb_conf[topo_type]};
	vector<string> fake_track = { "Half-reco. 1#gamma0p ",
		"sim_shower_pdg[0] == 22&&sim_track_pdg[0]!=2212&& mctruth_is_delta_radiative == 1&&"+ pandora_conf[topo_type],
		delta_num_str(1)+"&& mctruth_bobbyphotonshowerv==1&& mctruth_bobbyprotontrackv==0&&"+bb_conf[topo_type]};
	vector<string> fake_sig = {"0#gamma0p ",
		"(sim_shower_pdg[0] != 22&&sim_track_pdg[0]!=2212 &&  mctruth_is_delta_radiative == 1)&&"+pandora_conf[topo_type],
		delta_num_str(0)+"&& mctruth_bobbyphotonshowerv + mctruth_bobbyprotontrackv == 0 &&"+bb_conf[topo_type]};
*/

	//constraint
//	string same_track = "reco_bobbytracksv == reco_asso_tracks";
	vector<string> found_vertex = {"reco_asso_showers>0","reco_bobbyshowersv>0"};
//	string good_vertex = "sqrt((reco_bobbyvertex_xv-reco_vertex_x)^2+(reco_bobbyvertex_yv-reco_vertex_y)^2+(reco_bobbyvertex_zv-reco_vertex_z)^2)<5";
	vector <string> resolution = {
		"sqrt((mctruth_nu_vertex_x-reco_vertex_x)^2+(mctruth_nu_vertex_y-reco_vertex_y)^2+(mctruth_nu_vertex_z-reco_vertex_z)^2)",
	"sqrt((reco_bobbyvertex_xv-mctruth_nu_vertex_x)^2+(reco_bobbyvertex_yv-mctruth_nu_vertex_y)^2+(reco_bobbyvertex_zv-mctruth_nu_vertex_z)^2)"};

	//	MCTruth
//	string deltarad = "sim_shower_pdg[0] == 22&&reco_asso_showers == 1";	
//	string deltarad = "sim_shower_pdg[0] == 22&&sim_track_pdg[0]==2212&&reco_asso_tracks==1&&reco_asso_showers==1";//all nc with pi0 already
//	string atleastdeltarad = "((Sum$(sim_shower_pdg==22)>0)&&(Sum$(sim_track_pdg==2212)>0)&&!(Sum$(sim_shower_pdg==22)==1||Sum$(sim_track_pdg==2212)==1))&&((reco_asso_showers>0&&reco_asso_tracks>0)||!(reco_asso_showers==1&&reco_asso_tracks==1))";//>0p||>1gamma
	//	Reco
	
	//Well-reconstructed 1p2gamma
//	string bb_1t1s = bb_conf[1] + "&&" + deltarad;//BB correctly recognized signal
//	string bkg_bb_1t1s =  bb_conf[1]+ "&&!(" + deltarad+")";//BB's misreconstructed signal
//
//	//Potentially well-reconstructed 1p2gamma
//	string pbb_1t1s =  bb_conf[1] + "&&" + atleastdeltarad;//BB reconstructed signal
//	string pbkg_bb_1t1s =  bb_conf[1] + "&&!(" + atleastdeltarad+")";//BB's misreconstructed signal

	//{variables, binning, constraint,legend,  x-axis label, y-axis label, plot title}
	//pdf name is made from "legend + x-axis +plot title";
	string temp =" (mctruth_bobbydeltaradppdaughterv+mctruth_bobbydeltaradpdaughterv+mctruth_bobbydeltaradmdaughterv+mctruth_bobbydeltarad0daughterv)>0";
	vector <vector <string> > contents = {
		{parameter[0], binning[0],  parameter[0]+"<999&&" + true_sig[2], true_sig[0],"Track-Track distance within an event [cm]" ,"Vertices", plot_name[3]},
		{parameter[0], binning[0],  parameter[0]+"<999&&" + fake_sig[2], fake_sig[0] ,"Track-Track distance within an event [cm]" ,"Vertices", plot_name[3]},
		{parameter[1], binning[0],  parameter[1]+"<999&&" + true_sig[2], true_sig[0],"Shower bkw projection point-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", plot_name[3]},
		{parameter[1], binning[0],  parameter[1]+"<999&&" + fake_sig[2], fake_sig[0] ,"Shower bkw projection point-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", plot_name[3]},
		{parameter[2], binning[0],  parameter[2]+"<999&&" + true_sig[2], true_sig[0],"Bkw point of Shower & Shower to Track End Point Distance [cm]" ,"Vertices", plot_name[3]},
		{parameter[2], binning[0],  parameter[2]+"<999&&" + fake_sig[2], fake_sig[0] ,"Bkw point of Shower & Shower to Track End Point Distance [cm]" ,"Vertices", plot_name[3]},
		{parameter[3], binning[1],  parameter[3]+"<999&&" + true_sig[2], true_sig[0],"Shower Track Conversion Distance [cm]" ,"Vertices", plot_name[3]},
		{parameter[3], binning[1],  parameter[3]+"<999&&" + fake_sig[2], fake_sig[0] ,"Shower Track Conversion Distance [cm]" ,"Vertices", plot_name[3]},
		{"sqrt( (mctruth_nu_vertex_x-sim_shower_start_x)^2+(mctruth_nu_vertex_y-sim_shower_start_y)^2+(mctruth_nu_vertex_z-sim_shower_start_z)^2)", binning[1], true_sig[2], true_sig[0], "True Conversion distance [cm]", "Vertices", "MCTruth Info"},
		{"sqrt( (mctruth_nu_vertex_x-sim_shower_start_x)^2+(mctruth_nu_vertex_y-sim_shower_start_y)^2+(mctruth_nu_vertex_z-sim_shower_start_z)^2)", binning[1], fake_sig[2],  fake_sig[0], "True Conversion distance [cm]", "Vertices", "MCTruth Info"}
//		
//		{parameter[0], binning[1],  parameter[0]+"<999&&" + temp, signal_name[1] + true_sig[0],"Track-Track distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[0], binning[1],  parameter[0]+"<999&&" + fake_sig[2], signal_name[1] + fake_sig[0] ,"Track-Track distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[1], binning[1],  parameter[1]+"<999&&" + temp, signal_name[1] + true_sig[0],"Shower-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[1], binning[1],  parameter[1]+"<999&&" + fake_sig[2], signal_name[1] + fake_sig[0] ,"Shower-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[2], binning[1],  parameter[2]+"<999&&" + temp, signal_name[1] + true_sig[0],"Shower-new track distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[2], binning[1],  parameter[2]+"<999&&" + fake_sig[2], signal_name[1] + fake_sig[0] ,"Shower-  ne track distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[3], binning[1],  parameter[3]+"<999&&" + temp, signal_name[1] + true_sig[0],"Shower&Shower-Track distance within an event [cm]" ,"Vertices", plot_name[3]},
//		{parameter[3], binning[1],  parameter[3]+"<999&&" + fake_sig[2], signal_name[1] + fake_sig[0] ,"Shower&Shower-Track distance within an event [cm]" ,"Vertices", plot_name[3]},
//	
//		{"sqrt((sim_track_startx[0]-sim_shower_start_x[0])^2+(sim_track_starty[0]-sim_shower_start_y[0])^2+(sim_track_startz[0]-sim_shower_start_z[0])^2)",
//		binning[1], "mctruth_num_exiting_photons==1&&mctruth_num_exiting_protons==1", "MCTruth", "s-t distance [cm]", "Vertices", "Look at the MCTruth"},
//		{" sqrt((sim_track_startx[0]-sim_shower_start_x[0])^2+(sim_track_starty[0]-sim_shower_start_y[0])^2+(sim_track_startz[0]-sim_shower_start_z[0])^2)",
//		binning[1], "mctruth_num_exiting_photons==1&&mctruth_num_exiting_protons==1", "MCTruth", "s-t distance [cm]", "Vertices", "Look at the MCTruth"}
//		//Analysis: Well-constructed 1t1s
////		{parameter[0], binning1,  parameter[0]+"<999&&"+bb_1t1s, true_sig[2],"Track-Track distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[0], binning1,  parameter[0]+"<999&&"+bkg_bb_1t1s, fake_sig[2] ,"Track-Track distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[1], binning1,  parameter[1]+"<999&&"+bb_1t1s, true_sig[2],"Shower-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[1], binning1,  parameter[1]+"<999&&"+bkg_bb_1t1s, fake_sig[2] ,"Shower-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[2], binning1,  parameter[2]+"<999&&"+bb_1t1s, true_sig[2],"Shower-new track distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[2], binning1,  parameter[2]+"<999&&"+bkg_bb_1t1s, fake_sig[2] ,"Shower-  ne track distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[3], binning1,  parameter[3]+"<999&&"+bb_1t1s, true_sig[2],"Shower&Shower-Track distance within an event [cm]" ,"Vertices", plot_name2},
////		{parameter[3], binning1,  parameter[3]+"<999&&"+bkg_bb_1t1s, fake_sig[2] ,"Shower&Shower-Track distance within an event [cm]" ,"Vertices", plot_name2},
////
////		//Analysis: Potentially Well-reconstructed 1t1s
////		{parameter[0], binning1,  parameter[0]+"<999&&"+pbb_1t1s, true_sig[1],"Track-Track distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[0], binning1,  parameter[0]+"<999&&"+pbkg_bb_1t1s, fake_sig[1] ,"Track-Track distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[1], binning1,  parameter[1]+"<999&&"+pbb_1t1s, true_sig[1],"Shower-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[1], binning1,  parameter[1]+"<999&&"+pbkg_bb_1t1s, fake_sig[1] ,"Shower-Track/Shower/Vertex distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[2], binning1,  parameter[2]+"<999&&"+pbb_1t1s, true_sig[1],"Shower-new track distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[2], binning1,  parameter[2]+"<999&&"+pbkg_bb_1t1s, fake_sig[1] ,"Shower-  ne track distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[3], binning1,  parameter[3]+"<999&&"+pbb_1t1s, true_sig[1],"Shower&Shower-Track distance within an event [cm]" ,"Vertices", pplot_name2},
////		{parameter[3], binning1,  parameter[3]+"<999&&"+pbkg_bb_1t1s, fake_sig[1] ,"Shower&Shower-Track distance within an event [cm]" ,"Vertices", pplot_name2},
	};
	


// 2 types x 4 samples; ntypes = 2, nfiles = 4
	int nplots = ntypes*nfiles;//i.e. 8


	for(int index = 0; index < contents.size()/ntypes; index++){//each index for bobby/pandora plots; and there are contents.size()/2 of them;
		cout<<"Drawing the "<< index+1<< " plots of "<<contents.size()/ntypes<<endl;
		int i = ntypes*index;
		for(int j = 0; j < ntypes; j++){//draw all two types of events;
		TCanvas* c1 = new TCanvas("","",800,600);
//		TPad* p = (TPad*)c1->cd();//Make the Log
//		p->SetLogy();
		THStack * stacks = new THStack("hs","");
//		TPad *d1 = new TPad("","",0,0,1,1);
//		d1->Draw();
//		d1->cd();

		vector< TH1* > histograms(nplots);//draw two TH1s for 2 types
//		vector<int> events_num(nplots);

//make stack
		TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
		l->SetLineColor(kWhite);
		l->SetFillStyle(0);
		l->SetNColumns(2);
		max_height[j] = 0;
		//j=0 - signal; 1 - fake signal;

//			stacks[j] = new THStack("hs","" );

			for(int k = 0; k<nfiles; k++){//each type make 4 samples;
				int cplot = i+j;//current plot
				int job = j*nfiles + k;
				TTree* readThis = tree[k];

				readThis->Draw((contents[cplot][0]+">>events"+to_string(job)+contents[cplot][1]).c_str(), (contents[cplot][2]).c_str(),"goff");
				cout<<"Drawing "<<contents[cplot][2]<<endl;

//				events_num[job] = readThis->GetEntries((contents[cplot][2]).c_str());
//				if(j==1) events_num[j]=0;
				histograms[job] = (TH1*)gDirectory->Get(("events"+to_string(job)).c_str());//fill TH1*
				set_conf(histograms[job], j, k);

				stacks->Add(histograms[job]);
				cout<<"Add "<<job<<"th histogram ("<<j<<","<<k<<")"<<endl;
				//Legend
				l->AddEntry(histograms[job], (signal_name[j]+contents[cplot][3]+sample_name[k]).c_str(),"lf");
				//follwing line is for determine the max. height of plot;
				max_height[j]+=(histograms[job]->GetMaximum());
				//		if(contents[i][6]=="DeltaRad Sample Vertex Resolution"){
				//
				//			l->AddEntry(lee, (contents[i][3]+" (mean: " + to_string(lee->GetMean())+") ").c_str(),"lf");
				//			l->AddEntry(nue, (contents[i+1][3]+" (mean: "+to_string(nue->GetMean())+") ").c_str(),"lf");
				//
				//		} else{
				//			l->AddEntry(lee, (contents[i][3]+" (# events "+to_string(events_num[0])+") ").c_str(),"lf");
				//			l->AddEntry(nue, (contents[i+1][3]+" (# events "+to_string(events_num[1])+") ").c_str(),"lf");
				//		}

			}
//		stacks[1]->Draw("hist");//lee
		stacks->Draw("hist");//nue

		//Set the titles;
		stacks->SetTitle((contents[i][6]).c_str());
//		stacks->SetStats(false);
		stacks->GetYaxis()->SetTitle(contents[i][5].c_str());
		stacks->GetYaxis()->SetTitleOffset(1.5);
		stacks->GetXaxis()->SetTitle(contents[i][4].c_str());
		stacks->SetMinimum(1);
		stacks->SetMaximum( max_height[j]*1.2 );
//		stacks->SetMaximum(( *std::max_element(max_height.begin(), max_height.end()))*1.2 );

		l->Draw();
		
//Get numbers and optimize it; 

		cout<<__LINE__<<endl;
//		cout<<"save one file with events: "<<events_num[0]<<" "<<events_num[1]<<endl;

		string safe_name = contents[i][3]+contents[i][4]+contents[i][6];
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

//		c1->SaveAs("test.pdf","pdf");
		c1->SaveAs((directory+safe_name+to_string(j)+".pdf").c_str(),"pdf");
		delete c1;
	}
	}
	
//	int bobby_total = Get_events(TTree* tree, std::string & def);
//	double best_cut = Optimize_cut(Vector<TTree*> trees,  std::string & var, std::string & sig_def, std::string & bkg_def);
	
//	int bobby_1g1p = Get_events(tree[0], true_sig[2]);
//	double best_cut = Optimize_cut(tree, parameter[0], true_sig[2], fake_sig[2], 20);

/*	
	vector <string> vertex_info = {"reco_bobbyvertex_xv","reco_bobbyvertex_yv","reco_bobbyvertex_zv",
						"reco_vertex_x","reco_vertex_y","reco_vertex_z",
							"mctruth_nu_vertex_x","mctruth_nu_vertex_y","mctruth_nu_vertex_z"};
	int j = 0;
	int bb_p_mct = 0;//bobbyvertex, pandora, mctruth_vertex
	int bb_p = 0;
	int p_mct = 0;
	int bb_mct = 0;
	int ref_bb_p_mct = 0;
	int ref_bb_p = 0;
	int ref_bb_mct = 0;
	int ref_p_mct = 0;
	double radius = 5;

	int bb_num = 0;
	int bb_num_t = 0;
	int ref_temp = 0;
	int ref_temp_t = 0;
	while(readThis->GetEntries( ("reco_bobbyvertex_xv["+std::to_string(j)+"]>0").c_str())>0){
		int temp =readThis->GetEntries( ("reco_bobbyvertex_xv["+std::to_string(j)+"]>0&&"+true_sig[2]).c_str());
		int temp_t =readThis->GetEntries( ("reco_bobbyvertex_xv["+std::to_string(j)+"]>0").c_str());

		cout<<ref_temp<<" "<<temp<<endl;
		if(temp != ref_temp){
			ref_temp = temp;
			bb_num +=ref_temp;
			cout<<"count bb 1s1t "<<bb_num<<endl;
		}

		if(temp_t != ref_temp_t){
			ref_temp_t = temp_t;
			bb_num_t +=ref_temp_t;
			cout<<"cout all bb "<<bb_num_t<<endl;
		}

		string var1 = get_ref(vertex_info,0,3,j)+"<"+std::to_string(radius)+"&&"+true_sig[2];
		string var2 = get_ref(vertex_info,0,6,j)+"<"+std::to_string(radius)+"&&"+true_sig[2];
		string var3 = (var1+"&&"+var2)+"&&"+true_sig[2];
		string var4 = get_ref(vertex_info,3,6,j)+"<"+std::to_string(radius)+"&&"+true_sig[2];
		int t_bb_p = readThis->GetEntries( var1.c_str() );
		int t_bb_mct = readThis->GetEntries( var2.c_str()  );
		int t_bb_p_mct = readThis->GetEntries( var3.c_str()  ); 
		int t_p_mct = readThis->GetEntries( var4.c_str()  ); 

		cout<<"\nCheck the number of vertices: "<<endl;

		if(t_bb_p != ref_bb_p) {
			bb_p = t_bb_p+bb_p;
			ref_bb_p = t_bb_p;
			cout<<bb_p<<" from bb-pandora: "<<var1<<endl;
		}
		if(t_bb_mct != ref_bb_mct) {
			bb_mct = t_bb_mct+bb_mct;
			ref_bb_mct = t_bb_mct;
			cout<<bb_mct<<" from bb-MCTruth: "<<var2<<endl;
		}	
		if(t_bb_p_mct != ref_bb_p_mct) {
			bb_p_mct = t_bb_p_mct+bb_p_mct;
			ref_bb_p_mct = t_bb_p_mct;
			cout<<bb_p_mct<<" from all: "<<var3<<endl;
		}
		if(t_p_mct != ref_p_mct) {
			p_mct = t_p_mct+p_mct;
			ref_p_mct = t_p_mct;
			cout<<p_mct<<" from pandora-MCTruth: "<<var3<<endl;
		}
		j++;
	}
	cout<<"BB Clearnliness "<<bb_p_mct*1.0/bb_p<<endl;
	cout<<"BB Completeness "<<bb_p_mct*1.0/bb_mct<<endl;
	cout<<"Pandora Completeness "<<bb_p_mct*1.0/p_mct<<endl;
	

	//Text Output:
	cout<<"Total Number of Events (Pandora) all: "<<readThis->GetEntries()<<endl;
	cout<<"Total Number of Events (Pandora) has vertex: "<<readThis->GetEntries(found_vertex[0].c_str())<<endl;

	cout<<"Total Number of Vertices (BB) all: "<<bb_num_t<<endl;
	cout<<"Total Number of Vertices (BB) 1s1t: "<<bb_num<<endl;
	cout<<"----"<<endl;
	cout<<"Total Number of Vertices (BB=Pandora=MCTruth): "<<bb_p_mct<<endl;
	cout<<"Total Number of Vertices (BB=Pandora): "<<bb_p<<endl;
	cout<<"Total Number of Vertices (BB=MCTruth): "<<bb_mct<<endl;
	cout<<"Total Number of Vertices (Pandora=MCTruth): "<<p_mct<<endl;
*/
//	f1->Close();

}


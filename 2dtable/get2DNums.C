#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

int get2DNums(){
	//STEP 0 Prepare files names

	//data, MC1, MC2, MC3
	TString dir = "/scratch/condor-tmp/klin/data_timing_root/";
	vector<TString> mbfiles = {"data_full1875pot.root","timing_MC_dirt_may07_2.root","timing_MC_fullosc_may07_2.root","timing_MC_nue_may07_2.root"};
	vector<TString> labels = {"Data","Dirt","Best-fit","Other"};

	vector<TString> constraints = {"1","1","(backgroundType<4&&backgroundType>0)","(backgroundType<6||backgroundType>8)"};
	vector<TString> weights = {"1","wgt","wgt*pow(1.267*0.04*nuleng/enugen,2)","wgt"};
	vector<double> POTs = {18.75E20,4.108E21,1.3278E20,4.108E21};

	vector<TString> variables = {"el.UZ","el.E"};//y,x
	vector<int> bins = {22,150,1250,20,-1,1};//x,x1,x2,y,y1,y2;
	
	vector<TH2F*> a_2dhist(labels.size()+1);

	double x_start	= bins[1];
	double x_end	= bins[2];
	double y_start	= bins[4];
	double y_end	= bins[5];

	double x_steps = (x_end-x_start)/bins[0];
	double y_steps = (y_end-y_start)/bins[3];

	for(size_t index = 0; index<labels.size(); ++index ){//go through different types of events
		//STEP 1 Prepare histograms

		std::cout<<"Looking at files "<<dir+mbfiles[index]<<std::endl;
		TFile* current_file = TFile::Open(dir+mbfiles[index],"READ");
		TTree* e_tree = dynamic_cast <TTree* > (current_file->Get("TTiming"));
		a_2dhist[index] = new TH2F(labels[index], labels[index]+" 2dhist",bins[0],bins[1],bins[2],bins[3],bins[4],bins[5]);//bin for xnbins,xmin,xmax,ybins,ymin.ymax;
//		e_tree->Project(labels[index],variables[0]+":"+variables[1],constraints[index]+"*"+weights[index]);
		e_tree->Project(labels[index],variables[0]+":"+variables[1],constraints[index]+"*"+weights[index]);

//		gROOT->SetBatch(kTRUE);//No drawing windows pop up
//		a_2dhist->Draw("COLZ");
		std::cout<<"Scale factor: "<<POTs[index]/POTs[0]<<std::endl;
		a_2dhist[index]->Scale(POTs[0]/POTs[index]);


		//STEP 2 Extract Numbers for tables.

		std::cout<<labels[index]<<" 2dhistogram"<<std::endl;
		std::cout<<setw(5)<<"E--> ";
		for(size_t jndex = 1; jndex < bins[0]+1; ++jndex){//go through x
			std::cout<<std::setw(5)<<x_start+x_steps*(jndex-1);
		}
		std::cout<<std::endl;

		for(size_t jndex = 1; jndex < bins[3]+1; ++jndex){//go through y
			double jndex_m = bins[3]+1-jndex;
			std::cout<<std::setw(5)<<y_start+y_steps*(jndex_m);//start from the largest y;
			for(size_t kndex = 1; kndex < bins[0]+1; ++kndex){//go through x
				std::cout<<std::setw(5)<<std::round(a_2dhist[index]->GetBinContent(kndex,jndex_m)*10)/10;
//				std::cout<<std::setw(5)<<kndex+jndex;
			}
			std::cout<<std::endl;
		}
		std::cout<<"Finish! \n"<<std::endl;

	}
	//thats it!
	//one more for summing up MC (no best-fit);
	int extra = labels.size();
	std::cout<<" Make some extras "<<extra<<std::endl;
	a_2dhist[extra] = (TH2F*) a_2dhist[1]->Clone();
	std::cout<<" # of histograms "<<a_2dhist.size()<<std::endl;

	a_2dhist[extra]->Add(a_2dhist[3]);
	
	std::cout<<"bkg MC 2dhistogram"<<std::endl;
	std::cout<<setw(5)<<"E--> ";
	for(size_t jndex = 1; jndex < bins[0]+1; ++jndex){//go through x
		std::cout<<std::setw(5)<<x_start+x_steps*(jndex-1);
	}
	std::cout<<std::endl;

	for(size_t jndex = 1; jndex < bins[3]+1; ++jndex){//go through y
		double jndex_m = bins[3]+1-jndex;
		std::cout<<std::setw(5)<<y_start+y_steps*(jndex_m);//start from the largest y;
		for(size_t kndex = 1; kndex < bins[0]+1; ++kndex){//go through x
			std::cout<<std::setw(5)<<std::round(a_2dhist[extra]->GetBinContent(kndex,jndex_m)*10)/10;
			//				std::cout<<std::setw(5)<<kndex+jndex;
		}
		std::cout<<std::endl;
	}
	std::cout<<"\n"<<std::endl;

    //data-MC
	a_2dhist[0]->Add(a_2dhist[extra],-1);
	
	std::cout<<"Data- (bkg MC) 2dhistogram"<<std::endl;
	std::cout<<setw(5)<<"E--> ";
	for(size_t jndex = 1; jndex < bins[0]+1; ++jndex){//go through x
		std::cout<<std::setw(5)<<x_start+x_steps*(jndex-1);
	}
	std::cout<<std::endl;

	for(size_t jndex = 1; jndex < bins[3]+1; ++jndex){//go through y
		double jndex_m = bins[3]+1-jndex;
		std::cout<<std::setw(5)<<y_start+y_steps*(jndex_m);//start from the largest y;
		for(size_t kndex = 1; kndex < bins[0]+1; ++kndex){//go through x
			std::cout<<std::setw(5)<<std::round(a_2dhist[0]->GetBinContent(kndex,jndex_m)*10)/10;
			//				std::cout<<std::setw(5)<<kndex+jndex;
		}
		std::cout<<std::endl;
	}
	std::cout<<"\n"<<std::endl;

	//with best-fit
	a_2dhist[extra]->Add(a_2dhist[2]);

	std::cout<<"Best-fit+bkg MC 2dhistogram"<<std::endl;
	std::cout<<setw(5)<<"E--> ";
	for(size_t jndex = 1; jndex < bins[0]+1; ++jndex){//go through x
		std::cout<<std::setw(5)<<x_start+x_steps*(jndex-1);
	}
	std::cout<<std::endl;

	for(size_t jndex = 1; jndex < bins[3]+1; ++jndex){//go through y
		double jndex_m = bins[3]+1-jndex;
		std::cout<<std::setw(5)<<y_start+y_steps*(jndex_m);//start from the largest y;
		for(size_t kndex = 1; kndex < bins[0]+1; ++kndex){//go through x
			std::cout<<std::setw(5)<<std::round(a_2dhist[extra]->GetBinContent(kndex,jndex_m)*10)/10;
			//				std::cout<<std::setw(5)<<kndex+jndex;
		}
		std::cout<<std::endl;
	}
	std::cout<<"\n"<<std::endl;

	return 0;
}


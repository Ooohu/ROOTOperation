
using namespace std;

void printhist_v1(){
	
//	char sbnfile_name[] = "/uboone/app/users/klin/ubflux_2_sbnflux/testing_directory/sbnmakehist_tool/test_kzero.root";
	char sbnfile_name[] = "/uboone/app/users/klin/ubflux_2_sbnflux/testing_directory/sbnmakehist_tool/test_nucleonAll.root";
	char ubfile_name[] = "/uboone/app/users/klin/ubflux_2_sbnflux/testing_directory/zkmakehist_tool/ubflux_ref_world_nonhadrons.root";
	std::string hist_type = "nucleoninexsec";
	double ymax=30000;

	std::string sbn_hname = "numu_ms_"+hist_type+"_Flux/hEnumu_"+hist_type+"_Flux_ms_";

//Hadron
	std::string special_name = hist_type+"_FluxUnisim";
//	std::string special_name = "kzero_PrimaryHadronSanfordWang";
	std::string ub_hname = "numu_ms_"+special_name+"/hEnumu_"+special_name+"_ms_";
//NonHadron
// std::string ub_hname = "numu_ms_"+hist_type+"_Flux/hEnumu_"+hist_type+"_Flux_ms_";
	
//std::cout<<std::to_string(0)<<std::endl;
//exit(0);
	TFile* f1=TFile::Open(sbnfile_name); //open a root file
	TFile* f2=TFile::Open(ubfile_name); //open a root file

	TCanvas c1("c1","A hist",50,20,500,1000);
	TPad* top_pd = new TPad("top", "top", 0,0.65,1,1);
	TPad* mid_pd = new TPad("mid", "mid", 0,0.3,1,0.65);
	TPad* bot_pd = new TPad("bot", "bot", 0,0, 1,0.3);

	top_pd->Draw();//sbn
	mid_pd->Draw();//ub
	bot_pd->Draw();//ratio

	std::cout<<"Read sbn "<<sbn_hname<<std::endl;
	std::cout<<"Read ub "<<ub_hname<<std::endl;

	for(int index = 0; index < 1000; index++){//
	//top
		top_pd->cd();
		f1->cd();
		TH1D* h1 = (TH1D*) f1->Get((sbn_hname+std::to_string(index)).c_str());
		if(index == 0){//configure
			h1->SetStats(false);
			h1->SetMaximum(ymax);
			h1->GetXaxis()->SetRangeUser(0,5);
			h1->GetYaxis()->SetTitle("Event Rate");
			h1->GetXaxis()->SetTitle("True Nu Energy (GeV)");
			h1->GetYaxis()->SetTitleOffset(1.3);
			h1->SetTitle(("sbncode flux weight "+hist_type).c_str());
		}
		h1->Draw("hist same");

	//mid
		mid_pd->cd();
		f2->cd();
		TH1D* h2 = (TH1D*) f2->Get((ub_hname+std::to_string(index)).c_str());
		if(index == 0){//configure
			h2->SetStats(false);
			h2->SetMaximum(ymax);
			h2->GetXaxis()->SetRangeUser(0,5);
			h2->GetYaxis()->SetTitle("Event Rate");
			h2->GetYaxis()->SetTitleOffset(1.4);
			h2->SetTitle(("ubcode flux weight "+hist_type).c_str());
		}
		h2->Draw("hist same");

	//bot
		bot_pd->cd();
		TH1D* h3 = (TH1D*) h1->Clone();
		h3->Divide(h2);
		if(index == 0){//configure
			h3->SetStats(false);
			h3->SetMaximum(2);
			h3->SetMinimum(0);
			h3->GetXaxis()->SetRangeUser(0,5);
			h3->GetYaxis()->SetTitle("Ratio");
			h3->SetTitle("sbncode output/ ubcode output");
		}
		h3->Draw("hist same");

	}


	c1.SaveAs( ("./numu_"+hist_type+".pdf").c_str(),"pdf");
	exit(0);	
	}

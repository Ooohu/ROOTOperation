
using namespace std;

void printhist_v1(){
	
	char file_name[] = "./test_piplus.root";
	std::string hist_name = "numu_ms_piplus_Flux/hEnumu_piplus_Flux_ms_";
	
//std::cout<<std::to_string(0)<<std::endl;
//exit(0);
	TFile* f1=TFile::Open(file_name); //open a root file

	TCanvas c1("c1","A hist",10,10,1000,500);
	TPad* left_pd = new TPad("left", "left", 0,0,0.5,1);
	TPad* right_pd = new TPad("right", "right", 0.5,0, 1,1);
	left_pd->Draw();
	right_pd->Draw();

	for(int index = 0; index < 1000; index++){
		left_pd->cd();

		TH1D* h1 = (TH1D*) f1->Get((hist_name+std::to_string(index)).c_str());
		if(index == 0){//configure
			h1->SetStats(false);
			h1->SetMaximum(30000);
			h1->GetXaxis()->SetRangeUser(0,5);
			h1->SetTitle(hist_name.c_str());
		}

		h1->Draw("hist same");
	}

	for(int index = 0; index < 1000; index++){
		right_pd->cd();

		TH1D* h1 = (TH1D*) f1->Get((hist_name+std::to_string(index)).c_str());
		if(index == 0){//configure
			h1->SetStats(false);
			h1->SetMaximum(30000);
			h1->GetXaxis()->SetRangeUser(0,5);
			h1->SetTitle(hist_name.c_str());
		}

		h1->Draw("hist same");
	}



	c1.SaveAs("./test.png","png");
	exit(0);	
}

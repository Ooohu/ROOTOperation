#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TInterpreter.h>

#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCNeutrino.h"

#include "Math/Vector3D.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TPad.h"

//After make, use `./dumpwgh <artROOT file>`

int main(int argc, char* argv[]) {
  std::vector<std::string> filenames;
  for (int i=1; i<argc; i++) { 
    std::cout << "FILE: " << argv[i] << std::endl; 
    filenames.push_back(argv[i]);
  }

  std::vector<std::string> wprods = {
//    "genieeventweightA",
//    "fluxeventweightA",
//    "fluxeventweightMinibooneA"
  };

//  std::cout <<"RUN# / subRUN# Event#"<<std::endl;
  ROOT::Math::XYZVector BetaRef(0.4624, 0.0489, 0.8853);
  int EntryCount = 0;
  TH2D histp1("p1"," Photon 1 ", 40, 0 , 4, 40, 0, 20);//xbin, xmin,xmax,ybin,ymin,ymax
  TH2D histp2("p2"," Photon 2 ", 40, 0 , 4, 40, 0, 20);//xbin, xmin,xmax,ybin,ymin,ymax

	std::vector< TH2D * > hists{ &histp1, &histp2};

  for (gallery::Event ev(filenames); !ev.atEnd(); ev.next()) {
//    auto const& evaux = ev.eventAuxiliary();
//    int runno = evaux.run();
//    int subrunno = evaux.subRun();
//    int eventno = evaux.event();

//    std::cout << runno << "/" << subrunno << " " << eventno << std::endl;

	//art::Event const & e
	gallery::Handle< std::vector< simb::MCTruth > > gMCTruthHandle;
	ev.getByLabel("generator", gMCTruthHandle);

	std::vector< art::Ptr<simb::MCTruth> > mcTruthVec;
	art::fill_ptr_vector(mcTruthVec, gMCTruthHandle);
	
	art::Ptr<simb::MCTruth> mcTruth = mcTruthVec.at(0);

	std::cout<<"Entry "<<EntryCount++<<" has particles# "<<mcTruth->NParticles()<<std::endl;
	
	int NPars = mcTruth->NParticles();	

	ROOT::Math::XYZVector PTotal(0,0,0);

	for( int index = 0 ; index < NPars; index ++){
		const simb::MCParticle mcp = mcTruth->GetParticle(index);
		ROOT::Math::XYZVector tempP( mcp.Px(), mcp.Py(), mcp.Pz());

		PTotal  += tempP;
		std::cout<<"Momentum "<<mcp.Px()<<" "<<mcp.Py()<<" "<<mcp.Pz()<<" "<<std::endl;
		double betaDOT = tempP.Unit().Dot(BetaRef);
		hists[index]->Fill(TMath::ACos(betaDOT), TMath::Sqrt(tempP.Mag2()) );// ->Fill(x,y,weight);
		std::cout<<"Fill in x,y "<<TMath::ACos(betaDOT) << ", "<<TMath::Sqrt(tempP.Mag2())<<std::endl;
	}
	
//	std::cout<<"Total Momentum "<<PTotal.X()<<" "<<PTotal.Y()<<" "<<PTotal.Z()<<" "<<std::endl;
	std::cout<<"Beta Dot product "<<(PTotal.Unit()).Dot( BetaRef)<<std::endl;




//    for (auto const& wgen : wprods) {
//      auto const& mcwgh = *ev.getValidHandle<std::vector<evwgh::MCEventWeight> >({ wgen });
//
//      for (auto const& it : mcwgh[0].fWeight) {
//        std::cout << it.first << ": ";
//        for (auto const jt : it.second) {
//          std::cout << jt << " ";
//        }
//        std::cout << std::endl;
//      }
//    }
  }

  TCanvas *cobs = new TCanvas ("c1", "x - Ang wrt Beam; y - Momentum", 1800, 900 );
  cobs->cd();
  TPad *pad1 = new TPad("g1", "", 0, 0, 0.5, 1);

  pad1->Draw();
  pad1->cd();
  hists[0]->Draw("COLZ");

  cobs->cd();
  TPad *pad2 = new TPad("g2", "", 0.5, 0, 1, 1);
  pad2->Draw();
  pad2->cd();
  hists[1]->Draw("COLZ");

  cobs->SaveAs("./output/2dplot.png","png");

  return 0;
}


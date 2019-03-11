#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

int signal(){
    /*file location (old):
	    /pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/vertexed_nueintrinsic_fresh_v4.1.root
    */

    TFile* f1=TFile::Open("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/nueintrinsic_overlay_mcc9_v5.0.root"); //open a root file
    TTree * Tree_name = (TTree* )f1->Get("singlephoton/vertex_tree");
    Tree_name->AddFriend("singlephoton/vertex_tree = lee_signal_weights","/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/lee_weights_for_nueintrinsic_overlay_mcc9_v5.0.root");

    TTree* readThis = 0;//create an empty object for new stuff.
    f1->GetObject("singlephoton/vertex_tree", readThis);
    
    // readThis is the vertex_tree.
     
//    readThis->Draw("mctruth_nu_E[0]>>events(17,0,0.9)","lee_signal_weights.lee_weights","hist");
    

    int width = 29;//this is for the display in terminal.
    
//    string def = LEE_def; //this is the argument for applying weights.

//--------------------- Variables --------------------------------
    std::string fiducial_vertex = "(mctruth_nu_vertex_x > 10 && mctruth_nu_vertex_x < 246  && mctruth_nu_vertex_y > -106 && mctruth_nu_vertex_y < 106 && mctruth_nu_vertex_z > 10 && mctruth_nu_vertex_z < 1022)";

//--------------------- Start to calculate events --------------------
    cout<<"\n\nStarted with cosmic overlay nue mc file (?? POT) within a fiducial volumne"<<endl;

    string base_cut = "reco_vertex_size==1 && reco_asso_showers == 1 && reco_asso_tracks==1";
    string signal_cut = "(mctruth_nu_pdg==12&&mctruth_num_exiting_protons == 1&& ((mctruth_num_exiting_pi0+mctruth_num_exiting_pipm) ==0))";
    string training_cut = "abs(mctruth_nu_pdg)==12&&mctruth_interaction_type==1001&&sim_shower_matched[0]==1&&          sim_shower_overlay_fraction[0]<0.2&&mctruth_lepton_E[0]>0.02 && mctruth_exiting_proton_energy[0]>0.04&&(reco_shower_energy[0]/1000<(mctruth_lepton_E[0]*0.8+0.2))&&(reco_shower_energy[0]/1000>(mctruth_lepton_E[0]*0.55-0.05))"; 

    //start
    int index = 0;
    string sequential_cut = fiducial_vertex+"&&"+"mctruth_nu_pdg==12&&";

    double base = readThis->GetEntries((sequential_cut+"1").c_str());
    cout<< setw(45)<<"Total number of vertices: "<<base<<endl;

    //---------------------------------------------------------------------    
    string label,new_cut;
    while(index<4){
	switch (index){
	    case 0://1eXpXpi  note: "\" is for new line in c++!!
		label = "1eXpXpi";
		new_cut = "mctruth_num_exiting_protons >= 0"
		    "&& ((mctruth_num_exiting_pi0+mctruth_num_exiting_pipm) >= 0)";	
		break;
	    case 1://1eXp0pi
		label = "1eXp0pi";
		new_cut = "mctruth_num_exiting_protons >= 0"
		    "&& ((mctruth_num_exiting_pi0+mctruth_num_exiting_pipm) == 0)";	
		break;
	    case 2://1eNp0pi
		label = "1eNp0pi";
		new_cut = "mctruth_num_exiting_protons > 0"
		    "&& ((mctruth_num_exiting_pi0+mctruth_num_exiting_pipm) == 0)";	
		break;
	    case 3://1e1p0pi
		label = "1e1p0pi";
		new_cut = "mctruth_num_exiting_protons == 1"
		    "&& ((mctruth_num_exiting_pi0+mctruth_num_exiting_pipm) == 0)";	
		break;
	    default://do nothing
		    ;
	}
    index++;
    string cuts = sequential_cut+new_cut;
    cout<<setw(width)<<right<<label<<": ";
    cout<<setprecision(4)<<readThis->GetEntries( cuts.c_str() )/base*100<<"%";
			    //factor comes from fiducial volumne
    cout<<" | Wes's 1e20 POT: "<<0.827*206.2*readThis->GetEntries( cuts.c_str() )/base<<endl;
    }


    string LEE_def = "lee_signal_weights.lee_weights";
    double lee_base = readThis->GetEntries(LEE_def.c_str());

    cout<< setw(45)<<"Total number of vertices: "<<lee_base<<endl;

    cout<< setw(45)<<"Number of signal: ";
    cout<<readThis->GetEntries((LEE_def+"&&"+base_cut+"&&"+signal_cut).c_str());
    cout<<"("<<readThis->GetEntries((LEE_def+"&&"+base_cut+"&&"+signal_cut).c_str())/lee_base*100<<")%;"<<endl;

    cout<< setw(45)<<"Number of training sample: ";
    cout<<readThis->GetEntries((LEE_def+"&&"+base_cut+"&&"+training_cut).c_str());
    cout<<"("<<readThis->GetEntries((LEE_def+"&&"+base_cut+"&&"+training_cut).c_str())/lee_base*100<<")%;"<<endl;

//---------------------------------------------------------------------    
    cout<< setw(width)<<right<<"\nPerform cuts, respected to LEE reweighted: sequential (individual)"<<endl; 
    
    cout<<" signal cuts on LEENue sample)"<<endl;
    

    string allprecuts = "1";
    string precuts;
    index = 0;//reset index
    while(index < 6){
	switch(index){
	case 0:
	    label = "Photo-electron cut: ";
	    precuts = "reco_flash_total_pe_in_beamgate[0]>20";
	    break;
	case 1:
	    label = "Fiducial_cut: ";
	    precuts = fiducial_vertex;
	    break;
	case 2:
	    label = "Shower dEdx cut: ";
	    precuts = "reco_shower_dEdx_plane2_nhits[0]>1";
	    break;
	case 3:
	    label = "Minimum Shower Energy cut: ";
	    precuts = "reco_shower_energy[0]>0.00";
	    break;
	case 4:
	    label = "Track length cut: ";
	    precuts = "reco_track_displacement[0]<250";
	    break;
	case 5:
	    label = "Calometric cut: ";
	    precuts = "reco_track_good_calo[0]>0";
	    break;
	default:
	    ;
	}
    index++;
    allprecuts = allprecuts + "&&" + precuts;
    cout<< setw(width)<<right<<label;
    cout<<setprecision(4)<<readThis->GetEntries( (LEE_def+"&&"+allprecuts).c_str() )/lee_base*100<<"%";
    cout<<" ("<<readThis->GetEntries( (LEE_def+"&&"+precuts).c_str())/lee_base*100 <<"% );"<<endl;
    }

    return 0;
}


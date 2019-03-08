#include "TCanvas.h"
#include "TROOT.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"

using namespace std;

int signal(){
    /*file location:
	    /pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_v3/vertexed_nueintrinsic_fresh_v4.1.root
    */

    TFile* f1=TFile::Open("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/nueintrinsic_overlay_mcc9_v5.0.root"); //open a root file
    TTree * Tree_name = (TTree* )f1->Get("singlephoton/vertex_tree");
    Tree_name->AddFriend("singlephoton/vertex_tree = lee_signal_weights","/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v5/lee_weights_for_nueintrinsic_overlay_mcc9_v5.0.root");

    TTree* readThis = 0;//create an empty object for new stuff.
    f1->GetObject("singlephoton/vertex_tree", readThis);
    
    readThis->Draw("nu_energy>>hsqrt(17,0,0.7)","signal_weight","hist");
    return 0;
    int width = 29;
    
    string LEE_def = "lee_signal_weights";
    string def = LEE_def;

    cout<<"\n\nStarted with cosmic overlay nue mc file (5.7e22 POT)"<<endl;
    
    cout<< setw(45)<<"Total number of vertices: "<<readThis->GetEntries()<<endl;

    double base = readThis->GetEntries("signal_weight");
    cout<< setw(45)<<"Total number of vertices (LEE-reweighted): "<<base<<endl;//this shows the number of weighted events.
  
    //start

    cout<< setw(width)<<right<<"\nPerform cuts, respected to LEE reweighted: sequential (individual)"<<endl; 
    
    cout<<" signal cuts (based on LEE-reweighted sample)"<<endl;
    
    def = def + "&& abs(nu_pdg)==12";

    cout<< setw(width)<<right<<" abs(nu_pdg)==12: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%"<<endl;
    
    //individual cuts
    string ind_def = LEE_def + "&&(exiting_electron_number+exiting_antielectron_number==1)";
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<" 1e- or 1e+: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;
    //
    //repeat the same thing..
    ind_def = LEE_def + "&&exiting_proton_number ==1";
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<" 1 proton: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;


    //repeat the same thing..
    ind_def = LEE_def + "&&exiting_piplus_number==0&&exiting_piminus_number==0&&exiting_pi0_number==0";
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<" no pi0,pi-,pi+: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;

    //repeat the same thing..
    ind_def = LEE_def + "&&true_shower_energy[0]>0.02&&true_track_energy[0]>0.04";   
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<"KE_e>0.02; KE_p>0.04: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;


    //true_signal cuts
    cout<<"\n true_signal cuts (for training)"<<endl; 

    //ccnc == 0 is redundant
    //ind_def = LEE_def + "&&ccnc==0";
    //def = def + "&&" + ind_def;

    //cout<< setw(width)<<right<<"cc interaction: ";
    //cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    //cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;

    //repeat the same thing..
    ind_def = LEE_def + "&&true_track_pdg[0]==2212&&abs(true_shower_pdg[0])==11";
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<"p+ track, e- or e+ shower: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;

    
    //repeat the same thing..
    ind_def = LEE_def + "&&true_track_origin[0]==1&&true_shower_origin[0]==1";
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<"BNB signal: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;

    //repeat the same thing..
    ind_def = LEE_def + "&&(abs(true_nuvertz-true_track_startz[0])+abs(true_nuverty-true_track_starty[0])+abs(true_nuvertx-true_track_startx[0])<1)";
    def = def + "&&" + ind_def;

    cout<< setw(width)<<right<<"Well-reconstructed signal: ";
    cout<<setprecision(4)<<readThis->GetEntries( def.c_str() )/base*100<<"%";
    cout<<" ("<<readThis->GetEntries( ind_def.c_str())/base*100 <<"% );"<<endl;


	return 0;
}


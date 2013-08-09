

//TEST

#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TPaveText.h"
#include "tdrstyle.C"
#include "TCutG.h"
#include "TProfile.h"

#include "../../ICHEP_Analysis/Analysis_PlotFunction.h"
#include "../../ICHEP_Analysis/Analysis_Samples.h"



void MakePlot()
{
   setTDRStyle();
   gStyle->SetPadTopMargin   (0.06);
   gStyle->SetPadBottomMargin(0.08);
   gStyle->SetPadRightMargin (0.10);
   gStyle->SetPadLeftMargin  (0.10);
   gStyle->SetTitleSize(0.04, "XYZ");
   gStyle->SetTitleXOffset(1.1);
   gStyle->SetTitleYOffset(1.35);
   gStyle->SetPalette(1);
   gStyle->SetNdivisions(505);
   gStyle->SetPaintTextFormat("4.2f");
   gStyle->SetOptStat("");

   TCanvas* c1;
   TObject** Histos = new TObject*[10];
   std::vector<string> legend;



   TFile* InputFile = new TFile("pictures/Histos.root", "UPDATE");

   double PT[] = {50,60,70,80,100,150,999};
   int nPT = (sizeof(PT)/sizeof(double))-1;
   std::vector<TString> histoNames;
   histoNames.push_back("Beta_Gen");
   histoNames.push_back("Beta_GenChaged");
   histoNames.push_back("Beta_Triggered");
   histoNames.push_back("Beta_Skimmed");
   histoNames.push_back("Beta_Matched");
   histoNames.push_back("Beta_Preselected");
   histoNames.push_back("Beta_SelectedP");
   histoNames.push_back("Beta_SelectedI");
   histoNames.push_back("Beta_SelectedT");
   histoNames.push_back("Beta_SelectedM0");
   histoNames.push_back("Beta_SelectedM1");
   histoNames.push_back("Beta_SelectedM2");
   histoNames.push_back("Beta_SelectedM3");
   histoNames.push_back("Beta_SelectedM4");
   histoNames.push_back("Beta_SelectedM5");

   TH2F*** Beta_Map         = new TH2F**[histoNames.size()];
   for(int i=0;i<histoNames.size();i++){
      Beta_Map[i] = new TH2F*[nPT];
      for(int PTB=0;PTB<nPT;PTB++){
         char Name [1024];
         sprintf(Name,"pT%03i_%03i_%s",(int)PT[PTB],(int)PT[PTB+1], histoNames[i].Data());
         Beta_Map[i][PTB] = (TH2F*)GetObjectFromPath(InputFile, Name);
        
         if(i>0){//if not GEN MAP
            Beta_Map[i][PTB]->Divide(Beta_Map[0][PTB]); //normalize to NGen
         }
      }
   }

   for(int i=0;i<histoNames.size();i++){
      for(int PTB=0;PTB<nPT;PTB++){
         if(i>2){//normalized to efficiency of passing cuts after having passed the trigger
            Beta_Map[i][PTB]->Divide(Beta_Map[2][PTB]); 
         }  

         if(i!=0){//efficiencies
            Beta_Map[i][PTB]->SetMaximum(1.0); 
         }  
         Beta_Map[i][PTB]->Write(TString("Norm_")+Beta_Map[i][PTB]->GetName());
      }
   }


  for(unsigned int i=0;i<histoNames.size();i++){
      TPaveText* T1 = new TPaveText(0.2,0.99,0.8,0.94, "NDC");   T1->SetFillColor(0); T1->SetTextAlign(22); T1->AddText("pT=[ 50, 60] GeV");
      TPaveText* T2 = new TPaveText(0.2,0.99,0.8,0.94, "NDC");   T2->SetFillColor(0); T2->SetTextAlign(22); T2->AddText("pT=[ 70, 80] GeV");
      TPaveText* T3 = new TPaveText(0.2,0.99,0.8,0.94, "NDC");   T3->SetFillColor(0); T3->SetTextAlign(22); T3->AddText("pT=[100,150] GeV");
      TPaveText* T4 = new TPaveText(0.2,0.99,0.8,0.94, "NDC");   T4->SetFillColor(0); T4->SetTextAlign(22); T4->AddText("pT#geq150 GeV");

      c1 = new TCanvas("c1","c1",1500,900);
      c1->Divide(2,2);
      c1->cd(1);   Beta_Map[i][0]->Draw("COLZ text E"); T1->Draw("same");
      c1->cd(2);   Beta_Map[i][2]->Draw("COLZ text E"); T2->Draw("same");
      c1->cd(3);   Beta_Map[i][4]->Draw("COLZ text E"); T3->Draw("same");
      c1->cd(4);   Beta_Map[i][5]->Draw("COLZ text E"); T4->Draw("same");
      c1->cd(0);

      char plotindex[255];sprintf(plotindex,"%02i_",i);
      c1->SaveAs((TString("pictures/")+plotindex)+histoNames[i]+".png");
      delete c1;
   }
}



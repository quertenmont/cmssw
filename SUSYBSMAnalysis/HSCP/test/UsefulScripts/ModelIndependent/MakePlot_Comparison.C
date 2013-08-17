

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

struct dataModel{string Name; double mass; double massCut; double TEff; double TEffErr; double PEff; double PEffErr; double OEff; double OEffErr;};


std::vector<dataModel> readFromFile(string fileName, double mass_){
   std::vector<dataModel> dms;
   FILE* pFile=fopen(fileName.c_str(), "r");
   if(!pFile){printf("%s does not exist\n", fileName.c_str()); return dms; }
   char line[4096];
   while(fgets(line, 4096, pFile)){
      char NameBuffer[256];  dataModel dm;
      sscanf(line, "%s M>%lf Efficiencies: Trigger=%lf%%+-%lf%%  Presel=%lf%%+-%lf%% Offline=%lf%%+-%lf%%", NameBuffer, &dm.massCut, &dm.TEff, &dm.TEffErr, &dm.PEff, &dm.PEffErr, &dm.OEff, &dm.OEffErr);
      dm.Name = NameBuffer; dm.mass = mass_;
      dms.push_back(dm);
   }
   fclose(pFile);
   return dms;
}

TGraphErrors* MakePlot(int DrawIndex, std::vector< std::vector<dataModel> >& dmsVector){
   unsigned int N   = dmsVector.size();
   double* Xval     = new double   [N];
   double* Xerr     = new double   [N];
   double* Yval     = new double   [N];
   double* Yerr     = new double   [N];
   char YAxisTitle[256];
   for(unsigned int i=0;i<N;i++){
      Xval[i] = dmsVector[i][0].mass;
      Xerr[i] = 1;
      switch(DrawIndex){
         case -2:
         Yval[i] = dmsVector[i][0].TEff;
         Yerr[i] = dmsVector[i][0].TEffErr;
         if(i==0)sprintf(YAxisTitle,"Signal Acceptance at Trigger (%%)");
         break;
         case -1:
         Yval[i] = dmsVector[i][0].PEff;
         Yerr[i] = dmsVector[i][0].PEffErr;
         if(i==0)sprintf(YAxisTitle,"Signal Acceptance at PreSelection (%%)");
         break;
         default:
         Yval[i] = dmsVector[i][DrawIndex].OEff;
         Yerr[i] = dmsVector[i][DrawIndex].OEffErr;
         if(i==0)sprintf(YAxisTitle,"Signal Acceptance for M>%iGeV (%%)",(int)dmsVector[i][DrawIndex].massCut);
         break;
      }
   }
   double Ymin=1E100; double Ymax=-1E100; for(unsigned int i=0;i<N;i++){Ymin=std::min(Ymin, Yval[i]-Yerr[i]); Ymax=std::max(Ymax, Yval[i]+Yerr[i]);}
   TGraphErrors* graph = new TGraphErrors(N,Xval,Yval, Xerr, Yerr);
   graph->SetTitle("");
   graph->SetMinimum(0.9*Ymin);
   graph->SetMaximum(1.15*Ymax);
   graph->GetXaxis()->SetTitle("HSCP Mass (GeV)");
   graph->GetYaxis()->SetTitle(YAxisTitle);
   graph->GetYaxis()->SetTitleOffset(1.40);
   return graph;
}

TGraphErrors* MakeGraphRatio(TGraphErrors* A, TGraphErrors* B){
   TGraphErrors* toReturn = new TGraphErrors(A->GetN());   
   for(int i=0;i<A->GetN();i++){
      double x, ayval, ayerr, byval, byerr;
      A->GetPoint(i, x, ayval);  ayerr = A->GetErrorY(i);
      B->GetPoint(i, x, byval);  byerr = B->GetErrorY(i);
      ayerr=sqrt(pow(ayerr/byval,2) + pow(byerr*ayval/(byval*byval),2));  ayval/=byval;
      if(byval<=0){ayval=0.0, ayerr=0.0;}
      if(ayval<=0){ayval=0.0, ayerr=0.0;}
      toReturn->SetPoint(i, x, ayval);
   }
   return toReturn; 
}




void MakePlot_Comparison()
{
   setTDRStyle();
   gStyle->SetPadTopMargin   (0.06);
   gStyle->SetPadBottomMargin(0.10);
   gStyle->SetPadRightMargin (0.10);
   gStyle->SetPadLeftMargin  (0.12);
   gStyle->SetTitleSize(0.04, "XYZ");
   gStyle->SetTitleXOffset(1.1);
   gStyle->SetTitleYOffset(1.35);
   gStyle->SetPalette(1);
   gStyle->SetNdivisions(505);
   gStyle->SetPaintTextFormat("4.2f");
   gStyle->SetOptStat("");

   std::vector< std::vector<dataModel> > dms_PP_ST;
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M100.txt", 100.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M126.txt", 126.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M156.txt", 156.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M200.txt", 200.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M247.txt", 247.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M308.txt", 308.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M370.txt", 370.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M432.txt", 432.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M494.txt", 494.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M557.txt", 557.0));

   std::vector< std::vector<dataModel> > dms_GM_ST;
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M100.txt", 100.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M126.txt", 126.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M156.txt", 156.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M200.txt", 200.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M247.txt", 247.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M308.txt", 308.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M370.txt", 370.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M432.txt", 432.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M494.txt", 494.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M557.txt", 557.0));

   std::vector< std::vector<dataModel> > dms_PP_MI;
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau100.txt", 100.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau126.txt", 126.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau156.txt", 156.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau200.txt", 200.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau247.txt", 247.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau308.txt", 308.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau370.txt", 370.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau432.txt", 432.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau494.txt", 494.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau557.txt", 557.0));

   std::vector< std::vector<dataModel> > dms_GM_MI;
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau100.txt", 100.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau126.txt", 126.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau156.txt", 156.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau200.txt", 200.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau247.txt", 247.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau308.txt", 308.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau370.txt", 370.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau432.txt", 432.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau494.txt", 494.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau557.txt", 557.0));

   for(unsigned int M=0;M<2;M++){
      TString ModelName = M==0?"PPStau":"GMStau";
      std::vector< std::vector<dataModel> >& dms_ST = M==0?dms_PP_ST:dms_GM_ST;
      std::vector< std::vector<dataModel> >& dms_MI = M==0?dms_PP_MI:dms_GM_MI;

      for(int LevelIndex=-2; LevelIndex<6;LevelIndex++){
         TCanvas* c1 = new TCanvas("c1","c1",600, 600);
         TPad* t1 = new TPad("t1","t1", 0.0, 0.20, 1.0, 1.0);
         t1->Draw();
         t1->cd();
         t1->SetTopMargin(0.06);


         TH1D* frame = new TH1D("frame", "frame", 1,0,600);
         frame->SetTitle("");
         frame->SetStats(kFALSE);
         frame->GetXaxis()->SetTitle(ModelName+" Mass (GeV/#font[12]{c}^{2})");
         frame->GetYaxis()->SetTitleOffset(1.40);
         frame->SetMaximum(100);
         frame->SetMinimum(0);
         frame->GetXaxis()->SetLabelFont(43); //give the font size in pixel (instead of fraction)
         frame->GetXaxis()->SetLabelSize(15); //font size
         frame->GetXaxis()->SetTitleFont(43); //give the font size in pixel (instead of fraction)
         frame->GetXaxis()->SetTitleSize(15); //font size
         frame->GetXaxis()->SetNdivisions(505);
         frame->GetYaxis()->SetLabelFont(43); //give the font size in pixel (instead of fraction)
         frame->GetYaxis()->SetLabelSize(15); //font size
         frame->GetYaxis()->SetTitleFont(43); //give the font size in pixel (instead of fraction)
         frame->GetYaxis()->SetTitleSize(15); //font size
         frame->GetYaxis()->SetNdivisions(505);
         frame->Draw("AXIS");

         TGraphErrors* A = MakePlot(LevelIndex, dms_ST);
         TGraphErrors* B = MakePlot(LevelIndex, dms_MI);
         TGraphErrors* R = MakeGraphRatio(B,A);
         frame->GetYaxis()->SetTitle(A->GetYaxis()->GetTitle());
         frame->SetMaximum( 1.10*std::max(A->GetMaximum(), B->GetMaximum()));
         A->SetLineColor(2);   A->SetMarkerColor(2);  A->SetMarkerStyle(4);  A->Draw("same P2");
         B->SetLineColor(4);   B->SetMarkerColor(4);  B->SetMarkerStyle(8);  B->Draw("same P2");
         R->SetLineColor(4);   R->SetMarkerColor(4);  R->SetMarkerStyle(8);


      //   TLegend* leg = new TLegend(0.5,0.13,0.88,0.30);
         TLegend* leg = new TLegend(0.15,0.92,0.30,0.78);
         leg->SetFillStyle(0);
         leg->SetBorderSize(0);
         leg->SetTextFont(43);
         leg->SetTextSize(20);
         leg->SetHeader("Analysis used");
         leg->AddEntry(A, "Standard" ,"P");
         leg->AddEntry(B, "Model Independent" ,"P");
         leg->Draw();

         c1->cd();
         TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, 0.2);
         t2->Draw();
         t2->cd();
         t2->SetGridy(true);
         t2->SetPad(0,0.0,1.0,0.2);
         t2->SetTopMargin(0);
         t2->SetBottomMargin(0.5);

         TH1D* frameR = new TH1D("frameR", "frameR", 1,0,600);
         frameR->SetTitle("");
         frameR->SetStats(kFALSE);
         frameR->GetYaxis()->SetTitle("MI / STD");
         frameR->GetYaxis()->SetTitleOffset(1.40);
         frameR->SetMaximum(1.5);   
         frameR->SetMinimum(0.5);
         frameR->GetXaxis()->SetLabelFont(43); //give the font size in pixel (instead of fraction)
         frameR->GetXaxis()->SetLabelSize(15); //font size
         frameR->GetXaxis()->SetTitleFont(43); //give the font size in pixel (instead of fraction)
         frameR->GetXaxis()->SetTitleSize(15); //font size
         frameR->GetXaxis()->SetNdivisions(505);
         frameR->GetYaxis()->SetLabelFont(43); //give the font size in pixel (instead of fraction)
         frameR->GetYaxis()->SetLabelSize(15); //font size
         frameR->GetYaxis()->SetTitleFont(43); //give the font size in pixel (instead of fraction)
         frameR->GetYaxis()->SetTitleSize(15); //font size
         frameR->GetYaxis()->SetNdivisions(505);
         frameR->Draw("AXIS");

         TLine* LineAtOne = new TLine(0,1,600,1);      LineAtOne->SetLineStyle(1);   LineAtOne->Draw();
         R->Draw("same P2");

         c1->cd();
         char saveName[256]; sprintf(saveName,"pictures/CompPlot_%s_%i.png", ModelName.Data(), LevelIndex);
         c1->SaveAs(saveName);
         delete c1; delete frame; delete frameR;
      }
   }
}



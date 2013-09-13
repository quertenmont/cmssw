

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
//#include "tdrstyle.C"
#include "TCutG.h"
#include "TProfile.h"

#include "../../ICHEP_Analysis/Analysis_PlotFunction.h"
#include "../../ICHEP_Analysis/Analysis_Samples.h"
#include "../../ICHEP_Analysis/Analysis_Step6.C"

//#include "roostats_cl95.C"

struct dataModel{string Name; double mass; double massCut; double TEff; double TEffErr; double PEff; double PEffErr; double OEff; double OEffErr; double Limits[6];};


std::vector<dataModel> readFromFile(string fileName, double mass_){
   std::vector<dataModel> dms;
   FILE* pFile=fopen(fileName.c_str(), "r");
   if(!pFile){printf("%s does not exist\n", fileName.c_str()); return dms; }
   char line[4096];
   while(fgets(line, 4096, pFile)){
      char NameBuffer[256];  dataModel dm;
      sscanf(line, "%s M>%lf Efficiencies: Trigger=%lf%%+-%lf%%  Presel=%lf%%+-%lf%% Offline=%lf%%+-%lf%%  Limits=%lf %lf %lf %lf %lf %lf", NameBuffer, &dm.massCut, &dm.TEff, &dm.TEffErr, &dm.PEff, &dm.PEffErr, &dm.OEff, &dm.OEffErr, &dm.Limits[0], &dm.Limits[1], &dm.Limits[2], &dm.Limits[3], &dm.Limits[4], &dm.Limits[5]);
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
      toReturn->SetPointError(i, 0, ayerr);
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
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M651.txt", 651.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M745.txt", 745.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M871.txt", 871.0));
   dms_PP_ST.push_back(readFromFile("pictures/Std_PPStau_8TeV_M1029.txt",1029.0));

   std::vector< std::vector<dataModel> > dms_PP_MI;
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M100.txt", 100.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M126.txt", 126.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M156.txt", 156.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M200.txt", 200.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M247.txt", 247.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M308.txt", 308.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M370.txt", 370.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M432.txt", 432.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M494.txt", 494.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M557.txt", 557.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M651.txt", 651.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M745.txt", 745.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M871.txt", 871.0));
   dms_PP_MI.push_back(readFromFile("pictures/MI_PPStau_8TeV_M1029.txt",1029.0));

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
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M651.txt", 651.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M745.txt", 745.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M871.txt", 871.0));
   dms_GM_ST.push_back(readFromFile("pictures/Std_GMStau_8TeV_M1029.txt",1029.0));

   std::vector< std::vector<dataModel> > dms_GM_MI;
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M100.txt", 100.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M126.txt", 126.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M156.txt", 156.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M200.txt", 200.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M247.txt", 247.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M308.txt", 308.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M370.txt", 370.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M432.txt", 432.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M494.txt", 494.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M557.txt", 557.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M651.txt", 651.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M745.txt", 745.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M871.txt", 871.0));
   dms_GM_MI.push_back(readFromFile("pictures/MI_GMStau_8TeV_M1029.txt",1029.0));


   std::vector< std::vector<dataModel> > dms_DY_ST;
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M100_Q1.txt", 100.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M200_Q1.txt", 200.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M300_Q1.txt", 300.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M400_Q1.txt", 400.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M500_Q1.txt", 500.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M600_Q1.txt", 600.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M700_Q1.txt", 700.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M800_Q1.txt", 800.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M900_Q1.txt", 900.0));
   dms_DY_ST.push_back(readFromFile("pictures/Std_DY_8TeV_M1000_Q1.txt", 1000.0));
   
   std::vector< std::vector<dataModel> > dms_DY_MI;
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M100.txt", 100.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M200.txt", 200.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M300.txt", 300.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M400.txt", 400.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M500.txt", 500.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M600.txt", 600.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M700.txt", 700.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M800.txt", 800.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M900.txt", 900.0));
   dms_DY_MI.push_back(readFromFile("pictures/MI_DY_8TeV_M1000.txt", 1000.0));



   std::vector< std::vector<dataModel> > dms_pMSSM_ST;
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M100.txt", 100.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M200.txt", 200.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M300.txt", 300.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M400.txt", 400.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M500.txt", 500.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M600.txt", 600.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M700.txt", 700.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M800.txt", 800.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M900.txt", 900.0));
   dms_pMSSM_ST.push_back(readFromFile("pictures/Std_pMSSM_8TeV_M1000.txt", 1000.0));

   std::vector< std::vector<dataModel> > dms_pMSSM_MI;
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M100.txt", 100.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M200.txt", 200.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M300.txt", 300.0)); 
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M400.txt", 400.0)); 
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M500.txt", 500.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M600.txt", 600.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M700.txt", 700.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M800.txt", 800.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M900.txt", 900.0));
   dms_pMSSM_MI.push_back(readFromFile("pictures/MI_pMSSM_8TeV_M1000.txt", 1000.0));


   for(unsigned int M=0;M<4;M++){
      TString ModelName = M==0?"PPStau":M==1?"GMStau":M==2?"DY":"pMSSM";
      std::vector< std::vector<dataModel> >& dms_ST = (M==0)?dms_PP_ST:(M==1)?dms_GM_ST:(M==2)?dms_DY_ST:dms_pMSSM_ST;
      std::vector< std::vector<dataModel> >& dms_MI = (M==0)?dms_PP_MI:(M==1)?dms_GM_MI:(M==2)?dms_DY_MI:dms_pMSSM_MI;

      for(int LevelIndex=-2; LevelIndex<6;LevelIndex++){
         TCanvas* c1 = new TCanvas("c1","c1",600, 600);
         TPad* t1 = new TPad("t1","t1", 0.0, 0.30, 1.0, 1.0);
         t1->Draw();
         t1->cd();
         t1->SetTopMargin(0.06);


         TH1D* frame = new TH1D("frame", "frame", 1,0,M<2?1100:1100);
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
         TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, 0.3);
         t2->Draw();
         t2->cd();
         t2->SetGridx(true);
         t2->SetGridy(true);
         t2->SetPad(0,0.0,1.0,0.3);
         t2->SetTopMargin(0);
         t2->SetBottomMargin(0.5);

         TH1D* frameR = new TH1D("frameR", "frameR", 1,0,M<2?1100:1100);
         frameR->SetTitle("");
         frameR->SetStats(kFALSE);
         frameR->GetYaxis()->SetTitle("MI / STD");
         frameR->GetYaxis()->SetTitleOffset(1.40);
         frameR->SetMaximum(1.3);   
         frameR->SetMinimum(0.7);
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

         TLine* LineAt10 = new TLine(0,1.0,1100,1.0);      LineAt10->SetLineStyle(1);   LineAt10->Draw();
         TLine* LineAt11 = new TLine(0,1.1,1100,1.1);      LineAt11->SetLineStyle(2);   LineAt11->Draw();
         TLine* LineAt12 = new TLine(0,1.2,1100,1.2);      LineAt12->SetLineStyle(2);   LineAt12->Draw();
         TLine* LineAt09 = new TLine(0,0.9,1100,0.9);      LineAt09->SetLineStyle(2);   LineAt09->Draw();
         TLine* LineAt08 = new TLine(0,0.8,1100,0.8);      LineAt08->SetLineStyle(2);   LineAt08->Draw();
         R->Draw("same P2");

         c1->cd();
         char saveName[256]; sprintf(saveName,"pictures/CompPlot_%s_%i.png", ModelName.Data(), LevelIndex);
         c1->SaveAs(saveName);
         delete c1; delete frame; delete frameR;
      }

      bool makeLimitPlot=true;
      if(makeLimitPlot){
         //MakeLimitPlot
         TGraphErrors* MI_limit = new TGraphErrors(dms_MI.size());   
         TGraphErrors* ST_limit = new TGraphErrors(dms_ST.size());       
         for(unsigned int i=0;i<dms_ST.size();i++){
            int MassCut = std::min((int(dms_ST[i][0].mass * 0.6)/100), 5);
            printf("MI %f --> %f --> %E \n", dms_MI[i][MassCut].mass, MassCut*100.0, dms_MI[i][MassCut].Limits[5]);
            printf("ST %f --> %f --> %E \n", dms_ST[i][MassCut].mass, MassCut*100.0, dms_ST[i][MassCut].Limits[5]);
            MI_limit->SetPoint(i,  dms_MI[i][MassCut].mass, dms_MI[i][MassCut].Limits[5]);   MI_limit->SetPointError(i, 0, 0);
            ST_limit->SetPoint(i,  dms_ST[i][MassCut].mass, dms_ST[i][MassCut].Limits[5]);   ST_limit->SetPointError(i, 0, 0);
         }

         TCanvas* c1 = new TCanvas("c1","c1",600, 600);
         TPad* t1 = new TPad("t1","t1", 0.0, 0.30, 1.0, 1.0);
         t1->Draw();
         t1->cd();
         t1->SetTopMargin(0.06);
         t1->SetLogy(true);

         TH1D* frame = new TH1D("frame", "frame", 1,0,1100);
         frame->SetTitle("");
         frame->SetStats(kFALSE);
         frame->GetXaxis()->SetTitle(ModelName+" Mass (GeV/#font[12]{c}^{2})");
         frame->GetYaxis()->SetTitle("Observed Limit at 95%% C.L.");
         frame->GetYaxis()->SetTitleOffset(1.40);
         frame->SetMaximum(1.0);
         frame->SetMinimum(0.00001);
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


         TGraph* paper = new TGraph(10);
         if(M==0){
            paper->SetPoint(0,100,0.006176);
            paper->SetPoint(1,126,0.004401);
            paper->SetPoint(2,156,0.002385);
            paper->SetPoint(3,200,0.000998);
            paper->SetPoint(4,247,0.000583);
            paper->SetPoint(5,308,0.000353);
            paper->SetPoint(6,370,0.000292);
            paper->SetPoint(7,432,0.000264);
            paper->SetPoint(8,494,0.000252);
            paper->SetPoint(9,557,0.000237);
         }else{
            paper->SetPoint(0,100,0.006317);
            paper->SetPoint(1,126,0.004229);
            paper->SetPoint(2,156,0.001741);
            paper->SetPoint(3,200,0.000768);
            paper->SetPoint(4,247,0.000445);
            paper->SetPoint(5,308,0.00029);
            paper->SetPoint(6,370,0.000256);
            paper->SetPoint(7,432,0.000235);
            paper->SetPoint(8,494,0.000231);
            paper->SetPoint(9,557,0.000223);
         }

         TGraphErrors* A = ST_limit;
         TGraphErrors* B = MI_limit;
         TGraphErrors* R = MakeGraphRatio(B,A);
         B->SetLineColor(4);   B->SetLineWidth(2); B->SetMarkerColor(4);  B->SetMarkerStyle(8);  B->Draw("same LP2");
         A->SetLineColor(2);   A->SetLineWidth(2); A->SetMarkerColor(2);  A->SetMarkerStyle(4);  A->Draw("same LP2");
         R->SetLineColor(4);   R->SetMarkerColor(4);  R->SetMarkerStyle(8);
         paper->SetLineColor(1); paper->SetLineWidth(2);
         paper->Draw("same L");

      //   TLegend* leg = new TLegend(0.5,0.13,0.88,0.30);
         TLegend* leg = new TLegend(0.15,0.92,0.30,0.74);
         leg->SetFillStyle(0);
         leg->SetBorderSize(0);
         leg->SetTextFont(43);
         leg->SetTextSize(20);
         leg->SetHeader("Analysis used");
         leg->AddEntry(paper, "Paper Results", "L");
         leg->AddEntry(A, "Standard" ,"PL");
         leg->AddEntry(B, "Model Independent" ,"PL");
         leg->Draw();

         c1->cd();
         TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, 0.3);
         t2->Draw();
         t2->cd();
         t2->SetLogy(false);
         t2->SetGridx(true);
         t2->SetGridy(true);
         t2->SetPad(0,0.0,1.0,0.3);
         t2->SetTopMargin(0);
         t2->SetBottomMargin(0.5);

         TH1D* frameR = new TH1D("frameR", "frameR", 1,0,1100);
         frameR->SetTitle("");
         frameR->SetStats(kFALSE);
         frameR->GetYaxis()->SetTitle("MI / STD");
         frameR->GetYaxis()->SetTitleOffset(1.40);
         frameR->SetMaximum(1.3);   
         frameR->SetMinimum(0.7);
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

         TLine* LineAt10 = new TLine(0,1.0,1100,1.0);      LineAt10->SetLineStyle(1);   LineAt10->Draw();
         TLine* LineAt11 = new TLine(0,1.1,1100,1.1);      LineAt11->SetLineStyle(2);   LineAt11->Draw();
         TLine* LineAt12 = new TLine(0,1.2,1100,1.2);      LineAt12->SetLineStyle(2);   LineAt12->Draw();
         TLine* LineAt09 = new TLine(0,0.9,1100,0.9);      LineAt09->SetLineStyle(2);   LineAt09->Draw();
         TLine* LineAt08 = new TLine(0,0.8,1100,0.8);      LineAt08->SetLineStyle(2);   LineAt08->Draw();
         R->Draw("same P2");

         c1->cd();
         char saveName[256]; sprintf(saveName,"pictures/CompPlot_%s_Limit.png", ModelName.Data());
         c1->SaveAs(saveName);
         sprintf(saveName,"pictures/CompPlot_%s_Limit.C", ModelName.Data());
         c1->SaveAs(saveName);
         delete c1; delete frame; delete frameR;
      } 


   }
}



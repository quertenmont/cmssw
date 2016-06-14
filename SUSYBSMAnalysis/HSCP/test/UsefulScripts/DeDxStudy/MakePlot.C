
#include <exception>
#include <vector>
#include <fstream>

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
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TPaveText.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCutG.h"
#include "../../AnalysisCode/tdrstyle.C"
#include "../../AnalysisCode/Analysis_PlotFunction.h"

using namespace std;


void getScaleFactor(TFile* InputFile1, TFile* InputFile2, string ObjName1, string ObjName2, string SaveDir, string Prefix);
void ExtractConstants(TH2D* input, int FileIndex=0);
void CompareDeDx (TFile* InputFile1, string SaveDir, string SaveName, string ObjName1="harm2_SO", string ObjName2="harm2_SO_in");
void CompareMIPs (TFile* InputFile, string SaveDir, string Prefix, string dEdxString);
void CompareMIPs2 (TFile* InputFile, string SaveDir, string Prefix, string dEdxString);
void ShortMIPs (TFile* InputFile, string SaveDir, string Prefix, string dEdxString);
void DrawPreliminary (void);
void MakeMapPlots(TH3F* Charge_Vs_Path3D, string ObjName, string SaveDir, string Prefix);
void MakeROCPlot (TFile* InputFile1, TFile* InputFile2, vector<string> ObjNames, vector<string> LegendLabels, vector<Color_t> Colors, string SaveDir, string suffix="", bool WithErrorBars=false);
void MakeROCGeneral (TFile* InputFile1, TFile* InputFile2, vector<string> HistoNames, vector<string> LegendLabels, vector<Color_t> Colors, string SaveDir, string suffix, bool WithErrorBarse=false, bool Every2ndIsDashed=false);

//2015B prompt constants -- one for Each File (if we want to compare two files)
double K [2] = {2.779, 2.779}; double Kerr [2] = {0.001, 0.001};
double C [2] = {2.879, 2.779}; double Cerr [2] = {0.001, 0.001};

double GetMass(double P, double I, int FileIndex=0){
   return sqrt((I-C[FileIndex])/K[FileIndex])*P;
}

TF1* GetMassLine(double M, bool left=false, int FileIndex=0)
{  
   double BetaMax = 0.9;
   double PMax = sqrt((BetaMax*BetaMax*M*M)/(1-BetaMax*BetaMax));

   double BetaMin = 0.2;
   double PMin = sqrt((BetaMin*BetaMin*M*M)/(1-BetaMin*BetaMin));
   
   if(left){PMax*=-1; PMin*=-1;}

   TF1* MassLine = new TF1("MassLine","[2] + ([0]*[0]*[1])/(x*x)", PMin, PMax);
   MassLine->SetParName  (0,"M");
   MassLine->SetParName  (1,"K");
   MassLine->SetParName  (2,"C");
   MassLine->SetParameter(0, M);
   MassLine->SetParameter(1, K[FileIndex]);
   MassLine->SetParameter(2, C[FileIndex]);
   MassLine->SetLineWidth(2);
   return MassLine;
}

void MakePlot(string INPUT, string INPUT2="EMPTY")
{
   size_t firstToken = INPUT.find_first_of ("_"),
          lastToken  = INPUT.find_first_of (".") - firstToken;
   string SaveName (INPUT, firstToken, lastToken),
          SaveName2 ("");
   if (INPUT2 != "EMPTY"){
      firstToken = INPUT2.find_first_of ("_");
      lastToken  = INPUT2.find_first_of (".") - firstToken;
      SaveName2  = INPUT2.substr (firstToken, lastToken);
   }

   string SaveDir ("pictures" + SaveName + SaveName2 + "/");
   system(("mkdir -p " + SaveDir).c_str());
   system("mkdir -p fit");

   string SaveSuffix = INPUT2=="EMPTY" ? "" : SaveName;

   setTDRStyle();
   gStyle->SetPadTopMargin   (0.06);
   gStyle->SetPadBottomMargin(0.10);
   gStyle->SetPadRightMargin (0.18);
   gStyle->SetPadLeftMargin  (0.125);
   gStyle->SetTitleSize(0.04, "XYZ");
   gStyle->SetTitleXOffset(1.1);
   gStyle->SetTitleYOffset(1.35);
//   gStyle->SetPalette(51); 
   gStyle->SetPalette(1); 
   gStyle->SetNdivisions(510,"X");

   TF1* PionLine = GetMassLine(0.140);
   PionLine->SetLineColor(1);
   PionLine->SetLineWidth(2);

   TF1* KaonLine = GetMassLine(0.494);
   KaonLine->SetLineColor(1);
   KaonLine->SetLineWidth(2);

   TF1* ProtonLine = GetMassLine(0.938);
   ProtonLine->SetLineColor(1);
   ProtonLine->SetLineWidth(2);

   TF1* DeuteronLine = GetMassLine(1.88);
   DeuteronLine->SetLineColor(1);
   DeuteronLine->SetLineWidth(2);

   TF1* TritonLine = GetMassLine(2.80);
   TritonLine->SetLineColor(1);
   TritonLine->SetLineWidth(2);

   TF1* ProtonLineFit = GetMassLine(0.938);
   ProtonLineFit->SetLineColor(2);
   ProtonLineFit->SetLineWidth(2);
   ProtonLineFit->SetRange(0.6,1.2);

   TF1* KaonLineLeft = GetMassLine(0.494, true);
   KaonLineLeft->SetLineColor(1);
   KaonLineLeft->SetLineWidth(2);

   TF1* ProtonLineLeft = GetMassLine(0.938, true);
   ProtonLineLeft->SetLineColor(1);
   ProtonLineLeft->SetLineWidth(2);

   TF1* DeuteronLineLeft = GetMassLine(1.88, true);
   DeuteronLineLeft->SetLineColor(1);
   DeuteronLineLeft->SetLineWidth(2);

   TF1* TritonLineLeft = GetMassLine(2.80, true);
   TritonLineLeft->SetLineColor(1);
   TritonLineLeft->SetLineWidth(2);


   TFile* InputFile  = new TFile(INPUT .c_str());
   TFile* InputFile2 = INPUT2.find("EMPTY")!=string::npos ? NULL : new TFile(INPUT2.c_str());


   std::vector<string> ObjName;
   ObjName.push_back("hit_SP");
   ObjName.push_back("hit_SP_in_noC");
   ObjName.push_back("hit_SP_in_noC_CCC");
   ObjName.push_back("hit_SP_in_noC_CI");
   ObjName.push_back("hit_SP_in_noC_CC");
   ObjName.push_back("harm2_SO");
   ObjName.push_back("harm2_SO_in");
   ObjName.push_back("harm2_SO_in_noC");
   ObjName.push_back("harm2_SP");
   ObjName.push_back("harm2_SP_in");
   ObjName.push_back("harm2_SP_in_noC");
   ObjName.push_back("harm2_SP_in_noC_CI");
   ObjName.push_back("harm2_SP_in_noC_CC");
   ObjName.push_back("harm2_SP_in_noC_CCC");
   ObjName.push_back("harm2_PO_raw"); // FIXME does not fit well
   ObjName.push_back("Ias_PO");
   ObjName.push_back("Ias_SO_inc");
   ObjName.push_back("Ias_SO");
   ObjName.push_back("Ias_SO_in");
   ObjName.push_back("Ias_SO_in_noC");
   ObjName.push_back("Ias_SO_in_noC_CI");
   ObjName.push_back("Ias_SO_in_noC_CC");
   ObjName.push_back("Ias_SO_in_noC_CCC");
   ObjName.push_back("Ias_SP_inc");
   ObjName.push_back("Ias_SP");
   ObjName.push_back("Ias_SP_in");
   ObjName.push_back("Ias_SP_in_noC");
   ObjName.push_back("Ias_SP_in_noC_CI");
   ObjName.push_back("Ias_SP_in_noC_CC");
   ObjName.push_back("Ias_SP_in_noC_CCC");

   TPaveText* T = new TPaveText(0.05, 0.995, 0.95, 0.945, "NDC");
   T->SetTextFont(43);  //give the font size in pixel (instead of fraction)
   T->SetTextSize(21);  //font size
   T->SetBorderSize(0);
   T->SetFillColor(0);
   T->SetFillStyle(0);
   T->SetTextAlign(22);
   T->AddText("#bf{CMS} Preliminary   -   2.74 pb^{-1}   -    #sqrt{s} = 13 TeV");

   ofstream ExtractConstantsReport, ExtractConstantsReport2;
   ExtractConstantsReport.open ((SaveDir + "ConstantsReport" + SaveName + ".txt").c_str(), ofstream::out);
   if (InputFile2) ExtractConstantsReport2.open ((SaveDir + "ConstantsReport" + SaveName2 + ".txt").c_str(), ofstream::out);

   for(unsigned int i=0;i<ObjName.size();i++){
      TH3F*       dEdxTemplate       = (TH3F*)      GetObjectFromPath(InputFile, (ObjName[i] + "_ChargeVsPath"      ).c_str() );
      TH1D*       HdedxMIP           = (TH1D*)      GetObjectFromPath(InputFile, (ObjName[i] + "_MIP"               ).c_str() );
      TH1D*       HdedxMIP_U         = (TH1D*)      GetObjectFromPath(InputFile, (ObjName[i] + "_MIP_U"             ).c_str() );
      TH1D*       HMass              = (TH1D*)      GetObjectFromPath(InputFile, (ObjName[i] + "_Mass"              ).c_str() );
      TH2D*       HdedxVsP           = (TH2D*)      GetObjectFromPath(InputFile, (ObjName[i] + "_dedxVsP"           ).c_str() );
      TProfile*   HdedxVsPProfile    = (TProfile*)  GetObjectFromPath(InputFile, (ObjName[i] + "_Profile"           ).c_str() );
      TH2D*       HdedxVsEta         = (TH2D*)      GetObjectFromPath(InputFile, (ObjName[i] + "_Eta2D"             ).c_str() );
      TProfile*   HdedxVsEtaProfile  = (TProfile*)  GetObjectFromPath(InputFile, (ObjName[i] + "_Eta"               ).c_str() );
      TH3F*       dEdxTemplate2      = NULL;
      TH1D*       HdedxMIP2          = NULL;
      TH1D*       HdedxMIP2_U        = NULL;
      TH1D*       HMass2             = NULL;
      TH2D*       HdedxVsP2          = NULL;
      TProfile*   HdedxVsPProfile2   = NULL;
      TH2D*       HdedxVsEta2        = NULL;
      TProfile*   HdedxVsEtaProfile2 = NULL;

      if (InputFile2) {
         dEdxTemplate2      = (TH3F*)      GetObjectFromPath(InputFile2, (ObjName[i] + "_ChargeVsPath"  ).c_str() );
         HdedxMIP2          = (TH1D*)      GetObjectFromPath(InputFile2, (ObjName[i] + "_MIP"           ).c_str() );
         HdedxMIP2_U        = (TH1D*)      GetObjectFromPath(InputFile2, (ObjName[i] + "_MIP_U"         ).c_str() );
         HMass2             = (TH1D*)      GetObjectFromPath(InputFile2, (ObjName[i] + "_Mass"          ).c_str() );
         HdedxVsP2          = (TH2D*)      GetObjectFromPath(InputFile2, (ObjName[i] + "_dedxVsP"       ).c_str() );
         HdedxVsPProfile2   = (TProfile*)  GetObjectFromPath(InputFile2, (ObjName[i] + "_Profile"       ).c_str() );
         HdedxVsEta2        = (TH2D*)      GetObjectFromPath(InputFile2, (ObjName[i] + "_Eta2D"         ).c_str() );
         HdedxVsEtaProfile2 = (TProfile*)  GetObjectFromPath(InputFile2, (ObjName[i] + "_Eta"           ).c_str() );
      }

      if (ObjName[i].find("hit_SP")!=string::npos){
         dEdxTemplate->SetName("Charge_Vs_Path");
         dEdxTemplate->SaveAs (("dEdxTemplate_" + ObjName[i] + SaveName + ".root").c_str());
         MakeMapPlots (dEdxTemplate, ObjName[i], SaveDir, "Map" + SaveName);
         TH1D* hit_MIP = (TH1D*) GetObjectFromPath (InputFile, (ObjName[i]+"_Hit").c_str());

         TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
         c1->SetLogy(true);
         hit_MIP->SetStats(kFALSE);
         hit_MIP->GetXaxis()->SetTitle("cluster dE/dx");
         hit_MIP->GetYaxis()->SetTitle("number of hits");
         hit_MIP->Draw("hist");
         T->Draw("same");
         SaveCanvas (c1, SaveDir, ObjName[i]+SaveName+"_Hit");
         delete c1;

         // all the other graphs -- Charge_Vs_XYNLetc.
         for (unsigned int g=1;g<16;g++){
            char Id[255]; sprintf (Id, "%02i", g);
            TH2D*           Charge_Vs_XYH  = (TH2D*)       GetObjectFromPath (InputFile, (ObjName[i]+"_ChargeVsXYH"      + Id).c_str());
            TH2D*           Charge_Vs_XYHN = (TH2D*)       GetObjectFromPath (InputFile, (ObjName[i]+"_ChargeVsXYHN"     + Id).c_str());
            TH2D*           Charge_Vs_XYLN = (TH2D*)       GetObjectFromPath (InputFile, (ObjName[i]+"_ChargeVsXYLN"     + Id).c_str());

            TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
            Charge_Vs_XYH->SetStats(kFALSE);
            Charge_Vs_XYH->GetXaxis()->SetTitle("local x coordinate");
            Charge_Vs_XYH->GetYaxis()->SetTitle("local y coordinate");
            Charge_Vs_XYH->SetAxisRange (-7,7,"X");
            Charge_Vs_XYH->SetAxisRange (-15,15,"Y");
            Charge_Vs_XYH->Draw("COLZ");
            T->Draw("same");
            SaveCanvas (c1, SaveDir, ObjName[i]+SaveSuffix+"_ChargeVsXYH"+string(Id), true);
            delete c1;

            c1 = new TCanvas ("c1", "c1", 600, 600);
            Charge_Vs_XYHN->SetStats(kFALSE);
            Charge_Vs_XYHN->GetXaxis()->SetTitle("normalized x coordinate");
            Charge_Vs_XYHN->GetYaxis()->SetTitle("normalized y coordinate");
            Charge_Vs_XYHN->SetAxisRange (-1.5,1.5,"X");
            Charge_Vs_XYHN->SetAxisRange (-1.5,1.5,"Y");
            Charge_Vs_XYHN->Draw("COLZ");
            T->Draw("same");
            SaveCanvas (c1, SaveDir, ObjName[i]+SaveSuffix+"_ChargeVsXYHN"+string(Id), true);
            delete c1;

            c1 = new TCanvas ("c1", "c1", 600, 600);
            Charge_Vs_XYLN->SetStats(kFALSE);
            Charge_Vs_XYLN->GetXaxis()->SetTitle("normalized x coordinate");
            Charge_Vs_XYLN->GetYaxis()->SetTitle("normalized y coordinate");
            Charge_Vs_XYLN->SetAxisRange (-1.5,1.5,"X");
            Charge_Vs_XYLN->SetAxisRange (-1.5,1.5,"Y");
            Charge_Vs_XYLN->Draw("COLZ");
            T->Draw("same");
            SaveCanvas (c1, SaveDir, ObjName[i]+SaveSuffix+"_ChargeVsXYLN"+string(Id), true);
            delete c1;
         }

         if (InputFile2){
            dEdxTemplate2->SetName("Charge_Vs_Path");
            dEdxTemplate2->SaveAs (("dEdxTemplate_" + ObjName[i] + SaveName2 + ".root").c_str());
            MakeMapPlots (dEdxTemplate2, ObjName[i], SaveDir, "Map" + SaveName2);
            TH1D* hit_MIP2 = (TH1D*) GetObjectFromPath (InputFile2, (ObjName[i]+"_Hit").c_str());

            TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
            c1->SetLogy(true);
            hit_MIP2->SetStats(kFALSE);
            hit_MIP2->GetXaxis()->SetTitle("cluster dE/dx");
            hit_MIP2->GetYaxis()->SetTitle("number of hits");
            hit_MIP2->Draw("hist");
            T->Draw("same");
            SaveCanvas (c1, SaveDir, ObjName[i]+SaveName2+"_Hit");
            delete c1;

            for (unsigned int g=0;g<16;g++){
               char Id[255]; sprintf (Id, "%02i", g);
               TH2D*            Charge_Vs_XYH2 = (TH2D*)       GetObjectFromPath (InputFile2, (ObjName[i]+"_ChargeVsXYH"      + Id).c_str());
               TH2D*           Charge_Vs_XYHN2 = (TH2D*)       GetObjectFromPath (InputFile2, (ObjName[i]+"_ChargeVsXYHN"     + Id).c_str());
               TH2D*           Charge_Vs_XYLN2 = (TH2D*)       GetObjectFromPath (InputFile2, (ObjName[i]+"_ChargeVsXYLN"     + Id).c_str());

               TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
               Charge_Vs_XYH2->SetStats(kFALSE);
               Charge_Vs_XYH2->GetXaxis()->SetTitle("local x coordinate");
               Charge_Vs_XYH2->GetYaxis()->SetTitle("local y coordinate");
               Charge_Vs_XYH2->SetAxisRange (-7,7,"X");
               Charge_Vs_XYH2->SetAxisRange (-15,15,"Y");
               Charge_Vs_XYH2->Draw("COLZ");
               T->Draw("same");
               SaveCanvas (c1, SaveDir, ObjName[i]+SaveName2+"_ChargeVsXYH"+string(Id), true);
               delete c1;

               c1 = new TCanvas ("c1", "c1", 600, 600);
               Charge_Vs_XYHN2->SetStats(kFALSE);
               Charge_Vs_XYHN2->GetXaxis()->SetTitle("normalized x coordinate");
               Charge_Vs_XYHN2->GetYaxis()->SetTitle("normalized module y coordinate");
               Charge_Vs_XYHN2->SetAxisRange (-1.5,1.5,"X");
               Charge_Vs_XYHN2->SetAxisRange (-1.5,1.5,"Y");
               Charge_Vs_XYHN2->Draw("COLZ");
               T->Draw("same");
               SaveCanvas (c1, SaveDir, ObjName[i]+SaveName2+"_ChargeVsXYHN"+string(Id), true);
               delete c1;

               c1 = new TCanvas ("c1", "c1", 600, 600);
               Charge_Vs_XYLN2->SetStats(kFALSE);
               Charge_Vs_XYLN2->GetXaxis()->SetTitle("normalized x coordinate");
               Charge_Vs_XYLN2->GetYaxis()->SetTitle("normalized y coordinate");
               Charge_Vs_XYLN2->SetAxisRange (-1.5,1.5,"X");
               Charge_Vs_XYLN2->SetAxisRange (-1.5,1.5,"Y");
               Charge_Vs_XYLN2->Draw("COLZ");
               T->Draw("same");
               SaveCanvas (c1, SaveDir, ObjName[i]+SaveName2+"_ChargeVsXYLN"+string(Id), true);
               delete c1;
            }
         }

         continue;
      }

      ExtractConstants(HdedxVsP);
      ExtractConstantsReport << ObjName[i] << " ... K = " << K[0] << " +- " << Kerr[0] << "\t"
                                           << " ... C = " << C[0] << " +- " << Cerr[0] << endl;


      std::cout << "TESTA\n";
      TCanvas* c1 = new TCanvas("c1", "c1", 600,600);
      c1->SetLogz(true);
      HdedxVsP->SetStats(kFALSE);
      HdedxVsP->GetXaxis()->SetTitle("momentum (GeV/c)");
      HdedxVsP->GetYaxis()->SetTitle(ObjName[i].find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
      HdedxVsP->SetAxisRange(0,5,"X");
      HdedxVsP->SetAxisRange(0,15,"Y");
      HdedxVsP->Draw("COLZ");

      PionLine->Draw("same");
      KaonLine->Draw("same");
      ProtonLine->Draw("same");
      DeuteronLine->Draw("same");
      ProtonLineFit->Draw("same");
      T->Draw("same");
      SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix + "_dedxVsP", true);
      delete c1;

      c1 = new TCanvas("c1", "c1", 600,600);
      c1->SetLogz(true);
      HdedxVsEta->SetStats(kFALSE);
      HdedxVsEta->GetXaxis()->SetTitle("Eta");
      HdedxVsEta->GetYaxis()->SetTitle("I_{as}");
      HdedxVsEta->SetAxisRange(-2.1,2.1,"X");
      HdedxVsEta->Draw("COLZ");
      T->Draw("same");
      SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix + "_Eta2D", true);
      delete c1;

      if (InputFile2) {
         ExtractConstants(HdedxVsP, 1);
         ExtractConstantsReport2 << ObjName[i] << " ... K = " << K[1] << " +- " << Kerr[1] << "\t"
                                               << " ... C = " << C[1] << " +- " << Cerr[1] << endl;

         TF1* PionLine2 = GetMassLine(0.140, false, 1);
         PionLine2->SetLineColor(1);
         PionLine2->SetLineWidth(2);

         TF1* KaonLine2 = GetMassLine(0.494, false, 1);
         KaonLine2->SetLineColor(1);
         KaonLine2->SetLineWidth(2);

         TF1* ProtonLine2 = GetMassLine(0.938, false, 1);
         ProtonLine2->SetLineColor(1);
         ProtonLine2->SetLineWidth(2);

         TF1* DeuteronLine2 = GetMassLine(1.88, false, 1);
         DeuteronLine2->SetLineColor(1);
         DeuteronLine2->SetLineWidth(2);

         TF1* TritonLine2 = GetMassLine(2.80, false, 1);
         TritonLine2->SetLineColor(1);
         TritonLine2->SetLineWidth(2);

         TF1* ProtonLineFit2 = GetMassLine(0.938, false, 1);
         ProtonLineFit2->SetLineColor(2);
         ProtonLineFit2->SetLineWidth(2);
         ProtonLineFit2->SetRange(0.6,1.2);

         TCanvas* c1 = new TCanvas("c1", "c1", 600,600);
         c1->SetLogz(true);
         HdedxVsP2->SetStats(kFALSE);
         HdedxVsP2->GetXaxis()->SetTitle("momentum (GeV/c)");
         HdedxVsP2->GetYaxis()->SetTitle(ObjName[i].find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
         HdedxVsP2->SetAxisRange(0,5,"X");
         HdedxVsP2->SetAxisRange(0,15,"Y");
         HdedxVsP2->Draw("COLZ");
    
         PionLine2->Draw("same");
         KaonLine2->Draw("same");
         ProtonLine2->Draw("same");
         DeuteronLine2->Draw("same");
//         TritonLine->Draw("same");
         ProtonLineFit2->Draw("same");
         T->Draw("same");
         SaveCanvas(c1, SaveDir, ObjName[i] + SaveName2 + "_dedxVsP", true);
         delete c1;

         c1 = new TCanvas("c1", "c1", 600,600);
         c1->SetLogz(true);
         HdedxVsEta2->SetStats(kFALSE);
         HdedxVsEta2->GetXaxis()->SetTitle("Eta");
         HdedxVsEta2->GetYaxis()->SetTitle("I_{as}");
         HdedxVsEta2->SetAxisRange(-2.1,2.1,"X");
         HdedxVsEta2->Draw("COLZ");
         T->Draw("same");
         SaveCanvas(c1, SaveDir, ObjName[i] + SaveName2 + "_Eta2D", true);
         delete c1;
      }

      std::cout << "TESTB\n";

      SaveSuffix = InputFile2 ? SaveName + SaveName2 : "" ;

      c1 = new TCanvas("c1", "c1", 600,600);
      TLegend* leg = new TLegend (0.50, 0.80, 0.80, 0.90);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      HdedxVsPProfile->SetStats(kFALSE);
      HdedxVsPProfile->SetAxisRange(2.5,5,"Y");
      HdedxVsPProfile->GetXaxis()->SetTitle("track momentum (GeV/c)");
      HdedxVsPProfile->GetYaxis()->SetTitle(ObjName[i].find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
      HdedxVsPProfile->SetLineColor  (kBlack);
      HdedxVsPProfile->SetMarkerColor(kBlack);
      HdedxVsPProfile->SetMarkerStyle(20);
      HdedxVsPProfile->Draw("");
      if (InputFile2) {
         HdedxVsPProfile2->SetLineColor  (kBlue);
         HdedxVsPProfile2->SetMarkerColor(kBlue);
         HdedxVsPProfile2->SetMarkerStyle(22);
         HdedxVsPProfile2->Draw("same");
         leg->AddEntry (HdedxVsPProfile , SaveName .c_str(), "LP");
         leg->AddEntry (HdedxVsPProfile2, SaveName2.c_str(), "LP");
         leg->Draw();
      }
      T->Draw("same");
      SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix + "_Profile");
      delete leg;
      delete c1;

      c1 = new TCanvas("c1", "c1", 600,600);
      leg = new TLegend (0.50, 0.80, 0.80, 0.90);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      HdedxVsEtaProfile->SetStats(kFALSE);
      HdedxVsEtaProfile->GetXaxis()->SetTitle("#eta");
      HdedxVsEtaProfile->GetYaxis()->SetTitle("I_{as}");
      HdedxVsEtaProfile->SetAxisRange(-2.1,2.1,"X");
      HdedxVsEtaProfile->SetLineColor  (kBlack);
      HdedxVsEtaProfile->SetMarkerColor(kBlack);
      HdedxVsEtaProfile->SetMarkerStyle(20);
      HdedxVsEtaProfile->Draw("");
      if (InputFile2) {
         HdedxVsEtaProfile2->SetLineColor  (kBlue);
         HdedxVsEtaProfile2->SetMarkerColor(kBlue);
         HdedxVsEtaProfile2->SetMarkerStyle(22);
         HdedxVsEtaProfile2->Draw("same");
         leg->AddEntry (HdedxVsEtaProfile , SaveName .c_str(), "LP");
         leg->AddEntry (HdedxVsEtaProfile2, SaveName2.c_str(), "LP");
         leg->Draw();
      }
      T->Draw("same");
      SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix + "_HdedxVsEtaProfile");
      delete leg;
      delete c1;

      c1 = new TCanvas("c1", "c1", 600,600);
      leg = new TLegend (0.50, 0.70, 0.80, 0.80);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      c1->SetLogy(true);
      c1->SetGridx(true);
      HdedxMIP->SetStats(kFALSE);
      HdedxMIP->GetXaxis()->SetTitle(ObjName[i].find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
      HdedxMIP->GetYaxis()->SetTitle("fraction of tracks");
      HdedxMIP->GetXaxis()->SetRangeUser(0,19.5);
      HdedxMIP->GetYaxis()->SetRangeUser(5e-7,6e-1);
      HdedxMIP->SetLineColor(kBlack);
      HdedxMIP->SetLineWidth(2);
      HdedxMIP->Scale (1.0/HdedxMIP->Integral());
      HdedxMIP->Draw("");
      if (InputFile2) {
         HdedxMIP2->SetLineColor(kBlue);
         HdedxMIP2->SetLineWidth(2);
         HdedxMIP2->Scale (1.0/HdedxMIP2->Integral());
         HdedxMIP2->Draw("same");
         leg->AddEntry (HdedxMIP , SaveName .c_str(), "L");
         leg->AddEntry (HdedxMIP2, SaveName2.c_str(), "L");
         leg->Draw();
      }
      T->Draw("same");
      SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix + "_MIP");
      delete leg;
      delete c1;

      if (InputFile2){
         c1 = new TCanvas("c1", "c1", 600,600);
         c1->SetLogy(true);
         c1->SetGridx(true);
         leg = new TLegend (0.50, 0.70, 0.80, 0.80);
         leg->SetFillColor(0);
         leg->SetFillStyle(0);
         leg->SetBorderSize(0);
         HdedxMIP_U->SetStats(kFALSE);
         HdedxMIP_U->GetXaxis()->SetTitle(ObjName[i].find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
         HdedxMIP_U->GetYaxis()->SetTitle("fraction of tracks");
         HdedxMIP_U->SetAxisRange(0,8,"X");
         HdedxMIP2_U->SetLineColor (kBlue);
         HdedxMIP_U->SetLineColor (kBlack);
         HdedxMIP_U->SetLineWidth (2);
         HdedxMIP2_U->SetLineWidth (2);
         HdedxMIP2_U->Scale(1.0/HdedxMIP2_U->Integral());
         HdedxMIP_U->Scale(1.0/HdedxMIP_U->Integral());
         leg->AddEntry (HdedxMIP_U,  SaveName .c_str(), "L");
         leg->AddEntry (HdedxMIP2_U, SaveName2.c_str(), "L");
         HdedxMIP_U->Draw("hist");
         HdedxMIP2_U->Draw("same");
         leg->Draw();
         T->Draw("same");
         SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix + "_MIP_U");
         delete leg;
         delete c1;
      }
      std::cout << "TESTC\n";

      if (ObjName[i].find("harm2")!=std::string::npos){
            c1 = new TCanvas("c1", "c1", 600,600);
            leg = new TLegend (0.50, 0.80, 0.80, 0.90);
            leg->SetFillColor(0);
            leg->SetFillStyle(0);
            leg->SetBorderSize(0);
            c1->SetLogy(true);
            c1->SetGridx(true);
            HMass->Reset();
            for(int x=1;x<=HdedxVsP->GetNbinsX();x++){
               if(HdedxVsP->GetXaxis()->GetBinCenter(x)>3.0)continue;
               for(int y=1;y<=HdedxVsP->GetNbinsY();y++){
                  if(HdedxVsP->GetYaxis()->GetBinCenter(y)<5.0)continue;
                  HMass->Fill(GetMass (HdedxVsP->GetXaxis()->GetBinCenter(x),HdedxVsP->GetYaxis()->GetBinCenter(y)), HdedxVsP->GetBinContent(x,y));
               }
            }
            HMass->SetStats(kFALSE);
            HMass->GetXaxis()->SetTitle("Mass (GeV)");
            HMass->GetYaxis()->SetTitle("fraction of tracks");
            HMass->SetAxisRange(0,5,"X");
            HMass->Scale(1.0/HMass->Integral());
            HMass->SetLineColor  (kBlack);
            HMass->SetMarkerColor(kBlack);
            HMass->SetMarkerStyle(20);
            HMass->Draw("");
            if (InputFile2){
               HMass2->Reset();
               for(int x=1;x<=HdedxVsP->GetNbinsX();x++){
                  if(HdedxVsP2->GetXaxis()->GetBinCenter(x)>3.0)continue;
                  for(int y=1;y<=HdedxVsP2->GetNbinsY();y++){
                     if(HdedxVsP2->GetYaxis()->GetBinCenter(y)<5.0)continue;
                     HMass2->Fill(GetMass (HdedxVsP2->GetXaxis()->GetBinCenter(x),HdedxVsP2->GetYaxis()->GetBinCenter(y), 1), HdedxVsP2->GetBinContent(x,y));
                  }
               }
               HMass2->Scale(1.0/HMass2->Integral());
               HMass2->SetLineColor  (kBlue);
               HMass2->SetMarkerColor(kBlue);
               HMass2->SetMarkerStyle(22);
               HMass2->Draw("same");
               leg->AddEntry (HMass , SaveName .c_str(), "P");
               leg->AddEntry (HMass2, SaveName2.c_str(), "P");
               leg->Draw ();
            }
      
            double lineStart = InputFile2 ? std::min (HMass2->GetMinimum(), HMass->GetMinimum()) : HMass->GetMinimum(),
                   lineEnd   = InputFile2 ? std::max (HMass2->GetMaximum(), HMass->GetMaximum()) : HMass->GetMaximum();
            TLine* lineKaon = new TLine(0.493667, lineStart, 0.493667, lineEnd);
            lineKaon->SetLineWidth(2);
            lineKaon->SetLineStyle(2);
            lineKaon->SetLineColor(9);
            TLine* lineProton = new TLine(0.938272, lineStart, 0.938272, lineEnd);
            lineProton->SetLineWidth(2);
            lineProton->SetLineStyle(2);
            lineProton->SetLineColor(9);
            TLine* lineDeuteron = new TLine(1.88, lineStart, 1.88, lineEnd);
            lineDeuteron->SetLineWidth(2);
            lineDeuteron->SetLineStyle(2);
            lineDeuteron->SetLineColor(9);
            TLine* lineTriton = new TLine(2.80, lineStart, 2.80, lineEnd);
            lineTriton->SetLineWidth(2);
            lineTriton->SetLineStyle(2);
            lineTriton->SetLineColor(9);
      
            lineKaon->Draw("same");
            lineProton->Draw("same");
            lineDeuteron->Draw("same");
            T->Draw("same");
            SaveCanvas(c1, SaveDir, ObjName[i] + SaveSuffix +  "_Mass");
            delete leg;
            delete c1;
      } else continue;
      
   }
   CompareDeDx (InputFile, SaveDir, SaveName, "harm2_PO_raw" , "harm2_SO");
   CompareDeDx (InputFile, SaveDir, SaveName, "Ias_SO_inc"   , "Ias_SO");
   CompareDeDx (InputFile, SaveDir, SaveName, "Ias_SO"       , "Ias_SO_in");
   CompareDeDx (InputFile, SaveDir, SaveName, "Ias_SO_in"    , "Ias_SP_in_noC");
   CompareDeDx (InputFile, SaveDir, SaveName, "Ias_SO_in_noC", "Ias_SP_in_noC");
   CompareDeDx (InputFile, SaveDir, SaveName, "hit_SP"       , "hit_SP_in_noC");

   CompareMIPs (InputFile, SaveDir, SaveName, "Ias_SP");
   CompareMIPs (InputFile, SaveDir, SaveName, "Ias_SO");
   CompareMIPs (InputFile, SaveDir, SaveName, "harm2_SO");
   CompareMIPs (InputFile, SaveDir, SaveName, "harm2_SP");

   CompareMIPs2 (InputFile, SaveDir, SaveName, "Ias_SP");
   CompareMIPs2 (InputFile, SaveDir, SaveName, "Ias_SO");
   CompareMIPs2 (InputFile, SaveDir, SaveName, "harm2_SO");
   CompareMIPs2 (InputFile, SaveDir, SaveName, "harm2_SP");

   CompareMIPs2 (InputFile, SaveDir, SaveName, "hit_SP");

   ShortMIPs (InputFile, SaveDir, SaveName, "harm2_SP");
   ShortMIPs (InputFile, SaveDir, SaveName, "Ias_SP");
   ExtractConstantsReport.close();

   if (InputFile2) {
      ExtractConstantsReport2.close();
      CompareDeDx (InputFile2, SaveDir, SaveName2, "harm2_PO_raw" , "harm2_SO");
      CompareDeDx (InputFile2, SaveDir, SaveName2, "Ias_SO_inc"   , "Ias_SO");
      CompareDeDx (InputFile2, SaveDir, SaveName2, "Ias_SO"       , "Ias_SO_in");
      CompareDeDx (InputFile2, SaveDir, SaveName2, "Ias_SO_in"    , "Ias_SP_in_noC");
      CompareDeDx (InputFile2, SaveDir, SaveName2, "Ias_SO_in_noC", "Ias_SP_in_noC");
      CompareDeDx (InputFile2, SaveDir, SaveName2, "hit_SP"       , "hit_SP_in_noC");

      CompareMIPs (InputFile2, SaveDir, SaveName2, "Ias_SP");
      CompareMIPs (InputFile2, SaveDir, SaveName2, "Ias_SO");
      CompareMIPs (InputFile2, SaveDir, SaveName2, "harm2_SO");
      CompareMIPs (InputFile2, SaveDir, SaveName2, "harm2_SP");

      CompareMIPs2 (InputFile2, SaveDir, SaveName2, "Ias_SP");
      CompareMIPs2 (InputFile2, SaveDir, SaveName2, "Ias_SO");
      CompareMIPs2 (InputFile2, SaveDir, SaveName2, "harm2_SO");
      CompareMIPs2 (InputFile2, SaveDir, SaveName2, "harm2_SP");

      CompareMIPs2 (InputFile2, SaveDir, SaveName2, "hit_SP");

      ShortMIPs (InputFile2, SaveDir, SaveName2, "harm2_SP");
      ShortMIPs (InputFile2, SaveDir, SaveName2, "Ias_SP");

      // ROC curves for short tracks
      vector <string> ObjNames; vector <string> LegendLabels; vector <Color_t> Colors;
      ObjNames.push_back("harm2_PO_raw_MIP");          LegendLabels.push_back("no strips");                Colors.push_back(kYellow);
      ObjNames.push_back("harm2_SO_in_noC_MIP4");  LegendLabels.push_back("4 strip measurements");         Colors.push_back(kBlue);
      ObjNames.push_back("harm2_SO_in_noC_MIP8");  LegendLabels.push_back("8 strip measurements");         Colors.push_back(kRed);
      ObjNames.push_back("harm2_SO_in_noC_MIP12"); LegendLabels.push_back("12 strip measurements");        Colors.push_back(kGreen);
      ObjNames.push_back("harm2_SO_in_noC_MIP");   LegendLabels.push_back("unlimited strip measurements"); Colors.push_back(kBlack);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SO_short");

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("harm2_PO_raw_MIP");          LegendLabels.push_back("no strips");                Colors.push_back(kYellow);
      ObjNames.push_back("harm2_SP_in_noC_MIP4");  LegendLabels.push_back("4 strip measurements");         Colors.push_back(kBlue);
      ObjNames.push_back("harm2_SP_in_noC_MIP8");  LegendLabels.push_back("8 strip measurements");         Colors.push_back(kRed);
      ObjNames.push_back("harm2_SP_in_noC_MIP12"); LegendLabels.push_back("12 strip measurements");        Colors.push_back(kGreen);
      ObjNames.push_back("harm2_SP_in_noC_MIP");   LegendLabels.push_back("unlimited strip measurements"); Colors.push_back(kBlack);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SP_short");

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_PO_MIP");          LegendLabels.push_back("no strips");                    Colors.push_back(kYellow);
      ObjNames.push_back("Ias_SO_in_noC_MIP4");  LegendLabels.push_back("4 strip measurements");         Colors.push_back(kBlue);
      ObjNames.push_back("Ias_SO_in_noC_MIP8");  LegendLabels.push_back("8 strip measurements");         Colors.push_back(kRed);
      ObjNames.push_back("Ias_SO_in_noC_MIP12"); LegendLabels.push_back("12 strip measurements");        Colors.push_back(kGreen);
      ObjNames.push_back("Ias_SO_in_noC_MIP");   LegendLabels.push_back("unlimited strip measurements"); Colors.push_back(kBlack);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Ias_SO_short");

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_PO_MIP");          LegendLabels.push_back("no strips");                    Colors.push_back(kYellow);
      ObjNames.push_back("Ias_SP_in_noC_MIP4");  LegendLabels.push_back("4 strip measurements");         Colors.push_back(kBlue);
      ObjNames.push_back("Ias_SP_in_noC_MIP8");  LegendLabels.push_back("8 strip measurements");         Colors.push_back(kRed);
      ObjNames.push_back("Ias_SP_in_noC_MIP12"); LegendLabels.push_back("12 strip measurements");        Colors.push_back(kGreen);
      ObjNames.push_back("Ias_SP_in_noC_MIP");   LegendLabels.push_back("unlimited strip measurements"); Colors.push_back(kBlack);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Ias_SP_short");

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_SO_in_noC_MIP4");    LegendLabels.push_back("Strip-Only I_{as}, 4 measurements");   Colors.push_back(kBlack);
      ObjNames.push_back("Ias_SP_in_noC_MIP4");    LegendLabels.push_back("Strip+Pixel I_{as}, 4 measurements");  Colors.push_back(kBlue);
      ObjNames.push_back("harm2_SO_in_noC_MIP4");  LegendLabels.push_back("Strip-Only I_{h}, 4 measurements");    Colors.push_back(kGreen);
      ObjNames.push_back("harm2_SP_in_noC_MIP4");  LegendLabels.push_back("Strip+Pixel I_{h}, 4 measurements");   Colors.push_back(kRed);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best_short4");
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best_short4", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_SO_in_noC_MIP8");    LegendLabels.push_back("Strip-Only I_{as}, 8 measurements");   Colors.push_back(kBlack);
      ObjNames.push_back("Ias_SP_in_noC_MIP8");    LegendLabels.push_back("Strip+Pixel I_{as}, 8 measurements");  Colors.push_back(kBlue);
      ObjNames.push_back("harm2_SO_in_noC_MIP8");  LegendLabels.push_back("Strip-Only I_{h}, 8 measurements");    Colors.push_back(kGreen);
      ObjNames.push_back("harm2_SP_in_noC_MIP8");  LegendLabels.push_back("Strip+Pixel I_{h}, 8 measurements");   Colors.push_back(kRed);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best_short8");
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best_short8", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_SO_in_noC_MIP12");    LegendLabels.push_back("Strip-Only I_{as}, 12 measurements");  Colors.push_back(kBlack);
      ObjNames.push_back("Ias_SP_in_noC_MIP12");    LegendLabels.push_back("Strip+Pixel I_{as}, 12 measurements"); Colors.push_back(kBlue);
      ObjNames.push_back("harm2_SO_in_noC_MIP12");  LegendLabels.push_back("Strip-Only I_{h}, 12 measurements");   Colors.push_back(kGreen);
      ObjNames.push_back("harm2_SP_in_noC_MIP12");  LegendLabels.push_back("Strip+Pixel I_{h}, 12 measurements");  Colors.push_back(kRed);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best_short12");
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best_short12", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_SO_in_noC_MIP");      LegendLabels.push_back("Strip-Only trimmed+no cosmics");       Colors.push_back(kBlack);
      ObjNames.push_back("Ias_SO_in_noC_CC_MIP");   LegendLabels.push_back("After cluster cleaning (CC)");         Colors.push_back(kGreen);
      ObjNames.push_back("Ias_SO_in_noC_CI_MIP");   LegendLabels.push_back("After cross-talk inversion (CI)");     Colors.push_back(kBlue);
      ObjNames.push_back("Ias_SO_in_noC_CCC_MIP");  LegendLabels.push_back("CC + CI");                             Colors.push_back(kRed);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "SO_CC_CI_CCC");
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "SO_CC_CI_CCC", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back("Ias_SP_in_noC_MIP");      LegendLabels.push_back("Strip+Pixel trimmed+no cosmics");      Colors.push_back(kBlack);
      ObjNames.push_back("Ias_SP_in_noC_CC_MIP");   LegendLabels.push_back("After cluster cleaning (CC)");         Colors.push_back(kGreen);
      ObjNames.push_back("Ias_SP_in_noC_CI_MIP");   LegendLabels.push_back("After cross-talk inversion (CI)");     Colors.push_back(kBlue);
      ObjNames.push_back("Ias_SP_in_noC_CCC_MIP");  LegendLabels.push_back("CC + CI");                             Colors.push_back(kRed);
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "SP_CC_CI_CCC");
      MakeROCGeneral (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "SP_CC_CI_CCC", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back ("harm2_SO");           LegendLabels.push_back("Strip-Only split I_{h}");                 Colors.push_back(kBlue);
      ObjNames.push_back ("harm2_SO_in");        LegendLabels.push_back("Strip-Only trimmed I_{h}");               Colors.push_back(kGreen);
      ObjNames.push_back ("harm2_SO_in_noC");    LegendLabels.push_back("Strip-Only trimmed + no cosmics I_{h}");  Colors.push_back(kRed);
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SO_IncVsSplit");
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SO_IncVsSplit", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back ("Ias_SO_inc");       LegendLabels.push_back("Strip-Only inclusive I_{as}");             Colors.push_back(kBlack);
      ObjNames.push_back ("Ias_SO");           LegendLabels.push_back("Strip-Only split I_{as}");                 Colors.push_back(kBlue);
      ObjNames.push_back ("Ias_SO_in");        LegendLabels.push_back("Strip-Only trimmed I_{as}");               Colors.push_back(kGreen);
      ObjNames.push_back ("Ias_SO_in_noC");    LegendLabels.push_back("Strip-Only trimmed + no cosmics I_{as}");  Colors.push_back(kRed);
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Ias_SO_IncVsSplit");
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Ias_SO_IncVsSplit", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back ("Ias_PO");           LegendLabels.push_back("Pixel-Only I_{as}");                       Colors.push_back(kGreen);
      ObjNames.push_back ("Ias_SO_in_noC");    LegendLabels.push_back("Strip-Only trimmed + no cosmics I_{as}");  Colors.push_back(kBlue);
      ObjNames.push_back ("Ias_SP_in_noC");    LegendLabels.push_back("Strip+Pixel trimmed + no cosmics I_{as}"); Colors.push_back(kRed);
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Ias_SO_Vs_SP");
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Ias_SO_Vs_SP", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back ("harm2_SO");         LegendLabels.push_back("Strip-Only I_{h}");                        Colors.push_back(kBlack);
      ObjNames.push_back ("harm2_SO_in");      LegendLabels.push_back("Strip-Only trimmed I_{h}");                Colors.push_back(kBlue);
      ObjNames.push_back ("harm2_SO_in_noC");  LegendLabels.push_back("Strip-Only trimmed + no cosmics I_{h}");   Colors.push_back(kRed);
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SO");
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SO", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back ("harm2_SP");         LegendLabels.push_back("Strip+Pixel I_{h}");                       Colors.push_back(kBlack);
      ObjNames.push_back ("harm2_SP_in");      LegendLabels.push_back("Strip+Pixel trimmed I_{h}");               Colors.push_back(kBlue);
      ObjNames.push_back ("harm2_SP_in_noC");  LegendLabels.push_back("Strip+Pixel trimmed + no cosmics I_{h}");  Colors.push_back(kRed);
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SP");
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "harm2_SP", true);

      ObjNames.clear(); LegendLabels.clear(); Colors.clear();
      ObjNames.push_back ("Ias_SO_in_noC");    LegendLabels.push_back("Strip-Only trimmed + no cosmics I_{as}");  Colors.push_back(kBlack);
      ObjNames.push_back ("Ias_SP_in_noC");    LegendLabels.push_back("Strip+Pixel trimmed + no cosmics I_{as}"); Colors.push_back(kBlue);
      ObjNames.push_back ("harm2_SO_in_noC");  LegendLabels.push_back("Strip-Only trimmed + no cosmics I_{h}");   Colors.push_back(kGreen);
      ObjNames.push_back ("harm2_SP_in_noC");  LegendLabels.push_back("Strip+Pixel trimmed + no cosmics I_{h}");  Colors.push_back(kRed);
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best");
      MakeROCPlot (InputFile, InputFile2, ObjNames, LegendLabels, Colors, SaveDir, "Best", true);
   }


   std::cout << "TESTD\n";

   getScaleFactor(InputFile, NULL, "harm2_SO", "harm2_PO_raw", SaveDir, SaveName); // shift PO_raw to SO for File1
   if (InputFile2) {
      getScaleFactor(InputFile, InputFile2, "harm2_SO", "", SaveDir, SaveName+SaveName2); // shift File2 to File1
      getScaleFactor(InputFile2, NULL, "harm2_SO", "harm2_PO_raw", SaveDir, SaveName2);   // shift PO_raw to SO for File2
   }
}



void getScaleFactor(TFile* InputFile1, TFile* InputFile2, string ObjName1, string ObjName2, string SaveDir, string Prefix){
   TProfile*   HdedxVsPProfile1;
   TProfile*   HdedxVsPProfile2;
   TH1D*       HdedxMIP1;
   TH1D*       HdedxMIP2;

   if (InputFile2!=NULL){
      HdedxVsPProfile1 = (TProfile*)GetObjectFromPath(InputFile1, (ObjName1 + "_Profile"  ).c_str() );
      HdedxVsPProfile2 = (TProfile*)GetObjectFromPath(InputFile2, (ObjName1 + "_Profile"  ).c_str() );

      HdedxMIP1        = (TProfile*)GetObjectFromPath(InputFile1, (ObjName1 + "_MIP"  ).c_str() );
      HdedxMIP2        = (TProfile*)GetObjectFromPath(InputFile2, (ObjName1 + "_MIP"  ).c_str() );
   } else if (ObjName2!=""){
      HdedxVsPProfile1 = (TProfile*)GetObjectFromPath(InputFile1, (ObjName1 + "_Profile"  ).c_str() );
      HdedxVsPProfile2 = (TProfile*)GetObjectFromPath(InputFile1, (ObjName2 + "_Profile"  ).c_str() );

      HdedxMIP1        = (TProfile*)GetObjectFromPath(InputFile1, (ObjName1 + "_MIP"  ).c_str() );
      HdedxMIP2        = (TProfile*)GetObjectFromPath(InputFile1, (ObjName2 + "_MIP"  ).c_str() );
   } else return;

   TF1* mygausMIP = new TF1("mygausMIP","gaus", 1, 5);
   HdedxMIP1->Fit("mygausMIP","Q0","");
   double peakMIP1  = mygausMIP->GetParameter(1);
   HdedxMIP2->Fit("mygausMIP","Q0","");
   double peakMIP2  = mygausMIP->GetParameter(1);

   std::cout << "SCALE FACTOR WITH MIP     = " << peakMIP1/peakMIP2 << endl;

   TH1D* Chi2Dist = new TH1D("Chi2Dist","Chi2Dist",300, 0.9 ,1.15);

   double Minimum = 999999;
   double AbsGain = -1;

   for(int i=1;i<=Chi2Dist->GetNbinsX();i++){
      double ScaleFactor = Chi2Dist->GetXaxis()->GetBinCenter(i);
      TProfile* Rescaled = (TProfile*)HdedxVsPProfile2->Clone("Cloned");
      Rescaled->Scale(ScaleFactor);
      double Dist = 0;
      double Error = 0;
      for(int x=1;x<=HdedxVsPProfile1->GetNbinsX();x++){
         double Momentum = HdedxVsPProfile1->GetXaxis()->GetBinCenter(x);
         if(Momentum<5)continue;//|| Momentum>20)continue;
         if(HdedxVsPProfile1->GetBinError(x)<=0)continue;
         Dist += pow(HdedxVsPProfile1->GetBinContent(x) - Rescaled->GetBinContent(x),2) / std::max(1E-8,pow(HdedxVsPProfile1->GetBinError(x),2));
         Error += pow(HdedxVsPProfile1->GetBinError(x),2);
      }
      Dist *= Error;

      if(Dist<Minimum){Minimum=Dist;AbsGain=ScaleFactor;}

      //std::cout << "Rescale = " << ScaleFactor << " --> SquareDist = " << Dist << endl;
      Chi2Dist->Fill(ScaleFactor,Dist);
      delete Rescaled;
   }

   std::cout << "SCALE FACTOR WITH PROFILE = " << AbsGain << endl;

   TCanvas* c1 = new TCanvas("c1", "c1", 600,600);
   TLegend* leg = new TLegend (0.50, 0.75, 0.80, 0.90);
   leg->SetHeader ("Fitting the MIP");
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   HdedxMIP2->SetStats(kFALSE);
   HdedxMIP2->SetAxisRange(0,10,"X");
   HdedxMIP2->GetXaxis()->SetNdivisions(516);
   HdedxMIP2->GetXaxis()->SetTitle("dE/dx (MeV/cm)");
   HdedxMIP2->GetYaxis()->SetTitle("Tracks");
   HdedxMIP2->SetLineColor(1);
   HdedxMIP2->Draw("");
   TH1D* HdedxMIP3 = (TH1D*)HdedxMIP2->Clone("aaa");
   HdedxMIP3->SetLineColor(8);
   HdedxMIP3->GetXaxis()->Set(HdedxMIP3->GetXaxis()->GetNbins(), HdedxMIP3->GetXaxis()->GetXmin()*2.0, HdedxMIP3->GetXaxis()->GetXmax()*(peakMIP1/peakMIP2) );
//   HdedxMIP3->Draw("same");
   TH1D* HdedxMIP4 = (TH1D*)HdedxMIP2->Clone("bbb");
   HdedxMIP4->SetLineColor(4);
   HdedxMIP4->GetXaxis()->Set(HdedxMIP4->GetXaxis()->GetNbins(), HdedxMIP4->GetXaxis()->GetXmin()*2.0, HdedxMIP4->GetXaxis()->GetXmax()*(AbsGain) );
   HdedxMIP4->Draw("same");
   HdedxMIP1->SetLineColor(2);
   HdedxMIP1->Draw("same");
   c1->SetLogy(true);
   c1->SetGridx(true); 
   leg->AddEntry (HdedxMIP1, "Data", "L");
   leg->AddEntry (HdedxMIP2, "unscaled MC", "L");
   leg->AddEntry (HdedxMIP4, "scaled   MC", "L");
   leg->Draw();
   SaveCanvas(c1, SaveDir, "Rescale"+Prefix+"_"+ObjName1+ObjName2 + "_MIP");
   delete c1;


   c1 = new TCanvas("c1", "c1", 600,600);
   Chi2Dist->SetStats(kFALSE);
   Chi2Dist->GetXaxis()->SetNdivisions(504);
   Chi2Dist->GetXaxis()->SetTitle("Rescale Factor");
   Chi2Dist->GetYaxis()->SetTitle("Weighted Square Distance");
   Chi2Dist->Draw("");
   c1->SetLogy(true);
   c1->SetGridx(true); 
   SaveCanvas(c1, SaveDir, "Rescale"+Prefix+"_"+ObjName1+ObjName2 + "_Dist");
   delete c1;

   c1 = new TCanvas("c1", "c1", 600,600);
   leg = new TLegend (0.30, 0.25, 0.80, 0.55);
   leg->SetHeader ("Fitting the Profile");
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);
   HdedxVsPProfile1->SetStats(kFALSE);
   HdedxVsPProfile1->SetAxisRange(5,50,"X");
   HdedxVsPProfile1->SetAxisRange(2.5,3.5,"Y");
   HdedxVsPProfile1->GetXaxis()->SetTitle("track momentum (GeV/c)");
   HdedxVsPProfile1->GetYaxis()->SetTitle("dE/dx (MeV/cm)");
   HdedxVsPProfile1->SetMarkerColor(2);
   HdedxVsPProfile1->Draw("");

   HdedxVsPProfile2->SetMarkerColor(1);
   HdedxVsPProfile2->Draw("same");
   TProfile* HdedxVsPProfile3 = (TProfile*)HdedxVsPProfile2->Clone("abc");
   HdedxVsPProfile3->SetMarkerColor(8);
   HdedxVsPProfile3->Scale(peakMIP1/peakMIP2);
//   HdedxVsPProfile3->Draw("same");
   TProfile* HdedxVsPProfile4 = (TProfile*)HdedxVsPProfile2->Clone("afs");
   HdedxVsPProfile4->SetMarkerColor(4);
   HdedxVsPProfile4->Scale(AbsGain);
   HdedxVsPProfile4->Draw("same");
   leg->AddEntry (HdedxVsPProfile1, "Data", "LP");
   leg->AddEntry (HdedxVsPProfile2, "unscaled MC", "LP");
   leg->AddEntry (HdedxVsPProfile4, "scaled MC",   "LP");
   leg->Draw();

   DrawPreliminary();
   SaveCanvas(c1, SaveDir, "Rescale"+Prefix+"_"+ObjName1+ObjName2 + "_Profile");
   delete c1;
}



void ExtractConstants(TH2D* input, int FileIndex){
       double MinRange = 0.60;
       double MaxRange = 1.20;
       char buffer[2048];
       bool hasConverged = false;

       for(unsigned int loop=0;loop<5 and !hasConverged; loop++){
	      TH2D* inputnew = (TH2D*)input->Clone("tempTH2D");
	      inputnew->Rebin2D(5,10);
	      for(int x=1;x<=inputnew->GetNbinsX();x++){
	      for(int y=1;y<=inputnew->GetNbinsY();y++){
		double Mass = GetMass(inputnew->GetXaxis()->GetBinCenter(x),inputnew->GetYaxis()->GetBinCenter(y));
		if(isnan(float(Mass)) || Mass<0.94-0.3 || Mass>0.94+0.3)inputnew->SetBinContent(x,y,0);        
	      }}

	      TCanvas* c1 = new TCanvas("c1", "c1", 600,600);
	      c1->SetLogz(true);
	      inputnew->SetStats(kFALSE);
	      inputnew->GetXaxis()->SetTitle("track momentum (GeV/c)");
	      inputnew->GetYaxis()->SetTitle("dE/dx (MeV/cm)");
	      inputnew->SetAxisRange(0,5,"X");
	      inputnew->SetAxisRange(0,15,"Y");
	      inputnew->Draw("COLZ");

	//      KaonLine->Draw("same");
	//      ProtonLine->Draw("same");
	//      DeuteronLine->Draw("same");
	//      TritonLine->Draw("same");
	      SaveCanvas(c1, "fit/", "dedxVsP");
	      delete c1;


	       TH1D* FitResult = new TH1D("FitResult"       , "FitResult"      ,inputnew->GetXaxis()->GetNbins(),inputnew->GetXaxis()->GetXmin(),inputnew->GetXaxis()->GetXmax());
	       FitResult->SetTitle("");
	       FitResult->SetStats(kFALSE);  
	       FitResult->GetXaxis()->SetTitle("P [GeV/c]");
	       FitResult->GetYaxis()->SetTitle("dE/dx Estimator [MeV/cm]");
	       FitResult->GetYaxis()->SetTitleOffset(1.20);
	       FitResult->Reset();     

	       for(int x=1;x<inputnew->GetXaxis()->FindBin(5);x++){
		  double P       = inputnew->GetXaxis()->GetBinCenter(x);
	    
		  TH1D* Projection = (TH1D*)(inputnew->ProjectionY("proj",x,x))->Clone();
		  if(Projection->Integral()<100)continue;
		  Projection->SetAxisRange(0.1,25,"X");
		  Projection->Sumw2();
		  Projection->Scale(1.0/Projection->Integral());

		  TF1* mygaus = new TF1("mygaus","gaus", 2.5, 15);
		  Projection->Fit("mygaus","Q0 RME");
		  double chiFromFit  = (mygaus->GetChisquare())/(mygaus->GetNDF());
		  FitResult->SetBinContent(x, mygaus->GetParameter(1));
		  FitResult->SetBinError  (x, mygaus->GetParError (1));
		  mygaus->SetLineColor(2);
		  mygaus->SetLineWidth(2);

		  c1  = new TCanvas("canvas", "canvas", 600,600);
		  Projection->Draw();
		  Projection->SetTitle("");
		  Projection->SetStats(kFALSE);
		  Projection->GetXaxis()->SetTitle("dE/dx Estimator [MeV/cm]");
		  Projection->GetYaxis()->SetTitle("#Entries");
		  Projection->GetYaxis()->SetTitleOffset(1.30);
		  Projection->SetAxisRange(1E-5,1.0,"Y");

		  mygaus->Draw("same");


		  TPaveText* stt = new TPaveText(0.55,0.82,0.79,0.92, "NDC");
		  stt->SetFillColor(0);
		  stt->SetTextAlign(31);
		  sprintf(buffer,"Proton  #mu:%5.1fMeV/cm",mygaus->GetParameter(1));      stt->AddText(buffer);
		  sprintf(buffer,"Proton  #sigma:%5.1fMeV/cm",mygaus->GetParameter(2));      stt->AddText(buffer);
		  stt->Draw("same");

		  //std::cout << "P = " << P << "  --> Proton dE/dx = " << mygaus->GetParameter(1) << endl;

		  c1->SetLogy(true);
		  sprintf(buffer,"%sProjectionFit_P%03i_%03i","fit/",(int)(100*FitResult->GetXaxis()->GetBinLowEdge(x)),(int)(100*FitResult->GetXaxis()->GetBinUpEdge(x)) );
		  if(P>=MinRange && P<=MaxRange){SaveCanvas(c1,"./",buffer);}
		  delete c1;
                  delete Projection;
                  delete mygaus;
                  delete stt;
	       }
	       c1  = new TCanvas("canvas", "canvas", 600,600);
	       FitResult->SetAxisRange(0,2.5,"X");
	       FitResult->SetAxisRange(0,15,"Y");
	       FitResult->Draw("");

	       TLine* line1 = new TLine(MinRange, FitResult->GetMinimum(), MinRange, FitResult->GetMaximum());
	       line1->SetLineWidth(2);
	       line1->SetLineStyle(2);
	       line1->Draw();

	       TLine* line2 = new TLine(MaxRange, FitResult->GetMinimum(), MaxRange, FitResult->GetMaximum());
	       line2->SetLineWidth(2);
	       line2->SetLineStyle(2);
	       line2->Draw();

	       //   TF1* myfit = new TF1("myfit","[1]+(pow(0.93827,2) + x*x)/([0]*x*x)", MinRange, MaxRange);
	       TF1* myfit = new TF1("myfit","[0]*pow(0.93827/x,2) + [1]", MinRange, MaxRange);
	       myfit->SetParName  (0,"K");
	       myfit->SetParName  (1,"C");
	       myfit->SetParameter(0, 3.2);
	       myfit->SetParameter(1, 3.2);
	       myfit->SetParLimits(0, 2.00,4.0);
	       myfit->SetParLimits(1, 2.00,4.0);
	       myfit->SetLineWidth(2);
	       myfit->SetLineColor(2);
	       FitResult->Fit("myfit", "M R E I 0");
	       myfit->SetRange(MinRange,MaxRange);
	       myfit->Draw("same");

	       double prevConstants [] = {K[FileIndex], Kerr[FileIndex], C[FileIndex], Cerr[FileIndex]};
	       K   [FileIndex] = myfit->GetParameter(0);
	       C   [FileIndex] = myfit->GetParameter(1);
	       Kerr[FileIndex] = myfit->GetParError(0);
	       Cerr[FileIndex] = myfit->GetParError(1);

	       printf("K Constant changed from %6.4f+-%6.4f to %6.4f+-%6.4f    (diff = %6.3f%%)\n", prevConstants[0], prevConstants[1], K[FileIndex], Kerr[FileIndex], 100.0*(K[FileIndex]-prevConstants[0])/K[FileIndex]);
	       printf("C Constant changed from %6.4f+-%6.4f to %6.4f+-%6.4f    (diff = %6.3f%%)\n", prevConstants[2], prevConstants[3], C[FileIndex], Cerr[FileIndex], 100.0*(C[FileIndex]-prevConstants[2])/C[FileIndex]);

          if(std::max(fabs(100.0*(K[FileIndex]-prevConstants[0])/K[FileIndex]), fabs(100.0*(C[FileIndex]-prevConstants[2])/C[FileIndex]))<1.0)hasConverged=true;  //<1% variation of the constant --> converged


	       TPaveText* st = new TPaveText(0.40,0.78,0.79,0.89, "NDC");
	       st->SetFillColor(0);
	       sprintf(buffer,"K = %4.3f +- %6.4f",myfit->GetParameter(0), myfit->GetParError(0));
	       st->AddText(buffer);
	       sprintf(buffer,"C = %4.3f +- %6.4f",myfit->GetParameter(1), myfit->GetParError(1));
	       st->AddText(buffer);
	       st->Draw("same");
	       sprintf(buffer,"%sFit","fit/");
	       SaveCanvas(c1,"./",buffer);              
	       delete c1;

          delete line1;
          delete line2;
          delete myfit;
          delete FitResult;
          delete inputnew;
       }
}

void CompareDeDx (TFile* InputFile, string SaveDir, string SaveName, string ObjName1, string ObjName2){
   if (ObjName1.find("hit")==string::npos && ObjName2.find("hit")==string::npos){
      TProfile*   HdedxVsEtaProfile1  = (TProfile*)  GetObjectFromPath(InputFile, (ObjName1 + "_Eta" ).c_str() );
      TProfile*   HdedxVsEtaProfile2  = (TProfile*)  GetObjectFromPath(InputFile, (ObjName2 + "_Eta" ).c_str() );
      TH1D*       HdedxMIP1           = (TH1D*)      GetObjectFromPath(InputFile, (ObjName1 + "_MIP" ).c_str() );
      TH1D*       HdedxMIP2           = (TH1D*)      GetObjectFromPath(InputFile, (ObjName2 + "_MIP" ).c_str() );
	
   	TCanvas* c1  = new TCanvas("c1", "c1", 600,600);
   	TLegend* leg = new TLegend(0.50, 0.80, 0.80, 0.90);
   	leg->SetHeader (SaveName.c_str());
   	leg->SetFillColor(0);
   	leg->SetFillStyle(0);
   	leg->SetBorderSize(0);
   	leg->AddEntry (HdedxVsEtaProfile1, ObjName1.c_str(), "P");
   	leg->AddEntry (HdedxVsEtaProfile2, ObjName2.c_str(), "P");
   	HdedxVsEtaProfile1->SetStats(kFALSE);
   	HdedxVsEtaProfile2->SetMarkerStyle(23);
   	HdedxVsEtaProfile1->SetMarkerColor(kBlack);
   	HdedxVsEtaProfile2->SetMarkerColor(kBlue);
   	HdedxVsEtaProfile1->GetXaxis()->SetTitle("#eta");
   	HdedxVsEtaProfile1->GetYaxis()->SetTitle(ObjName1.find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
   	HdedxVsEtaProfile1->Draw("");
   	HdedxVsEtaProfile2->Draw("same");
   	leg->Draw();
   	SaveCanvas(c1, SaveDir, "Comparison"+SaveName+"_"+ObjName1+"_"+ObjName2+"_HdedxVsEtaProfile");
   	delete leg;
   	delete c1;

   	c1 = new TCanvas("c1", "c1", 600,600);
   	leg = new TLegend (0.50, 0.80, 0.80, 0.90);
   	c1->SetLogy(true);
   	c1->SetGridx(true);
   	leg->SetHeader (SaveName.c_str());
   	leg->SetFillColor(0);
   	leg->SetFillStyle(0);
   	leg->SetBorderSize(0);
   	HdedxMIP1->SetStats(kFALSE);
   	HdedxMIP1->GetXaxis()->SetTitle(ObjName1.find("Ias")!=std::string::npos?"I_{as}":"dE/dx (MeV/cm)");
   	HdedxMIP1->GetYaxis()->SetTitle("fraction of tracks");
   	HdedxMIP2->Scale(1.0/HdedxMIP2->Integral());
   	HdedxMIP1->Scale(1.0/HdedxMIP1->Integral());
   	HdedxMIP1->GetXaxis()->SetRangeUser(0,8);
   	HdedxMIP1->GetYaxis()->SetRangeUser(2e-5,1);
   	HdedxMIP1->SetLineColor (kBlack);
   	HdedxMIP2->SetLineColor (kBlue);
   	leg->AddEntry (HdedxMIP1, ObjName1.c_str(), "L");
   	leg->AddEntry (HdedxMIP2, ObjName2.c_str(), "L");
   	HdedxMIP1->Draw("");
   	HdedxMIP2->Draw("same");
   	leg->Draw();
      DrawPreliminary();
      SaveCanvas(c1, SaveDir, "Comparison"+SaveName+"_"+ObjName1+"_"+ObjName2+"_MIP", true);
      delete leg;
      delete c1;

   } else if (ObjName1.find("hit")!=string::npos && ObjName2.find("hit")!=string::npos){
      for (unsigned int g=0;g<16;g++){
         char Id[255]; sprintf (Id, "%02i", g);
         TH2D* Charge_Vs_XYLN1 = (TH2D*) GetObjectFromPath (InputFile, (ObjName1 + "_ChargeVsXYLN" + Id).c_str());
         TH2D* Charge_Vs_XYLN2 = (TH2D*) GetObjectFromPath (InputFile, (ObjName2 + "_ChargeVsXYLN" + Id).c_str());
         TH1D* ProjX1          = Charge_Vs_XYLN1->ProjectionX (("X1_"+string(Id)).c_str());
         TH1D* ProjY1          = Charge_Vs_XYLN1->ProjectionY (("Y1_"+string(Id)).c_str());
         TH1D* ProjX2          = Charge_Vs_XYLN2->ProjectionX (("X2_"+string(Id)).c_str());
         TH1D* ProjY2          = Charge_Vs_XYLN2->ProjectionY (("Y2_"+string(Id)).c_str());

         TCanvas* c1  = new TCanvas ("c1", "c1", 600, 600);
         TLegend* leg = new TLegend (0.50, 0.75, 0.80, 0.90);
         c1->SetLogy (true);
         leg->SetHeader (("Module No. " + string(Id)).c_str());
         leg->SetHeader (SaveName.c_str());
         leg->SetFillColor(0);
         leg->SetFillStyle(0);
         leg->SetBorderSize(0);
         ProjX1->SetStats(kFALSE);
         ProjX1->SetLineColor (kBlack);
         ProjX2->SetLineColor (kBlue);
         ProjX1->GetXaxis()->SetTitle("normalized x coordinate");
         ProjX1->GetYaxis()->SetTitle("number of hits");
         ProjX1->SetAxisRange (-1.5, 1.5, "X");
         ProjX1->Draw("L");
         ProjX2->Draw("same");
         leg->AddEntry(ProjX1, ObjName1.c_str(), "L");
         leg->AddEntry(ProjX2, ObjName2.c_str(), "L");
         SaveCanvas(c1, SaveDir, "Comparison"+SaveName+"_"+ObjName1+"_"+ObjName2+"_ProjX"+string(Id), true);
         delete leg;
         delete c1;
 
         c1  = new TCanvas ("c1", "c1", 600, 600);
         leg = new TLegend (0.50, 0.75, 0.80, 0.90);
         c1->SetLogy (true);
         leg->SetHeader (("Module No. " + string(Id)).c_str());
         leg->SetHeader (SaveName.c_str());
         leg->SetFillColor(0);
         leg->SetFillStyle(0);
         leg->SetBorderSize(0);
         ProjY1->SetStats(kFALSE);
         ProjY1->SetLineColor (kBlack);
         ProjY2->SetLineColor (kBlue);
         ProjY1->GetXaxis()->SetTitle("normalized y coordinate");
         ProjY1->GetYaxis()->SetTitle("number of hits");
         ProjY1->SetAxisRange (-1.5, 1.5, "X");
         ProjY1->Draw("L");
         ProjY2->Draw("same");
         leg->AddEntry(ProjY1, ObjName1.c_str(), "L");
         leg->AddEntry(ProjY2, ObjName2.c_str(), "L");
         SaveCanvas(c1, SaveDir, "Comparison"+SaveName+"_"+ObjName1+"_"+ObjName2+"_ProjY"+string(Id), true);
         delete leg;
         delete c1;
         delete ProjX1;
         delete ProjX2;
         delete ProjY1;
         delete ProjY2;
      }
   }
}

void CompareMIPs (TFile* InputFile, string SaveDir, string Prefix, string dEdxString)
{
   TH1D** MIPs = new TH1D* [dEdxString.find("Ias")!=string::npos?4:3]; vector <string> legend;
   MIPs[0] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_MIP"       ).c_str());
   MIPs[1] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_MIP"    ).c_str());
   MIPs[2] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_MIP").c_str());

   MIPs[0]->Scale(1.0/MIPs[0]->Integral()); legend.push_back("no trimming");
   MIPs[1]->Scale(1.0/MIPs[1]->Integral()); legend.push_back("trimmed"    );
   MIPs[2]->Scale(1.0/MIPs[2]->Integral()); legend.push_back("trimmed + no cosmics");

   if (dEdxString.find("Ias")!=string::npos){
      MIPs[3] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_inc_MIP").c_str());
      MIPs[3]->Scale(1.0/MIPs[3]->Integral()); legend.push_back("inclusive, untrimmed");
   }

   TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
   c1->SetLogy(true);
   DrawSuperposedHistos ((TH1**) MIPs, legend, "", dEdxString.find("Ias")!=string::npos?"I_{as}":"dE/dx (MeV/cm)", "fraction of tracks", 0,dEdxString.find("Ias")!=string::npos?1:8,1e-6,1);
   DrawLegend ((TObject**) MIPs, legend, "Stages", "L");
   DrawPreliminary();
   SaveCanvas (c1, SaveDir, "Comparison"+Prefix+"_"+dEdxString+"_MIPs", true);
   delete c1;
}

void CompareMIPs2 (TFile* InputFile, string SaveDir, string Prefix, string dEdxString)
{
   TH1D** MIPs = new TH1D* [4]; vector <string> legend;
   if (dEdxString.find("hit")==string::npos){
      MIPs[0] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_MIP"     ).c_str());
      MIPs[1] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_CI_MIP"  ).c_str());
      MIPs[2] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_CC_MIP"  ).c_str());
      MIPs[3] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_CCC_MIP" ).c_str());
   } else {
      MIPs[0] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_Hit"     ).c_str());
      MIPs[1] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_CI_Hit"  ).c_str());
      MIPs[2] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_CC_Hit"  ).c_str());
      MIPs[3] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_in_noC_CCC_Hit" ).c_str());
   }

   MIPs[0]->Scale(1.0/MIPs[0]->Integral());
   MIPs[1]->Scale(1.0/MIPs[1]->Integral());
   MIPs[2]->Scale(1.0/MIPs[2]->Integral());
   MIPs[3]->Scale(1.0/MIPs[3]->Integral());

   legend.push_back("trimmed + no cosmics"  );
   legend.push_back("Cross-talk Inv. (CI)"  );
   legend.push_back("Cluster cleaning (CC)" );
   legend.push_back("CC + CI"               );

   TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
   c1->SetLogy(true);
   DrawSuperposedHistos ((TH1**) MIPs, legend, "", dEdxString.find("Ias")!=string::npos?"I_{as}":"dE/dx (MeV/cm)", "fraction of tracks", 0,dEdxString.find("Ias")!=string::npos?1:(dEdxString.find("hit")!=string::npos?20:8),1e-6,1);
   DrawLegend ((TObject**) MIPs, legend, "Stages", "L");
   DrawPreliminary();
   SaveCanvas (c1, SaveDir, "Comparison"+Prefix+"_"+dEdxString+"_MIPs2", true);
   delete c1;
}



void ShortMIPs (TFile* InputFile, string SaveDir, string Prefix, string dEdxString){
   TH1D** MIPs = new TH1D* [5]; vector <string> legend;
   MIPs[0] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString.find("Ias")!=string::npos?"Ias_PO_MIP":"harm2_PO_raw_MIP"));
   MIPs[1] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_MIP4" ).c_str());
   MIPs[2] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_MIP8" ).c_str());
   MIPs[3] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_MIP12").c_str());
   MIPs[4] = (TH1D*) GetObjectFromPath (InputFile, (dEdxString + "_MIP"  ).c_str());

   MIPs[0]->Scale(1.0/MIPs[0]->Integral()); legend.push_back ("no strip");
   MIPs[1]->Scale(1.0/MIPs[1]->Integral()); legend.push_back ("4 measurements");
   MIPs[2]->Scale(1.0/MIPs[2]->Integral()); legend.push_back ("8 measurements");
   MIPs[3]->Scale(1.0/MIPs[3]->Integral()); legend.push_back ("12 measurements");
   MIPs[4]->Scale(1.0/MIPs[4]->Integral()); legend.push_back ("no limit");
   TCanvas* c1 = new TCanvas ("c1", "c1", 600, 600);
   c1->SetLogy(true);
   DrawSuperposedHistos ((TH1**) MIPs, legend, "", dEdxString.find("Ias")!=string::npos?"I_{as}":"dE/dx (MeV/cm)", "number of tracks", 0,dEdxString.find("Ias")!=string::npos?1:8,1e-6,1);
   DrawLegend ((TObject**) MIPs, legend, "No. of strip dE/dx measurements", "L");
   DrawPreliminary();
   SaveCanvas (c1, SaveDir, "Comparison"+Prefix+"_"+dEdxString+"_ShortMIPs", true);
   delete c1;
}

void MakeMapPlots(TH3F* Charge_Vs_Path3D, string ObjName, string SaveDir, string Prefix)
{
   for(int x=0;x<17;x++){
      char xProjName[255];
      if(x==0){
         sprintf(xProjName,"%s","SO_inc");
         Charge_Vs_Path3D->GetXaxis()->SetRange(1,14);
      }else if (x==16){
         sprintf(xProjName,"%s","SP_inc");
         Charge_Vs_Path3D->GetXaxis()->SetRange(1,15);
      }else if (x==15){
         sprintf(xProjName,"%s", "PO");
         Charge_Vs_Path3D->GetXaxis()->SetRange(x,x);
      }else{
         sprintf(xProjName,"%02i",x);
         Charge_Vs_Path3D->GetXaxis()->SetRange(x,x);
      }
      printf("---------------\n%s------------\n",xProjName);
      string xProjNameStr(xProjName);


      TH2D*  Charge_Vs_Path2D = (TH2D*)Charge_Vs_Path3D->Project3D("zy");
      char legEntry[128];
      double binMinA = Charge_Vs_Path2D->GetXaxis()->GetBinLowEdge(4);
      double binMaxA = Charge_Vs_Path2D->GetXaxis()->GetBinUpEdge(6);

      TH1D*  Charge_Vs_PathA  = (TH1D*)Charge_Vs_Path2D->ProjectionY("projA",4,6);
      Charge_Vs_PathA->Rebin(2);
      sprintf(legEntry,"[%5.2f,%5.2f]",binMinA,binMaxA); string ALegend (legEntry);

      double binMinB = Charge_Vs_Path2D->GetXaxis()->GetBinLowEdge(7);
      double binMaxB = Charge_Vs_Path2D->GetXaxis()->GetBinUpEdge(9);
      TH1D*  Charge_Vs_PathB  = (TH1D*)Charge_Vs_Path2D->ProjectionY("projB",7,9);
      Charge_Vs_PathB->Rebin(2);
      sprintf(legEntry,"[%5.2f,%5.2f]",binMinB,binMaxB); string BLegend (legEntry);

      double binMinC = Charge_Vs_Path2D->GetXaxis()->GetBinLowEdge(10);
      double binMaxC = Charge_Vs_Path2D->GetXaxis()->GetBinUpEdge(12);
      TH1D*  Charge_Vs_PathC  = (TH1D*)Charge_Vs_Path2D->ProjectionY("projC",10,12);
      Charge_Vs_PathC->Rebin(2);
      sprintf(legEntry,"[%5.2f,%5.2f]",binMinC,binMaxC); string CLegend (legEntry);

      double binMinD = Charge_Vs_Path2D->GetXaxis()->GetBinLowEdge(13);
      double binMaxD = Charge_Vs_Path2D->GetXaxis()->GetBinUpEdge(15);
      TH1D*  Charge_Vs_PathD  = (TH1D*)Charge_Vs_Path2D->ProjectionY("projD",13,15);
      Charge_Vs_PathD->Rebin(2);
      sprintf(legEntry,"[%5.2f,%5.2f]",binMinD,binMaxD); string DLegend (legEntry);

      printf("%f to %f\n",binMinA,binMaxA);
      printf("%f to %f\n",binMinB,binMaxB);
      printf("%f to %f\n",binMinC,binMaxC);
      printf("%f to %f\n",binMinD,binMaxD);


      TCanvas* c0;
      TH1D** Histos = new TH1D* [4]; 
      vector <string> legend;

      c0  = new TCanvas("c0", "c0", 600,600);
      Charge_Vs_Path2D->SetTitle("");
      Charge_Vs_Path2D->SetStats(kFALSE);
      Charge_Vs_Path2D->GetXaxis()->SetTitle("pathlength (mm)");
      Charge_Vs_Path2D->GetYaxis()->SetTitle("#Delta E/#Delta x (ADC/mm)");
      Charge_Vs_Path2D->GetYaxis()->SetTitleOffset(1.80);
      Charge_Vs_Path2D->GetZaxis()->SetRangeUser(1.0,4e+4);
      Charge_Vs_Path2D->Draw("COLZ");

      c0->SetLogz(true);
      SaveCanvas(c0, SaveDir, Prefix + "_" + ObjName+xProjName + "_TH2", true);
      delete c0;


      //Compute Probability Map.
      TH2D* Prob_ChargePath  = new TH2D ("Prob_ChargePath"     , "Prob_ChargePath" , Charge_Vs_Path2D->GetXaxis()->GetNbins(), Charge_Vs_Path2D->GetXaxis()->GetXmin(), Charge_Vs_Path2D->GetXaxis()->GetXmax(), Charge_Vs_Path2D->GetYaxis()->GetNbins(), Charge_Vs_Path2D->GetYaxis()->GetXmin(), Charge_Vs_Path2D->GetYaxis()->GetXmax());
      for(int j=0;j<=Prob_ChargePath->GetXaxis()->GetNbins()+1;j++){
         double Ni = 0;
         for(int k=0;k<=Prob_ChargePath->GetYaxis()->GetNbins()+1;k++){ Ni+=Charge_Vs_Path2D->GetBinContent(j,k);} 

         for(int k=0;k<=Prob_ChargePath->GetYaxis()->GetNbins()+1;k++){
            double tmp = 1E-10;
	         for(int l=0;l<=k;l++){ tmp+=Charge_Vs_Path2D->GetBinContent(j,l);}

            if(Ni>0){
               Prob_ChargePath->SetBinContent (j, k, tmp/Ni);
            }else{
               Prob_ChargePath->SetBinContent (j, k, 0);
            }
         }
      }

      c0  = new TCanvas("c0", "c0", 600,600);
      Prob_ChargePath->SetTitle("Probability MIP(#DeltaE/#DeltaX) < Obs (#DeltaE/#DeltaX)");
      Prob_ChargePath->SetStats(kFALSE);
      Prob_ChargePath->GetXaxis()->SetTitle("pathlength (mm)");
      Prob_ChargePath->GetYaxis()->SetTitle("Observed #DeltaE/#DeltaX (ADC/mm)");
      Prob_ChargePath->GetYaxis()->SetTitleOffset(1.80);
      Prob_ChargePath->GetXaxis()->SetRangeUser(0.28,1.2);
      Prob_ChargePath->GetYaxis()->SetRangeUser(0.0, 1000);
//      Prob_ChargePath->GetYaxis()->SetRangeUser(0,1000);
      Prob_ChargePath->Draw("COLZ");

      //c0->SetLogz(true);
      SaveCanvas(c0, SaveDir, Prefix + "_" + ObjName+xProjName + "_TH2Proba", true);
      delete c0;

      c0 = new TCanvas("c1","c1,",600,600);          legend.clear();
      Histos[0] = Charge_Vs_PathA;                   legend.push_back(ALegend);
      Histos[1] = Charge_Vs_PathB;                   legend.push_back(BLegend);
      Histos[2] = Charge_Vs_PathC;                   legend.push_back(CLegend);
      Histos[3] = Charge_Vs_PathD;                   legend.push_back(DLegend);
      if(Histos[0]->Integral()>=1)Histos[0]->Scale(1/Histos[0]->Integral());
      if(Histos[1]->Integral()>=1)Histos[1]->Scale(1/Histos[1]->Integral());
      if(Histos[2]->Integral()>=1)Histos[2]->Scale(1/Histos[2]->Integral());
      if(Histos[3]->Integral()>=1)Histos[3]->Scale(1/Histos[3]->Integral());
   //   DrawSuperposedHistos((TH1D**)Histos, legend, "",  "Normalized Cluster Charge (ADC/mm)", "u.a.", 0,1200, 0,0);
      DrawSuperposedHistos((TH1**)Histos, legend, "",  "Normalized Cluster Charge (ADC/mm)", "u.a.", 0,600, 0,0);
      DrawLegend((TObject**)Histos,legend,"PathLength (mm):","L");
      c0->SetGridx(true);
      Charge_Vs_PathA->GetXaxis()->SetNdivisions(520);
      SaveCanvas(c0, SaveDir, Prefix+"_"+ObjName+xProjName+"_TH1Linear");
      delete c0;


      c0 = new TCanvas("c1","c1,",600,600);          legend.clear();
      Histos[0] = Charge_Vs_PathA;                   legend.push_back(ALegend);
      Histos[1] = Charge_Vs_PathB;                   legend.push_back(BLegend);
      Histos[2] = Charge_Vs_PathC;                   legend.push_back(CLegend);
      Histos[3] = Charge_Vs_PathD;                   legend.push_back(DLegend);
      if(Histos[0]->Integral()>=1)Histos[0]->Scale(1.0/Histos[0]->Integral());
      if(Histos[1]->Integral()>=1)Histos[1]->Scale(1.0/Histos[1]->Integral());
      if(Histos[2]->Integral()>=1)Histos[2]->Scale(1.0/Histos[2]->Integral());
      if(Histos[3]->Integral()>=1)Histos[3]->Scale(1.0/Histos[3]->Integral());
      DrawSuperposedHistos((TH1**)Histos, legend, "",  "Normalized Cluster Charge (ADC/mm)", "u.a.", 0,3000, 0,0);
//      DrawLegend((TObject**)Histos,legend,"PathLength (mm):","L");
      c0->SetLogy(true);
      SaveCanvas(c0, SaveDir, Prefix + "_"+ObjName+xProjName+"_TH1");
      delete c0;
      delete Charge_Vs_Path2D;
      delete Charge_Vs_PathA;
      delete Charge_Vs_PathB;
      delete Charge_Vs_PathC;
      delete Charge_Vs_PathD;
      delete Prob_ChargePath;
   }
}

void MakeROCPlot (TFile* InputFile1, TFile* InputFile2, vector<string> ObjNames, vector<string> LegendLabels, vector<Color_t> Colors, string SaveDir, string suffix, bool withErrorBars) {

      TCanvas* c1   = new TCanvas ("c1", "c1", 600,600); 
      TLegend* leg  = new TLegend (0.30, 0.15, 0.80, 0.15+0.05*ObjNames.size());
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      c1->SetLogx(true);
      TH1D h ("tmp", "tmp", 1, 8E-6, 1);
      h.GetXaxis()->SetTitle("background efficiency");
      h.GetXaxis()->SetNdivisions(5);
      h.GetYaxis()->SetTitle("signal efficiency");
      h.GetYaxis()->SetNdivisions(5);
      h.SetAxisRange (0.65,1.0,"Y");
      h.SetStats(0);
      h.Draw();
      TGraphErrors** ROC = new TGraphErrors* [ObjNames.size()];
      for (size_t NameIndex = 0; NameIndex < ObjNames.size(); NameIndex++)
      {
         int divide = 1;
         TH1D* HdedxMIP1 = (TH1D*) GetObjectFromPath(InputFile1, (ObjNames[NameIndex] + "_MIP").c_str() );
         TH1D* HdedxMIP2 = (TH1D*) GetObjectFromPath(InputFile2, (ObjNames[NameIndex] + "_MIP").c_str() );
         ROC[NameIndex]  = new TGraphErrors(HdedxMIP1->GetNbinsX()/divide + 1);

         double fullBkg  = HdedxMIP1->Integral(0, HdedxMIP1->GetNbinsX()+1),
                fullSig  = HdedxMIP2->Integral(0, HdedxMIP2->GetNbinsX()+1);
         for (unsigned int cut_i = 1; cut_i <= HdedxMIP1->GetNbinsX()/divide; cut_i++){
            double a = HdedxMIP2->Integral(0, cut_i*divide),
                   e = (fullSig-a)/fullSig;
            ROC[NameIndex]->SetPoint (cut_i-1, 1 - HdedxMIP1->Integral(0, cut_i*divide)/fullBkg, 1 - a/fullSig);
            ROC[NameIndex]->SetPointError (cut_i-1, 0, withErrorBars?sqrt(e*(1-e)/(fullSig-a)):0);
         }
         double a = HdedxMIP2->Integral(0, HdedxMIP2->GetNbinsX()+1),
                e = (fullSig-a)/fullSig;
         ROC[NameIndex]->SetPoint (HdedxMIP1->GetNbinsX(), 1 - HdedxMIP1->Integral(0, HdedxMIP1->GetNbinsX()+1)/fullBkg, 1 - a/fullSig);
         ROC[NameIndex]->SetPointError (HdedxMIP1->GetNbinsX(), 0, withErrorBars?sqrt(e*(1-e)/(fullSig-a)):0);

         ROC[NameIndex]->SetLineColor(Colors[NameIndex]);
         ROC[NameIndex]->SetLineWidth(2);
         ROC[NameIndex]->Draw("same");

         leg->AddEntry (ROC[NameIndex], LegendLabels[NameIndex].c_str(), "L");
      }
      leg->Draw();
      DrawPreliminary();
      SaveCanvas(c1, SaveDir, "Comparison_ROC_" + string(withErrorBars?"WError_":"") + suffix);
      for (size_t NameIndex = 0; NameIndex < ObjNames.size(); NameIndex++)
         delete ROC[NameIndex];
      delete ROC;
      delete leg;
      delete c1;
}

void MakeROCGeneral (TFile* InputFile1, TFile* InputFile2, vector<string> HistoNames, vector<string> LegendLabels, vector<Color_t> Colors, string SaveDir, string suffix, bool withErrorBars, bool Every2ndIsDashed) {

      TCanvas* c1   = new TCanvas ("c1", "c1", 600,600); 
      TLegend* leg  = new TLegend (0.30, 0.15, 0.80, 0.15+0.05*HistoNames.size()/(Every2ndIsDashed?2:1));
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      c1->SetLogx(true);
      TH1D h ("tmp", "tmp", 1, 8E-7, 1);
      h.GetXaxis()->SetTitle("background efficiency");
      h.GetXaxis()->SetNdivisions(5);
      h.GetYaxis()->SetTitle("signal efficiency");
      h.GetYaxis()->SetNdivisions(5);
      h.SetAxisRange (0.65,1.0,"Y");
      h.SetStats(0);
      h.Draw();
      TGraphErrors** ROC = new TGraphErrors* [HistoNames.size()];
      for (size_t NameIndex = 0; NameIndex < HistoNames.size(); NameIndex++)
      {
         int divide = 1;
         TH1D* HdedxMIP1 = (TH1D*) GetObjectFromPath(InputFile1, (HistoNames[NameIndex]).c_str() );
         TH1D* HdedxMIP2 = (TH1D*) GetObjectFromPath(InputFile2, (HistoNames[NameIndex]).c_str() );
         ROC[NameIndex]  = new TGraphErrors(HdedxMIP1->GetNbinsX()/divide + 1);

         double fullBkg  = HdedxMIP1->Integral(0, HdedxMIP1->GetNbinsX()+1),
                fullSig  = HdedxMIP2->Integral(0, HdedxMIP2->GetNbinsX()+1);
         for (unsigned int cut_i = 1; cut_i <= HdedxMIP1->GetNbinsX()/divide; cut_i++){
            double a = HdedxMIP2->Integral(0, cut_i*divide),
                   e = (fullSig-a)/fullSig;
            ROC[NameIndex]->SetPoint (cut_i-1, 1 - HdedxMIP1->Integral(0, cut_i*divide)/fullBkg, 1 - a/fullSig);
            ROC[NameIndex]->SetPointError (cut_i-1, 0, withErrorBars?sqrt(e*(1-e)/(fullSig-a)):0);
         }
         double a = HdedxMIP2->Integral(0, HdedxMIP2->GetNbinsX()+1),
                e = (fullSig-a)/fullSig;
         ROC[NameIndex]->SetPoint (HdedxMIP1->GetNbinsX(), 1 - HdedxMIP1->Integral(0, HdedxMIP1->GetNbinsX()+1)/fullBkg, 1 - a/fullSig);
         ROC[NameIndex]->SetPointError (HdedxMIP1->GetNbinsX(), 0, withErrorBars?sqrt(e*(1-e)/(fullSig-a)):0);

         ROC[NameIndex]->SetLineColor(Colors[NameIndex]);
         if (Every2ndIsDashed && NameIndex&1) {ROC[NameIndex]->SetLineStyle(3); ROC[NameIndex]->SetLineWidth(1);}
         else  {leg->AddEntry (ROC[NameIndex], LegendLabels[NameIndex].c_str(), "L"); ROC[NameIndex]->SetLineWidth(2);}

         ROC[NameIndex]->Draw("same");
      }
      leg->Draw();
      DrawPreliminary();
      SaveCanvas(c1, SaveDir, "Comparison_ROC_" + string(withErrorBars?"WError_":"") + suffix);
      for (size_t NameIndex = 0; NameIndex < HistoNames.size(); NameIndex++)
         delete ROC[NameIndex];
      delete ROC;
      delete leg;
      delete c1;
}

void DrawPreliminary (void){
   TPaveText* T = new TPaveText(0.05, 0.995, 0.95, 0.945, "NDC");
   T->SetTextFont(43);  //give the font size in pixel (instead of fraction)
   T->SetTextSize(21);  //font size
   T->SetBorderSize(0);
   T->SetFillColor(0);
   T->SetFillStyle(0);
   T->SetTextAlign(22);
   T->AddText("#bf{CMS} Preliminary   -   2.74 pb^{-1}   -    #sqrt{s} = 13 TeV");
   T->Draw("same");
}

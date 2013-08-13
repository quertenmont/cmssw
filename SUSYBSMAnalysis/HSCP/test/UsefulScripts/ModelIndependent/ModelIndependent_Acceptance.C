#include <exception>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TDCacheFile.h"
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
#include "TPaveText.h"
#include "TRandom3.h"
#include "TProfile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TPaveText.h"


namespace reco { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm {class TriggerResults; class TriggerResultsByName; class InputTag; class LumiReWeighting;}
namespace reweight{class PoissonMeanShifter;}

#if !defined(__CINT__) && !defined(__MAKECINT__)
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

using namespace fwlite;
using namespace reco;
using namespace susybsm;
using namespace std;
using namespace edm;
using namespace trigger;

#include "../../ICHEP_Analysis/Analysis_Step3.C"

#endif


void  GetGenHSCPBetaCustom(const std::vector<reco::GenParticle>& genColl, double& beta1, bool onlyCharged, int& index){
   beta1=-1;
   for(unsigned int g=0;g<genColl.size();g++){
      if(genColl[g].pt()<5)continue;
      if(genColl[g].status()!=1)continue;
      int AbsPdg=abs(genColl[g].pdgId());
      if(AbsPdg<1000000)continue;
      if(onlyCharged && (AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324))continue; //Skip neutral gluino RHadrons
      if(onlyCharged && (AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333))continue;  //skip neutral stop RHadrons
      if(beta1<0){beta1=genColl[g].p()/genColl[g].energy(); index=g; return;}
   }
}


void ModelIndependent_Acceptance(string MODE="COMPILE", string fileurl="")
{
  if(MODE=="COMPILE") return;

   Event_Weight = 1;
   MaxEntry = -1;


   system("mkdir -p pictures");

   setTDRStyle();
   gStyle->SetPadTopMargin   (0.06);
   gStyle->SetPadBottomMargin(0.15);
   gStyle->SetPadRightMargin (0.03);
   gStyle->SetPadLeftMargin  (0.07);
   gStyle->SetTitleSize(0.04, "XYZ");
   gStyle->SetTitleXOffset(1.1);
   gStyle->SetTitleYOffset(1.35);
   gStyle->SetPalette(1);
   gStyle->SetNdivisions(505,"X");
   TH1::AddDirectory(kTRUE);



   TFile* InputFile = new TFile("pictures/Histos.root");
   unsigned int nM = 6;

   TString histoNames = "Beta_Triggered";
   TH2F* Beta_Triggered1 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT050_060_")+histoNames).Data());
   TH2F* Beta_Triggered2 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT060_070_")+histoNames).Data());
   TH2F* Beta_Triggered3 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT070_080_")+histoNames).Data());
   TH2F* Beta_Triggered4 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT080_100_")+histoNames).Data());
   TH2F* Beta_Triggered5 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT100_150_")+histoNames).Data());
   TH2F* Beta_Triggered6 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT150_999_")+histoNames).Data());

   histoNames = "Beta_Preselected";
   TH2F* Beta_PreSelected1 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT050_060_")+histoNames).Data());
   TH2F* Beta_PreSelected2 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT060_070_")+histoNames).Data());
   TH2F* Beta_PreSelected3 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT070_080_")+histoNames).Data());
   TH2F* Beta_PreSelected4 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT080_100_")+histoNames).Data());
   TH2F* Beta_PreSelected5 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT100_150_")+histoNames).Data());
   TH2F* Beta_PreSelected6 = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT150_999_")+histoNames).Data());

   TH2F** Beta_Offline1 = new TH2F*[nM];
   TH2F** Beta_Offline2 = new TH2F*[nM];
   TH2F** Beta_Offline3 = new TH2F*[nM];
   TH2F** Beta_Offline4 = new TH2F*[nM];
   TH2F** Beta_Offline5 = new TH2F*[nM];
   TH2F** Beta_Offline6 = new TH2F*[nM];
   for(unsigned int Mi=0;Mi<nM;Mi++){
      char tmpHistoName[256];
      sprintf(tmpHistoName, "Beta_SelectedM%i", Mi);
      histoNames = tmpHistoName;
      Beta_Offline1[Mi] = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT050_060_")+histoNames).Data());
      Beta_Offline2[Mi] = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT060_070_")+histoNames).Data());
      Beta_Offline3[Mi] = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT070_080_")+histoNames).Data());
      Beta_Offline4[Mi] = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT080_100_")+histoNames).Data());
      Beta_Offline5[Mi] = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT100_150_")+histoNames).Data());
      Beta_Offline6[Mi] = (TH2F*)GetObjectFromPath(InputFile, (TString("Norm_pT150_999_")+histoNames).Data());
   }

   vector<string> InputFiles;
   InputFiles.push_back(fileurl);

   for(unsigned int s=0;s<InputFiles.size();s++){
      int CutIndex=0;
      TypeMode = 0;
      GlobalMaxEta   = 2.1;
      GlobalMinPt    = 70;
      system("mkdir pictures/");

      double NEvents = 0;
      double NTEvents = 0, NTEventsErr = 0;
      double NPSEvents = 0, NPSEventsErr = 0;
      double* NSEvents = new double[nM];
      double* NSEventsErr = new double[nM];
      for(unsigned int Mi=0;Mi<nM;Mi++){NSEvents[Mi]=0; NSEventsErr[Mi]=0;}

      printf("%s\n",InputFiles[s].c_str());
      vector<string> DataFileName;
      DataFileName.push_back(InputFiles[s]);

      fwlite::ChainEvent treeS(DataFileName);
      printf("Progressing Bar              :0%%       20%%       40%%       60%%       80%%       100%%\n");
      printf("Looping on Tree              :");
      int TreeStep = treeS.size()/50;if(TreeStep==0)TreeStep=1;
      for(Long64_t e=0;e<treeS.size();e++){
         treeS.to(e); 
         if(e%TreeStep==0){printf(".");fflush(stdout);}
         fwlite::Handle< std::vector<reco::GenParticle> > genCollHandle;
         genCollHandle.getByLabel(treeS, "genParticles");
         if(!genCollHandle.isValid()){printf("GenParticle Collection NotFound\n");continue;}
         std::vector<reco::GenParticle> genColl = *genCollHandle;

         NEvents++;
         bool onlyCharged = true;
         double ProbT1 = -1E-9,  ProbErrT1=0;
         double ProbT2 = -1E-9,  ProbErrT2=0;
         double ProbPS1 = -1E-9,  ProbErrPS1=0;
         double ProbPS2 = -1E-9,  ProbErrPS2=0;
         double* ProbO1 = new double[nM];
         double* ProbErrO1 = new double[nM];
         double* ProbO2 = new double[nM];
         double* ProbErrO2 = new double[nM];
         for(unsigned int Mi=0;Mi<nM;Mi++){ProbO1[Mi]=-1E-9; ProbErrO1[Mi]=0; ProbO2[Mi]=-1E-9; ProbErrO2[Mi]=0;}
         for(unsigned int g=0;g<genColl.size();g++){
            if(genColl[g].pt()<5)continue;
            if(genColl[g].status()!=1)continue;
            int AbsPdg=abs(genColl[g].pdgId());
            if(AbsPdg<1000000)continue;
            if(onlyCharged && (AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324))continue; //Skip neutral gluino RHadrons
            if(onlyCharged && (AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333))continue;  //skip neutral stop RHadrons
   //         if(beta1<0){beta1=genColl[g].p()/genColl[g].energy(); index=g; return;}

            TH2* histoTrigger = Beta_Triggered6;
            TH2* histoPreselected = Beta_PreSelected6;
            TH2F** histoOffline = Beta_Offline6;
            if(genColl[g].pt()>= 50 && genColl[g].p()< 60){histoTrigger = Beta_Triggered1; histoPreselected= Beta_PreSelected1  ; histoOffline = Beta_Offline1;}
            if(genColl[g].pt()>= 60 && genColl[g].p()< 70){histoTrigger = Beta_Triggered2; histoPreselected= Beta_PreSelected2  ; histoOffline = Beta_Offline2;}
            if(genColl[g].pt()>= 70 && genColl[g].p()< 80){histoTrigger = Beta_Triggered3; histoPreselected= Beta_PreSelected3  ; histoOffline = Beta_Offline3;}
            if(genColl[g].pt()>= 80 && genColl[g].p()<100){histoTrigger = Beta_Triggered4; histoPreselected= Beta_PreSelected4  ; histoOffline = Beta_Offline4;}
            if(genColl[g].pt()>=100 && genColl[g].p()<150){histoTrigger = Beta_Triggered5; histoPreselected= Beta_PreSelected5  ; histoOffline = Beta_Offline5;}
            if(genColl[g].pt()>=150                      ){histoTrigger = Beta_Triggered6; histoPreselected= Beta_PreSelected6  ; histoOffline = Beta_Offline6;}

            if(genColl[g].pt()<40 || fabs(genColl[g].eta())>2.1)continue;    //why?!

            int BinX = histoTrigger->GetXaxis()->FindBin(genColl[g].p()/genColl[g].energy()); 
            int BinY = histoTrigger->GetYaxis()->FindBin(fabs(genColl[g].eta()) );

            if(ProbT1<0){       ProbT1=histoTrigger->GetBinContent(BinX, BinY);  ProbErrT1=histoTrigger->GetBinError(BinX, BinY);
            }else if(ProbT2<0){ ProbT2=histoTrigger->GetBinContent(BinX, BinY);  ProbErrT2=histoTrigger->GetBinError(BinX, BinY);
            }

	    if(ProbPS1<0){       ProbPS1=histoPreselected->GetBinContent(BinX, BinY);  ProbErrPS1=histoPreselected->GetBinError(BinX, BinY);
            }else if(ProbPS2<0){ ProbPS2=histoPreselected->GetBinContent(BinX, BinY);  ProbErrPS2=histoPreselected->GetBinError(BinX, BinY);
            }

            if(genColl[g].pt()<70 || fabs(genColl[g].eta())>2.1)continue;     //why?!

            for(unsigned int Mi=0;Mi<nM;Mi++){            
               if(ProbO1[Mi]<0){       ProbO1[Mi]=histoOffline[Mi]->GetBinContent(BinX, BinY);  ProbErrO1[Mi]=histoOffline[Mi]->GetBinError(BinX, BinY);
               }else if(ProbO2[Mi]<0){ ProbO2[Mi]=histoOffline[Mi]->GetBinContent(BinX, BinY);  ProbErrO2[Mi]=histoOffline[Mi]->GetBinError(BinX, BinY);
               }
            }
         }
         double EventTProbErr = 0;
         double EventTProb    = 0;      
         double EventPSProbErr = 0;
         double EventPSProb    = 0;      

         if(ProbT1<=0 && ProbT2<=0)continue;
         EventTProb    = ProbT1 + ProbT2 - ProbT1*ProbT2;
         if(EventTProb<0 || EventTProb>1)printf("T %f - %f --> %f\n", ProbT1,ProbT2,EventTProb);
         EventTProbErr = pow(ProbErrT1*(1-ProbT2),2) + pow(ProbErrT2*(1-ProbT1),2);
         NTEvents    += EventTProb;
         NTEventsErr += EventTProbErr;

         if(ProbPS1<=0 && ProbPS2<=0)continue;
         EventPSProb    = ProbPS1 + ProbPS2 - ProbPS1*ProbPS2;
         if(EventPSProb<0 || EventPSProb>1)printf("Pre %f - %f --> %f\n", ProbPS1,ProbPS2,EventPSProb);
         EventPSProbErr = pow(ProbErrPS1*(1-ProbPS2),2) + pow(ProbErrPS2*(1-ProbPS1),2);
         NPSEvents    += EventPSProb;
         NPSEventsErr += EventPSProbErr;

         for(unsigned int Mi=0;Mi<nM;Mi++){
            double EventProb    = EventTProb;
            double EventProbErr = EventTProbErr;

            if(ProbO1[Mi]<=0 && ProbO2[Mi]<=0)continue;
            EventProb    *= (ProbO1[Mi] + ProbO2[Mi] - ProbO1[Mi]*ProbO2[Mi]);
            if(EventProb<0 || EventProb>1)printf("O %f - %f --> %f\n", ProbO1[Mi],ProbO2[Mi],EventProb);
            EventProbErr += pow(ProbErrO1[Mi]*(1-ProbO2[Mi]),2) + pow(ProbErrO2[Mi]*(1-ProbO1[Mi]),2);
   //           EventProb[Mi] = (ProbT1[Mi]*ProbO1[Mi] + ProbT2[Mi]*ProbO2[Mi] - ProbT1[Mi]*ProbO1[Mi]*ProbT2[Mi]*ProbO2[Mi]);
            NSEvents[Mi]     += EventProb;
            NSEventsErr[Mi]  += EventProbErr;
         }
        
      }// end of Event Loop
      FILE* pFile = fopen(MODE.c_str(), "w");
      //FILE* pFile = fopen("testtt.tx", "w");

      for(unsigned int Mi=0;Mi<nM;Mi++){
         printf("%30s M>%3i Efficiencies: Trigger=%6.2f%%+-%6.2f%%  Presel=%6.2f%%+-%6.2f%% Offline=%6.2f%%+-%6.2f%%\n",MODE.c_str(), Mi*100, 100.0*NTEvents/NEvents, 100.0*sqrt(pow(sqrt(NTEventsErr)/NEvents,2) + pow(NTEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NPSEvents/NEvents, 100.0*sqrt(pow(sqrt(NPSEventsErr)/NEvents,2) + pow(NPSEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NSEvents[Mi]/NEvents, 100.0*sqrt(pow(sqrt(NSEventsErr[Mi])/NEvents,2) + pow(NSEvents[Mi]*sqrt(NEvents)/pow(NEvents,2),2)));      
         fprintf(pFile, "%30s M>%3i Efficiencies: Trigger=%6.2f%%+-%6.2f%%  Presel=%6.2f%%+-%6.2f%% Offline=%6.2f%%+-%6.2f%%\n",MODE.c_str(), Mi*100, 100.0*NTEvents/NEvents, 100.0*sqrt(pow(sqrt(NTEventsErr)/NEvents,2) + pow(NTEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NPSEvents/NEvents, 100.0*sqrt(pow(sqrt(NPSEventsErr)/NEvents,2) + pow(NPSEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NSEvents[Mi]/NEvents, 100.0*sqrt(pow(sqrt(NSEventsErr[Mi])/NEvents,2) + pow(NSEvents[Mi]*sqrt(NEvents)/pow(NEvents,2),2)));      
      }
      fclose(pFile);
   }
}



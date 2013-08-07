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
//#include "tdrstyle.C"
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


//#include "../../ICHEP_Analysis/Analysis_PlotFunction.h"
//#include "../../ICHEP_Analysis/Analysis_Samples.h"
//#include "../../ICHEP_Analysis/Analysis_Global.h"
#include "../../ICHEP_Analysis/Analysis_Step3.C"
#endif


bool PassTriggerCustom(const fwlite::ChainEvent& ev){
      edm::TriggerResultsByName tr = ev.triggerResultsByName("MergeHLT");
      if(!tr.isValid())return false;

//      if(tr.accept("HSCPHLTTriggerHtDeDxFilter"))return true;
//      if(tr.accept("HSCPHLTTriggerMetDeDxFilter"))return true;
//      if(tr.accept("HscpPathSingleMu"))return true;
      if(tr.accept("HSCPHLTTriggerMuFilter"))return true;
      return false;
}

bool PassSkimCustom(const fwlite::ChainEvent& ev){
//      return true;
      edm::TriggerResultsByName tr = ev.triggerResultsByName("MergeHLT");
      if(!tr.isValid())return false;

      edm::TriggerResultsByName skim = ev.triggerResultsByName("HLT");
      if(!skim.isValid())return false;
      //for(unsigned int i=0;i<skim.size();i++){
      //   printf("Path %3i %50s --> %1i\n",i, skim.triggerName(i).c_str(),skim.accept(i));
      //}fflush(stdout);
      if((tr.accept("HSCPHLTTriggerMetDeDxFilter") || tr.accept("HSCPHLTTriggerMuDeDxFilter") || tr.accept("HSCPHLTTriggerHtDeDxFilter") || tr.accept("HSCPHLTTriggerMuFilter") ||  tr.accept("HSCPHLTTriggerMetFilter") ||  tr.accept("HSCPHLTTriggerHtFilter") || tr.accept("HSCPHLTTriggerL2MuFilter") || tr.accept("HSCPHLTTriggerCosmicFilter")) && skim.accept("HSCP_step"))return true;
      if((tr.accept("HSCPHLTTriggerMetDeDxFilter") || tr.accept("HSCPHLTTriggerMuDeDxFilter") || tr.accept("HSCPHLTTriggerHtDeDxFilter"))&&skim.accept("HSCPdedx_step"))return true;

      return false;
}

void  GetGenHSCPBetaCustom(const std::vector<reco::GenParticle>& genColl, double& beta1, bool onlyCharged, int& index){
   beta1=-1;
   for(unsigned int g=0;g<genColl.size();g++){
      if(genColl[g].pt()<5)continue;
      if(genColl[g].status()!=1)continue;
      int AbsPdg=abs(genColl[g].pdgId());
      if(AbsPdg<1000000)continue;
      if(onlyCharged && (AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324))continue; //Skip neutral gluino RHadrons
      if(onlyCharged && (AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333))continue;  //skip neutral stop RHadrons

      if(genColl[g].pt()<50 || fabs(genColl[g].eta())>2.0)continue;

      if(beta1<0){beta1=genColl[g].p()/genColl[g].energy(); index=g; return;}
   }
}


void StandardAnalysis_Acceptance(string MODE="COMPILE")
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

   TypeMode = 0;
   CutPt .push_back(95 );CutPt .push_back(95 );CutPt .push_back(95 );
   CutI  .push_back(0.0);CutI  .push_back(0.37);CutI .push_back(0.37);
   CutTOF.push_back(-1);CutTOF.push_back(-1);CutTOF.push_back(-1);
   int CutIndex=0;
//   GlobalMaxV3D=1000000; //should be defined like this because V3D is not well defined on singleParticle event
   system("mkdir pictures/");
//   TFile* OutputHisto = new TFile((string("pictures/") + "/Histos.root").c_str(),"RECREATE");

/*
   double PT[] = {50,75,100,150,999};
   TH2F** Beta_Gen         = new TH2F*[4];
   TH2F** Beta_GenCharged  = new TH2F*[4];
   TH2F** Beta_Triggered   = new TH2F*[4];
   TH2F** Beta_Skimmed     = new TH2F*[4];
   TH2F** Beta_Matched     = new TH2F*[4];
   TH2F** Beta_Preselected = new TH2F*[4];
   TH2F** Beta_SelectedP   = new TH2F*[4];
   TH2F** Beta_SelectedI   = new TH2F*[4];
   TH2F** Beta_SelectedT   = new TH2F*[4];
   TH2F** Beta_SelectedM   = new TH2F*[4];
   for(int PTB=0;PTB<4;PTB++){
      char PTname [255];
      sprintf(PTname,"pT%03i_%03i_",(int)PT[PTB],(int)PT[PTB+1]);
      TString PTName(PTname);

      TString Name;
      Name = PTName+"Beta_Gen"         ; Beta_Gen        [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_Gen         [PTB]->Sumw2();
      Name = PTName+"Beta_GenChaged"   ; Beta_GenCharged [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_GenCharged  [PTB]->Sumw2();
      Name = PTName+"Beta_Triggered"   ; Beta_Triggered  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_Triggered   [PTB]->Sumw2();
      Name = PTName+"Beta_Skimmed"     ; Beta_Skimmed    [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_Skimmed     [PTB]->Sumw2();
      Name = PTName+"Beta_Matched"     ; Beta_Matched    [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_Matched     [PTB]->Sumw2();
      Name = PTName+"Beta_Preselected" ; Beta_Preselected[PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_Preselected [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedP"   ; Beta_SelectedP  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_SelectedP   [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedI"   ; Beta_SelectedI  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_SelectedI   [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedT"   ; Beta_SelectedT  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_SelectedT   [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM"   ; Beta_SelectedM  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 3, 0, 1.5);  Beta_SelectedM   [PTB]->Sumw2();
   }
*/


   vector<string> InputFiles;
   InputFiles.push_back("root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/PPStau_8TeV_M494.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/PPStauPU100.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/PPStauPU156.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/PPStauPU247.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/PPStauPU308.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/PPStauPU494.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/GMSBStauPU100.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/GMSBStauPU156.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/GMSBStauPU247.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/GMSBStauPU308.root");
//   InputFiles.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP/GMSBStauPU494.root");

   for(unsigned int s=0;s<InputFiles.size();s++){


      vector<string> DataFileName;
      printf("%s\n",InputFiles[s].c_str());
      DataFileName.push_back(InputFiles[s]);
   //   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP//PPStau308.root");
   //   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP//PPStauPU494.root");
   //   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_PairHSCP//GMSBStauPU494.root");
   //   DataFileName.push_back("/tmp/querten/SingleStau.root");
   //   DataFileName.push_back("/tmp/querten/PPStau308BX1.root");


      double NEvents = 0;
      double NTEvents = 0;
      double NSEvents = 0;

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

         double HSCPGenBeta1; int genIndex=-1;
         GetGenHSCPBetaCustom(genColl,HSCPGenBeta1,false,genIndex);
         if(genIndex<0 || genColl[genIndex].pt()<45 || fabs(genColl[genIndex].eta())>2.1)continue;
   //      int PTB = 0;while(genColl[genIndex].pt()>PT[PTB+1] && PTB<3)PTB++;

   //      if(HSCPGenBeta1>=0)Beta_Gen[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);        
   //      GetGenHSCPBetaCustom(genColl,HSCPGenBeta1,true,genIndex);
   //      if(HSCPGenBeta1>=0)Beta_GenCharged[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

         if(!PassTriggerCustom(treeS) )continue;
   //      if(HSCPGenBeta1>=0)Beta_Triggered[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);


//         if(!PassSkimCustom(treeS) )continue;
   //      if(HSCPGenBeta1>=0)Beta_Skimmed[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         NTEvents++;

         fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
         hscpCollHandle.getByLabel(treeS,"HSCParticleProducer");
         if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
         const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

         fwlite::Handle<DeDxDataValueMap> dEdxSCollH;
         dEdxSCollH.getByLabel(treeS, dEdxS_Label.c_str());
         if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");continue;}

         fwlite::Handle<DeDxDataValueMap> dEdxMCollH;
         dEdxMCollH.getByLabel(treeS, dEdxM_Label.c_str());
         if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

         fwlite::Handle<MuonTimeExtraMap> TOFCollH;
         TOFCollH.getByLabel(treeS, "muontiming",TOF_Label.c_str());
         if(!TOFCollH.isValid()){printf("Invalid TOF collection\n");continue;}

         fwlite::Handle<MuonTimeExtraMap> TOFDTCollH;
         TOFDTCollH.getByLabel(treeS, "muontiming",TOFdt_Label.c_str());
         if(!TOFDTCollH.isValid()){printf("Invalid DT TOF collection\n");continue;}

         fwlite::Handle<MuonTimeExtraMap> TOFCSCCollH;
         TOFCSCCollH.getByLabel(treeS, "muontiming",TOFcsc_Label.c_str());
         if(!TOFCSCCollH.isValid()){printf("Invalid CSC TOF collection\n");continue;}

         bool Passed = false;
         for(unsigned int c=0;c<hscpColl.size();c++){
            susybsm::HSCParticle hscp  = hscpColl[c];
            reco::MuonRef  muon  = hscp.muonRef();
            reco::TrackRef track = hscp.trackRef();
            if(track.isNull())continue;

            int ClosestGen;
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)continue;
   //         if(HSCPGenBeta1>=0)Beta_Matched[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

            const DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
            const DeDxData& dedxMObj  = dEdxMCollH->get(track.key());
            const reco::MuonTimeExtra* tof = NULL;
            const reco::MuonTimeExtra* dttof = NULL;
            const reco::MuonTimeExtra* csctof = NULL;
            if(TypeMode==2 && !hscp.muonRef().isNull()){ tof  = &TOFCollH->get(hscp.muonRef().key()); dttof  = &TOFDTCollH->get(hscp.muonRef().key()); csctof  = &TOFCSCCollH->get(hscp.muonRef().key()); }


            if(!PassPreselection(hscp,  &dedxSObj, &dedxMObj, tof, dttof, csctof, treeS,           NULL, HSCPGenBeta1))continue;        
   //         if(HSCPGenBeta1>=0)Beta_Preselected[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
            if(!PassSelection   (hscp,  &dedxSObj, &dedxMObj, tof, treeS, CutIndex+0, NULL, false, HSCPGenBeta1))continue;    
   //         if(HSCPGenBeta1>=0)Beta_SelectedP[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
            if(!PassSelection   (hscp,  &dedxSObj, &dedxMObj, tof, treeS, CutIndex+1, NULL, false, HSCPGenBeta1))continue;
   //         if(HSCPGenBeta1>=0)Beta_SelectedI[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
            if(!PassSelection   (hscp,  &dedxSObj, &dedxMObj, tof, treeS, CutIndex+2, NULL, false, HSCPGenBeta1))continue;
   //         if(HSCPGenBeta1>=0)Beta_SelectedT[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

            double Mass     = GetMass(track->p(),dedxMObj.dEdx(), true);
            //         double MassTOF  = -1; if(tof)MassTOF = GetTOFMass(track->p(),tof->inverseBeta());
            //         double MassComb = Mass;if(tof)MassComb=GetMassFromBeta(track->p(), (GetIBeta(dedxMObj.dEdx()) + (1/tof->inverseBeta()))*0.5 ) ;

            if(Mass<120)continue;
//            if(Mass<220)continue;
            Passed = true;
   //         if(HSCPGenBeta1>=0)Beta_SelectedM[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

         } // end of Track Loop 
         if(Passed)NSEvents+=1.0;

      }// end of Event Loop
      printf("\nEfficiency is %6.2f+-%6.2f%% - %6.2f+-%6.2f%%\n",100.0*NTEvents/NEvents, 100.0*sqrt(pow(sqrt(NTEvents)/NEvents,2)+pow(NTEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NSEvents/NEvents, 100.0*sqrt(pow(sqrt(NSEvents)/NEvents,2)+pow(NSEvents*sqrt(NEvents)/pow(NEvents,2),2)));
   }

/*
   for(int i=0;i<4;i++){
//      Beta_GenCharged  [i]->Divide(Beta_Gen[i]);
//      Beta_Triggered   [i]->Divide(Beta_Gen[i]);
      Beta_Skimmed     [i]->Divide(Beta_Gen[i]);
      Beta_Matched     [i]->Divide(Beta_Gen[i]);
      Beta_Preselected [i]->Divide(Beta_Gen[i]);
      Beta_SelectedP   [i]->Divide(Beta_Gen[i]);
      Beta_SelectedI   [i]->Divide(Beta_Gen[i]);
      Beta_SelectedT   [i]->Divide(Beta_Gen[i]);
      Beta_SelectedM   [i]->Divide(Beta_Gen[i]);

//      Beta_Skimmed     [i]->Divide(Beta_Triggered[i]);
//      Beta_Matched     [i]->Divide(Beta_Triggered[i]);
//      Beta_Preselected [i]->Divide(Beta_Triggered[i]);
//      Beta_SelectedP   [i]->Divide(Beta_Triggered[i]);
//      Beta_SelectedI   [i]->Divide(Beta_Triggered[i]);
//      Beta_SelectedT   [i]->Divide(Beta_Triggered[i]);
//      Beta_SelectedM   [i]->Divide(Beta_Triggered[i]);

      Beta_GenCharged  [i]->Divide(Beta_Gen[i]);
      Beta_Triggered   [i]->Divide(Beta_Gen[i]);


//      Beta_Gen         [i]->Divide(Beta_Gen[i]);
   }
*/

//   OutputHisto->Write();
//   OutputHisto->Close();  
}



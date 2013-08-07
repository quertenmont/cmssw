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
//#include "../../ICHEP_Analysis/Analysis_Global.h"
//#include "../../ICHEP_Analysis/Analysis_CommonFunction.h"
//#include "../../ICHEP_Analysis/Analysis_PlotFunction.h"
//#include "../../ICHEP_Analysis/Analysis_PlotStructure.h"
//#include "../../ICHEP_Analysis/Analysis_Samples.h"
//#include "tdrstyle.C"
#endif

bool PassTriggerCustom(const fwlite::ChainEvent& ev){
      edm::TriggerResultsByName tr = ev.triggerResultsByName("MergeHLT");
      if(!tr.isValid())return false;

//      if(tr.accept("HSCPHLTTriggerHtDeDxFilter"))return true;
//      if(tr.accept("HSCPHLTTriggerMetDeDxFilter"))return true;
//      if(tr.accept("HSCPHLTTriggerMuDeDxFilter"))return true;
      if(tr.accept("HSCPHLTTriggerMuFilter"))return true;
      return false;
}

bool PassSkimCustom(const fwlite::ChainEvent& ev){
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
//      if(AbsPdg!=13)     continue;
      if(AbsPdg<1000000)continue;
//      if(onlyCharged && (AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324))continue; //Skip neutral gluino RHadrons
//      if(onlyCharged && (AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333))continue;  //skip neutral stop RHadrons
      if(beta1<0){beta1=genColl[g].p()/genColl[g].energy(); index=g; return;}
   }
}


void GetEfficiencyMaps(string MODE="COMPILE", string fileurl="")
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
   GlobalMaxEta   = 2.1;
   GlobalMinPt    = 70;
   CutPt .push_back(70 );CutPt .push_back(70 );CutPt .push_back(70 );
   CutI  .push_back(0.0);CutI  .push_back(0.40);CutI .push_back(0.40);
   CutTOF.push_back(-1);CutTOF.push_back(-1);CutTOF.push_back(-1);
   int CutIndex=0;
//   GlobalMaxV3D=1000000; //should be defined like this because V3D is not well defined on singleParticle event
   system("mkdir pictures/");
//   TFile* OutputHisto = new TFile((string("pictures/") + "/Histos_" + MODE + ".root").c_str(),"RECREATE");
   TFile* OutputHisto = new TFile(MODE.c_str(),"RECREATE");


   double PT[] = {50,60,70,80,100,150,999};
   TH2F** Beta_Gen         = new TH2F*[6];
   TH2F** Beta_GenCharged  = new TH2F*[6];
   TH2F** Beta_Triggered   = new TH2F*[6];
   TH2F** Beta_Skimmed     = new TH2F*[6];
   TH2F** Beta_Matched     = new TH2F*[6];
   TH2F** Beta_Preselected = new TH2F*[6];
   TH2F** Beta_SelectedP   = new TH2F*[6];
   TH2F** Beta_SelectedI   = new TH2F*[6];
   TH2F** Beta_SelectedT   = new TH2F*[6];
   TH2F** Beta_SelectedM0  = new TH2F*[6];
   TH2F** Beta_SelectedM1  = new TH2F*[6];
   TH2F** Beta_SelectedM2  = new TH2F*[6];
   TH2F** Beta_SelectedM3  = new TH2F*[6];
   TH2F** Beta_SelectedM4  = new TH2F*[6];
   TH2F** Beta_SelectedM5  = new TH2F*[6];

   TString Name;
   for(int PTB=0;PTB<6;PTB++){
      char PTname [255];
      sprintf(PTname,"pT%03i_%03i_",(int)PT[PTB],(int)PT[PTB+1]);
      TString PTName(PTname);

      Name = PTName+"Beta_Gen"         ; Beta_Gen        [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_Gen         [PTB]->Sumw2();
      Name = PTName+"Beta_GenChaged"   ; Beta_GenCharged [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_GenCharged  [PTB]->Sumw2();
      Name = PTName+"Beta_Triggered"   ; Beta_Triggered  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_Triggered   [PTB]->Sumw2();
      Name = PTName+"Beta_Skimmed"     ; Beta_Skimmed    [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_Skimmed     [PTB]->Sumw2();
      Name = PTName+"Beta_Matched"     ; Beta_Matched    [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_Matched     [PTB]->Sumw2();
      Name = PTName+"Beta_Preselected" ; Beta_Preselected[PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_Preselected [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedP"   ; Beta_SelectedP  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedP   [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedI"   ; Beta_SelectedI  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedI   [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedT"   ; Beta_SelectedT  [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedT   [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM0"  ; Beta_SelectedM0 [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedM0  [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM1"  ; Beta_SelectedM1 [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedM1  [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM2"  ; Beta_SelectedM2 [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedM2  [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM3"  ; Beta_SelectedM3 [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedM3  [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM4"  ; Beta_SelectedM4 [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedM4  [PTB]->Sumw2();
      Name = PTName+"Beta_SelectedM5"  ; Beta_SelectedM5 [PTB] = new TH2F(Name, Name+";#beta;|#eta|", 20, 0,  1, 8, 0, 2.1);  Beta_SelectedM5  [PTB]->Sumw2();
   }


   vector<string> DataFileName;
   DataFileName.push_back(fileurl);
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau100.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau126.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau156.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau200.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau247.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau308.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau370.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau432.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau494.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau557.root");



//   DataFileName.push_back("/afs/cern.ch/user/q/querten/workspace/public/13_06_01_2012_HSCP_Analysis_Archive/SingleHSCP_Gen/CMSSW_5_2_5_patch3/src/GEN/SingleStauPythiaMuon/mu_1_0.root");
//   DataFileName.push_back("/afs/cern.ch/user/q/querten/workspace/public/13_06_01_2012_HSCP_Analysis_Archive/SingleHSCP_Gen/EDM/CMSSW_5_3_2_patch4/src/SUSYBSMAnalysis/HSCP/test/BuildHSCParticles/Signals/MERGE/outputs/stau100_09999_EDM.root");
//   DataFileName.push_back("root://eoscms//eos/cms/store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/PPStau_8TeV_M100.root");
//     DataFileName.push_back("root://eoscms//eos/cms/store/user/querten/12_07_03_SingleHSCP/PPStauPU100.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_100.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_126.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_156.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_200.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_247.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_308.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_370.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_432.root");
//   DataFileName.push_back("rfio:/castor/cern.ch/user/q/querten/12_07_03_SingleHSCP/SingleStau_494.root");


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

      double HSCPGenBeta1; int genIndex=-1;
      GetGenHSCPBetaCustom(genColl,HSCPGenBeta1,false,genIndex);
      if(genIndex<0 || genColl[genIndex].pt()<45 || fabs(genColl[genIndex].eta())>2.1)continue;
      int PTB = 0;while(genColl[genIndex].pt()>PT[PTB+1] && PTB<5)PTB++;

      if(HSCPGenBeta1>=0)Beta_Gen[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);        
      GetGenHSCPBetaCustom(genColl,HSCPGenBeta1,true,genIndex);
      if(HSCPGenBeta1>=0)Beta_GenCharged[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

      if(!PassTriggerCustom(treeS) )continue;
      if(HSCPGenBeta1>=0)Beta_Triggered[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

//      if(!PassSkimCustom(treeS) )continue;
      if(HSCPGenBeta1>=0)Beta_Skimmed[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

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

      for(unsigned int c=0;c<hscpColl.size();c++){
         susybsm::HSCParticle hscp  = hscpColl[c];
         reco::MuonRef  muon  = hscp.muonRef();
         reco::TrackRef track = hscp.trackRef();
         if(track.isNull())continue;

         int ClosestGen;
         if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)continue;
         if(HSCPGenBeta1>=0)Beta_Matched[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

         const DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
         const DeDxData& dedxMObj  = dEdxMCollH->get(track.key());
         const reco::MuonTimeExtra* tof = NULL;
         const reco::MuonTimeExtra* dttof = NULL;
         const reco::MuonTimeExtra* csctof = NULL;
         if(TypeMode==2 && !hscp.muonRef().isNull()){ tof  = &TOFCollH->get(hscp.muonRef().key()); dttof  = &TOFDTCollH->get(hscp.muonRef().key()); csctof  = &TOFCSCCollH->get(hscp.muonRef().key()); }

         if(genColl[g].pt()<40 || fabs(genColl[g].eta())>2.1)continue;

         if(!PassPreselection(hscp,  &dedxSObj, &dedxMObj, tof, dttof, csctof, treeS,           NULL, HSCPGenBeta1))continue;        
         if(HSCPGenBeta1>=0)Beta_Preselected[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

          if(genColl[g].pt()<70 || fabs(genColl[g].eta())>2.1)continue;

         if(!PassSelection   (hscp,  &dedxSObj, &dedxMObj, tof, treeS, CutIndex+0, NULL, false, HSCPGenBeta1))continue;    
         if(HSCPGenBeta1>=0)Beta_SelectedP[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(!PassSelection   (hscp,  &dedxSObj, &dedxMObj, tof, treeS, CutIndex+1, NULL, false, HSCPGenBeta1))continue;
         if(HSCPGenBeta1>=0)Beta_SelectedI[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(!PassSelection   (hscp,  &dedxSObj, &dedxMObj, tof, treeS, CutIndex+2, NULL, false, HSCPGenBeta1))continue;
         if(HSCPGenBeta1>=0)Beta_SelectedT[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);

         double Mass     = GetMass(track->p(),dedxMObj.dEdx(), true);
         //         double MassTOF  = -1; if(tof)MassTOF = GetTOFMass(track->p(),tof->inverseBeta());
         //         double MassComb = Mass;if(tof)MassComb=GetMassFromBeta(track->p(), (GetIBeta(dedxMObj.dEdx()) + (1/tof->inverseBeta()))*0.5 ) ;

         if(HSCPGenBeta1<0)continue;
         if(Mass<0)continue;
         Beta_SelectedM0[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(Mass<100)continue;
         Beta_SelectedM1[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(Mass<200)continue;
         Beta_SelectedM2[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(Mass<300)continue;
         Beta_SelectedM3[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(Mass<400)continue;
         Beta_SelectedM4[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
         if(Mass<500)continue;
         Beta_SelectedM5[PTB]->Fill(HSCPGenBeta1, fabs(genColl[genIndex].eta()), Event_Weight);
      } // end of Track Loop 


   }// end of Event Loop

   OutputHisto->Write();
   OutputHisto->Close();  
}



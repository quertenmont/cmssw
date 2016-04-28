
#include <exception>
#include <vector>
#include <algorithm>

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
#include "TCutG.h"
#include "TGraphAsymmErrors.h"
#include "TProfile.h"
#include "TPaveText.h"


namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}

#if !defined(__CINT__) && !defined(__MAKECINT__)
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/DeDxHitInfo.h"

using namespace fwlite;
using namespace reco;
using namespace susybsm;
using namespace std;
using namespace edm;
using namespace trigger;


#include "../../AnalysisCode/Analysis_Step1_EventLoop.C"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"


#endif


double DistToHSCP (const reco::TrackRef& track, const std::vector<reco::GenParticle>& genColl);
bool isCompatibleWithCosmic (const reco::TrackRef& track, const std::vector<reco::Vertex>& vertexColl);


const double P_Min               = 1   ;
const double P_Max               = 16  ; // 1 + 14 + 1; final one is for pixel!
const int    P_NBins             = 15  ; // 15th bin = pixel; 0 is underflow
const double Path_Min            = 0.2 ;
const double Path_Max            = 1.6 ;
const int    Path_NBins          = 42  ;
const double Charge_Min          = 0   ;
const double Charge_Max          = 5000;
const int    Charge_NBins        = 500 ;

struct dEdxStudyObj
{
   string Name;
   bool isDiscrim;
   bool isEstim;
   bool isHit;

   bool usePixel;
   bool useStrip;

   bool mustBeInside;
   bool removeCosmics;
   bool correctFEDSat;
   bool useClusterCleaning;

   int crossTalkInvAlgo; // 0  -- do not use crossTalkInversion
                         // 1  -- use existing algorithm developed by Claude

   TH3D* Charge_Vs_Path;
   TH1D* HdedxMIP;
   TH1D* HdedxMIP_U;
   TH1D* HdedxMIP4;
   TH1D* HdedxMIP8;
   TH1D* HdedxMIP12;
   TH2D* HdedxVsP;
   TH2D* HdedxVsPSyst;
//   TH2D* HdedxVsQP;
//   TProfile2D* HdedxVsP_NS;
   TProfile* HdedxVsPProfile;
   TProfile* HdedxVsPProfile_U;
   TProfile* HdedxVsEtaProfile;
   TProfile* HdedxVsNOH;
   TProfile* HNOMVsdEdxProfile;
   TH2D* HdedxVsEta;
   TH2D* HNOMVsdEdx;
   TProfile* HNOSVsEtaProfile;
   TProfile* HNOMVsEtaProfile;
   TProfile* HNOMSVsEtaProfile;
   TH1D* HMass;
   TH1D* HMassHSCP;
   TH1D* HP;
   TH1D* HHit; 
   TProfile* Charge_Vs_FS[16];
   TH2D* Charge_Vs_XYH[16];
   TH2D* Charge_Vs_XYL[16];
   TH2D* Charge_Vs_XYHN[16];
   TH2D* Charge_Vs_XYLN[16];


   TH3F* dEdxTemplates = NULL;
   std::unordered_map<unsigned int,double>* TrackerGains = NULL;

   dEdxStudyObj(string Name_, int type_, int subdet_, TH3F* dEdxTemplates_=NULL, std::unordered_map<unsigned int,double>* TrackerGains_=NULL, bool mustBeInside_=false, bool removeCosmics_=false, bool correctFEDSat_=false, bool useClusterCleaning_=false, int crossTalkInvAlgo_=0){
      Name = Name_;

      if     (type_==0){ isHit=true;  isEstim= false; isDiscrim = false;}
      else if(type_==1){ isHit=false; isEstim= true;  isDiscrim = false;}
      else if(type_==2){ isHit=false; isEstim= false; isDiscrim = true; }
      else             { isHit=false; isEstim= false; isDiscrim = false;}

           if(subdet_==1){ usePixel = true;  useStrip = false;}
      else if(subdet_==2){ usePixel = false; useStrip = true; }
      else               { usePixel = true;  useStrip = true; }

      dEdxTemplates      = dEdxTemplates_;
      TrackerGains       = TrackerGains_;
      mustBeInside       = mustBeInside_;
      removeCosmics      = removeCosmics_; 
      correctFEDSat      = correctFEDSat_;
      useClusterCleaning = useClusterCleaning_;
      crossTalkInvAlgo   = crossTalkInvAlgo_;

      string HistoName;
      //HitLevel plot      
      if(isHit){ 
         HistoName = Name + "_Hit";               HHit                  = new TH1D(      HistoName.c_str(), HistoName.c_str(),  200, 0, 20); 
         if(usePixel && useStrip){ 
            HistoName = Name + "_ChargeVsPath";      Charge_Vs_Path        = new TH3D(      HistoName.c_str(), HistoName.c_str(), P_NBins, P_Min, P_Max, Path_NBins, Path_Min, Path_Max, Charge_NBins, Charge_Min, Charge_Max);
            for(unsigned int g=0;g<16;g++){
               char Id[255]; sprintf(Id, "%02i", g);
               HistoName = Name + "_ChargeVsFS"+Id;       Charge_Vs_FS[g]       = new TProfile  ( HistoName.c_str(), HistoName.c_str(),  769, 0, 769);
               HistoName = Name + "_ChargeVsXYH"+Id;      Charge_Vs_XYH[g]      = new TH2D      ( HistoName.c_str(), HistoName.c_str(),  250, -15, 15, 250, -15, 15);
               HistoName = Name + "_ChargeVsXYL"+Id;      Charge_Vs_XYL[g]      = new TH2D      ( HistoName.c_str(), HistoName.c_str(),  250, -15, 15, 250, -15, 15);
               HistoName = Name + "_ChargeVsXYHN"+Id;     Charge_Vs_XYHN[g]     = new TH2D      ( HistoName.c_str(), HistoName.c_str(),  250, -1.5, 1.5, 250, -1.5, 1.5);
               HistoName = Name + "_ChargeVsXYLN"+Id;     Charge_Vs_XYLN[g]     = new TH2D      ( HistoName.c_str(), HistoName.c_str(),  250, -1.5, 1.5, 250, -1.5, 1.5);
            }
         }
      }

      //Track Level plots
      if(isEstim || isDiscrim){
         HistoName = Name + "_MIP";               HdedxMIP              = new TH1D(      HistoName.c_str(), HistoName.c_str(), 1000, 0, isDiscrim?1.0:25);
         HistoName = Name + "_MIP_U";             HdedxMIP_U            = new TH1D(      HistoName.c_str(), HistoName.c_str(), 1000, 0, isDiscrim?1.0:25);
         HistoName = Name + "_MIP4";              HdedxMIP4             = new TH1D(      HistoName.c_str(), HistoName.c_str(), 1000, 0, isDiscrim?1.0:25);
         HistoName = Name + "_MIP8";              HdedxMIP8             = new TH1D(      HistoName.c_str(), HistoName.c_str(), 1000, 0, isDiscrim?1.0:25);
         HistoName = Name + "_MIP12";             HdedxMIP12            = new TH1D(      HistoName.c_str(), HistoName.c_str(), 1000, 0, isDiscrim?1.0:25);
         HistoName = Name + "_dedxVsP";           HdedxVsP              = new TH2D(      HistoName.c_str(), HistoName.c_str(),  500, 0, 10,1000,0, isDiscrim?1.0:15);
         HistoName = Name + "_dedxVsPSyst";       HdedxVsPSyst          = new TH2D(      HistoName.c_str(), HistoName.c_str(),  500, 0, 10,1000,0, isDiscrim?1.0:15);
//       HistoName = Name + "_dedxVsQP";          HdedxVsQP             = new TH2D(      HistoName.c_str(), HistoName.c_str(), 6000, -30, 30,1500,0, isDiscrim?1.0:15);
//       HistoName = Name + "_dedxVsP_NS";        HdedxVsP_NS           = new TProfile2D(HistoName.c_str(), HistoName.c_str(), 3000, 0, 30,1500,0, isDiscrim?1.0:15);
         HistoName = Name + "_Profile";           HdedxVsPProfile       = new TProfile(  HistoName.c_str(), HistoName.c_str(),   50, 0,100);
         HistoName = Name + "_Profile_U";         HdedxVsPProfile_U     = new TProfile(  HistoName.c_str(), HistoName.c_str(),   50, 0,100);
         HistoName = Name + "_Eta";               HdedxVsEtaProfile     = new TProfile(  HistoName.c_str(), HistoName.c_str(),   60,-3,  3);
         HistoName = Name + "_dedxVsNOH";         HdedxVsNOH            = new TProfile(  HistoName.c_str(), HistoName.c_str(),   80, 0, 80);
         HistoName = Name + "_NOMVsdEdxProfile";  HNOMVsdEdxProfile     = new TProfile(  HistoName.c_str(), HistoName.c_str(),   200, 0, isDiscrim?1.0:25);
         HistoName = Name + "_NOMVsdEdx";         HNOMVsdEdx            = new TH2D(      HistoName.c_str(), HistoName.c_str(), 200, 0, isDiscrim?1.0:25, 30, 0, 30);
         HistoName = Name + "_Eta2D";             HdedxVsEta            = new TH2D(      HistoName.c_str(), HistoName.c_str(),   60,-3,  3, 100,0, isDiscrim?1.0:5);
         HistoName = Name + "_NOS";               HNOSVsEtaProfile      = new TProfile(  HistoName.c_str(), HistoName.c_str(),   60,-3,  3);
         HistoName = Name + "_NOM";               HNOMVsEtaProfile      = new TProfile(  HistoName.c_str(), HistoName.c_str(),   60,-3,  3);
         HistoName = Name + "_NOMS";              HNOMSVsEtaProfile     = new TProfile(  HistoName.c_str(), HistoName.c_str(),   60,-3,  3);
         HistoName = Name + "_P";                 HP                    = new TH1D(      HistoName.c_str(), HistoName.c_str(),   50, 0, 100);  
      }

      //estimator plot only
      if(isEstim){
         HistoName = Name + "_Mass";              HMass                 = new TH1D(      HistoName.c_str(), HistoName.c_str(),  250, 0, 10);
         HistoName = Name + "_MassHSCP";          HMassHSCP             = new TH1D(      HistoName.c_str(), HistoName.c_str(),  300, 0, 3000);
      }

   }
};

void DeDxStudy(string DIRNAME="COMPILE", string INPUT="dEdx.root", string OUTPUT="out.root")
{
  if(DIRNAME=="COMPILE") return;

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

   bool isData   = !(INPUT.find("MC")!=string::npos);
   bool isSignal = false;
   bool removeCosmics = true;
   std::vector<string> FileName;
   if(INPUT.find(".root")<std::string::npos){
      char* pch=strtok(&INPUT[0],",");
      while (pch!=NULL){
         FileName.push_back(pch);    
         pch=strtok(NULL,",");
      }
   }else{
      string SampleId = INPUT;
      InitBaseDirectory();
      GetSampleDefinition(samples , DIRNAME+"/../../AnalysisCode/Analysis_Samples.txt");
      stSample& sample = samples[JobIdToIndex(SampleId, samples)];
      isData   = (sample.Type==0);
      isSignal = (sample.Type==2);
      GetInputFiles(sample, BaseDirectory, FileName, 0);
   }

   TH3F* dEdxTemplates      = NULL;
   TH3F* dEdxTemplatesIn    = NULL;
   TH3F* dEdxTemplatesInc   = NULL;
   TH3F* dEdxTemplatesCCC   = NULL;
   TH3F* dEdxTemplatesCC    = NULL;
   TH3F* dEdxTemplatesCI    = NULL;
   double dEdx_U [2] = {1.0, 1.0};

   if(isData){   
         dEdxSF [0] = 1.00000;
         dEdxSF [1] = 1.21836;
//       dEdxTemplates    = loadDeDxTemplate(DIRNAME + "/../../../data/Data13TeV_Deco_SiStripDeDxMip_3D_Rcd.root", true);
//       dEdxTemplatesInc = loadDeDxTemplate(DIRNAME + "/../../../data/Data13TeV_Deco_SiStripDeDxMip_3D_Rcd.root", false);
         dEdxTemplates      = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_Run251252.root"           , true);
         dEdxTemplatesIn    = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_Run251252.root"    , true);
         dEdxTemplatesInc   = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_Run251252.root"           , false);
         dEdxTemplatesCC    = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_CC_Run251252.root" , true);
         dEdxTemplatesCI    = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_CI_Run251252.root" , true);
         dEdxTemplatesCCC   = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_CCC_Run251252.root", true);
   }else{
         dEdxSF [0] = 1.09708;
         dEdxSF [1] = 1.01875;
//       dEdxTemplates    = loadDeDxTemplate(DIRNAME + "/../../../data/MC13TeV_Deco_SiStripDeDxMip_3D_Rcd.root", true);
//       dEdxTemplatesInc = loadDeDxTemplate(DIRNAME + "/../../../data/MC13TeV_Deco_SiStripDeDxMip_3D_Rcd.root", false); 
         dEdxTemplates      = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_MCMinBias.root"           , true);
         dEdxTemplatesIn    = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_MCMinBias.root"    , true);
         dEdxTemplatesInc   = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_MCMinBias.root"           , false);
         dEdxTemplatesCC    = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_CC_MCMinBias.root" , true);
         dEdxTemplatesCI    = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_CI_MCMinBias.root" , true);
         dEdxTemplatesCCC   = loadDeDxTemplate (DIRNAME+"/Templates/dEdxTemplate_hit_SP_in_noC_CCC_MCMinBias.root", true);
   }

   std::unordered_map<unsigned int,double> TrackerGains;
   LoadDeDxCalibration(TrackerGains, DIRNAME+"/../../../data/Data13TeVGains.root");

   TFile* OutputHisto = new TFile((OUTPUT).c_str(),"RECREATE");  //File must be opened before the histogram are created

   std::vector<dEdxStudyObj*> results;
   results.push_back(new dEdxStudyObj("hit_PO"      , 0, 1, NULL            , NULL) );
   results.push_back(new dEdxStudyObj("hit_SO_raw"  , 0, 2, NULL            , NULL) );
   results.push_back(new dEdxStudyObj("hit_SO"      , 0, 2, NULL            , &TrackerGains) );
   results.push_back(new dEdxStudyObj("hit_SP"      , 0, 3, NULL            , &TrackerGains) );
   results.push_back(new dEdxStudyObj("hit_SO_in"   , 0, 2, NULL            , &TrackerGains, true) );
   results.push_back(new dEdxStudyObj("hit_SP_in_noC", 0, 3, NULL           , &TrackerGains, true) );
   results.push_back(new dEdxStudyObj("hit_SP_in_noC_CI" , 0, 3, NULL       , &TrackerGains, true, true, false, false, 1) );
   results.push_back(new dEdxStudyObj("hit_SP_in_noC_CC" , 0, 3, NULL       , &TrackerGains, true, true, false, true,  0) );
   results.push_back(new dEdxStudyObj("hit_SP_in_noC_CCC", 0, 3, NULL       , &TrackerGains, true, true, false, true,  1) );
   results.push_back(new dEdxStudyObj("harm2_PO_raw", 1, 1, NULL            , NULL) );
   results.push_back(new dEdxStudyObj("harm2_SO"    , 1, 2, NULL            , &TrackerGains) );
   results.push_back(new dEdxStudyObj("harm2_SO_FS" , 1, 2, NULL            , &TrackerGains, false, false, true) );
   results.push_back(new dEdxStudyObj("harm2_SO_in" , 1, 2, NULL            , &TrackerGains, true) );
   results.push_back(new dEdxStudyObj("harm2_SO_in_noC"       , 1, 2, NULL  , &TrackerGains, true, true) );
   results.push_back(new dEdxStudyObj("harm2_SO_in_noC_CI"    , 1, 2, NULL  , &TrackerGains, true, true, false, false, 1) );
   results.push_back(new dEdxStudyObj("harm2_SO_in_noC_CC"    , 1, 2, NULL  , &TrackerGains, true, true, false, true,  0) );
   results.push_back(new dEdxStudyObj("harm2_SO_in_noC_CCC"   , 1, 2, NULL  , &TrackerGains, true, true, false, true,  1) );
   results.push_back(new dEdxStudyObj("harm2_SP"    , 1, 3, NULL            , &TrackerGains) );
   results.push_back(new dEdxStudyObj("harm2_SP_in" , 1, 3, NULL            , &TrackerGains, true) );
   results.push_back(new dEdxStudyObj("harm2_SP_in_noC"       , 1, 3, NULL  , &TrackerGains, true, true) );
   results.push_back(new dEdxStudyObj("harm2_SP_in_noC_CI"    , 1, 3, NULL  , &TrackerGains, true, true, false, false, 1) );
   results.push_back(new dEdxStudyObj("harm2_SP_in_noC_CC"    , 1, 3, NULL  , &TrackerGains, true, true, false, true,  0) );
   results.push_back(new dEdxStudyObj("harm2_SP_in_noC_CCC"   , 1, 3, NULL  , &TrackerGains, true, true, false, true,  1) );
   results.push_back(new dEdxStudyObj("Ias_PO"      , 2, 1, dEdxTemplates   , NULL) );
   results.push_back(new dEdxStudyObj("Ias_SO_inc"  , 2, 2, dEdxTemplatesInc, NULL) );
   results.push_back(new dEdxStudyObj("Ias_SO"      , 2, 2, dEdxTemplates   , NULL) );
   results.push_back(new dEdxStudyObj("Ias_SO_in"   , 2, 2, dEdxTemplatesIn , NULL, true) );
   results.push_back(new dEdxStudyObj("Ias_SO_in_noC"         , 2, 2, dEdxTemplatesIn , NULL, true, true) );
   results.push_back(new dEdxStudyObj("Ias_SO_in_noC_CI"      , 2, 2, dEdxTemplatesCI , NULL, true, true, false, false, 1) );
   results.push_back(new dEdxStudyObj("Ias_SO_in_noC_CC"      , 2, 2, dEdxTemplatesCC , NULL, true, true, false, true,  0) );
   results.push_back(new dEdxStudyObj("Ias_SO_in_noC_CCC"     , 2, 2, dEdxTemplatesCCC, NULL, true, true, false, true,  1) );
   results.push_back(new dEdxStudyObj("Ias_SP_inc"  , 2, 3, dEdxTemplatesInc, NULL) );
   results.push_back(new dEdxStudyObj("Ias_SP"      , 2, 3, dEdxTemplates   , NULL) );
   results.push_back(new dEdxStudyObj("Ias_SP_in"   , 2, 3, dEdxTemplatesIn , NULL, true) );
   results.push_back(new dEdxStudyObj("Ias_SP_in_noC"         , 2, 3, dEdxTemplatesIn , NULL, true, true) );
   results.push_back(new dEdxStudyObj("Ias_SP_in_noC_CI"      , 2, 3, dEdxTemplatesCI , NULL, true, true, false, false, 1) );
   results.push_back(new dEdxStudyObj("Ias_SP_in_noC_CC"      , 2, 3, dEdxTemplatesCC , NULL, true, true, false, true,  0) );
   results.push_back(new dEdxStudyObj("Ias_SP_in_noC_CCC"     , 2, 3, dEdxTemplatesCCC, NULL, true, true, false, true,  1) );

   printf("Progressing Bar           :0%%       20%%       40%%       60%%       80%%       100%%\n");
   for(unsigned int f=0;f<FileName.size();f++){
     TFile* file = TFile::Open(FileName[f].c_str() );
     fwlite::Event ev(file);
     printf("Scanning the ntuple %2i/%2i :", (int)f+1, (int)FileName.size());
     int treeStep(ev.size()/50), iev=0;
     for(ev.toBegin(); !ev.atEnd(); ++ev){ iev++;
         if(iev%treeStep==0){printf(".");fflush(stdout);}

         fwlite::Handle<DeDxHitInfoAss> dedxCollH;
         dedxCollH.getByLabel(ev, "dedxHitInfo");
         if(!dedxCollH.isValid()){printf("Invalid dedxCollH\n");continue;}

         fwlite::Handle< std::vector<reco::Track> > trackCollHandle;
         trackCollHandle.getByLabel(ev,"RefitterForDeDx");
         if(!trackCollHandle.isValid()){
            trackCollHandle.getByLabel(ev,"generalTracks");
               if (!trackCollHandle.isValid()){
                  printf("Invalid trackCollHandle\n");
                  continue;
               }
         }

         fwlite::Handle < std::vector<reco::Vertex> > vertexCollHandle;
         vertexCollHandle.getByLabel(ev, "offlinePrimaryVertices");
         if(!vertexCollHandle.isValid()){printf("Vertex Collection not found!\n"); continue;}
         const std::vector<reco::Vertex>& vertexColl = *vertexCollHandle;
         if(vertexColl.size()<1){printf("NO VERTICES\n"); continue;}


         fwlite::Handle< std::vector<reco::GenParticle> > genCollHandle;
         if(isSignal){
            //get the collection of generated Particles
            genCollHandle.getByLabel(ev, "genParticlesSkimmed");
            if(!genCollHandle.isValid()){
               genCollHandle.getByLabel(ev, "genParticles");
               if(!genCollHandle.isValid()){printf("GenParticle Collection NotFound\n");continue;}
            }
         }


         for(unsigned int c=0;c<trackCollHandle->size();c++){
            //basic track quality cuts
            reco::TrackRef track = reco::TrackRef( trackCollHandle.product(), c );
            if(track.isNull())continue;
            if(track->chi2()/track->ndof()>5 )continue;
            if(track->found()<8)continue;

            if(isSignal){
               if(track->pt()<45)continue;
               const std::vector<reco::GenParticle>& genColl = *genCollHandle;
               if (DistToHSCP (track, genColl)>0.03) continue;
            }else{
               if(track->pt()>=45)continue;
            }

            //load dEdx informations
            const DeDxHitInfo* dedxHits = NULL;
            DeDxHitInfoRef dedxHitsRef = dedxCollH->get(track.key());
            if(!dedxHitsRef.isNull())dedxHits = &(*dedxHitsRef);
            if(!dedxHits)continue;

            //hit level dEdx information (only done for MIPs)
            if(track->pt() > 5){
               for(unsigned int h=0;h<dedxHits->size();h++){
                   DetId detid(dedxHits->detId(h));
                   double scaleFactor = dEdxSF[0];
                   if (detid.subdetId()<3) scaleFactor *= dEdxSF[1];
                   double Norm = (detid.subdetId()<3)?3.61e-06:3.61e-06*265;

                   int moduleGeometry = 0; // underflow bin -- debug purposes
                   if(detid.subdetId()>=3){ SiStripDetId SSdetId(detid); moduleGeometry = SSdetId.moduleGeometry(); if (moduleGeometry==15) {cerr << "ERROR! There is no SiStrip geometry 15!" << endl; exit (EXIT_FAILURE);}}
                   else if(detid.subdetId()<3){moduleGeometry = 15;} // 15 is for pixel

                   for(unsigned int R=0;R<results.size();R++){
                      if(!results[R]->isHit) continue; //only consider results related to hit info here
                      if(!results[R]->usePixel && detid.subdetId() <3)continue; // skip pixels
                      if(!results[R]->useStrip && detid.subdetId()>=3)continue; // skip strips
                      if(results[R]->mustBeInside && !isHitInsideTkModule(dedxHits->pos(h), detid, detid.subdetId()>=3?dedxHits->stripCluster(h):NULL) )continue;
                      if(results[R]->removeCosmics){ if (isCompatibleWithCosmic(track, vertexColl))continue;} //don't consider hits, which belong to cosmic tracks
                      if(results[R]->useClusterCleaning && detid.subdetId()>=3 && !clusterCleaning(dedxHits->stripCluster(h), results[R]->crossTalkInvAlgo)) continue; //if it fails clusterCleaning, skip it!

                      int charge = dedxHits->charge(h);
                      if (detid.subdetId()>=3 && results[R]->crossTalkInvAlgo!=0){ //in case of crossTalkInv, give the corrected cluster charge
                         if (results[R]->Name=="hit_SP_in_noC_CC") std::cout<<"But I said NO crossTalkInversion!"<<std::endl;
                         vector <int> amps = CrossTalkInv(convert(dedxHits->stripCluster(h)->amplitudes()), 0.10, 0.04, true);
                         charge = std::accumulate(amps.begin(), amps.end(), 0);
                      }
                      double ChargeOverPathlength = scaleFactor*Norm*charge/dedxHits->pathlength(h);

                      results[R]->HHit->Fill(ChargeOverPathlength);
                      if(results[R]->usePixel && results[R]->useStrip){
                         
                         results[R]->Charge_Vs_Path->Fill (moduleGeometry, dedxHits->pathlength(h)*10, scaleFactor * charge/(dedxHits->pathlength(h)*10*(detid.subdetId()<3?265:1))); 
                         if(detid.subdetId()>=3)results[R]->Charge_Vs_FS[moduleGeometry]->Fill(dedxHits->stripCluster(h)->firstStrip(), charge); 
                         results[R]->Charge_Vs_XYH[moduleGeometry]->Fill(dedxHits->pos(h).x(), dedxHits->pos(h).y()); 
                         if(ChargeOverPathlength<1.6)results[R]->Charge_Vs_XYL[moduleGeometry]->Fill(dedxHits->pos(h).x(), dedxHits->pos(h).y()); 
    
                         if(moduleGeometry>=1 && moduleGeometry<=14){ // FIXME we don't have the geometry information for Pixels yet (TkModGeom* arrays) !!!
                            double nx, ny;
                            if(moduleGeometry<=4){
                               ny = dedxHits->pos(h).y() /  TkModGeomLength[moduleGeometry];
                               nx = dedxHits->pos(h).x() /  TkModGeomWidthT[moduleGeometry];
                            }else{
                               double  offset = TkModGeomLength[moduleGeometry] * (TkModGeomWidthT[moduleGeometry]+TkModGeomWidthB[moduleGeometry]) / (TkModGeomWidthT[moduleGeometry]-TkModGeomWidthB[moduleGeometry]);  // check sign if GeomWidthT[moduleGeometry] < TkModGeomWidthB[moduleGeometry] !!! 
                               double  tan_a = TkModGeomWidthT[moduleGeometry] / std::abs(offset + TkModGeomLength[moduleGeometry]);
                               ny = dedxHits->pos(h).y() /  TkModGeomLength[moduleGeometry];
                               nx = dedxHits->pos(h).x() / (tan_a*std::abs(dedxHits->pos(h).y()+offset));
                            }
                            //printf("%i - %f - %f --> %f - %f\n", moduleGeometry, dedxHits->pos(h).x(), dedxHits->pos(h).y(), nx, ny);
                            results[R]->Charge_Vs_XYHN[moduleGeometry]->Fill(nx, ny); 
                            if(ChargeOverPathlength<1.6)results[R]->Charge_Vs_XYLN[moduleGeometry]->Fill(nx, ny);
                         }
                      }
                   }
                }
             }

             bool isCosmic = isCompatibleWithCosmic(track, vertexColl);
             bool lockOnTrack=false;
             double dEdxDebug = 0;
             bool IhIsHighEnough = false;
             for(unsigned int R=0;R<results.size();R++){
                if(!results[R]->isEstim and !results[R]->isDiscrim) continue; //only consider results related to estimator/discriminator variables here
                if(results[R]->removeCosmics && isCosmic)continue; //don't consider cosmic tracks

                DeDxData dedxObj   = computedEdx(dedxHits, dEdxSF, results[R]->dEdxTemplates, results[R]->usePixel, results[R]->useClusterCleaning, false, false, results[R]->TrackerGains, results[R]->useStrip, results[R]->mustBeInside, 99, results[R]->correctFEDSat, results[R]->crossTalkInvAlgo);
                DeDxData dedxObj_U = computedEdx(dedxHits, dEdx_U, results[R]->dEdxTemplates, results[R]->usePixel, results[R]->useClusterCleaning, false, false, results[R]->TrackerGains, results[R]->useStrip, results[R]->mustBeInside, 99, results[R]->correctFEDSat, results[R]->crossTalkInvAlgo);

                results[R]->HdedxVsP    ->Fill(track->p(), dedxObj.dEdx() );
                if (!IhIsHighEnough && results[R]->isEstim && dedxObj.dEdx() > 4) IhIsHighEnough=true;
   //             results[R]->HdedxVsQP   ->Fill(track->p()*track->charge(), dedxObj.dEdx() );
   //             results[R]->HdedxVsP_NS ->Fill(track->p(), dedxObj.dEdx(), dedxObj.numberOfSaturatedMeasurements() );

                if(track->pt()>10 && track->pt()<45 && dedxObj.numberOfMeasurements()>=(results[R]->useStrip?7:3) ){
                  results[R]->HdedxVsEtaProfile->Fill(track->eta(), dedxObj.dEdx() );
                  results[R]->HdedxVsEta->Fill(track->eta(), dedxObj.dEdx() );
                  results[R]->HNOMVsEtaProfile->Fill(track->eta(),dedxObj.numberOfMeasurements() );
                  results[R]->HNOSVsEtaProfile->Fill(track->eta(),dedxObj.numberOfSaturatedMeasurements() );
                  results[R]->HNOMSVsEtaProfile->Fill(track->eta(),dedxObj.numberOfMeasurements() - dedxObj.numberOfSaturatedMeasurements() );
                }

                if(fabs(track->eta())>2.1) continue;
                if((int)dedxObj.numberOfMeasurements()<(results[R]->useStrip?10:3))continue;
//                if(track->found()<10) continue; // we cut on total number of hits instead of valid measurements

                if(track->pt()>5){
                   results[R]->HdedxVsNOH->Fill(track->found(), dedxObj.dEdx());
                   results[R]->HNOMVsdEdxProfile->Fill(dedxObj.dEdx(), (int)dedxObj.numberOfMeasurements());
                   results[R]->HNOMVsdEdx->Fill(dedxObj.dEdx(), (int)dedxObj.numberOfMeasurements());
                   results[R]->HdedxMIP  ->Fill(dedxObj.dEdx());
                   results[R]->HP->Fill(track->p());

                   DeDxData dedxObj4  = computedEdx(dedxHits, dEdxSF, results[R]->dEdxTemplates, results[R]->usePixel, results[R]->useClusterCleaning, false, false, results[R]->TrackerGains, results[R]->useStrip, results[R]->mustBeInside, 4, results[R]->correctFEDSat, results[R]->crossTalkInvAlgo);
                   DeDxData dedxObj8  = computedEdx(dedxHits, dEdxSF, results[R]->dEdxTemplates, results[R]->usePixel, results[R]->useClusterCleaning, false, false, results[R]->TrackerGains, results[R]->useStrip, results[R]->mustBeInside, 8, results[R]->correctFEDSat, results[R]->crossTalkInvAlgo);
                   DeDxData dedxObj12 = computedEdx(dedxHits, dEdxSF, results[R]->dEdxTemplates, results[R]->usePixel, results[R]->useClusterCleaning, false, false, results[R]->TrackerGains, results[R]->useStrip, results[R]->mustBeInside,12, results[R]->correctFEDSat, results[R]->crossTalkInvAlgo);

                   results[R]->HdedxMIP_U->Fill(dedxObj_U.dEdx());
                   results[R]->HdedxMIP4 ->Fill(dedxObj4 .dEdx());
                   results[R]->HdedxMIP8 ->Fill(dedxObj8 .dEdx());
                   results[R]->HdedxMIP12->Fill(dedxObj12.dEdx());
                }
                if(fabs(track->eta())<0.4){
                   results[R]->HdedxVsPProfile  ->Fill(track->p(), dedxObj  .dEdx() );
                   results[R]->HdedxVsPProfile_U->Fill(track->p(), dedxObj_U.dEdx() );
                }

                if(results[R]->isEstim && dedxObj.dEdx()>4.0){  //mass can only be computed for dEdx estimators
                   double Mass = GetMass(track->p(),dedxObj.dEdx(), false);

                   if(track->p()<3.0){      results[R]->HMass->Fill(Mass);
                   }else{                   results[R]->HMassHSCP->Fill(Mass);
                   }

                   if (Mass > 0.938-0.15 && Mass < 0.938+0.15 && IhIsHighEnough){// proton candidates
                      results[R]->HdedxVsPSyst->Fill(track->p(), dedxObj.dEdx() );
                   }
                }
                // FIXME DEBUG -- only for Ias -- to check what's going on during cluster cleaning
                /*if(results[R]->isDiscrim){
                   if (!results[R]->useClusterCleaning && !results[R]->crossTalkInvAlgo && results[R]->Name=="Ias_SO_in_noC" && dedxObj.dEdx()>0.9){ lockOnTrack=true; dEdxDebug = dedxObj.dEdx(); }
                   if (lockOnTrack && results[R]->useClusterCleaning && !results[R]->crossTalkInvAlgo && results[R]->Name=="Ias_SO_in_noC_CC" && dedxObj.dEdx()<0.9){
                      std::cout << "Track Ias dropped below 0.9 after ClusterCleaning!" << std::endl;
                      std::cout << "Track index = " << c << "\tdEdx difference = " << dEdxDebug - dedxObj.dEdx() << std::endl;
                      std::cout << "Failed clusters:" << std::endl;
                      lockOnTrack=false; dEdxDebug=0; //reset to the original values
                      // have to know which are the clusters that are removed, and how they look like!
                      // print out rejected clusters
                      for (unsigned int h=0;h<dedxHits->size();h++){
                         uint8_t exitCode;
                         if (!clusterCleaning(dedxHits->stripCluster(h), 0, &exitCode)){
                            std::cout << "\t[ ";
                            for (unsigned int digi_i=0;digi_i<dedxHits->stripCluster(h)->amplitudes().size();digi_i++)
                               std::cout << static_cast<unsigned int>(dedxHits->stripCluster(h)->amplitudes()[digi_i]) << " ";
                            std::cout << "]\t"; printClusterCleaningMessage (exitCode);
                         }
                      }
                   }
                }*/
             }
         }
      }printf("\n");
      delete file;
   }

   OutputHisto->Write();
   OutputHisto->Close();  
}

double DistToHSCP (const reco::TrackRef& track, const std::vector<reco::GenParticle>& genColl){
   if(track.isNull())return false; // FIXME does this make sense? returning false to a double function?

   double RMin = 9999;
   for(unsigned int g=0;g<genColl.size();g++){
      if(genColl[g].pt()<5)continue;
      if(genColl[g].status()!=1)continue;
      int AbsPdg=abs(genColl[g].pdgId());
      if(AbsPdg<1000000 && AbsPdg!=17)continue;
      double dR = deltaR(track->eta(), track->phi(), genColl[g].eta(), genColl[g].phi());
      if(dR<RMin)RMin=dR;
   }
   return RMin;
}

bool isCompatibleWithCosmic (const reco::TrackRef& track, const std::vector<reco::Vertex>& vertexColl){
   for (unsigned int vertex_i=0;vertex_i<vertexColl.size();vertex_i++){
      if(fabs(track->dz (vertexColl[vertex_i].position())) < 0.5 && fabs(track->dxy(vertexColl[vertex_i].position())) < 0.2)return false;
   }
   return true;
}


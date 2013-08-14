// Original Author:  Loic Quertenmont


namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra; class PFMET; class HitPattern;}
namespace susybsm { class HSCParticle; class HSCPIsolation; class MuonSegment; class HSCPDeDxInfo;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     { class TriggerResults; class TriggerResultsByName; class InputTag; class LumiReWeighting;}
namespace reweight{ class PoissonMeanShifter;}

#if !defined(__CINT__) && !defined(__MAKECINT__)
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPDeDxInfo.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/MuonSegment.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/METReco/interface/PFMET.h"

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
using namespace reweight;

#endif


//the define here is simply need to load FWLITE code from the include
#define FWLITE
#include "../../ICHEP_Analysis/Analysis_Step3.C"

/////////////////////////// FUNCTION DECLARATION /////////////////////////////


void Analysis_Compute(string inputname, string outputname, string MODE,  double MassCut, string MassS);
bool   PassTriggerCustom(const fwlite::ChainEvent& ev);


void StandardAnalysis_Acceptance(string MODE="COMPILE", int TypeMode_=0, string dEdxSel_=dEdxS_Label, string dEdxMass_=dEdxM_Label, string TOF_Label_=TOF_Label, double CutPt_=-1.0, double CutI_=-1, double CutTOF_=-1, float MinPt_=GlobalMinPt, float MaxEta_=GlobalMaxEta, float MaxDZ_=GlobalMaxDZ, float MaxDXY_=GlobalMaxDXY)
{
	if(MODE=="COMPILE")return;

	// redefine global variable dependent on the arguments given to the function
	dEdxS_Label    = dEdxSel_;
	dEdxM_Label    = dEdxMass_;
	TOF_Label      = TOF_Label_;
	InitdEdx(dEdxS_Label);
	TypeMode       = TypeMode_;
	GlobalMaxEta   = MaxEta_;
	GlobalMinPt    = MinPt_;
	GlobalMaxDZ    = MaxDZ_;
	GlobalMaxDXY    = MaxDXY_;

	if(TypeMode<2){      GlobalMinNDOF   = 0; 
		GlobalMinTOF    = 0;
	}else if(TypeMode==2) { 
	}else{
                printf("Code only suited for TkOnly and TkTOF Analysis, exit now\n"); exit(0);
	}

	if(TypeMode<2){   
		CutPt .push_back(70.0);   CutI  .push_back(0.4);  CutTOF.push_back(-1);
	}else if(TypeMode==2){
		CutPt .push_back(70.0);   CutI  .push_back(0.125);  CutTOF.push_back(-1);
	}

	string SignalName="PPStau"; 
	//string SignalName="GMStau"; 
	string SignalPath="root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/"+SignalName+"_8TeV_M";

//FIXME should handle automatically cut depending on signalname
//	double MassCuts [10]={ 0   , 30  , 60  , 100 , 140 , 180 , 230 , 270 , 310 , 350 };   //for GMStau in paper
	double MassCuts [10]={ 10  , 20  , 50  , 90  , 130 , 180 , 230 , 280 , 320 , 370 };   //for PPStau in paper
	string MassSs [10]  ={"100","126","156","200","247","308","370","432","494","557"}; 
	int SizeInput=10;   //FIXME Masscuts and signal mass should be changed to vector and handle cleaner and so this should not be used	
	//	double MassCuts [3]={10,320, 370 }; string MassSs [3]  ={"100","494","557"}; //int SizeInput=3; //for debuging only on 3 files

	// run the analyis
	printf("nombre masses considerees:%i\n",SizeInput);

        system("mkdir -p pictures");
	for (int Fileitt=0;Fileitt<SizeInput;Fileitt++){
		string inputname= SignalPath+MassSs[Fileitt]+".root";
		string outputname ="pictures/Std_"+SignalName+MassSs[Fileitt]+".txt";
		Analysis_Compute(inputname ,outputname, SignalName+MassSs[Fileitt],MassCuts[Fileitt],MassSs[Fileitt]);
	}	
	return;
}


// check if the event is passing trigger or not --> note that the function has two part (one for 2011 analysis and the other one for 2012)
bool PassTriggerCustom(const fwlite::ChainEvent& ev)
{
	edm::TriggerResultsByName tr = ev.triggerResultsByName("MergeHLT");
	if(!tr.isValid())return false;

	if(tr.accept("HSCPHLTTriggerMuFilter"))return true;
//	if(tr.accept("HSCPHLTTriggerPFMetFilter"))return true;

	return false;
}


// Looping on all events, tracks, selection and check how many events are entering the mass distribution
void Analysis_Compute(string inputname, string outputname, string MODE,  double MassCut, string MassS)
{
       //Initialize a RandomNumberGenerator
       TRandom3* RNG = new TRandom3();

       bool isData   = false ;//(samples[s].Type==0);
       bool isMC     = false; //(samples[s].Type==1);
       bool isSignal = true; //(samples[s].Type>=2);
       int CutIndex=0;

       dEdxTemplates = loadDeDxTemplate("../../../data/Discrim_Templates_MC_2012.root");
       dEdxSF = 1.05;


       //do two loops through signal for samples with and without trigger changes.
       //load the files corresponding to this sample
       std::vector<string> FileName;

       vector<string> DataFileName;
       printf("%s\n",inputname.c_str());
       DataFileName.push_back(inputname);
       fwlite::ChainEvent ev(DataFileName);

       double NEvents = 0;
       double NTEvents = 0;
       double NPSEvents = 0;
       double NSEvents = 0;
       double NSEventsM[6] = {0,0,0,0,0,0};


       //Loop on the events
       printf("Progressing Bar                   :0%%       20%%       40%%       60%%       80%%       100%%\n");
       printf("Looping on Tree              :");
       int TreeStep = ev.size()/50;if(TreeStep==0)TreeStep=1;

       for(Long64_t ientry=0;ientry<ev.size();ientry++){
               ev.to(ientry);
               if(MaxEntry>0 && ientry>MaxEntry)break;
               if(ientry%TreeStep==0){printf(".");fflush(stdout);}
               NEvents++;

               //    		Event_Weight = SampleWeight * GetPUWeight(ev, samples[s].Pileup, PUSystFactor, LumiWeightsMC, LumiWeightsMCSyst);

               std::vector<reco::GenParticle> genColl;

               //get the collection of generated Particles
               fwlite::Handle< std::vector<reco::GenParticle> > genCollHandle;
               genCollHandle.getByLabel(ev, "genParticles");
               if(!genCollHandle.isValid()){printf("GenParticle Collection NotFound\n");continue;}
               genColl = *genCollHandle;
               int NChargedHSCP=HowManyChargedHSCP(genColl);

               //check if the event is passing trigger
               if(!PassTriggerCustom(ev ) )continue;
               NTEvents++;

               //load all event collection that will be used later on (HSCP COll, dEdx and TOF)
               fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
               hscpCollHandle.getByLabel(ev,"HSCParticleProducer");
               //if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
               if(!hscpCollHandle.isValid())continue;
               const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

               fwlite::Handle<DeDxDataValueMap> dEdxSCollH;
               dEdxSCollH.getByLabel(ev, dEdxS_Label.c_str());
               if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");continue;}

               fwlite::Handle<DeDxDataValueMap> dEdxMCollH;
               dEdxMCollH.getByLabel(ev, dEdxM_Label.c_str());
               if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

               fwlite::Handle<MuonTimeExtraMap> TOFCollH;
               TOFCollH.getByLabel(ev, "muontiming",TOF_Label.c_str());
               if(!TOFCollH.isValid()){printf("Invalid TOF collection\n");return;}

               fwlite::Handle<MuonTimeExtraMap> TOFDTCollH;
               TOFDTCollH.getByLabel(ev, "muontiming",TOFdt_Label.c_str());
               if(!TOFDTCollH.isValid()){printf("Invalid DT TOF collection\n");return;}

               fwlite::Handle<MuonTimeExtraMap> TOFCSCCollH;
               TOFCSCCollH.getByLabel(ev, "muontiming",TOFcsc_Label.c_str());
               if(!TOFCSCCollH.isValid()){printf("Invalid CSC TOF collection\n");return;}

               bool Passed = false;
               bool PSPassed = false;
               bool PassedM[6] = {false,false,false,false,false,false};

               //loop on HSCP candidates
               for(unsigned int c=0;c<hscpColl.size();c++){
                       //define alias for important variable
                       susybsm::HSCParticle hscp  = hscpColl[c];
                       reco::MuonRef  muon  = hscp.muonRef();
                       reco::TrackRef track = hscp.trackRef();
                       if(track.isNull())continue;

                       //for signal only, make sure that the candidate is associated to a true HSCP
                       int ClosestGen;
                       if(isSignal && DistToHSCP(hscp, genColl, ClosestGen)>0.03)continue;

                       //load quantity associated to this track (TOF and dEdx)
                       const DeDxData* dedxSObj = NULL;
                       const DeDxData* dedxMObj = NULL;
                       if(!track.isNull()) {
                          dedxSObj  = &dEdxSCollH->get(track.key());
                          dedxMObj  = &dEdxMCollH->get(track.key());
                       }

                       const reco::MuonTimeExtra* tof = NULL;
                       const reco::MuonTimeExtra* dttof = NULL;
                       const reco::MuonTimeExtra* csctof = NULL;
                       if(TypeMode>1 && !hscp.muonRef().isNull()){ tof  = &TOFCollH->get(hscp.muonRef().key()); dttof = &TOFDTCollH->get(hscp.muonRef().key());  csctof = &TOFCSCCollH->get(hscp.muonRef().key());}

                       //Recompute dE/dx on the fly
                       if(dedxSObj){
                               dedxMObj = dEdxEstimOnTheFly(ev, track, dedxMObj, dEdxSF, false, useClusterCleaning);
                               dedxSObj = dEdxOnTheFly(ev, track, dedxSObj, dEdxSF, dEdxTemplates, false, useClusterCleaning);
                       }

                       //check if the candiate pass the preselection cuts
                       if(!PassPreselection( hscp, dedxSObj, dedxMObj, tof, dttof, csctof, ev, NULL, 0)) continue;
                       PSPassed=true;

                       //check if the candiate pass the selection cuts
                       if(!PassSelection   (hscp, dedxSObj, dedxMObj, tof, ev, CutIndex, NULL, false, 0))continue;

                       //compute the mass of the candidate
                       double Mass     = -1; if(dedxMObj) Mass = GetMass(track->p(),dedxMObj->dEdx(),!isData);
                       if(Mass<MassCut) continue; 
                       Passed = true;

                       for(int M=0;M<6;M++){
                               if(Mass>100.0*M) PassedM[M]=true;
                       }		
               }// end of Track Loop

               if(PSPassed)NPSEvents+=1.0;
               if(Passed)NSEvents+=1.0;
               for(int M=0;M<6;M++){
                       if(PassedM[M]) NSEventsM[M]+=1;
               }	
       }printf("\n");// end of Event Loop

       FILE* pFile = fopen(outputname.c_str(), "w");
       printf("%30s M>%3.0f Efficiencies: Trigger=%6.2f%%+-%6.2f%%  Offline=%6.2f%%+-%6.2f%%\n",MODE.c_str(), MassCut, 100.0*NTEvents/NEvents, 100.0*sqrt(pow(sqrt(NTEvents)/NEvents,2)+pow(NTEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NSEvents/NEvents, 100.0*sqrt(pow(sqrt(NSEvents)/NEvents,2)+pow(NSEvents*sqrt(NEvents)/pow(NEvents,2),2)));      
       for(int M=0;M<6;M++){
               fprintf(pFile, "%30s M>%3.0f Efficiencies: Trigger=%6.2f%%+-%6.2f%%  Presel=%6.2f%%+-%6.2f%% Offline=%6.2f%%+-%6.2f%%\n",MODE.c_str(), M*100.0, 100.0*NTEvents/NEvents, 100.0*sqrt(pow(sqrt(NTEvents)/NEvents,2)+pow(NTEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NPSEvents/NEvents, 100.0*sqrt(pow(sqrt(NPSEvents)/NEvents,2)+pow(NPSEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NSEventsM[M]/NEvents, 100.0*sqrt(pow(sqrt(NSEventsM[M])/NEvents,2)+pow(NSEventsM[M]*sqrt(NEvents)/pow(NEvents,2),2)));      
       }	
       fprintf(pFile, "%30s M>%3.0f Efficiencies: Trigger=%6.2f%%+-%6.2f%%  Presel=%6.2f%%+-%6.2f%% Offline=%6.2f%%+-%6.2f%%\n",MODE.c_str(), MassCut, 100.0*NTEvents/NEvents, 100.0*sqrt(pow(sqrt(NTEvents)/NEvents,2)+pow(NTEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NPSEvents/NEvents, 100.0*sqrt(pow(sqrt(NPSEvents)/NEvents,2)+pow(NPSEvents*sqrt(NEvents)/pow(NEvents,2),2)), 100.0*NSEvents/NEvents, 100.0*sqrt(pow(sqrt(NSEvents)/NEvents,2)+pow(NSEvents*sqrt(NEvents)/pow(NEvents,2),2)));      
      fclose(pFile);


      delete RNG;
}


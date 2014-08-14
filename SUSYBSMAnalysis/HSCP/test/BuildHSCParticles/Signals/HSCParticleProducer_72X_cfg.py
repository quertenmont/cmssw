import FWCore.ParameterSet.Config as cms

process = cms.Process("HSCPAnalysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load('Configuration.StandardSequences.Services_cff')

process.options   = cms.untracked.PSet(
      wantSummary = cms.untracked.bool(True),
      SkipEvent = cms.untracked.vstring('ProductNotFound'),
)
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(250) )
process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(),
   inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*")
)
process.source.duplicateCheckMode = cms.untracked.string("noDuplicateCheck")

for i in range(0,25):
   process.source.fileNames.extend(["file:/afs/cern.ch/user/q/querten/workspace/public/14_08_12_Run2HSCP/CMSSW_7_2_0_pre3/src/SampleProd/FARM_Reco/outputs/gluino1TeV_RECO_%04i.root" % i])

process.GlobalTag.globaltag = 'START72_V1::All'


########################################################################

process.load("SUSYBSMAnalysis.HSCP.HSCParticleProducerFromSkim_cff")  #IF RUNNING ON HSCP SKIM


process.load('Configuration.Skimming.PDWG_EXOHSCP_cff')

process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
process.HSCPTrigger = process.hltHighLevel.clone()
#process.HSCPTrigger.TriggerResultsTag = cms.InputTag( "TriggerResults", "", "HLT" )
process.HSCPTrigger.TriggerResultsTag = cms.InputTag( "TriggerResults", "", "RECO" )
process.HSCPTrigger.HLTPaths = ["*"] #not apply any trigger filter for MC
process.HSCPTrigger.andOr = cms.bool( True ) #OR
process.HSCPTrigger.throw = cms.bool( False )

########################################################################  SPECIAL CASE FOR MC
process.load("PhysicsTools.HepMCCandAlgos.genParticles_cfi")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.allGenParticles = cms.EDProducer("GenParticleProducer",
     saveBarCodes = cms.untracked.bool(False),
     src = cms.InputTag("generator"),
     abortOnUnknownPDGCode = cms.untracked.bool(False)
)
process.genParticles = cms.EDFilter("GenParticleSelector",
     filter = cms.bool(False),
     src = cms.InputTag("allGenParticles"),
     cut = cms.string('charge != 0 & pt > 5.0'),
     stableOnly = cms.bool(True)
)

########################################################################
process.nEventsBefSkim  = cms.EDProducer("EventCountProducer")
process.nEventsBefEDM   = cms.EDProducer("EventCountProducer")
########################################################################

process.Out = cms.OutputModule("PoolOutputModule",
     outputCommands = cms.untracked.vstring(
         "drop *",
         "keep EventAux_*_*_*",
         "keep LumiSummary_*_*_*",
         "keep edmMergeableCounter_*_*_*",
         "keep *_genParticles_*_HSCPAnalysis",
         "keep GenEventInfoProduct_generator_*_*",
         "keep *_offlinePrimaryVertices_*_*",
         "keep SiStripClusteredmNewDetSetVector_generalTracksSkim_*_*",
         "keep SiPixelClusteredmNewDetSetVector_generalTracksSkim_*_*",
         "keep *_TrackRefitter_*_*",
         "keep *_standAloneMuons_*_*",
         "keep *_globalMuons_*_*",  #
         "keep *_muonsSkim_*_*",
         "keep edmTriggerResults_TriggerResults_*_*",
         "keep *_ak5PFJetsPt15__*", #
         "keep recoPFMETs_pfMet__*",     #
         "keep *_HSCParticleProducer_*_*",
         "keep *_HSCPIsolation01__*",
         "keep *_HSCPIsolation03__*",
         "keep *_HSCPIsolation05__*",
         "keep *_dedx*_*_HSCPAnalysis",
         "keep *_muontiming_*_HSCPAnalysis",
         "keep triggerTriggerEvent_hltTriggerSummaryAOD_*_*",
         "keep *_RefitMTSAMuons_*_*",
         "keep *_MTMuons_*_*",
         "keep *_MTSAMuons_*_*",
         "keep *_MTmuontiming_*_*",
         "keep *_refittedStandAloneMuons_*_*",
         "keep *_offlineBeamSpot_*_*",
         "drop *_offlineBeamSpot_*_HSCPAnalysis", #no need to save the BS from this process
         "keep *_MuonSegmentProducer_*_*",
         "drop TrajectorysToOnerecoTracksAssociation_TrackRefitter__",
         "drop recoTrackExtras_*_*_*",
         "keep recoTrackExtras_TrackRefitter_*_*",
         "drop TrackingRecHitsOwned_*Muon*_*_*",
         "keep *_g4SimHits_StoppedParticles*_*",
         "keep PileupSummaryInfos_addPileupInfo_*_*"
    ),
    fileName = cms.untracked.string('HSCP.root'),
#    SelectEvents = cms.untracked.PSet(
#       SelectEvents = cms.vstring('p1')
#    ),
)


########################################################################

#LOOK AT SD PASSED PATH IN ORDER to avoid as much as possible duplicated events (make the merging of .root file faster)
process.p1 = cms.Path(process.nEventsBefSkim + process.allGenParticles + process.genParticles + process.HSCPTrigger + process.exoticaHSCPSeq + process.nEventsBefEDM + process.HSCParticleProducerSeq)
process.endPath1 = cms.EndPath(process.Out)
process.schedule = cms.Schedule(process.p1, process.endPath1)

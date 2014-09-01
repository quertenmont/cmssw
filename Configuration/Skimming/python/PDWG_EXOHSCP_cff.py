import FWCore.ParameterSet.Config as cms

#global variable used in the skimming
TRACK_PT = 20.0

############################################################################
#                                                                          #
#    FILTER EVENTS BASED ON TRACK PT, MUON PT and dEDX                     #
#                                                                          #
############################################################################

#Skim the track collection, in order to keep only potential candidates
from Alignment.CommonAlignmentProducer.AlignmentTrackSelector_cfi import *
generalTracksSkim = AlignmentTrackSelector.clone(
    src = 'generalTracks',
    filter = False,
    applyBasicCuts = True,
    ptMin = TRACK_PT,
    ptMax = cms.double(999999.0),
    nHitMin = 5,
    chi2nMax = 10.,
)
trackerSeq = cms.Sequence( generalTracksSkim)

#Refit the skimmed track collection in order to reobtain the trajectory, needed for dEdx computations
from RecoVertex.BeamSpotProducer.BeamSpot_cff import *
from RecoTracker.MeasurementDet.MeasurementTrackerEventProducer_cfi import *
from RecoTracker.TrackProducer.TrackRefitters_cff import *
TrackRefitterSkim = TrackRefitter.clone()
TrackRefitterSkim.src = "generalTracksSkim"

#compute dEdx associate to the interesting tracks
dedxSkimNPHarm2 = cms.EDProducer("DeDxEstimatorProducer",
    tracks                     = cms.InputTag("TrackRefitterSkim"),
    trajectoryTrackAssociation = cms.InputTag("TrackRefitterSkim"),

    estimator      = cms.string('generic'),
    exponent       = cms.double(-2.0),

    UseStrip       = cms.bool(True),
    UsePixel       = cms.bool(False),
    MeVperADCStrip = cms.double(3.61e-06*265),
    MeVperADCPixel = cms.double(3.61e-06),

    MisCalib_Mean      = cms.untracked.double(1.0),
    MisCalib_Sigma     = cms.untracked.double(0.00),

    UseCalibration  = cms.bool(False),
    calibrationPath = cms.string(""),
    ShapeTest       = cms.bool(True),
)

#Filter based on dEdx and other quantities
DedxFilter = cms.EDFilter("HSCPFilter",
     inputMuonCollection = cms.InputTag("muons"),
     inputTrackCollection = cms.InputTag("TrackRefitterSkim"),
     inputDedxCollection =  cms.InputTag("dedxSkimNPHarm2"),
     SAMuPtMin = cms.double(60),
     trkPtMin = cms.double(TRACK_PT),
     dedxMin =cms.double(3.0),
     dedxMaxLeft =cms.double(2.8),
     ndedxHits = cms.int32(3),
     etaMin= cms.double(-2.4),
     etaMax= cms.double(2.4),
     chi2nMax = cms.double(10),
     dxyMax = cms.double(2.0),
     dzMax = cms.double(5),
     filter = cms.bool(True)

)
dedxSeq = cms.Sequence(offlineBeamSpot + MeasurementTrackerEvent + TrackRefitterSkim + dedxSkimNPHarm2+DedxFilter)


############################################################################
#                                                                          #
#    SAVE ECAL HITS CLOSE TO MUONS OR HIGH PT TRACKS                       #
#                                                                          #
############################################################################
from TrackingTools.TrackAssociator.DetIdAssociatorESProducer_cff import *
from TrackingTools.TrackAssociator.default_cfi import *
HSCPSkimMuonEcalDetIds = cms.EDProducer("InterestingEcalDetIdProducer",
   inputCollection = cms.InputTag("muons")	
)

highPtTrackEcalDetIds = cms.EDProducer("HighPtTrackEcalDetIdProducer",
   TrackAssociatorParameters=TrackAssociatorParameterBlock.TrackAssociatorParameters,
   inputCollection = cms.InputTag("generalTracksSkim"),
   TrackPt=cms.double(TRACK_PT)
)

reducedHSCPEcalRecHitsEB = cms.EDProducer("ReducedRecHitCollectionProducer",
     recHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
     interestingDetIdCollections = cms.VInputTag(
	     cms.InputTag("highPtTrackEcalDetIds"),
             cms.InputTag("HSCPSkimMuonEcalDetIds")
     ),
     reducedHitsCollection = cms.string('')
)
reducedHSCPEcalRecHitsEE = cms.EDProducer("ReducedRecHitCollectionProducer",
     recHitsLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
     interestingDetIdCollections = cms.VInputTag(
	     cms.InputTag("highPtTrackEcalDetIds"),
             cms.InputTag("HSCPSkimMuonEcalDetIds")
     ),
     reducedHitsCollection = cms.string('')
)
ecalSeq = cms.Sequence(HSCPSkimMuonEcalDetIds+highPtTrackEcalDetIds+reducedHSCPEcalRecHitsEB+reducedHSCPEcalRecHitsEE)


############################################################################
#                                                                          #
#    SAVE HCAL HITS CLOSE TO HIGH PT TRACKS                                #
#    FIXME: WOULD BE GOOD TO ALSO SAVE HITS CLOSE TO MUONS                 #
############################################################################

reducedHSCPhbhereco = cms.EDProducer("ReduceHcalRecHitCollectionProducer",
     recHitsLabel = cms.InputTag("hbhereco",""),
     TrackAssociatorParameters=TrackAssociatorParameterBlock.TrackAssociatorParameters,
     inputCollection = cms.InputTag("generalTracksSkim"),
     TrackPt=cms.double(TRACK_PT),
     reducedHitsCollection = cms.string('')
)
hcalSeq = cms.Sequence(reducedHSCPhbhereco)


############################################################################
#                                                                          #
#    NEED TO ASSIGN THE REFITTED TRACK TO THE MUONS                        #
#    SINCE ONLY THE LATTER IS SAVED TO THE ROOT FILE                       #
############################################################################


muonsSkim = cms.EDProducer("UpdatedMuonInnerTrackRef",
    MuonTag        = cms.untracked.InputTag("muons"),
    OldTrackTag    = cms.untracked.InputTag("generalTracks"),
    NewTrackTag    = cms.untracked.InputTag("generalTracksSkim"),
    maxInvPtDiff   = cms.untracked.double(0.005),
    minDR          = cms.untracked.double(0.01),
)
muonSeq = cms.Sequence(muonsSkim)


############################################################################
#                                                                          #
#    SAVE ISOLATION VARIABLE IN ORDER TO BE CAPABLE OF DROPPING ALL        #
#    OTHER OBJECTS (this saves a lot of space)                             #
############################################################################

TrackAssociatorParametersForHSCPIsol = TrackAssociatorParameterBlock.TrackAssociatorParameters.clone()
TrackAssociatorParametersForHSCPIsol.useHO = cms.bool(False)
TrackAssociatorParametersForHSCPIsol.CSCSegmentCollectionLabel     = cms.InputTag("cscSegments")
TrackAssociatorParametersForHSCPIsol.DTRecSegment4DCollectionLabel = cms.InputTag("dt4DSegments")
TrackAssociatorParametersForHSCPIsol.EERecHitCollectionLabel       = cms.InputTag("ecalRecHit","EcalRecHitsEE")
TrackAssociatorParametersForHSCPIsol.EBRecHitCollectionLabel       = cms.InputTag("ecalRecHit","EcalRecHitsEB")
TrackAssociatorParametersForHSCPIsol.HBHERecHitCollectionLabel     = cms.InputTag("hbhereco")

HSCPIsolation01 = cms.EDProducer("ProduceIsolationMap",
      inputCollection  = cms.InputTag("generalTracksSkim"),
      IsolationConeDR  = cms.double(0.1),
      TkIsolationPtCut = cms.double(10),
      TKLabel          = cms.InputTag("generalTracks"),
      TrackAssociatorParameters=TrackAssociatorParametersForHSCPIsol,
)

HSCPIsolation03 = HSCPIsolation01.clone()
HSCPIsolation03.IsolationConeDR  = cms.double(0.3)

HSCPIsolation05 = HSCPIsolation01.clone()
HSCPIsolation05.IsolationConeDR  = cms.double(0.5)


ak5PFJetsPt15 = cms.EDFilter( "EtMinPFJetSelector",
     src = cms.InputTag( "ak5PFJets" ),
     filter = cms.bool( False ),
     etMin = cms.double( 15.0 )
)



############################################################################
#                                                                          #
#    DEFINE THE COMPLETE EXO HSCP SKIM SEQUENCE BELLOW                     #
#                                                                          #
############################################################################

nEventsBefSkim  = cms.EDProducer("EventCountProducer")
exoticaHSCPSeq = cms.Sequence(nEventsBefSkim+trackerSeq+dedxSeq+ecalSeq+hcalSeq+muonSeq+HSCPIsolation01+HSCPIsolation03+HSCPIsolation05+ak5PFJetsPt15)


############################################################################
#                                                                          #
#    DEFINE THE EXO HSCPSkim EVENT CONTENT                                 #
#                                                                          #
############################################################################


EXOHSCPSkim_EventContent=cms.PSet(
    outputCommands = cms.untracked.vstring(
      "drop *",
      "keep edmMergeableCounter_*_*_*",
      "keep GenEventInfoProduct_generator_*_*",
      "keep L1GlobalTriggerReadoutRecord_*_*_*",
      "keep recoVertexs_offlinePrimaryVertices_*_*",
      "keep recoMuons_muonsSkim_*_*",
      "keep SiStripClusteredmNewDetSetVector_generalTracksSkim_*_*",
      "keep SiPixelClusteredmNewDetSetVector_generalTracksSkim_*_*",
      "keep recoTracks_generalTracksSkim_*_*",
      "keep recoTrackExtras_generalTracksSkim_*_*",
      "keep TrackingRecHitsOwned_generalTracksSkim_*_*",
      'keep *_dt1DRecHits_*_*',
      'keep *_dt4DSegments_*_*',
      'keep *_csc2DRecHits_*_*',
      'keep *_cscSegments_*_*',
      'keep *_rpcRecHits_*_*',
      'keep recoTracks_standAloneMuons_*_*',
      'keep recoTrackExtras_standAloneMuons_*_*',
      'keep TrackingRecHitsOwned_standAloneMuons_*_*',
      'keep recoTracks_globalMuons_*_*',
      'keep recoTrackExtras_globalMuons_*_*',
      'keep TrackingRecHitsOwned_globalMuons_*_*',
      'keep EcalRecHitsSorted_reducedHSCPEcalRecHitsEB_*_*',
      'keep EcalRecHitsSorted_reducedHSCPEcalRecHitsEE_*_*',
      'keep HBHERecHitsSorted_reducedHSCPhbhereco__*',
      'keep edmTriggerResults_TriggerResults__*',
      'keep *_hltTriggerSummaryAOD_*_*',
      'keep *_HSCPIsolation01__*',
      'keep *_HSCPIsolation03__*',
      'keep *_HSCPIsolation05__*',
      'keep recoPFJets_ak5PFJetsPt15__*',
      'keep recoPFMETs_pfMet__*',
      'keep recoBeamSpot_offlineBeamSpot__*',
      )
)



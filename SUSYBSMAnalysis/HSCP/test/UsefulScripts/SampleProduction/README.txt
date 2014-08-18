#Execute the followin commands:
cmsenv
mkdir $CMSSW_BASE/src/Configuration
cp -rd /afs/cern.ch/user/c/cnuttens/public/TriggerSamples/Configuration/* $CMSSW_BASE/src/Configuration/.

#Make a templace configuration for GEN-SIM-RAW of 1TeV Gluino at 13TeV  
#(add some commands at the end in order to allow parrallel production)
#WARNING: No Pileup is generated for the time being
cmsDriver.py Configuration/GenProduction/python/ThirteenTeV/HSCPgluino_M_1000_Tune4C_13TeV_pythia8_cff.py --fileout file:fileout.root --mc --eventcontent RAWSIM --datatier GEN-SIM-RAW --conditions START72_V1::All --step GEN,SIM,DIGI,L1,DIGI2RAW --python_filename GEN_SIM_Template_cfg.py --magField 38T_PostLS1 --geometry Extended2015 --customise Configuration/GenProduction/ThirteenTeV/Exotica_HSCP_SIM_cfi.customise --no_exec -n 10
echo 'process.RandomNumberGeneratorService.generator.initialSeed = cms.untracked.uint32(XXX_SEED_XXX)' >> GEN_SIM_Template_cfg.py 
echo 'process.RandomNumberGeneratorService.mix.initialSeed = cms.untracked.uint32(XXX_SEED_XXX)'  >> GEN_SIM_Template_cfg.py
echo 'process.maxEvents.input = cms.untracked.int32(XXX_NEVENTS_XXX)'  >> GEN_SIM_Template_cfg.py
echo 'process.RAWSIMoutput.fileName = cms.untracked.string("file:XXX_OUTPUT_XXX_GENSIM_XXX_I_XXX.root")'  >> GEN_SIM_Template_cfg.py
echo 'process.source.firstLuminosityBlock =  cms.untracked.uint32(1XXX_I_XXX)'  >> GEN_SIM_Template_cfg.py

#Make a template configuration for HLT-RECO configuration
cmsDriver.py --filein file:$CMSSW_BASE/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/FARM_SIM/outputs/gluino1TeV_GENSIM_XXX_I_XXX.root --fileout XXX_OUTPUT_XXX_RECO_XXX_I_XXX.root --mc --eventcontent RECOSIM  --datatier GEN-SIM-RECO --conditions START72_V1::All --step HLT,RAW2DIGI,L1Reco,RECO --python_filename RECO_Template_cfg.py --magField 38T_PostLS1 --geometry Extended2015 --datamix NODATAMIXER --processName HLT --no_exec -n -1

#!/usr/bin/env python

import urllib
import string
import os
import sys
import SUSYBSMAnalysis.HSCP.LaunchOnCondor as LaunchOnCondor  
import glob



samples = [
  ['Gluino300' , 'Configuration/GenProduction/python/ThirteenTeV/HSCPgluino_M_300_Tune4C_13TeV_pythia8_cff.py'],
  ['Gluino1000', 'Configuration/GenProduction/python/ThirteenTeV/HSCPgluino_M_1000_Tune4C_13TeV_pythia8_cff.py'],
  ['Gluino1500', 'Configuration/GenProduction/python/ThirteenTeV/HSCPgluino_M_1500_Tune4C_13TeV_pythia8_cff.py'],
  ['GMStau126', 'Configuration/GenProduction/python/ThirteenTeV/HSCPgmstau_M_126_TuneZ2star_13TeV_pythia6_cff.py'],
  ['GMStau494', 'Configuration/GenProduction/python/ThirteenTeV/HSCPgmstau_M_494_TuneZ2star_13TeV_pythia6_cff.py'],
]

if sys.argv[1]=='1':
   for S in samples:
      os.system('cmsDriver.py ' + S[1] + ' --fileout file:fileout.root --mc --eventcontent RAWSIM --datatier GEN-SIM-RAW --conditions START72_V1::All --step GEN,SIM,DIGI,L1,DIGI2RAW --python_filename GEN_SIM_Template_cfg.py --magField 38T_PostLS1 --geometry Extended2015 --customise Configuration/GenProduction/ThirteenTeV/Exotica_HSCP_SIM_cfi.customise --no_exec -n 10')
      with open("GEN_SIM_Template_cfg.py", "a") as f:
         f.write('process.RandomNumberGeneratorService.generator.initialSeed = cms.untracked.uint32(XXX_SEED_XXX)\n')
         f.write('process.RandomNumberGeneratorService.mix.initialSeed = cms.untracked.uint32(XXX_SEED_XXX)\n')
         f.write('process.maxEvents.input = cms.untracked.int32(XXX_NEVENTS_XXX)\n')
         f.write('process.RAWSIMoutput.fileName = cms.untracked.string("file:XXX_OUTPUT_XXX_XXX_I_XXX.root")\n')
         f.write('process.source.firstLuminosityBlock =  cms.untracked.uint32(1XXX_I_XXX)\n')

      JobName = S[0]+"_SIM"
      FarmDirectory = "FARM_"+JobName
      LaunchOnCondor.Jobs_NEvent = 25
      LaunchOnCondor.Jobs_Skip = 0
      LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
      for i in range(0,10):
           LaunchOnCondor.Jobs_Count = i
           LaunchOnCondor.SendCluster_Push  (["CMSSW", "GEN_SIM_Template_cfg.py"])
           LaunchOnCondor.Jobs_Skip+=LaunchOnCondor.Jobs_NEvent
      LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='2':
   for S in samples:
      os.system('cmsDriver.py --filein file:'+os.getcwd()+'/FARM_'+S[0]+'_SIM/outputs/'+S[0]+'_SIM_XXX_I_XXX.root --fileout XXX_OUTPUT_XXX_XXX_I_XXX.root --mc --eventcontent RECOSIM  --datatier GEN-SIM-RECO --conditions START72_V1::All --step HLT,RAW2DIGI,L1Reco,RECO --python_filename RECO_Template_cfg.py --magField 38T_PostLS1 --geometry Extended2015 --datamix NODATAMIXER --processName HLT --no_exec -n -1')

      JobName = S[0]+"_RECO"
      FarmDirectory = "FARM_"+JobName
      LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
      for i in range(0,10):
           LaunchOnCondor.Jobs_Count = i
           LaunchOnCondor.SendCluster_Push  (["CMSSW", "RECO_Template_cfg.py"])
      LaunchOnCondor.SendCluster_Submit()

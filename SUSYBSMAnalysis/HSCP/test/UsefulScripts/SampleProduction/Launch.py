#!/usr/bin/env python

import urllib
import string
import os
import sys
import SUSYBSMAnalysis.HSCP.LaunchOnCondor as LaunchOnCondor  
import glob

if sys.argv[1]=='1':
   FarmDirectory = "FARM_SIM"
   JobName = "gluino1TeV"
   LaunchOnCondor.Jobs_NEvent = 25
   LaunchOnCondor.Jobs_Skip = 0
   LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
   for i in range(0,10):
           LaunchOnCondor.Jobs_Count = i
           LaunchOnCondor.SendCluster_Push  (["CMSSW", "GEN_SIM_Template_cfg.py"])
           LaunchOnCondor.Jobs_Skip+=LaunchOnCondor.Jobs_NEvent
   LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='2':
   FarmDirectory = "FARM_RECO"
   JobName = "gluino1TeV"
   LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
   for i in range(0,10):
           LaunchOnCondor.Jobs_Count = i
           LaunchOnCondor.SendCluster_Push  (["CMSSW", "RECO_Template_cfg.py"])
   LaunchOnCondor.SendCluster_Submit()

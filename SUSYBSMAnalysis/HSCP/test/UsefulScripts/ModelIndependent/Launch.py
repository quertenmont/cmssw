#!/usr/bin/env python

import urllib
import string
import os
import sys
import LaunchOnCondor
import glob

EffMassPoints=[100, 126, 156, 200, 247, 308, 370, 432, 494, 557]


CMSSW_VERSION = os.getenv('CMSSW_VERSION','CMSSW_VERSION')
if CMSSW_VERSION == 'CMSSW_VERSION':
  print 'please setup your CMSSW environement'
  sys.exit(0)


if len(sys.argv)==1:
	print "Please pass in argument a number between 0 and 2"
	sys.exit()

elif sys.argv[1]=='0':	
        print 'Build Efficiency maps'
        FarmDirectory = "FARM"
        JobName = "HscpBuildEffMaps"
	LaunchOnCondor.Jobs_RunHere = 1
	LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)	
        for m in EffMassPoints :
           LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/GetEfficiencyMaps.C", '"pictures/Histos_SStau'+str(m)+'.root"', '"root://eoscms//eos/cms/store/user/querten/ModelIndep/ModelIndep_SingleStau'+str(m)+'.root"'])
	LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='1':
        print 'Merge efficiencies'
        fileList = ''
        for m in EffMassPoints :
           fileList+=' pictures/Histos_SStau'+str(m)+'.root'        
        os.system('hadd -f pictures/Histos.root' + fileList)
        os.system('root MakePlot.C++ -l -b -q');
elif sys.argv[1]=='2':
        print 'Compute model independent acceptance'
        FarmDirectory = "FARM"
        JobName = "HscpModelIndepAccep"
        LaunchOnCondor.Jobs_RunHere = 1
        LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
        for m in EffMassPoints :
           LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/ModelIndependent_Acceptance.C", '"pictures/PPStau'+str(m)+'.txt"', '"root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/PPStau_8TeV_M'+str(m)+'.root"'])
           LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/ModelIndependent_Acceptance.C", '"pictures/GMStau'+str(m)+'.txt"', '"root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/GMStau_8TeV_M'+str(m)+'.root"'])
        LaunchOnCondor.SendCluster_Submit()

else:
	print 'Unknown case: use an other argument or no argument to get help'




#!/usr/bin/env python

import urllib
import string
import os
import sys
import LaunchOnCondor
import glob

StauMassPoints=[100, 126, 156, 200, 247, 308, 370, 432, 494, 557, 651, 745, 871, 1029]
StauMassCut   =[10, 20, 50, 90, 130, 180, 230, 280, 320, 370, 440, 500, 570, 660]
DYMassPoints=[100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
DYMassCut   =[40, 120, 190, 270, 340, 400, 470, 530, 590, 650]
Energy = ["8TeV"]#, "7TeV"]

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
        for m in StauMassPoints :
           LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/GetEfficiencyMaps.C", '"SStau'+str(m)+'"', "1", '"root://eoscms//eos/cms/store/user/querten/ModelIndepSept/ModelIndep_SingleStau'+str(m)+'.root"'])
	LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='1':
        print 'Merge efficiencies'
        fileList = ''
        for m in StauMassPoints :
           fileList+=' pictures/Histos_SStau'+str(m)+'.root'        
        os.system('hadd -f pictures/Histos.root' + fileList)
        os.system('root MakePlot.C++ -l -b -q');
elif sys.argv[1]=='2':
        print 'Compute model independent acceptance'
        FarmDirectory = "FARM"
        JobName = "HscpModelIndepAccep"
        LaunchOnCondor.Jobs_RunHere = 1
        LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
        for E in Energy:           
           Suffix=''
           if(E=="7TeV"):Suffix="_BX1";
#           for m in StauMassPoints :
#              LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/ModelIndependent_Acceptance.C", '"MI_PPStau_'+E+'_M'+str(m)+'"', '"root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/PPStau_'+E+'_M'+str(m)+Suffix+'.root"'])
#              LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/ModelIndependent_Acceptance.C", '"MI_GMStau_'+E+'_M'+str(m)+'"', '"root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/GMStau_'+E+'_M'+str(m)+Suffix+'.root"'])
           for m in DYMassPoints :
#              LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/ModelIndependent_Acceptance.C", '"MI_DY_'+E+'_M'+str(m)+'"', '"root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/HSCP_'+E+'_M'+str(m)+Suffix+'_Q1.root"'])
              LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/ModelIndependent_Acceptance.C", '"MI_pMSSM_'+E+'_M'+str(m)+'"', '"root://eoscms//eos/cms//store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/pMSSM_'+E+'_ChipmChipm_mChi'+str(m)+Suffix+'.root"'])

        LaunchOnCondor.SendCluster_Submit()
elif sys.argv[1]=='3':
        print 'Compute acceptance for standard analysis'
        FarmDirectory = "FARM"
        JobName = "HscpStandardAccep"
        LaunchOnCondor.Jobs_RunHere = 1
        LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
        for E in Energy:
           Suffix=''
           if(E=="7TeV"):Suffix="_BX1";
           I=0;
           for m in StauMassPoints :
  #            LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/StandardAnalysis_Acceptance.C", '"PPStau_'+E+'_M'+str(m)+Suffix+'"', '1', str(StauMassCut[I])])
              I+=1;
           I=0;
           for m in StauMassPoints :
 #             LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/StandardAnalysis_Acceptance.C", '"GMStau_'+E+'_M'+str(m)+Suffix+'"', '1', str(StauMassCut[I])])
              I+=1;
           I=0;
           for m in DYMassPoints :
#              LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/StandardAnalysis_Acceptance.C", '"DY_'+E+'_M'+str(m)+'_Q1'+Suffix+'"', '1', str(DYMassCut[I])])
              I+=1;
           I=0;
           for m in DYMassPoints :
              LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/StandardAnalysis_Acceptance.C", '"pMSSM_'+E+'_M'+str(m)+Suffix+'"', '1', str(DYMassCut[I])])
              I+=1;
        LaunchOnCondor.SendCluster_Submit()

else:
	print 'Unknown case: use an other argument or no argument to get help'




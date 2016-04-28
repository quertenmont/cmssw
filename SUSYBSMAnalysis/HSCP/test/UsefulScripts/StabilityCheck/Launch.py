#!/usr/bin/env python

import urllib
import string
import os
import sys
import SUSYBSMAnalysis.HSCP.LaunchOnCondor as LaunchOnCondor
import glob

LaunchOnCondor.Jobs_InitCmds       = ['ulimit -c 0;']  #disable production of core dump in case of job crash

if sys.argv[1]=='1':
   print("compile the Stability code")
   os.system("sh " + os.getcwd() + "/StabilityCheck.sh ") #just compile

   print 'STABILITY'
   FarmDirectory = "FARM"
   JobName = "HSCPStability"
   LaunchOnCondor.Jobs_RunHere = 1
   LaunchOnCondor.Jobs_Queue = "8nh"
   LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
   #LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/StabilityCheck.C", '"ANALYSE"'])
   #LaunchOnCondor.SendCluster_Push(["BASH", "sh " + os.getcwd()+"/StabilityCheck.sh " + os.getcwd()+"/pictures"])


   #NJobs = 500
   #for Job in range(0,NJobs) :
   #      LaunchOnCondor.SendCluster_Push(["BASH", "sh " + os.getcwd()+"/StabilityCheck.sh " + os.getcwd()+"/pictures " + str(Job) +" " + str(NJobs)])
   #LaunchOnCondor.SendCluster_Submit()


   f= open('../../AnalysisCode/Analysis_Samples.txt','r')
   index = -1
   for line in f :
      index+=1           
      if(line.startswith('#')):continue
      vals=line.split(',')
      if(int(vals[1])==2):continue
      LaunchOnCondor.SendCluster_Push(["BASH", "sh " + os.getcwd()+"/StabilityCheck.sh " + os.getcwd()+"/pictures " + str(index) +" " + str(1)])
   f.close()
   LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='2':
   os.system('find pictures/Histos_*.root  -type f -size +1024c | xargs hadd -f  pictures/Histos.root')

if sys.argv[1]=='3':
   os.system('sh MakePlot.sh')


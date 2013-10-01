#!/usr/bin/env python

import urllib
import string
import os
import sys
import LaunchOnCondor
import glob
import errno

def skipSamples(type, name):
   if(type==3):
      if(name.find("Gluino")==-1 and name.find("Stop")==-1 and name.find("Stau")==-1 and name.find("o3")==-1):return True;
   elif(type==4):
      if(name.find("DY")==-1 or name.find("o3")>=0):return True;
   elif(type==5):
      if(name.find("DY")==-1 or (name.find("1o3")==-1 and name.find("2o3")==-1 and name.find("Q1")==-1)):return True;
   
   return False

def makedirs(path):
   try:
      os.makedirs(path)
   except OSError as exception:
      if exception.errno != errno.EEXIST:
         raise

#the vector below contains the "TypeMode" of the analyses that should be run
AnalysesToRun = [0,2,3,4,5]

def make_excl_dirs():
   for analysis in AnalysesToRun:
      makedirs("Results/Type" + str(analysis) + "/EXCLUSION7TeV")
      makedirs("Results/Type" + str(analysis) + "/EXCLUSION8TeV")
      makedirs("Results/Type" + str(analysis) + "/EXCLUSIONCOMB")

CMSSW_VERSION = os.getenv('CMSSW_VERSION','CMSSW_VERSION')
if CMSSW_VERSION == 'CMSSW_VERSION':
  print 'please setup your CMSSW environement'
  sys.exit(0)


if len(sys.argv)==1:
	print "Please pass in argument a number between 0 and 2"
        print "  0 - Submit the Core of the (TkOnly+TkTOF) Analysis                 --> submitting 5xSignalPoints jobs"
        print "  1 - Merge all output files and estimate backgrounds                --> submitting              5 jobs"
        print "  2 - Run the control plot macro                                     --> submitting              0 jobs"
        print "  3 - Run the Optimization macro based on best Exp Limit             --> submitting 5xSignalPoints jobs (OPTIONAL)"
        print "  4 - compute the limits from the cuts set in Analysis_Cuts.txt file --> submitting 5xSignalPoints jobs (must edit by hand Analysis_Cuts.txt)"
        print "  5 - Run the exclusion plot macro                                   --> submitting              0 jobs"
	sys.exit()

elif sys.argv[1]=='0':	
        print 'ANALYSIS'
        FarmDirectory = "FARM"
        JobName = "HscpAnalysis"
	LaunchOnCondor.Jobs_RunHere = 1
	LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)	
        f= open('Analysis_Samples.txt','r')
        index = -1
        for line in f :
           index+=1
           vals=line.split(',')
           if("CMSSW_5_3" in (vals[0].replace('"',''))):
              for Type in AnalysesToRun:
                 if(int(vals[1])>=2 and skipSamples(Type, vals[2])==True):continue
                 filename = vals[2].strip()[1:-1] + '_' + vals[3].strip()[1:-1] + '.root'
                 if  (Type==0):
                    makedirs("Results/Type0")
                    LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/Analysis_Step3.C", [('Histos_Type0_' +filename, "Results/Type0/Histos_" + filename)], '"ANALYSE_'+str(index)+'_to_'+str(index)+'"'  , 0, '"dedxASmi"'  ,'"dedxHarm2"'  , '"combined"', 0.0, 0.0, 0.0, 45, 2.1])
                 elif(Type==2):
                    makedirs("Results/Type2")
                    LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/Analysis_Step3.C", [('Histos_Type2_' +filename, "Results/Type2/Histos_" + filename)], '"ANALYSE_'+str(index)+'_to_'+str(index)+'"'  , 2, '"dedxASmi"'  ,'"dedxHarm2"'  , '"combined"', 0.0, 0.0, 0.0, 45, 2.1])
                 elif(Type==3):
                    makedirs("Results/Type3")
                    LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/Analysis_Step3.C", [('Histos_Type3_' +filename, "Results/Type3/Histos_" + filename)], '"ANALYSE_'+str(index)+'_to_'+str(index)+'"'  , 3, '"dedxASmi"'  ,'"dedxHarm2"'  , '"combined"', 0.0, 0.0, 0.0, 80, 2.1, 15, 15])
                 elif(Type==4):
                    makedirs("Results/Type4")
                    LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/Analysis_Step3.C", [('Histos_Type4_' +filename, "Results/Type4/Histos_" + filename)], '"ANALYSE_'+str(index)+'_to_'+str(index)+'"'  , 4, '"dedxASmi"'  ,'"dedxHarm2"'  , '"combined"', 0.0, 0.0, 0.0, 45, 2.1])
                 elif(Type==5):
                    makedirs("Results/Type5")
                    LaunchOnCondor.SendCluster_Push(["FWLITE", os.getcwd()+"/Analysis_Step3.C", [('Histos_Type5_' +filename, "Results/Type5/Histos_" + filename)], '"ANALYSE_'+str(index)+'_to_'+str(index)+'"'  , 5, '"dedxASmi"'  ,'"dedxHarm2"'  , '"combined"', 0.0, 0.0, 0.0, 45, 2.1])
        f.close()
	LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='1':
        print 'MERGING FILE AND PREDICTING BACKGROUNDS'  
        FarmDirectory = "FARM"
        JobName = "HscpPred"
        LaunchOnCondor.Jobs_RunHere = 1
        LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)
        for Type in AnalysesToRun:
           Path = "Results/Type"+str(Type)+"/"
           os.system('rm -f ' + Path + 'Histos_NoPred.root')
           os.system('hadd -f ' + Path + 'Histos_NoPred.root ' + Path + '*.root')
           Output_Files = ['Histos.root', 'Info_Data8TeV.txt', 'Info_MCTr_8TeV.txt', 'Info_Data8TeV_Flip.txt', 'Info_MCTr_8TeV_Flip.txt', 'Info_Data7TeV.txt', 'Info_MCTr_7TeV.txt', 'Info_Data7TeV_Flip.txt', 'Info_MCTr_7TeV_Flip.txt']
           Remaps = [('Type' + str(Type) + '_' + File, Path + File) for File in Output_Files]
           LaunchOnCondor.SendCluster_Push(["ROOT", os.getcwd()+"/Analysis_Step4.C", Remaps, '"Type' + str(Type) + '"'])
        LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='2':
        print 'PLOTTING'
	os.system('root Analysis_Step5.C++ -l -b -q')

elif sys.argv[1]=='3':
        print 'OPTIMIZATION & LIMIT COMPUTATION'
        FarmDirectory = "FARM"
        JobName = "HscpLimits"
        LaunchOnCondor.Jobs_RunHere = 1
        LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)

        f= open('Analysis_Samples.txt','r')
        for line in f :
           vals=line.split(',')
           if(int(vals[1])!=2):continue
           for Type in AnalysesToRun:
              if(int(vals[1])>=2 and skipSamples(Type, vals[2])==True):continue
              if(vals[2].find("8TeV")<0):continue
              Path = "Results/Type"+str(Type)+"/"
              LaunchOnCondor.SendCluster_Push(["ROOT", os.getcwd()+"/Analysis_Step6.C", '"OPTIMIZE"', '"'+Path+'"', vals[2] ])
        f.close()
        LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='4':
        print 'LIMIT COMPUTATION (ONLY)'
        FarmDirectory = "FARM"
        JobName = "HscpLimits"
        LaunchOnCondor.Jobs_Queue   = '8nh'
        LaunchOnCondor.Jobs_RunHere = 1
        LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)

        make_excl_dirs()

        f = open('Analysis_Samples.txt','r')
        for line in f :
           vals=line.split(',')
           if(int(vals[1])<2):continue
           for Type in AnalysesToRun:            
              if(int(vals[1])>=2 and skipSamples(Type, vals[2])==True):continue     
              skip = False
              TypeStr = "Type"+str(Type)
              Path = "Results/"+TypeStr+"/"

              #skip 8TeV samples that have already been processed together with the  7TeV (since for each sample we do 7TeV+8TeV+Comb)
              if(vals[2].find("8TeV")>=0):
                  key = vals[2]
                  key = key.replace("8TeV","7TeV")
                  f2= open('Analysis_Samples.txt','r')
                  for line2 in f2 :                     
                     vals2=line2.split(',')
                     if(vals2[1]==vals[1] and vals2[2] == key): skip = True;
                  if(skip==True): continue;
                  f2.close()
              #print vals[2] + "   " + str(skip)
              pre_7TeV = vals[2].strip().replace('"', '')
              Files_7TeV = [pre_7TeV + ".info", pre_7TeV + ".txt", pre_7TeV + ".dat", pre_7TeV + ".log"]
              Remaps = [(TypeStr + "_" + File, Path + "EXCLUSION7TeV/" + File) for File in Files_7TeV]

              pre_8TeV = pre_7TeV.replace("7TeV", "8TeV")
              Files_8TeV = [pre_8TeV + ".info", pre_8TeV + ".txt", pre_8TeV + ".dat", pre_8TeV + ".log"]
              Remaps.extend([(TypeStr + "_" + File, Path + "EXCLUSION8TeV/" + File) for File in Files_8TeV])

              pre_Comb = pre_7TeV.replace("_7TeV", "")
              Files_Comb = [pre_Comb + ".txt", pre_Comb + ".dat", pre_Comb + ".log"]
              Remaps.extend([(TypeStr + "_" + File, Path + "EXCLUSIONCOMB/" + File) for File in Files_Comb])
              
              LaunchOnCondor.SendCluster_Push(["ROOT", os.getcwd()+"/Analysis_Step6.C", Remaps, '"COMBINE"', '"'+Path+'"', vals[2] ]) #compute 2011, 2012 and 2011+2012 in the same job
        f.close()
        LaunchOnCondor.SendCluster_Submit()

elif sys.argv[1]=='5':
        print 'EXCLUSION'
        os.system('sh Analysis_Step6.sh')
else:
	print 'Unknown case: use an other argument or no argument to get help'


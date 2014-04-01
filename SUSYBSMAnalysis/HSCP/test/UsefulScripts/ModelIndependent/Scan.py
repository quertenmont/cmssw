#!/usr/bin/env python

import urllib
import string
import os
import sys
import LaunchOnCondor
import glob
import ROOT


#beta = [0.05, 0.10,0.15,0.20,0.25,0.30,0.35,0.40,0.45,0.50,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.90,0.95,0.99]
#mass = [100,126,156,200,247,308,370,432,494]



Rfile = ROOT.TFile("pMSSM_ctauGT10m.root")
Rtree = Rfile.Get('MCMC') 

FarmDirectory = "FARM"
JobName = "pMSSMGen"
LaunchOnCondor.Jobs_NEvent = 1000
LaunchOnCondor.Jobs_Skip = 0
LaunchOnCondor.SendCluster_Create(FarmDirectory, JobName)


i = 0
for Revent in Rtree:     
     m = Revent.mw1
#     if(Revent.itr!=951686):continue
     print "%8i --> M=%6.2E ctau=%6.2E " % (Revent.itr , Revent.mw1, Revent.ctau)
     i = i+1;   
     if(i>100):continue

     workingDir = 'InFiles/%i/' % Revent.itr
     os.system('mkdir -p ' + workingDir)

     config_file=open('particles.txt','r')
     config_txt = config_file.read()
     config_file.close()
     config_txt = config_txt.replace("XXX_CHARGINOMASS_XXX"          ,str(m))
     config_file=open(workingDir+'particles.txt','w')
     config_file.write(config_txt)
     config_file.close()

     config_file=open('hscppythiapdt.tbl','r')
     config_txt = config_file.read()
     config_file.close()
     config_txt = config_txt.replace("XXX_CHARGINOMASS_XXX"          ,str(m))
     config_file=open(workingDir+'hscppythiapdt.tbl','w')
     config_file.write(config_txt)
     config_file.close()

     config_file=open('pMSSM_ctauGT10m_slhas/pMSSM12_MCMC1_'+str(int(Revent.chainno))+'_'+str(int(Revent.itr))+'.slha','r')
     config_txt = config_file.read()
     config_file.close()

#     config_txt = config_txt.replace("XXX_CHARGINOMASS_XXX"          ,str(m))
     config_file=open(workingDir+'spectrum.slha','w')
     config_file.write(config_txt)
     config_file.close()
     os.system("sed -i 's/^DECAY   1000024.*/DECAY   1000024     0.000000000000   # chargino1+ decays/' " + workingDir+"spectrum.slha")

     #LaunchOnCondor.Jobs_Count = i
     Command =  ["CMSSW", "SimpleGenSim_cfg.py"]
     Command += ["XXX_OUTFILE_XXX", os.getcwd()+'/'+workingDir+"out.root", "XXX_CHARGINOMASS_XXX", str(m), "XXX_SLHA_XXX", 'pMSSM_Scan/'+workingDir+'spectrum.slha', "XXX_PARTICLES_XXX", 'pMSSM_Scan/'+workingDir+'particles.txt', "XXX_PYTHIATABLE_XXX", 'pMSSM_Scan/'+workingDir+'hscppythiapdt.tbl']
     LaunchOnCondor.SendCluster_Push  (Command)
LaunchOnCondor.SendCluster_Submit()

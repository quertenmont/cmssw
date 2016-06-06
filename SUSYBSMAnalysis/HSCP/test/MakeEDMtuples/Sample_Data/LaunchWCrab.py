#!/bin/env python

import urllib
import string
import os,sys,time
import SUSYBSMAnalysis.HSCP.LaunchOnCondor as LaunchOnCondor
import glob
import commands
import json
import collections

PrimaryDatasetsToMatch = "/*/Run2016*-PromptReco-v*/AOD"
GlobalTag = "80X_dataRun2_Prompt_v8"
StorageSite = 'T2_BE_UCL'
AllLumisFile = "Cert_271036-274240_13TeV_PromptReco_Collisions16_JSON.txt"

LumiFiles = []
FilesToPush = []

def getRunNum(lumiFileName):
   return lumiFileName.split('_')[1].split('.')[0]

def splitLumiFileByRuns (lumiFileName):
   globalLumiFile = open(lumiFileName)
   runList = json.load(globalLumiFile).items()
   runList.sort()
   for run in runList:
      fileName = "Run_%s.json" % str(run[0])
      LumiFiles.append(fileName)
      f = open (fileName, 'w')
      f.write("{\"%s\": %s}" % (str(run[0]), str(run[1])))
      f.close()

def findDatasetsForRun (lumiFileName, dataset):
   runNumber = getRunNum (lumiFileName)
   return os.popen("das_client --limit 0 --query 'dataset dataset=%s run=%s' | grep -E '(/SingleMuon/|/DoubleMuon/)|(/MET/)'" % (dataset, runNumber)).read().split()

def createCrabConfigFile (lumiFileName, dataset):
   appendString = "MET"
   if dataset.find("SingleMu") != -1:
      appendString = "SingleMuon"
   elif dataset.find("DoubleMu") != -1:
      appendString = "DoubleMuon"

   runNumber    = getRunNum(lumiFileName)
   confFileName = "crab3_%s_%s.py" % (runNumber, appendString)
   FilesToPush.append(confFileName)
   f = open(confFileName, "w")
   f.write("from WMCore.Configuration import Configuration\n")
   f.write("config = Configuration()\n")
   f.write("config.section_('General')\n")
   f.write("config.General.requestName = 'Run%s_%s'\n" % (runNumber, appendString))
   f.write("config.General.transferOutputs = True\n")
   f.write("config.section_('JobType')\n")
   f.write("config.JobType.psetName = '../../HSCParticleProducer_Data_cfg.py'\n")
   f.write("config.JobType.pluginName = 'Analysis'\n")
   f.write("config.JobType.pyCfgParams = ['globalTag=%s']\n" % GlobalTag)
   f.write("config.JobType.outputFiles = ['HSCP.root']\n")
   f.write("config.section_('Data')\n")
   f.write("config.Data.inputDataset = '%s'\n" % dataset)
   f.write("config.Data.publication = False\n")
   f.write("config.Data.unitsPerJob = 50\n")
   f.write("config.Data.splitting = 'LumiBased'\n")
   f.write("config.Data.inputDBS = 'global'\n")
   f.write("config.Data.lumiMask = '%s'\n" % lumiFileName)
   f.write("config.section_('User')\n")
   f.write("config.section_('Site')\n")
   f.write("config.Site.storageSite = '%s'\n" % StorageSite)
   f.close()

if sys.argv[1] == '1':
   print "Splitting lumis ..."
   splitLumiFileByRuns (AllLumisFile)
   print "Done!\nWriting crab3 config files ...\n============================"
   for lumiFile in LumiFiles:
      matchedDatasets = findDatasetsForRun (lumiFile, PrimaryDatasetsToMatch)
      matchedDatasets.sort()
      print "----------------------------"
      for dataset in matchedDatasets:
         createCrabConfigFile (lumiFile, dataset)
         print "Run %s: %s ready!" % (getRunNum(lumiFile), dataset)

   print "Done!\nSubbmitting jobs on crab!\n============================"
   for confFile in FilesToPush:
      print "Submitting %s ..." % confFile
      os.system("crab submit -c %s" % confFile)
   

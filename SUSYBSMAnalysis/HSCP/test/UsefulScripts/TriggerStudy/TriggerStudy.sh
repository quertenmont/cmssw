#!/bin/bash
root -l -b << EOF
  TString makeshared(gSystem->GetMakeSharedLib());
  makeshared.ReplaceAll("-W ", "-Wno-deprecated-declarations -Wno-deprecated -Wno-unused-local-typedefs -Wno-attributes ");
  makeshared.ReplaceAll("-Woverloaded-virtual ", " ");
  makeshared.ReplaceAll("-Wshadow ", " -std=c++0x -D__USE_XOPEN2K8 ");
  cout << "Compilling with the following arguments: " << makeshared << endl;
  gSystem->SetMakeSharedLib(makeshared);
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();
  gSystem->Load("libDataFormatsFWLite.so");
  gSystem->Load("libDataFormatsHepMCCandidate.so");
  gSystem->Load("libDataFormatsCommon.so");
  gSystem->Load("libDataFormatsTrackerRecHit2D.so");
  gSystem->Load("libAnalysisDataFormatsSUSYBSMObjects.so");
  .x TriggerStudy.C+("", "")

  .x TriggerStudy.C+("GMStau_13TeV_M1029WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/GMStau_13TeV_M1029.root")
  .x TriggerStudy.C+("GMStau_13TeV_M156WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/GMStau_13TeV_M156.root")
  .x TriggerStudy.C+("GMStau_13TeV_M1599WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/GMStau_13TeV_M1599.root")
  .x TriggerStudy.C+("GMStau_13TeV_M308WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/GMStau_13TeV_M308.root")
  .x TriggerStudy.C+("GMStau_13TeV_M651WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/GMStau_13TeV_M651.root")
  .x TriggerStudy.C+("Gluino_13TeV_M1200WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Gluino_13TeV_M1200.root")
  .x TriggerStudy.C+("Gluino_13TeV_M1800WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Gluino_13TeV_M1800.root")
  .x TriggerStudy.C+("Gluino_13TeV_M200WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Gluino_13TeV_M200.root")
  .x TriggerStudy.C+("Gluino_13TeV_M2400WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Gluino_13TeV_M2400.root")
  .x TriggerStudy.C+("Gluino_13TeV_M600WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Gluino_13TeV_M600.root")
  .x TriggerStudy.C+("PPStau_13TeV_M1029WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/PPStau_13TeV_M1029.root")
  .x TriggerStudy.C+("PPStau_13TeV_M156WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/PPStau_13TeV_M156.root")
  .x TriggerStudy.C+("PPStau_13TeV_M1599WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/PPStau_13TeV_M1599.root")
  .x TriggerStudy.C+("PPStau_13TeV_M308WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/PPStau_13TeV_M308.root")
  .x TriggerStudy.C+("PPStau_13TeV_M651WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/PPStau_13TeV_M651.root")
  .x TriggerStudy.C+("Stop_13TeV_M1200WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Stop_13TeV_M1200.root")
  .x TriggerStudy.C+("Stop_13TeV_M1800WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Stop_13TeV_M1800.root")
  .x TriggerStudy.C+("Stop_13TeV_M200WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Stop_13TeV_M200.root")
  .x TriggerStudy.C+("Stop_13TeV_M2400WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Stop_13TeV_M2400.root")
  .x TriggerStudy.C+("Stop_13TeV_M600WiTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WiTS/FARM_EDM/outputs/Stop_13TeV_M600.root")


  .x TriggerStudy.C+("GMStau_13TeV_M1029WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/GMStau_13TeV_M1029.root")
  .x TriggerStudy.C+("GMStau_13TeV_M156WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/GMStau_13TeV_M156.root")
  .x TriggerStudy.C+("GMStau_13TeV_M1599WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/GMStau_13TeV_M1599.root")
  .x TriggerStudy.C+("GMStau_13TeV_M308WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/GMStau_13TeV_M308.root")
  .x TriggerStudy.C+("GMStau_13TeV_M651WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/GMStau_13TeV_M651.root")
  .x TriggerStudy.C+("Gluino_13TeV_M1200WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Gluino_13TeV_M1200.root")
  .x TriggerStudy.C+("Gluino_13TeV_M1800WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Gluino_13TeV_M1800.root")
  .x TriggerStudy.C+("Gluino_13TeV_M200WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Gluino_13TeV_M200.root")
  .x TriggerStudy.C+("Gluino_13TeV_M2400WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Gluino_13TeV_M2400.root")
  .x TriggerStudy.C+("Gluino_13TeV_M600WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Gluino_13TeV_M600.root")
  .x TriggerStudy.C+("PPStau_13TeV_M1029WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/PPStau_13TeV_M1029.root")
  .x TriggerStudy.C+("PPStau_13TeV_M156WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/PPStau_13TeV_M156.root")
  .x TriggerStudy.C+("PPStau_13TeV_M1599WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/PPStau_13TeV_M1599.root")
  .x TriggerStudy.C+("PPStau_13TeV_M308WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/PPStau_13TeV_M308.root")
  .x TriggerStudy.C+("PPStau_13TeV_M651WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/PPStau_13TeV_M651.root")
  .x TriggerStudy.C+("Stop_13TeV_M1200WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Stop_13TeV_M1200.root")
  .x TriggerStudy.C+("Stop_13TeV_M1800WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Stop_13TeV_M1800.root")
  .x TriggerStudy.C+("Stop_13TeV_M200WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Stop_13TeV_M200.root")
  .x TriggerStudy.C+("Stop_13TeV_M2400WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Stop_13TeV_M2400.root")
  .x TriggerStudy.C+("Stop_13TeV_M600WoTS", "/nfs/home/fynu/quertenmont/scratch/15_05_13_HSCP_SampleProd/CMSSW_7_4_2/src/SUSYBSMAnalysis/HSCP/test/UsefulScripts/SampleProduction/WoTS/FARM_EDM/outputs/Stop_13TeV_M600.root")


EOF

#!/bin/bash

if [ -z "$2" ]; then
   arg2="EMPTY"
else
   arg2="$2"
fi

root -l -b << EOF
  TString makeshared(gSystem->GetMakeSharedLib());
  TString dummy = makeshared.ReplaceAll("-W ", "-D__USE_XOPEN2K8 ");
  TString dummy = makeshared.ReplaceAll("-Wshadow ", "");
  gSystem->SetMakeSharedLib(makeshared);
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();
  gSystem->Load("libDataFormatsFWLite.so");
  gSystem->Load("libDataFormatsHepMCCandidate.so");
  gSystem->Load("libDataFormatsCommon.so");
  gSystem->Load("libDataFormatsTrackerRecHit2D.so");
  gSystem->Load("libAnalysisDataFormatsSUSYBSMObjects.so");
  .x MakePlot.C+("$1", "$arg2");
EOF

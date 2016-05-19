#! /usr/bin/env python

import os

indir = "Results/"
jobfilesdir = "FARM/inputs/" 
cmdfile = "FARM/inputs/HscpAnalysis.cmd"
samplesFiles = "Analysis_Samples.txt"
types = set()

for root, dirs, files in os.walk(indir):
    if "Type" not in root: continue
    anaType = int(root.split("/")[-1].replace("Type",""))
    types.add(anaType)

todo = []
with open(samplesFiles) as ifile:
    for iline, l in enumerate(ifile):
        line = l.strip()
        if len(line)==0 or line[0]=='#' : continue
        spl = [l.strip().strip('"') for l in line.split(",")]
        expectedFileName= "Histos_{}_{}.root".format(spl[2], spl[3])
        sampleString = "ANALYSE_{}_to_{}".format(iline, iline)
        for t in types:
            fp = indir+"Type{}".format(t)+"/"+expectedFileName
            if not os.path.isfile(fp) or os.path.getsize(fp)<1024:
                typeString = ", {},".format(t)
                for root, _, files in os.walk(jobfilesdir):
                    for f in files:
                        if "_HscpAnalysis.sh" not in f: continue
                        contents = open(os.path.join(root, f)).read()
                        if sampleString not in contents or typeString not in contents:
                            continue
                        todo.append(f)

with open(cmdfile) as f:
    for l in f:
        line = l.strip()
        for t in todo:
            if t in line:
                print line
                #os.system(line)
                break


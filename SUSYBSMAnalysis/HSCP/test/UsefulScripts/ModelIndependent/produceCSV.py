import re

path="pictures/"
Signal="PPStau"
MassesSign=[100,126,156,200,247,308,370,432,494,557]
#MassesSign=[100,494,557]

path_modI=path
path_std=path+"Std_"
path_pap=path+"Pap_"
outputname="all_"+Signal+".csv"

output=open(outputname,"w")

def extract_data(pathn,output,MassesSign,type):
	for M in MassesSign:
		filename=pathn+Signal+str(M)+".txt"
#			        pictures/PPStau308.txt M>  0 Efficiencies: Trigger= 92.78%+-  0.29%  Presel= 84.59%+-  0.27% Offline= 40.14%+-  0.13%
		pat = re.compile(r" +pictures.+([0-9]{3})\.txt M>(.{3}) Efficiencies: Trigger= (.+)\%\+-(.+)\%  Presel= (.+)\%\+-(.+)\% Offline= (.+)\%\+-(.+)\%")
		with open(filename) as f:
			for line in f:
				(fname, mcut, EffT, ErrEffT,EffPS, ErrEffPS, EffS, ErrEffS) = pat.match(line).groups()
				output.write(str(fname)+","+str(type)+","+str(mcut)+","+str(EffT)+","+str(ErrEffT)+","+str(EffPS)+","+str(ErrEffPS)+","+str(EffS)+","+str(ErrEffS)+"\n")
				#print str(fname)+","+str(type)+","+str(mcut)+","+str(EffT)+","+str(ErrEffT)+","+str(EffPS)+","+str(ErrEffPS)+","+str(EffS)+","+str(ErrEffS)

#print "Ms,type,Mcut,EffT,ErrEffT,Eff,ErrEff"
output.write("#Ms,type,Mcut,EffT,ErrEffT,Eff,ErrEff\n")
extract_data(path_modI,output, MassesSign,1)   #type=1 is model independant
extract_data(path_std ,output, MassesSign,2)   #type=2 is Standard Analysis

# filenamepaper="8TeVPaperAccept_"+Signal+".csv"
# with open(filenamepaper) as f2:
# 	for line in f2:
# 		output.write(line);

// Original Author:  Loic Quertenmont

#ifndef HSCP_ANALYSIS_GLOBAL
#define HSCP_ANALYSIS_GLOBAL

//Include widely used in all the codes
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "TROOT.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TCutG.h" 
#include "TDCacheFile.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "TObject.h"
#include "TPaveText.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TRandom3.h"
#include "TTree.h"

//double IntegratedLuminosity13TeV               = 72.63; //pb
//double IntegratedLuminosity13TeV               = 84.557; //pb
double IntegratedLuminosity13TeV15             = 2490.518; //2439.264; //pb
//double IntegratedLuminosity13TeV16             = 4002.946; // pb
double IntegratedLuminosity13TeV16             = 7696.76;  // pb

double               SQRTS                     = 13;
double               SQRTS15                   = 1315;
double               SQRTS16                   = 1316;
double               SQRTS1615                 = 131615;
int                  RunningPeriods            = 1;
double IntegratedLuminosity                    = IntegratedLuminosity13TeV15; //pb
double IntegratedLuminosityBeforeTriggerChange =    0; //pb


string IntegratedLuminosityFromE(double SQRTS_){
  char LumiText[1024];

  if(SQRTS_==13 || SQRTS_==1315)
     sprintf(LumiText,"2015: %1.1f fb^{-1} (%1.0f TeV)", 0.001*IntegratedLuminosity13TeV15, 13.0);
  else if(SQRTS_==1316.0)
     sprintf(LumiText,"2016: %1.1f fb^{-1} (%1.0f TeV)", 0.001*IntegratedLuminosity13TeV16, 13.0);
  else if(SQRTS_==131615 || SQRTS_==131516)
     sprintf(LumiText,"2016: %1.1f fb^{-1}   2015: %1.1f fb^{-1} (%1.0f TeV)", 0.001*IntegratedLuminosity13TeV16, 0.001*IntegratedLuminosity13TeV15, 13.0);
  else sprintf(LumiText, "unknown energy and int. lumi");
//  if(SQRTS_==78 || SQRTS_==87)sprintf(LumiText,"#sqrt{s} = %1.0f TeV, L = %1.1f fb^{-1}   #sqrt{s} = %1.0f TeV, L = %1.1f fb^{-1}", 7.0, 0.001*IntegratedLuminosity7TeV,8.0, 0.001*IntegratedLuminosity8TeV);
  return LumiText;
}

// Type of the analysis
int		   TypeMode         = 0; //0 = Tracker-Only analysis (used in 2010 and 2011 papers)
					 //1 = Tracker+Muon analysis (used in 2010 paper)
                                         //2 = Tracker+TOF  analysis (used in 2011 paper)
                                         //3 = TOF Only     analysis (to be used in 2012 paper)
                                         //4 = Q>1          analysis (to be used in 2012 paper)
                                         //5 = Q<1          analysis (to be used in 2012 paper)
                                         //? do not hesitate to define your own --> TOF-Only, mCHAMPs, fractional charge

// directory where to find the EDM files --> check the function at the end of this file, to see how it is defined interactively
std::string BaseDirectory = "undefined... Did you call InitBaseDirectory() ? --> ";


// binning for the pT, mass, and IP distributions
double             PtHistoUpperBound   = 1200;
double             MassHistoUpperBound = 3000;
int                MassNBins           = 300;
double             IPbound             = 1.0;

// Thresholds for candidate preselection --> note that some of the followings can be replaced at the beginning of Analysis_Step1_EventLoop function
double             GlobalMaxEta       =   2.1;    // cut on inner tracker track eta
double             GlobalMaxV3D       =   99999;  //0.50;   // cut on 3D distance (cm) to closest vertex
double             GlobalMaxDZ        =   0.50;   // cut on 1D distance (cm) to closest vertex in "Z" direction
double             GlobalMaxDXY       =   0.50;   // cut on 2D distance (cm) to closest vertex in "R" direction
double             GlobalMaxChi2      =   5.0;    // cut on Track maximal Chi2/NDF
int                GlobalMinQual      =   2;      // cut on track quality (2 meaning HighPurity tracks)
unsigned int       GlobalMinNOH       =   8;//7AMSB;      // cut on number of (valid) track pixel+strip hits 
int                GlobalMinNOPH      =   2;      // cut on number of (valid) track pixel hits 
double             GlobalMinFOVH      =   0.8;//0.0AMSB;    // cut on fraction of valid track hits
unsigned int       GlobalMaxNOMHTillLast = 99999;//1AMSB;     // cut on the number of missing hits from IP till last hit (excluding hits behind the last hit)
double             GlobalMinFOVHTillLast =-99999;//0.85AMSB;   // cut on the fraction of valid hits divided by total expected hits until the last one
unsigned int       GlobalMinNOM       =   6;//7AMSB;      // cut on number of dEdx hits (generally equal to #strip+#pixel-#ClusterCleaned hits, but this depend on estimator used)
double             GlobalMinNDOF      =   8;      // cut on number of     DegreeOfFreedom used for muon TOF measurement
double             GlobalMinNDOFDT    =   6;      // cut on number of DT  DegreeOfFreedom used for muon TOF measurement
double             GlobalMinNDOFCSC   =   6;      // cut on number of CSC DegreeOfFreedom used for muon TOF measurement
double             GlobalMaxTOFErr    =   0.15;//0.07;   // cut on error on muon TOF measurement
double             GlobalMaxPterr     =   0.25;//0.50;//0.25;   // cut on error on track pT measurement 
double             GlobalMaxTIsol     =  50;      // cut on tracker isolation (SumPt)
double             GlobalMaxRelTIsol  =  9999999; // cut on relative tracker isolation (SumPt/Pt)
double             GlobalMaxEIsol     =  0.30;    // cut on calorimeter isolation (E/P)
double             GlobalMinPt        =  55.00;   // cut on pT    at PRE-SELECTION
double             GlobalMinIs        =   0.0;    // cut on dEdxS at PRE-SELECTION (dEdxS is generally a  discriminator)
double             GlobalMinIm        =   0.0;    // cut on dEdxM at PRE-SELECTION (dEdxM is generally an estimator    )
double             GlobalMinTOF       =   1.0;    // cut on TOF   at PRE-SELECTION
const int          MaxPredBins        =   6;      // The maximum number of different bins prediction is done in for any of the analyses (defines array size)
int                PredBins           =   0;      //  How many different bins the prediction is split in for analysis being run, sets how many histograms are actually initialized.

// dEdx related variables, Name of dEdx estimator/discriminator to be used for selection (dEdxS) and for mass reconstruction (dEdxM)
// as well as the range for the dEdx variable and K/C constant for mass reconstruction
std::string        dEdxS_Label     = "dedxASmi";
double             dEdxS_UpLim     = 1.0;
std::string        dEdxS_Legend    = "I_{as}";
std::string        dEdxM_Label     = "dedxHarm2";
double             dEdxM_UpLim     = 30.0;
std::string        dEdxM_Legend    = "I_{h} (MeV/cm)";
//double             dEdxK_Data      = 2.779;
//double             dEdxC_Data      = 2.879;
//double             dEdxK_MC        = 2.683;
//double             dEdxC_MC        = 2.453;
// below are the new constants computed with harm2_SP_in for both MC and Data USED FOR THE PAS
//double             dEdxK_Data      = 2.535;
//double             dEdxC_Data      = 3.339;
//double             dEdxK_MC        = 2.535;
//double             dEdxC_MC        = 3.339;
// below are the new constants computed with hhybrid2_SP_in for both MC and Data USED FOR THE PAPER
double             dEdxK_Data15    = 2.535;
double             dEdxC_Data15    = 3.339;
double             dEdxK_MC15      = 2.535;
double             dEdxC_MC15      = 3.339;

double             dEdxK_Data16    = 2.580;
double             dEdxC_Data16    = 3.922;
double             dEdxK_MC16      = 2.935;
double             dEdxC_MC16      = 3.197;

double             dEdxK_Data      = dEdxK_Data16;
double             dEdxC_Data      = dEdxC_Data16;
double             dEdxK_MC        = dEdxK_MC16;
double             dEdxC_MC        = dEdxC_MC16;


//below is for harm2_SP where 20% of the lower dEdx hits are drop
//double             dEdxK_Data      = 2.40;
//double             dEdxC_Data      = 3.45;
//double             dEdxK_MC        = 2.78;
//double             dEdxC_MC        = 3.35;

// TOF object to be used for combined, DT and CSC TOF measurement
std::string        TOF_Label       = "combined";
std::string        TOFdt_Label     = "dt";
std::string        TOFcsc_Label    = "csc";

//Variables used in the TOF only HSCP search
float              DTRegion        =   0.9;  //Define the dividing line between DT and 
float              CSCRegion       =   0.9;  //CSC regions of CMS
float              CosmicMinDz     =   70.;  //Min dz displacement to be tagged as cosmic muon
float              CosmicMaxDz     =   120.; //Max dz displacement for cosmic tagged tracks
double             minSegEtaSep    =  0.1;   //Minimum eta separation between SA track and muon segment on opposite side of detector
const int          DzRegions       =  6;     //Number of different Dz side regions used to make cosmic background prediction
int                minMuStations   =  2;


//for initializing PileupReweighting utility.
const   float TrueDist2012_f[100] = {6.53749e-07 ,1.73877e-06 ,4.7972e-06 ,1.57721e-05 ,2.97761e-05 ,0.000162201 ,0.000931952 ,0.00272619 ,0.0063166 ,0.0128901 ,0.0229009 ,0.0355021 ,0.045888 ,0.051916 ,0.0555598 ,0.0580188 ,0.059286 ,0.0596022 ,0.059318 ,0.0584214 ,0.0570249 ,0.0553875 ,0.0535731 ,0.0512788 ,0.0480472 ,0.0436582 ,0.0382936 ,0.0323507 ,0.0262419 ,0.0203719 ,0.0151159 ,0.0107239 ,0.00727108 ,0.00470101 ,0.00288906 ,0.00168398 ,0.000931041 ,0.000489695 ,0.000246416 ,0.00011959 ,5.65558e-05 ,2.63977e-05 ,1.23499e-05 ,5.89242e-06 ,2.91502e-06 ,1.51247e-06 ,8.25545e-07 ,4.71584e-07 ,2.79203e-07 ,1.69571e-07 ,1.04727e-07 ,6.53264e-08 ,4.09387e-08 ,2.56621e-08 ,1.60305e-08 ,9.94739e-09 ,6.11516e-09 ,3.71611e-09 ,2.22842e-09 ,1.3169e-09, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};  // MB xsec = 69.3mb
const   float TrueDist2012_XSecShiftUp_f[100] = {6.53749e-07 ,1.73877e-06 ,4.7972e-06 ,1.57721e-05 ,2.97761e-05 ,0.000162201 ,0.000931952 ,0.00272619 ,0.0063166 ,0.0128901 ,0.0229009 ,0.0355021 ,0.045888 ,0.051916 ,0.0555598 ,0.0580188 ,0.059286 ,0.0596022 ,0.059318 ,0.0584214 ,0.0570249 ,0.0553875 ,0.0535731 ,0.0512788 ,0.0480472 ,0.0436582 ,0.0382936 ,0.0323507 ,0.0262419 ,0.0203719 ,0.0151159 ,0.0107239 ,0.00727108 ,0.00470101 ,0.00288906 ,0.00168398 ,0.000931041 ,0.000489695 ,0.000246416 ,0.00011959 ,5.65558e-05 ,2.63977e-05 ,1.23499e-05 ,5.89242e-06 ,2.91502e-06 ,1.51247e-06 ,8.25545e-07 ,4.71584e-07 ,2.79203e-07 ,1.69571e-07 ,1.04727e-07 ,6.53264e-08 ,4.09387e-08 ,2.56621e-08 ,1.60305e-08 ,9.94739e-09 ,6.11516e-09 ,3.71611e-09 ,2.22842e-09 ,1.3169e-09, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // MB xsec = 73.5mb; observed in Z-->MuMu see https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData#Calculating_Your_Pileup_Distribu
const   float TrueDist2012_XSecShiftDown_f[100] = {6.53749e-07 ,1.73877e-06 ,4.7972e-06 ,1.57721e-05 ,2.97761e-05 ,0.000162201 ,0.000931952 ,0.00272619 ,0.0063166 ,0.0128901 ,0.0229009 ,0.0355021 ,0.045888 ,0.051916 ,0.0555598 ,0.0580188 ,0.059286 ,0.0596022 ,0.059318 ,0.0584214 ,0.0570249 ,0.0553875 ,0.0535731 ,0.0512788 ,0.0480472 ,0.0436582 ,0.0382936 ,0.0323507 ,0.0262419 ,0.0203719 ,0.0151159 ,0.0107239 ,0.00727108 ,0.00470101 ,0.00288906 ,0.00168398 ,0.000931041 ,0.000489695 ,0.000246416 ,0.00011959 ,5.65558e-05 ,2.63977e-05 ,1.23499e-05 ,5.89242e-06 ,2.91502e-06 ,1.51247e-06 ,8.25545e-07 ,4.71584e-07 ,2.79203e-07 ,1.69571e-07 ,1.04727e-07 ,6.53264e-08 ,4.09387e-08 ,2.56621e-08 ,1.60305e-08 ,9.94739e-09 ,6.11516e-09 ,3.71611e-09 ,2.22842e-09 ,1.3169e-09, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // MB xsec = 65.835mb

const float TrueDist2015_f[100] = {141899.9,    622421.5,    818662.3,    1253138,    2114164,    4937264,    1.627961e+07,    6.475337e+07,    1.817159e+08,    3.220406e+08,    4.16444e+08,    4.291912e+08,    3.663307e+08,    2.557795e+08,    1.447021e+08,    6.727821e+07,    2.736262e+07,    1.139197e+07,    5668287,    3025054,    1401679,    511811,    146735.4,    35294.85,    8269.747,    2235.301,    721.3224,    258.8397,    97.26937,    36.8714,    13.72746,    4.931709,    1.692407,    0.5518936,    0.1706013,    0.0499358,    0.01383391,    0.003626617,    0.0008996063,    0.0002111484,    4.689276e-05,    9.85392e-06,    1.959294e-06,    3.686198e-07,    6.562482e-08,    1.105342e-08,    1.762478e-09,    2.614969e-10,    4.768003e-11, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const   float TrueDist2015_XSecShiftUp_f[100] = {   119011.7,    575187.4,    742505.6,    1119922,    1752064,    3603643,    1.025708e+07,    3.766355e+07,    1.209274e+08,    2.47339e+08,    3.605043e+08,    4.119143e+08,    3.926434e+08,    3.147836e+08,    2.092462e+08,    1.150021e+08,    5.334319e+07,    2.238585e+07,    9895584,    5174317,    2843878,    1368343,    530329.1,    164616.8,    43068.57,    10705.86,    2954.413,    964.7733,    354.4973,    138.0159,    54.82292,    21.6373,    8.325977,    3.086652,    1.095163,    0.3706287,    0.1194443,    0.03663022,    0.0106862,    0.002965238,    0.0007825775,    0.0001964351,    4.689582e-05,    1.064815e-05,    2.29954e-06,    4.723236e-07,    9.227752e-08,    1.714345e-08,    3.031843e-09,    5.050254e-10, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const   float TrueDist2015_XSecShiftDown_f[100] = {166787.7,    674049,    913830.3,    1425367,    2638444,    7155207,    2.778629e+07,    1.094527e+08,    2.587231e+08,    3.982389e+08,    4.560547e+08,    4.197129e+08,    3.116313e+08,    1.836509e+08,    8.643619e+07,    3.42708e+07,    1.339142e+07,    6282790,    3245101,    1445026,    494664.3,    130033.8,    28603.11,    6332.219,    1680.587,    533.0118,    185.3784,    66.66396,    23.86613,    8.286534,    2.746725,    0.8620616,    0.2551907,    0.07113181,    0.01865672,    0.004603238,    0.001068332,    0.0002332118,    4.788436e-05,    9.247787e-06,    1.679908e-06,    2.870404e-07,    4.613098e-08,    6.975087e-09,    9.940954e-10,    1.331527e-10,    1.59604e-11, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

const float TrueDist2016_f[100] = {2344.23, 88123.74, 409003.8, 879273, 1375568, 1848254, 2989862, 1.503519e+07, 5.106283e+07, 1.131082e+08, 1.827391e+08, 2.390124e+08, 2.97999e+08, 3.78672e+08, 4.754178e+08, 5.64318e+08, 6.231244e+08, 6.450583e+08, 6.345138e+08, 5.950019e+08, 5.323917e+08, 4.566512e+08, 3.743531e+08, 2.890467e+08, 2.072878e+08, 1.375908e+08, 8.470642e+07, 4.832028e+07, 2.55258e+07, 1.260297e+07, 5908160, 2655838, 1146599, 475208, 189819.7, 74519.52, 30465.61, 14543.6, 9031.346, 7136.581, 6439.66, 6131.581, 5947.555, 5794.083, 5640.281, 5468.72, 5270.733, 5043.279, 4787.175, 4505.832, 4204.273, 3888.385, 3564.36, 3238.286, 2915.853, 2602.152, 2301.536, 2017.544, 1752.875, 1509.397, 1288.199, 1089.66, 913.5429, 759.0987, 625.1725, 510.3105, 412.86, 331.0589, 263.1128, 207.2588, 161.8149, 125.2156, 96.03566, 73.00296, 55.00244, 41.07302, 30.39934, 22.29995, 16.21346, 11.68366, 8.34474, 5.907139, 4.144494, 2.882008, 1.986313, 1.35684, 0.9186232, 0.6164155, 0.4099554, 0.2702253, 0.1765393, 0.1143095, 0.07335819, 0.04665943, 0.02941403, 0.0183778, 0.01138036, 0.006984602, 0.004248642, 0.002561422};
const float TrueDist2016_XSecShiftUp_f[100] = {1565.416, 70442.92, 359542.9, 723665.1, 1226079, 1659739, 2145943, 7764946, 3.165059e+07, 7.789126e+07, 1.41776e+08, 1.993707e+08, 2.4825e+08, 3.080973e+08, 3.877826e+08, 4.761554e+08, 5.519217e+08, 5.990589e+08, 6.145737e+08, 6.025616e+08, 5.657023e+08, 5.089868e+08, 4.407376e+08, 3.665906e+08, 2.893575e+08, 2.139097e+08, 1.473979e+08, 9.48264e+07, 5.696867e+07, 3.190343e+07, 1.672327e+07, 8309582, 3961676, 1820681, 806425.6, 344337.7, 142619, 58653.79, 25451.75, 12957.07, 8422.328, 6780.105, 6145.367, 5855.657, 5683.014, 5542.16, 5404.572, 5254.216, 5082.497, 4885.734, 4663.64, 4418.292, 4153.324, 3873.299, 3583.222, 3288.184, 2993.096, 2702.495, 2420.41, 2150.278, 1894.889, 1656.373, 1436.217, 1235.296, 1053.932, 891.9613, 748.8114, 623.5809, 515.1192, 422.1024, 343.1022, 276.6464, 221.2699, 175.5561, 138.1673, 107.8675, 83.53556, 64.17231, 48.90115, 36.96454, 27.71698, 20.61582, 15.21069, 11.13244, 8.082112, 5.820383, 4.157863, 2.946322, 2.071006, 1.44402, 0.998746, 0.6852147, 0.4663239, 0.3148021, 0.2108026, 0.140024, 0.09226074, 0.06030019, 0.03909376, 0.02514102};
const float TrueDist2016_XSecShiftDown_f[100] = {3188.354, 110833.6, 469232, 1056159, 1566064, 2061807, 5262816, 2.775641e+07, 7.99348e+07, 1.569102e+08, 2.263883e+08, 2.871538e+08, 3.668972e+08, 4.705356e+08, 5.739284e+08, 6.472878e+08, 6.778875e+08, 6.697927e+08, 6.276525e+08, 5.583891e+08, 4.740186e+08, 3.823082e+08, 2.878745e+08, 1.994568e+08, 1.269137e+08, 7.430528e+07, 3.994996e+07, 1.977887e+07, 9156130, 4028564, 1694750, 681452, 262385.5, 98214.64, 37673.82, 16696.22, 9789.711, 7547.696, 6768.311, 6436.7, 6238.568, 6069.915, 5896.415, 5699.18, 5469.607, 5205.621, 4909.531, 4586.47, 4243.191, 3887.181, 3526.012, 3166.88, 2816.286, 2479.821, 2162.045, 1866.436, 1595.392, 1350.301, 1131.63, 939.0542, 771.5973, 627.7775, 505.7508, 403.4446, 318.675, 249.2466, 193.0313, 148.0278, 112.4025, 84.51329, 62.92026, 46.3844, 33.85859, 24.47263, 17.51487, 12.41215, 8.709644, 6.051548, 4.163366, 2.836183, 1.913087, 1.277749, 0.8450182, 0.5533447, 0.3587849, 0.2303459, 0.1464315, 0.09217124, 0.05744642, 0.03545166, 0.02166286, 0.01310691, 0.007852163, 0.004657821, 0.002735775, 0.001591041, 0.0009161902, 0.0005223876, 0.0002949189, 0.0001648597};

const   float Pileup_MC_Fall11[100]= {1.45346E-01, 6.42802E-02, 6.95255E-02, 6.96747E-02, 6.92955E-02, 6.84997E-02, 6.69528E-02, 6.45515E-02, 6.09865E-02, 5.63323E-02, 5.07322E-02, 4.44681E-02, 3.79205E-02, 3.15131E-02, 2.54220E-02, 2.00184E-02, 1.53776E-02, 1.15387E-02, 8.47608E-03, 6.08715E-03, 4.28255E-03, 2.97185E-03, 2.01918E-03, 1.34490E-03, 8.81587E-04, 5.69954E-04, 3.61493E-04, 2.28692E-04, 1.40791E-04, 8.44606E-05, 5.10204E-05, 3.07802E-05, 1.81401E-05, 1.00201E-05, 5.80004E-06, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const   float Pileup_MC_Summer2012[100] = { 2.560E-06, 5.239E-06, 1.420E-05, 5.005E-05, 1.001E-04, 2.705E-04, 1.999E-03, 6.097E-03, 1.046E-02, 1.383E-02, 1.685E-02, 2.055E-02, 2.572E-02, 3.262E-02, 4.121E-02, 4.977E-02, 5.539E-02, 5.725E-02, 5.607E-02, 5.312E-02, 5.008E-02, 4.763E-02, 4.558E-02, 4.363E-02, 4.159E-02, 3.933E-02, 3.681E-02, 3.406E-02, 3.116E-02, 2.818E-02, 2.519E-02, 2.226E-02, 1.946E-02, 1.682E-02, 1.437E-02, 1.215E-02, 1.016E-02, 8.400E-03, 6.873E-03, 5.564E-03, 4.457E-03, 3.533E-03, 2.772E-03, 2.154E-03, 1.656E-03, 1.261E-03, 9.513E-04, 7.107E-04, 5.259E-04, 3.856E-04, 2.801E-04, 2.017E-04, 1.439E-04, 1.017E-04, 7.126E-05, 4.948E-05, 3.405E-05, 2.322E-05, 1.570E-05, 5.005E-06, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const   float Pileup_MC_Startup2015_25ns[100] = {4.8551E-07, 1.74806E-06, 3.30868E-06, 1.62972E-05, 4.95667E-05, 0.000606966, 0.003307249, 0.010340741, 0.022852296, 0.041948781, 0.058609363, 0.067475755, 0.072817826, 0.075931405, 0.076782504, 0.076202319, 0.074502547, 0.072355135, 0.069642102, 0.064920999, 0.05725576, 0.047289348, 0.036528446, 0.026376131, 0.017806872, 0.011249422, 0.006643385, 0.003662904, 0.001899681, 0.00095614, 0.00050028, 0.000297353, 0.000208717, 0.000165856, 0.000139974, 0.000120481, 0.000103826, 8.88868E-05, 7.53323E-05, 6.30863E-05, 5.21356E-05, 4.24754E-05, 3.40876E-05, 2.69282E-05, 2.09267E-05, 1.5989E-05, 4.8551E-06, 2.42755E-06, 4.8551E-07, 2.42755E-07, 1.21378E-07, 4.8551E-08, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};


// function used to define interactively the directory containing the EDM files
// you are please to add the line for your case and not touch the line of the other users
void InitBaseDirectory(){  
   char* analystTmp=getenv("USER");
   char* hostTmp   =getenv("HOSTNAME");
   char* remoteStoragePath =getenv("REMOTESTORAGEPATH");
   if(!hostTmp||!analystTmp)return;
   string analyst(analystTmp);
   string host   (hostTmp);
   if(getenv("PWD")!=NULL)host+=string(" PWD=") + getenv("PWD");

   // BaseDirectory is first set for the AAA protocol ...
   if(remoteStoragePath!=NULL){
      BaseDirectory = "root://cms-xrd-global.cern.ch/"+string(remoteStoragePath);
      printf("Accessing remote files using BaseDirectory = %s\n.", BaseDirectory.c_str());
   // if we give no path to remote storage site, BaseDirectory is then set via hostname
   }else if(host.find("ucl.ac.be")!=std::string::npos){
      //BaseDirectory = "/storage/data/cms/users/quertenmont/HSCP/CMSSW_4_2_8/12_08_16/"; //for run1
      BaseDirectory = "/nfs/user/quertenmont/HSCP/2015/"; //for run2
   }else if(host.find("cern.ch")!=std::string::npos){
      //BaseDirectory = "rfio:/castor/cern.ch/user/r/rybinska/HSCPEDMFiles/";
      //BaseDirectory = "root://eoscms//eos/cms/store/cmst3/user/querten/12_08_30_HSCP_EDMFiles/";    //for run1
      BaseDirectory = "root://eoscms//eos/cms/store/cmst3/user/querten/15_03_25_HSCP_Run2EDMFiles/";  //for run2
   }else if(host.find("fnal.gov")!=std::string::npos){
     BaseDirectory = "dcache:/pnfs/cms/WAX/11/store/user/lpchscp/2012HSCPEDMFiles/"; //for run1
   }else if(host.find("ingrid-ui")!=std::string::npos){
      BaseDirectory = "/storage/data/cms/store/user/querten/HSCP/2015/"; // run2 ingrid
   }else if(host.find(".cis.gov.pl")!=std::string::npos){
      BaseDirectory = "root://se.cis.gov.pl//cms/store/user/fruboes/HSCP/15_03_25_HSCP_Run2EDMFiles/"; // run2 Swierk
   }else{
      BaseDirectory = "dcache:/pnfs/cms/WAX/11/store/user/venkat12/2012Data/";  //for run1
      printf("YOUR MACHINE (%s) IS NOT KNOWN --> please add your machine to the 'InitBaseDirectory' function of 'Analysis_Global.h'\n", host.c_str());
      printf("HOST=%s  USER=%s\n",host.c_str(), analyst.c_str());
      printf("In the mean time, the directory containing the HSCP EDM file is assumed to be %s\n",BaseDirectory.c_str());
   }

   // BaseDirectory is defined a function of the username
//   if(analyst.find("querten")!=std::string::npos && host.find("ucl.ac.be")!=std::string::npos){
//      BaseDirectory = "/storage/data/cms/users/quertenmont/HSCP/CMSSW_4_2_3/11_11_01/";
//   }   
}


#endif

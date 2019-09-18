// #include "constants.h"
#include "Calibration.h"
const  	Float_t	Calibration::QARATIO_MIN = 0.0;
const  	Float_t	Calibration::QARATIO_MAX = 10.0;
const char 		*Calibration::EVTCHARA_PARAMS = "/lustre/nyx/hades/user/bkardan/param/centrality_epcorr_mar19_gen0_pass2_2019_06_pass2.root";
const char 		*Calibration::RATIO_PDF = "ratios.pdf";
const  float 	Calibration::XSHIFT = -19.4; 	
const  float 	Calibration::YSHIFT = 4.4; 	
const char 		*Calibration::PARAMS 				= "params.txt";			
const 	   int 	Calibration::EMPTYCELLS[]= {65,66,77,78};		
const  	   int 	Calibration::BADCELLS[]	= {41,42,43,52,53,54,55,64,67,68,76,79,88,89,90,91};		
const  char 	*Calibration::header		= "##############################################################################\n\
# Class:   HWallEventPlanePar\n\
# Context: WallEventPlaneProduction\n\
##############################################################################\n\
[WallEventPlanePar]\n\
//----------------------------------------------------------------------------\n\
fT1_cut:  Float_t  22 \n\
fT2_cut:  Float_t  30 ";			
const  char *Calibration::footer		= "fR0_cut:  Float_t  85 \n\
fZ1_cut_s:  Float_t  83 \n\
fZ1_cut_m:  Float_t  84 \n\
fZ1_cut_l:  Float_t  88 \n\
##############################################################################\n";			


ClassImp(HitMapsBlock);
ClassImp(TimeMoment);
ClassImp(Calibration);

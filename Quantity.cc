#include "Quantity.h"

//VIEW
const bool 		Quantity::VERBOSE 					= true;
const int 		Quantity::QA_SHIFT					= -20;
//FIT
const float  	Quantity::MIN_RELATIVE_PEAK_HEIGHT	= 0.05;
const int 		Quantity::ITER_N					= N_BINS/10;
const int 		Quantity::FLAT_N					= N_BINS/100;
const float 	Quantity::THRESHOLD					= 0.0025;
//QA
const int 		Quantity::CRITICAL_RATIO			= 40;
//HIST SETTINGS
const int 		Quantity::WIDTH_MIN 				= 0;
const int 		Quantity::WIDTH_MAX 				= 2500;
const int 		Quantity::RAWTIME_MIN 				= 0;
const int 		Quantity::RAWTIME_MAX 				= 2000;
const int 		Quantity::CHARGE_MIN 				= 0;
const int 		Quantity::CHARGE_MAX 				= 350;
const int 		Quantity::TIME_MIN					= 0;
const int 		Quantity::TIME_MAX					= 100;

const int 		Quantity::N_BINS 					= 400;
const string 	Quantity::WIDTH_STR					= "width";
const string 	Quantity::CHARGE_STR				= "charge";


ClassImp(Quantity);
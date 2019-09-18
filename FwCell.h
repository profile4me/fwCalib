#ifndef __FWCELL__
#define __FWCELL__

#include "Quantity.h"
#include <TRandom.h>
#include <map>
#include <vector>
#include <algorithm>

struct FwCalPars {
	float adcSlope;
	float adcOffset;
	ClassDef(FwCalPars,1);
};

class FwCell : public TNamed {
private:
	static const int		S_SIZE;
	static const int		M_SIZE;
	static const int		L_SIZE;
	static 		 TRandom	*r;

	int 					id;

	map<string,Quantity*>	*widthMap;
	TH1F					*curWidth;			//!
	TH2F					*curRawTimeVsWidth;	//!SetTit

	map<string,Quantity*>	*chargeMap;
	TH1F					*curCharge;			//!
	TH2F					*curTimeVsCharge;	//!

	map<int,FwCalPars*>		*calParsMap;

	FwCalPars				*calPars;			//!
	int 					fMode;				//!
public:
	FwCell() {
		id=-1;
		widthMap=0;
		chargeMap=0;
		calParsMap=0;
	}
	//fMode=0 - fill from hit cat
	//fMode=1 - by raw and cal pars
	FwCell(int id, int fMode=0) {
		this->id = id;
		this->SetName(Form("cell%d",id));
		this->fMode=fMode;
		widthMap=new map<string,Quantity*>;
		chargeMap=new map<string,Quantity*>;
		calParsMap=new map<int,FwCalPars*>;
	}
	float applyShift(float coord) {
		int size;
		if (id<144) size = S_SIZE;
		else if (id <208) size = M_SIZE;
		else size = L_SIZE;
		return  coord + r->Uniform(-size/2,size/2);
	}
	bool isGoodHit(float ch, float t) {
		bool res=true;
		if (id<144 && ch<83) res=false;
		else if (id<208 && ch<84) res = false;
		else if (ch<88) res=false; 
		if (t<22||t>30) res=false;
		return res;
	}
	void setFillMode(int fMode) {
		this->fMode=fMode;
	}
	void setCItem(string baseName) {
		string filePrefix = baseName.substr(0,7);
		if (!(*widthMap)[filePrefix]) {
			Quantity *q = new Quantity("width", id, baseName);
			(*widthMap)[filePrefix]=q;
			(*chargeMap)[filePrefix]=new Quantity("charge", id, baseName);
		}
		(*widthMap)[filePrefix]->setCurrent(curWidth, curRawTimeVsWidth);
		(*chargeMap)[filePrefix]->setCurrent(curCharge, curTimeVsCharge);
		
		if (fMode==1) {
			int day = (*widthMap)[filePrefix]->getDay();
			setCalPars(day);
			(*widthMap)[filePrefix]->reset();
			(*chargeMap)[filePrefix]->reset();
			if (id==0) printf("------>FwCell::setCItem(): chargeHists for %s for cell0 is cleaned. ADCparams setted as: {slope=%.2f, offset=%.2f)\n",filePrefix.data(),calPars->adcSlope,calPars->adcOffset);
		}
	}
	void fillRaw(float w, float rt) {
		curWidth->Fill(w);
		curRawTimeVsWidth->Fill(w, rt);
		if (fMode==1) {
			curCharge->Fill(w*calPars->adcSlope+calPars->adcOffset);
			curTimeVsCharge->Fill(w*calPars->adcSlope+calPars->adcOffset, rt);
		}
	}
	void fillCal(float ch, float t) {
		if (fMode==0) {
			curCharge->Fill(ch);
			curTimeVsCharge->Fill(ch, t);
		} else if (fMode==1) {
			//nothing; fill be filled in fillRaw()
		}
	}
	void fit() {
		for (auto &p : *widthMap) {
			p.second->fit();
		}
		for (auto &p : *chargeMap) {
			p.second->fit();
		}
	}
	TH1 *getWidthHist(const char *prefix, int histType) {
		return (*widthMap)[string(prefix)]->getHist(histType);
	}
	TF1 *getWidthFit(const char *prefix) {
		return (*widthMap)[string(prefix)]->getFit();
	}
	TH1 *getChargeHist(const char *prefix, int histType) {
		return (*chargeMap)[string(prefix)]->getHist(histType);
	}
	TF1 *getChargeFit(const char *prefix) {
		return (*chargeMap)[string(prefix)]->getFit();
	}
	int getFitStatus(const char *prefix) {
		return (*widthMap)[string(prefix)]->getFitStatus();
	}
	float getRatio(const string &cItem) {
		return (*widthMap)[cItem]->checkFit();
	}
	TPolyMarker *getSpectr(const char *prefix) {
		return (*widthMap)[string(prefix)]->getSpectr();
	}
	map<int,string> *formBeamCItemMap() {
		map<int,string> *beamMap = new map<int,string>;
		for (auto &p : *widthMap) {
			if (p.second->isBeamRun()) {
				(*beamMap)[p.second->getDay()]=p.first;
			}
		}
		return beamMap;
	}

	
	void calculatePars() {
		map<int,string> *beamCItems=formBeamCItemMap();
		cout << "beamCItems map: \n";
		for (auto &p : *beamCItems) {
			printf("\tday%d: %s\n",p.first, p.second.data());
		}
		for (auto &p : *beamCItems) {
			(*calParsMap)[p.first]=new FwCalPars;
			float slope,offset;
			if (getFitStatus(p.second.data())==1) {
				float peakZ1=(*widthMap)[p.second]->getPeakZ1();
				float peakZ2=(*widthMap)[p.second]->getPeakZ2();
				slope = 100.0/(peakZ2-peakZ1);
				offset = 100-slope*peakZ1;
			} else {
				slope=offset=0;
			}
			(*calParsMap)[p.first]->adcSlope=slope;
			(*calParsMap)[p.first]->adcOffset=offset;
		}
	}
	FwCalPars *getPars(int d) {
		return (*calParsMap)[d];
	}
	void setCalPars(int d) {
		calPars = (*calParsMap)[d];
		if (!calPars) {calPars->adcSlope=1; calPars->adcOffset=0;}
	}
	void setPars(float adcSlope, float adcOffset) {
		calPars=new FwCalPars;
		calPars->adcSlope = adcSlope;
		calPars->adcOffset=adcOffset;
	}
	// void checkFit(const char *prefix, float &minRatio, float &maxRatio) {
		// (*widthMap)[string(prefix)]->checkFit(minRatio,maxRatio);
	// }

	float getPeakZ1(const char *prefix) {
		return (*widthMap)[string(prefix)]->getPeakZ1();
	}
	void setPeakZ1(const char *prefix, float z1) {
		(*widthMap)[string(prefix)]->setPeakZ1(z1);
	}
	float getPeakZ2(const char *prefix) {
		return (*widthMap)[string(prefix)]->getPeakZ2();
	}
	void setPeakZ2(const char *prefix, float z2) {
		(*widthMap)[string(prefix)]->setPeakZ2(z2);
	}

	vector<float> *getWidthPeaks(const char *prefix) {
		return (*widthMap)[string(prefix)]->getPeaks();
	}
	vector<float> *getChargePeaks(const char *prefix) {
		return (*chargeMap)[string(prefix)]->getPeaks();
	}
	Quantity::NullPointerErr *checkNullPointers() {
		for (auto &p : *widthMap) {
			Quantity::NullPointerErr *err = p.second->checkNullPointers();
			if (err) return err;
		}
		for (auto &p : *chargeMap) {
			Quantity::NullPointerErr *err = p.second->checkNullPointers();
			if (err) return err;
		}
		return NULL;
	}
	ClassDef(FwCell,1);
};


#endif
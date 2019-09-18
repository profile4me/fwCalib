
#ifndef __CALIBRATION__
#define __CALIBRATION__

#include "FwCell.h"
// #include "constants.h"

#include <THStack.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TVector2.h>
#include <TGraph.h>

#include <hloop.h>
#include <hcategorymanager.h>
#include <hwallraw.h>
#include <hwallhit.h>
#include <hparticleevtcharaBK.h>

#include <iostream>
#include <stdio.h>
#include <fstream>


struct HitMapsBlock {
	const static	int 	CC_BINS		=7;
	const 		static int 	X_MIN			= -900;		
	const 		static int 	X_MAX			= 900;		
	const 		static int 	Y_MIN			= -900;		
	const 		static int 	Y_MAX			= 900;		
	
	TH2F	*block[CC_BINS];
	TH1F 	*rdists[CC_BINS];
	TGraph	*meanR_vs_asym[CC_BINS];
	HitMapsBlock() {
		for (int bin=0; bin<CC_BINS; bin++) {
			block[bin]=0;
			rdists[bin]=0;
			meanR_vs_asym[bin]=0;
		}
	}
	HitMapsBlock(int d) {
		for (int bin=0; bin<CC_BINS; bin++) {
			block[bin]=new TH2F(Form("hitMap_day%d_cc%d",d,bin+1),Form("hitMap for day%d cc%d",d,bin+1),Quantity::N_BINS, X_MIN, X_MAX, Quantity::N_BINS, Y_MIN, Y_MAX);
			rdists[bin]=new TH1F(Form("rDist_day%d_cc%d",d,bin+1),Form("rDist for day%d cc%d",d,bin+1),Quantity::N_BINS, 0, TMath::Sqrt(X_MAX*X_MAX+Y_MAX*Y_MAX));
			meanR_vs_asym[bin] = new TGraph();
			meanR_vs_asym[bin]->SetName(Form("meanR_vs_asym_day%d_cc%d",d,bin+1));
			meanR_vs_asym[bin]->SetTitle(Form("meanR_vs_asym for day%d cc%d",d,bin+1));
		}
	}
	TH2F* getHitMap(int cbin) {return block[cbin];}
	TH1F* getRDist(int cbin) {return rdists[cbin];}
	TGraph *getGraph(int cbin) {return meanR_vs_asym[cbin];}
	void resetHists() {
		for (int bin=0; bin<CC_BINS; bin++) {
			block[bin]->Reset();
		}
	}
	void resetRDists() {
		for (int bin=0; bin<CC_BINS; bin++) {
			rdists[bin]->Reset();
			meanR_vs_asym[bin]->Set(0);
		}
	}
	void printBlock() {
		for (int bin=0; bin<CC_BINS; bin++) {
			printf("%s  ",block[bin]->GetName());
		}
	}
	void writeBlock() {
		for (int bin=0; bin<CC_BINS; bin++) {
			block[bin]->Write("", TObject::kOverwrite);
		}
	}
	void writeRDists() {
		for (int bin=0; bin<CC_BINS; bin++) {
			rdists[bin]->Write("", TObject::kOverwrite);
			meanR_vs_asym[bin]->Write("", TObject::kOverwrite);
		}
	}
	ClassDef(HitMapsBlock,1);
};


struct TimeMoment {
	int day;
	int hour;
	bool operator< (const TimeMoment &op1) const {
		return op1.day<day || op1.day==day && op1.hour<hour;
	}
	ClassDef(TimeMoment,1);
};

class Calibration : public TNamed {
private:
	static const  	Float_t	QARATIO_MIN;
	static const  	Float_t	QARATIO_MAX;
	const static 	Int_t 	eCentEst   = HParticleEvtCharaBK::kTOFRPC;
   	const static 	Int_t 	eCentClass = HParticleEvtCharaBK::k10;
	static  const char 		*EVTCHARA_PARAMS;

	static  const float 	XSHIFT 			; 
	static  const float 	YSHIFT  		; 
	static const char 		*PARAMS ;			
	const 		static int 	N_EMPTYCELLS	= 4;		
	static const 	   int 	EMPTYCELLS[];		
	const 		static int 	N_BADCELLS		= 16;		
	static const  	   int 	BADCELLS[];	

	static const  char *header;		
	static const  char *footer;			
	static const  char *RATIO_PDF;			

	bool 			useRaw;			//!
	map<int,TH1F*>	*qaRatioMap;
	map<TimeMoment,TH2F*> 	*hitMap;
	map<int,HitMapsBlock*> 	*hitMap_CC;
	map<int,TH2F*> 	*hitMapFlat;
	map<int,TH1F*> 	*phiMap;
	map<int,TH1F*> 	*phiMapRecalib;
	map<int,string> *beamCItemMap;

	const char		*infiles;		//!
	TFile 			*ioF;			//!
	TH1F 			*qa_g;			//!
	TH1F			*qa_b;			//!
	TH2F			*curHitMap;		//!
	TH2F			*curHitMap_CC[HitMapsBlock::CC_BINS];	//!
	TH1F			*curRDists_CC[HitMapsBlock::CC_BINS];	//!
	TGraph			*curGraphs_CC[HitMapsBlock::CC_BINS];	//!
	TH2F			*curHitMapFlat;	//!
	TH1F			*curPhi;		//!
	TH1F			*curPhiRecalib;	//!
	float			curXshift;		//!
	float			curYshift;		//!
	float			curXshift_CC[HitMapsBlock::CC_BINS];		//!
	float			curYshift_CC[HitMapsBlock::CC_BINS];		//!

	HWallRaw 		*raw;			//!
	HWallHit 		*hit;			//!
	HCategory 		*rawCat;		//!
	HCategory 		*hitCat;		//!
	HParticleEvtCharaBK evtChara;   //!
	
	long			nEvts;			//!
	long			curEvt;			//!
	TString 		fName;			//!
	int 			rawMult;		//!
	int 			curRaw;			//!
	int 			hitMult;		//!
	int 			curHit;			//!
	int 			centClass;		//!

	float 			hitX;			//!
	float 			hitY;			//!
	float 			hitZ;			//!
	float 			hitCh;			//!
	float 			rawW;			//!
	float 			hitT;			//!
	float 			rawT;			//!
	int 			cellId;			//!

	TH2F 			*panaramas[3];		//!
public:
	const 		static int 	N_PADS 			= 12;				
	const 		static int 	N_CELLS 		= 304;	
	FwCell 			*cells[N_CELLS];
	static bool isEmpty(int id) {
		for (int c=0; c<N_EMPTYCELLS; c++) if (id==EMPTYCELLS[c]) return true;
		return false;
	} 
	Calibration() {
		for (int id=0; id<N_CELLS; id++) cells[id]=0;
		qaRatioMap=0;
		hitMap=0;
		hitMapFlat=0;
		hitMap_CC=0;
		phiMap = phiMapRecalib =0;
		beamCItemMap=0;
	}
	Calibration(const char *runName) {
		this->SetName(runName); 
		useRaw=false;
		for (int id=0; id<N_CELLS; id++)  cells[id]=new FwCell(id);
		// if (fMode==1) parsFromFile();
		qaRatioMap = new map<int,TH1F*>;
		hitMap = new map<TimeMoment,TH2F*>;
		hitMap_CC = new map<int,HitMapsBlock*>;
		hitMapFlat = new map<int,TH2F*>;
		phiMap = new map<int,TH1F*>;
		phiMapRecalib = new map<int,TH1F*>;
		beamCItemMap=new map<int,string>;
	}
	void setRawUsage(bool useRaw) {
		this->useRaw=useRaw;
	}
	void setInput(const char *input) {
		infiles=input;
	}
	void setFillMode(int fMode) {
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) {
			cells[id]->setFillMode(fMode);
		}
	}
	void setCItem(string baseName) {
		if (baseName.substr(0,2).compare("be")) {
			curHitMap=NULL;
			return;
		}
		
		TimeMoment tm;
		tm.day = std::stoi(baseName.substr(4,3));
		tm.hour = std::stoi(baseName.substr(7,2));
		// printf("COMPARING tm [d=%d,h=%d] WITH ONES ALREADY IN MAP: \n",tm.day,tm.hour);
		// for (auto &p : *hitMap) {
			// printf("\t[d=%d,h=%d]: %s\n", p.first.day,p.first.hour, p.first==tm ? "equal" : "not equal");
		// }
		if (!(*hitMap)[tm]) {
			printf("Calibration::setCItem(): adding item into hitMap for day %d and hour %d...\n",tm.day,tm.hour);
			(*qaRatioMap)[tm.day]=new TH1F(Form("qaRatioMap_day%d",tm.day), Form("qaRatio for day %d",tm.day), Quantity::N_BINS,QARATIO_MIN,QARATIO_MAX);

			(*hitMap)[tm]= new TH2F(Form("hitMap_day%d_hour%d",tm.day,tm.hour), Form("hitMap for day%d",tm.day), Quantity::N_BINS, HitMapsBlock::X_MIN, HitMapsBlock::X_MAX, Quantity::N_BINS, HitMapsBlock::Y_MIN, HitMapsBlock::Y_MAX);
			////
			(*hitMap_CC)[tm.day]=new HitMapsBlock(tm.day);
			////
			(*hitMapFlat)[tm.day]= new TH2F(Form("hitMapFlat_day%d",tm.day), Form("flattened hitMap for day%d",tm.day), Quantity::N_BINS, HitMapsBlock::X_MIN, HitMapsBlock::X_MAX, Quantity::N_BINS, HitMapsBlock::Y_MIN, HitMapsBlock::Y_MAX);
			(*phiMap)[tm.day]= new TH1F(Form("phi_day%d",tm.day), Form("epPhi for day%d(init EPpars)",tm.day), 20, -200, 200);
			(*phiMapRecalib)[tm.day]= new TH1F(Form("phiRecalib_day%d",tm.day), Form("epPhi for day%d(recalib)",tm.day), 50, -200, 200);
		}
		curHitMap = (*hitMap)[tm];
		curXshift=curHitMap->GetMean(1);
		curYshift=curHitMap->GetMean(2);

		for (int cbin=0; cbin<HitMapsBlock::CC_BINS; cbin++) {
			curHitMap_CC[cbin] = (*hitMap_CC)[tm.day]->getHitMap(cbin);
			curRDists_CC[cbin] = (*hitMap_CC)[tm.day]->getRDist(cbin);
			curGraphs_CC[cbin] = (*hitMap_CC)[tm.day]->getGraph(cbin);
			curXshift_CC[cbin] = curHitMap_CC[cbin]->GetMean(1);
			curYshift_CC[cbin] = curHitMap_CC[cbin]->GetMean(2);
		}
		curHitMapFlat = (*hitMapFlat)[tm.day];
		curPhi = (*phiMap)[tm.day];
		curPhiRecalib = (*phiMapRecalib)[tm.day];
	}
	void initLoop() {
		new HLoop(1);
		gLoop->addMultFiles(infiles);
		const char *categories = Form("-*,+HWallHit%s",useRaw ? ",+HWallRaw" : ",+HParticleEvtInfo,+HParticleCand");
		printf("========>SETTED CATEGORIES: %s\n",categories);
		gLoop->setInput(categories);

		if (useRaw)rawCat = gLoop->getCategory("HWallRaw");
		hitCat = gLoop->getCategory("HWallHit");
	
		nEvts=gLoop->getChain()->GetEntries();
		curEvt=0;
	
		if (!useRaw) {
			evtChara.setParameterFile(EVTCHARA_PARAMS);
	    	evtChara.init();
	    }
	}
	bool nextEvent() {
		if (curEvt>=nEvts) return false; 
		gLoop->nextEvent(curEvt);
		if (curEvt%1000==0) printf("Event %d\n",curEvt);
		if (gLoop->isNewFile(fName)) {
			printf("Calibration::nextEvent(): new file \'%s\'\n",fName.Data());
			for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) 	{
				cells[id]->setCItem(gSystem->BaseName(fName.Data()));
			}
			setCItem(gSystem->BaseName(fName.Data()));
		}
		if (useRaw)rawMult = rawCat->getEntries();
		hitMult = hitCat->getEntries();
		curRaw=curHit=0;
		centClass = evtChara.getCentralityClass(eCentEst, eCentClass);	
		curEvt++;
		return true;
	}
	bool nextRaw() {
		if (curRaw>=rawMult) return false;
		raw = (HWallRaw*)HCategoryManager::getObject(raw, rawCat, curRaw++);
		cellId=raw->getCell();
		rawW=raw->getWidth(1);
		rawT = raw->getTime(1);
		return true;
	}
	bool nextHit() {
		if (curHit>=hitMult) return false;
		hit = (HWallHit*)HCategoryManager::getObject(hit, hitCat, curHit++);
		cellId=hit->getCell();
		hitCh=hit->getCharge();
		hitT = hit->getTime();
		hit->getXYZLab(hitX,hitY,hitZ);
		return true;
	}
	void resetEP() {
		for (auto &p : *phiMap) {
			p.second->Reset();
			(*phiMapRecalib)[p.first]->Reset();
		}
	}
	void fillEP() {
		initLoop();
		TVector2 eX(1,0);
		while (nextEvent()) {
			TVector2 vect(0,0),vsum(0,0);
			TVector2 vectRe(0,0),vsumRe(0,0);
			while (nextHit()) {
				if (cellId>=N_CELLS) continue;
				if (!cells[cellId]->isGoodHit(hitCh,hitT)) continue;
				float corrX = hitX-XSHIFT;
				float corrY = hitY-YSHIFT;
				vect.Set(corrX,corrY);
				vect.Unit();
				vsum+=vect;
				
				float recorrX = hitX-curXshift;//curXshift_CC[centClass-1];
				float recorrY = hitY-curYshift;//curYshift_CC[centClass-1];
				// printf("\tcorrX=%.2f\tcorrY=%.2f\n",corrX,corrY);
				vectRe.Set(recorrX,recorrY);
				vectRe.Unit();
				// printf("\thitPhi: %.2f\n",vect.DeltaPhi(eX)*TMath::RadToDeg());
				vsumRe+=vectRe;
			}
			curPhi->Fill(vsum.DeltaPhi(eX)*TMath::RadToDeg());
			curPhiRecalib->Fill(vsumRe.DeltaPhi(eX)*TMath::RadToDeg());
			// printf("curPhi filled by: %.2f\n",vsum.DeltaPhi(eX)*TMath::RadToDeg());
		}
	}
	void resetCells() {
		for (auto &p : *hitMap) {
			p.second->Reset();
			(*hitMap_CC)[p.first.day]->resetHists();
		}
	}
	void resetRDists() {
		for (auto &p : *hitMap_CC) {
			p.second->resetRDists();
		}
	}
	void fillCells() {
		initLoop();
		TH1F eventRdist("eventRdist","",Quantity::N_BINS, 0, TMath::Sqrt(HitMapsBlock::X_MAX*HitMapsBlock::X_MAX+HitMapsBlock::Y_MAX*HitMapsBlock::Y_MAX));
		while (nextEvent()) {
			while ((useRaw)&&nextRaw()) {
				if (cellId<N_CELLS) cells[cellId]->fillRaw(rawW,rawT);	
			}
			eventRdist.Reset();
			while (nextHit()) {
				if (cellId>=N_CELLS) continue;
				if (!cells[cellId]->isGoodHit(hitCh,hitT)) continue;
				curHitMap->Fill(hitX,hitY);
				// printf("centClass: %d\n",centClass);
				if (centClass>0&&centClass<=HitMapsBlock::CC_BINS) {
					curHitMap_CC[centClass-1]->Fill(hitX,hitY);
					float shifted_X=cells[cellId]->applyShift(hitX)-curXshift;
					float shifted_Y=cells[cellId]->applyShift(hitY)-curYshift;
					//RDists filling
					curRDists_CC[centClass-1]->Fill(TMath::Sqrt(shifted_X*shifted_X+shifted_Y*shifted_Y));
					int bin = curRDists_CC[centClass-1]->FindBin(TMath::Sqrt(shifted_X*shifted_X+shifted_Y*shifted_Y));
					float content = curRDists_CC[centClass-1]->GetBinContent(bin);
					curRDists_CC[centClass-1]->SetBinContent(bin,content+hitCh*hitCh*.0001);
					///////////
					eventRdist.SetBinContent(bin,content+hitCh*hitCh*.0001);
				}
				// curHitMapFlat->Fill(cells[cellId]->applyShift(hitX),cells[cellId]->applyShift(hitY));
			}
			float innerEdep = eventRdist.Integral(0,eventRdist.FindBin(400));
			float outerEdep = eventRdist.Integral(eventRdist.FindBin(400),Quantity::N_BINS);
			curGraphs_CC[centClass-1]->SetPoint(curGraphs_CC[centClass-1]->GetN(),eventRdist.GetMean(),(innerEdep - outerEdep)/(innerEdep+outerEdep));
		}
		printf("Entries hitMap : %d\n",hitMap->size());
	}
	const char *getBeamPrefix(int day) {
		return (*beamCItemMap)[day].data();
	}
	FwCell *getCell(int id) {return cells[id];}
	void fitCells() {
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id))  cells[id]->fit();
		beamCItemMap = cells[0]->formBeamCItemMap();
		for (auto &p : *beamCItemMap) {
			int day = p.first;
			for (int id=0; id<N_CELLS; id++) if (!isEmpty(id))  {
				// float ratio = cells[id]->getRatio((*cItemMap)[day]);
				float ratio = 1.0;
				(*qaRatioMap)[day]->Fill(ratio);
				// printf("%s was filled by %.2f\n", (*qaRatioMap)[day]->GetName(), ratio);
			}
		}

	}
	TH1F *getWidthForCell(int id, const char *prefix, int hType=0) {
		return (TH1F*)cells[id]->getWidthHist(prefix, hType);
	}
	// TH1F *getFit(const char *prefix, const char *qname="width", int id) {
		// cells[id]->
	// }
	TPolyMarker *getSpectr(int id, const char *prefix) {
		return cells[id]->getSpectr(prefix);
	}
	TList *getStacks(const char *prefix, const char *qname="width", int hType=0, int fitStatus=0) {
		printf("getStacks(): fitStatus=%d\n",fitStatus);
		TH1 *(FwCell::*getter)(const char*, int) = NULL;
		if (!Quantity::WIDTH_STR.compare(qname)) getter=&FwCell::getWidthHist;
		else if (!Quantity::CHARGE_STR.compare(qname)) getter=&FwCell::getChargeHist;
		else return NULL;

		THStack *curSt=new THStack;
		int curPad = 0;
		TList *l = new TList;
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) {
			if (fitStatus && cells[id]->getFitStatus(prefix)!=fitStatus) continue;
			TH1 *h=(cells[id]->*getter)(prefix,hType);
			// h->GetXaxis()->SetRangeUser(300,600);
			curSt->Add( h, "same" );
			if (++curPad==N_PADS) {
				l->Add(curSt);
				curSt=new THStack;
				curPad=0;
			}
		}
		if (curPad) l->Add(curSt);
		return l;
	}
	
	
	void doCal() {
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) {
			cells[id]->calculatePars();
		}
	}
	
	void printRatioHists() {
		TCanvas canva;
		canva.Print(Form("%s[",RATIO_PDF));
		TFile ioF("test.root","recreate");
		printf("Size of qaRatioMap: %d\n",qaRatioMap->size());
		for (auto &p : *qaRatioMap) {
			p.second->Draw();
			canva.Print(RATIO_PDF);
			p.second->Write("",TObject::kOverwrite);
		}
		canva.Print(Form("%s]",RATIO_PDF));
		ioF.Close();
	}
	void printPars(int d) {
		cout << "CALL PARS FOR CELLS WITH GOOD FIT:\n";
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) {
			FwCalPars *calPars = cells[id]->getPars(d);
			if (calPars) printf("\tadcSlope=%.2f \t adcOffset=%.2f\n",calPars->adcSlope,calPars->adcOffset);
		}
	}
	void writePars(int d) {
		FILE *fp;
		fp=freopen(Form("adcParams/adcParams_day%d.txt",d),"w",stdout);
		for (int id=0; id<N_CELLS; id++) {
			float slope=0, offset=0;
			if (!isEmpty(id)) {
				FwCalPars *calPars = cells[id]->getPars(d);
				slope=calPars->adcSlope;
				offset=calPars->adcOffset;
			}		
			printf("%4d %5.2f %5.2f\n",id,slope,offset);
		}
		fclose(fp);
	}
	void parsFromFile() {
		ifstream ifile(PARAMS);
		int id;
		float adcSlope, adcOffset;
		while (ifile>>id>>adcSlope>>adcOffset) {
			cells[id]->setPars(adcSlope,adcOffset);
		}
	}
	/*
	void getFitCheck(const char *prefix, TH1F *&h1, TH1F *&h2) {
		h1 = new TH1F("minRatio","minRatio",200,0,2);
		h2 = new TH1F("maxRatio","maxRatio",200,0,2);
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) {
			float minRatio, maxRatio;
			cells[id]->checkFit(prefix,minRatio,maxRatio);
			h1->Fill(minRatio);
			h2->Fill(maxRatio);
		}
	}*/

	void checkHitMap() {
		printf("HITMAP:\n" );
		for (auto &p : *hitMap) {
			printf("\t%s\n",p.second->GetName());
		}
	}
	void checkHitMapCC() {
		printf("HITMAP_CC: \n" );
		for (auto &p : *hitMap_CC) {
			printf("\tDay %d:  ",p.first);
			p.second->printBlock();
			printf("\n");
		}
	}

	void generateEPpars() {
		printf("generateEPpars()...\n");
		FILE *fp;
		printf("Size of hitMap: %d\n",hitMap->size());
		for (auto &p : *hitMap) {
			const char *fname = Form("recalib_params/EPparams_recalib_day%d_hour%d.txt",p.first.day,p.first.hour);
			printf("Writting ep params into file: %s\n",fname);
			fp=freopen(fname,"w",stdout);
			printf("%s\n",header);
			printf("fX_shift:  Float_t  %.2f\n",p.second->GetMean(1));
			printf("fY_shift:  Float_t  %.2f\n",p.second->GetMean(2));
			printf("%s",footer);
			fclose(fp);
		}
	}

	//mode==0 - discrete hitMap
	//mode==1 - flattened by randomized X/Y shifts in range of corresponding cell  
	void writeHitMaps() {
		ioF = new TFile("test.root","update");
		
		for (auto &p : *hitMap) {
			p.second->Write("",TObject::kOverwrite);
			(*hitMap_CC)[p.first.day]->writeBlock();
			(*hitMapFlat)[p.first.day]->Write("",TObject::kOverwrite);
		}
		ioF->Close();
	}
	void writeRDists() {
		ioF = new TFile("test.root","update");
		for (auto &p : *hitMap_CC) {
			p.second->writeRDists();
		}
		ioF->Close();
	}
	void writeEP() {
		ioF = new TFile("test.root","update");
		
		for (auto &p : *phiMap) {
			p.second->Write("",TObject::kOverwrite);
			(*phiMapRecalib)[p.first]->Write("",TObject::kOverwrite);
		}
		ioF->Close();
	}

	void pasteBins(TH2F *panarama, TH1F *h, int bin) {
		for (int b=1; b<=Quantity::N_BINS; b++) {
			if (h) panarama->SetBinContent(b,bin,h->GetBinContent(b));
			else panarama->SetBinContent(b,bin,panarama->GetMaximum());
		}
	}
	void markBadCells(const char *prefix) {
		int locId;
		for (int id=0; id<144; id++) if (!isEmpty(id)) 	{
			if (id<144) {
				if ( cells[id]->getFitStatus(prefix) != 1) { pasteBins(panaramas[0], NULL, id+1); printf("------>markBadCells(): %d marked as bad\n",id); }
			} else if (id<208) {
				locId=id-144;
				if ( cells[id]->getFitStatus(prefix) != 1) { pasteBins(panaramas[1], NULL, locId+1); printf("------>markBadCells(): %d marked as bad\n",id); }
			} else {
				locId=id-208;
				if ( cells[id]->getFitStatus(prefix) != 1) { pasteBins(panaramas[2], NULL, locId+1); printf("------>markBadCells(): %d marked as bad\n",id); }
			}
		}
	}
	TList *getPanaramas(const char *prefix) {
		TList *panList=new TList;
		panaramas[0]= new TH2F("panaramaInner","",Quantity::N_BINS,Quantity::CHARGE_MIN,Quantity::CHARGE_MAX, 144,0,144);
		panaramas[1]= new TH2F("panaramaMiddle","",Quantity::N_BINS,Quantity::CHARGE_MIN,Quantity::CHARGE_MAX, 64,144,208);
		panaramas[2]= new TH2F("panaramaOuter","",Quantity::N_BINS,Quantity::CHARGE_MIN,Quantity::CHARGE_MAX,126,208,N_CELLS);
		panList->Add(panaramas[0]);
		panList->Add(panaramas[1]);
		panList->Add(panaramas[2]);
		int locId;
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)){
			
			if (id < 144) {
				if (cells[id]->getFitStatus(prefix)==1) {
					pasteBins(panaramas[0], (TH1F*)cells[id]->getChargeHist(prefix,0), id+1);
				}
			}
			else if (id < 208) {
				locId = id-144;
				if (cells[id]->getFitStatus(prefix)==1) {
					pasteBins(panaramas[1], (TH1F*)cells[id]->getChargeHist(prefix,0), locId+1);
				}
			}
			else  {
				locId = id-208;
				if (cells[id]->getFitStatus(prefix)==1) {
					pasteBins(panaramas[2], (TH1F*)cells[id]->getChargeHist(prefix,0), locId+1);
				}
			}
		}
		markBadCells(prefix);
		return panList;
	}

	float setPeakZ1(int id, int day, float z1) {
		const char *prefix= getBeamPrefix(day);
		cells[id]->setPeakZ1(prefix, z1); 
	}
	float setPeakZ2(int id, int day, float z2) {
		const char *prefix= getBeamPrefix(day);
		cells[id]->setPeakZ2(prefix, z2); 
	}

	TH1F *getZ1dist(int day) {
		TH1F *res = new TH1F(Form("peakZ1dist_day%d",day),"",Quantity::N_BINS,Quantity::WIDTH_MIN,Quantity::WIDTH_MAX);
		const char *prefix= getBeamPrefix(day);
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)){
			res->Fill( cells[id]->getPeakZ1(prefix) );
		}
		return res;
	}
	TH1F *getZ2dist(int day) {
		TH1F *res = new TH1F(Form("peakZ2dist_day%d",day),"",Quantity::N_BINS,Quantity::WIDTH_MIN,Quantity::WIDTH_MAX);
		const char *prefix= getBeamPrefix(day);
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)){
			res->Fill( cells[id]->getPeakZ2(prefix) );
		}
		return res;
	}
	vector<float> *getWidthPeaks(int id, int day) {
		const char *prefix= getBeamPrefix(day);
		return cells[id]->getWidthPeaks(prefix);	
	}
	vector<float> *getChargePeaks(int id, int day) {
		const char *prefix= getBeamPrefix(day);
		return cells[id]->getChargePeaks(prefix);	
	}
	int getFitStatus(int id, int day) {
		const char *prefix= getBeamPrefix(day);
		return cells[id]->getFitStatus(prefix);
	}
	Quantity::NullPointerErr *checkNullPointers() {
		Quantity::NullPointerErr *err;
		for (int id=0; id<N_CELLS; id++) if (!isEmpty(id)) {
			err = cells[id]->checkNullPointers();
			if (err) return err;
		}
		return NULL;
	}
	ClassDef(Calibration,1);
};

#endif
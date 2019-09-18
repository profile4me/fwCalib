#ifndef __QUANTITY__
#define __QUANTITY__

#include <TNamed.h>
#include <TH1F.h>
#include <TF1.h>
#include <TH2F.h>
#include <TSpectrum.h>
#include <TMath.h>
#include <RooNumber.h>
#include <TFile.h>
#include <TPolyMarker.h>

#include <string>
#include <vector>
using namespace std;





class Quantity : public TNamed {
private:
	//VIEW
	static const bool		VERBOSE				;
	static const int		QA_SHIFT				;
	//FIT
	static const float		MIN_RELATIVE_PEAK_HEIGHT		;
	static const int 		ITER_N				;
	static const int   		FLAT_N;
	static const float 		THRESHOLD				;
	
	//QA
	static const int		CRITICAL_RATIO		;
	//HIST SETTINGS	
	static const int 		RAWTIME_MIN 		;
	static const int 		RAWTIME_MAX 		;
	static const int 		TIME_MIN			;
	static const int 		TIME_MAX			;
	
	int 			id;
	int 			day;
	int 			runType;
	string 			*namePostfix;

	TH1F*			initQ;
	TH1F*  			bgOfQ;
	TH1F*			approxQ;
	TH1F*			qa;
	TF1*			fitOfQ;
	TH2F*			timeVsQ;
	vector<float>	*peaks;
	TPolyMarker*	spectr;

	int 			qFitStatus;
	float 			peakZ1;
	float 			peakZ2;
public:
	//HIST SETTINGS
	static const int 		WIDTH_MIN 			;
	static const int 		WIDTH_MAX 			;
	static const int 		CHARGE_MIN 			; 
	static const int 		CHARGE_MAX 			; 
	static const int 		N_BINS 				;
	
	static const string 	WIDTH_STR			;
	static const string 	CHARGE_STR			;
	
	struct NullPointerErr {
		int 		id;
		const char *qname;
		int 		day;
		NullPointerErr(int id, const char *qname, int day) {
			this->id=id; 
			this->qname=qname; 
			this->day=day; 
		}
	};

	Quantity() {
		id=-1;
		day=0;
		runType=-1;
		namePostfix=0;
		
		initQ=bgOfQ=approxQ=qa=0;
		fitOfQ=0;
		timeVsQ=0;
		peaks=0;
		spectr=0;

		qFitStatus=0;
		peakZ1=peakZ2=0;
	}
	Quantity(const char *name, int id, string &baseName) {
		this->SetName(name);
		this->id=id;
		int qmin, qmax, tmin, tmax;
		const char *prefix;
		if (!WIDTH_STR.compare(name)) {
			qmin = WIDTH_MIN;
			qmax = WIDTH_MAX;
			tmin = RAWTIME_MIN;
			tmax = RAWTIME_MAX;
			prefix = "rawTime";
		} else if (!CHARGE_STR.compare(name)) {
			qmin = CHARGE_MIN;
			qmax = CHARGE_MAX;
			tmin = TIME_MIN;
			tmax = TIME_MAX;
			prefix = "time";
		}
		day=stoi(baseName.substr(4,3));

		namePostfix = new string( Form("cell%d_day%d",id,day) ); 
		if (!baseName.substr(0,2).compare("be")) {
			*namePostfix+="_be";
			runType=2;
		} else if (!baseName.substr(0,2).compare("co")) {
			*namePostfix+="_co";
			runType=1;
		}

		initQ = new TH1F(Form("%s_%s",name,namePostfix->data()), Form("CELL %d",id), N_BINS, qmin, qmax);
		bgOfQ = new TH1F(Form("%sBg_%s",name,namePostfix->data()), Form("CELL %d",id), N_BINS, qmin, qmax);
		approxQ = new TH1F(Form("%sApprox_%s",name,namePostfix->data()), "", N_BINS, qmin, qmax);
		qa = new TH1F(Form("%sQA_%s",name,namePostfix->data()), "", N_BINS, qmin, qmax);
		fitOfQ = new TF1(Form("%sFit_%s",name,namePostfix->data()), "gaus(0)+gaus(3)+gaus(6)+gaus(9)", qmin, qmax);
		timeVsQ = new TH2F(Form("%sVs%s_%s",prefix,name,namePostfix->data()), "", N_BINS, qmin, qmax, N_BINS, tmin, tmax);
		peaks= new vector<float>;
		spectr=new TPolyMarker();
	}
	
	string getNamePostfix() {
		return *namePostfix;
	}
	int getDay() {
		return day;
	}
	TH1 *getHist(int type) {
		switch (type) {
			case 0: 	return initQ;
			case 1: 	return approxQ;
			case 2: 	return qa;
			case 3: 	return bgOfQ;
			case 4: 	return timeVsQ;
			default:	return NULL;
		}
	}
	TF1 *getFit() {
		return fitOfQ;
	}
	TPolyMarker *getSpectr() {
		return spectr;
	}
	int getFitStatus() {
		return qFitStatus;
	}

	float getPeakZ1() {return peakZ1;}
	void setPeakZ1(float z1) {peakZ1=z1;} 
	float getPeakZ2() {return peakZ2;}
	void setPeakZ2(float z2) {peakZ2=z2;} 
	
	bool isCosmicRun() {
		return runType==1;
	}
	bool isBeamRun() {
		return runType==2;
	}
	
	void setCurrent(TH1F *&initQ, TH2F *&timeVsQ) {
		initQ = this->initQ;
		timeVsQ = this->timeVsQ;
	}
	void fill(float v, float t) {
		initQ->Fill(v);
		timeVsQ->Fill(v,t);
	}
	void reset() {
		initQ->Reset();
		timeVsQ->Reset();
	}
	void getPeaks(int nToFind, TH1F *h, int &n, float *&posSorted, float thresh=0.05) {
		TSpectrum sp(nToFind);
		n = sp.Search(h,2,"",thresh);
		float *pos=sp.GetPositionX();
		int *ind = new int[n];
		TMath::Sort(n,pos,ind,0);
		posSorted = new float[n];
		for (int p=0; p<n; p++) posSorted[p]=pos[ind[p]];
	}

	//FIT STATUSES:
	// qFitStatus==-4	no hits in cell
	// qFitStatus==-3	approximation is noticly deviates from init dist
	// qFitStatus==-2	Zpeaks not fitted
	// qFitStatus==-1   only one zpeak
	// qFitStatus==1 	OK 
	void fit() {
		peaks->clear();
		if (!initQ->GetEntries()) {qFitStatus=-4; return; }
		// initQ->GetYaxis()->SetRangeUser(-0.2*initQ->GetMaximum(), initQ->GetMaximum());

		TSpectrum sp(runType+3);
		initQ=(TH1F*)sp.Background(initQ, FLAT_N);
		initQ->SetLineColor(kBlue);
		bgOfQ=(TH1F*)sp.Background(initQ, ITER_N);
		bgOfQ->SetName(Form("%sBg_%s",this->GetName(),namePostfix->data()));
		TH1F *woBg = (TH1F*)initQ->Clone();
		woBg->Add(bgOfQ,-1);

		/////////////////////////////////////////////
		float *posSorted;
		int n;
		getPeaks(5,woBg,n,posSorted,THRESHOLD);
		
		// printf("-------->Quantity::fit(): funcs of woBg: ");
		// for (int i=0; i<woBg->GetListOfFunctions()->GetEntries(); i++) printf("%s\t",woBg->GetListOfFunctions()->At(i)->GetName());
		// printf("\n");

		if (n) spectr=(TPolyMarker*)woBg->GetListOfFunctions()->At(0);
		// spectr->SetName(Form("spectr_%s",namePostfix));
		TF1 *localFit;
		string formula="";
		double pars[15];
		TH1F *clone;
		for (int p=0; p<n; p++) {
			float ledge= (p==0) ? 0 : (posSorted[p-1]+posSorted[p])/2;
			float redge= (p==n-1) ? 2500 : (posSorted[p]+posSorted[p+1])/2;
			localFit=new TF1(Form("fit%d",p+1),"gaus",0,2500);
			clone= (p==0) ? (TH1F*)woBg->Clone(Form("cloneWoBg%d_%s",p+1,namePostfix->data())) : (TH1F*)clone->Clone(Form("cloneWoBg%d_%s",p+1,namePostfix->data()));
			if (p>0) localFit->SetParLimits(0,MIN_RELATIVE_PEAK_HEIGHT*clone->GetMaximum(),1.1*clone->GetMaximum());
			clone->Fit(localFit,"0q","",ledge,redge);
			clone->Add(localFit,-1);

			localFit->GetParameters(&pars[p*3]);
			formula+= (p==0) ? Form("gaus(%d)",p*3) : Form("+gaus(%d)",p*3);
		}
		fitOfQ = new TF1(fitOfQ->GetName(), formula.data(), 0, 2500);
		fitOfQ->SetParameters(pars);
		approxQ=(TH1F*)bgOfQ->Clone(approxQ->GetName());
		approxQ->Add(fitOfQ);

		if (VERBOSE) {
			printf("CELL %d: Found peaks (%d): ",id,n);
			for (int p=0; p<n; p++) printf("%.2f\t",posSorted[p]);
			printf("\n");
		}
		for (int p=0; p<n; p++) peaks->push_back(fitOfQ->GetParameter(p*3+1));

		//QA
		const char *qaname=qa->GetName();
		qa=(TH1F*)woBg->Clone(qaname);
		qa->Add(fitOfQ,-1);
		for (int bin=1; bin<=qa->GetXaxis()->GetNbins(); bin++) qa->SetBinContent(bin,qa->GetBinContent(bin)+QA_SHIFT);
		qa->SetLineColor(kMagenta);
		if (checkFit()>CRITICAL_RATIO) {
			qFitStatus=-3;
			return;
		}
		int shift = (posSorted[0]<400) ? 2 : 1;
		if (n-shift>=2) {
			qFitStatus=1;
			peakZ1=fitOfQ->GetParameter(shift*3+1);
			peakZ2=fitOfQ->GetParameter((shift+1)*3+1);
		}
		else if (n-shift==1) {
			qFitStatus=-1;
		}
		else qFitStatus=-2;

	}
	float checkFit() {
		int n;
		float *posSorted;
		getPeaks(10,qa,n,posSorted);	
		float ratio;
		float maxRatio=0;
		for (int p=0; p<n; p++) {
			int bin = initQ->GetXaxis()->FindBin(posSorted[p]);
			if (approxQ->GetBinContent(bin)<0.001*approxQ->GetMaximum()) continue;
			ratio=initQ->Integral(bin,bin)/approxQ->Integral(bin,bin);
			ratio= (ratio<1) ? 1.0/ratio : ratio;
			if (VERBOSE) printf("\t{pos: %.2f, ratio: %.2f}\n",posSorted[p],ratio);
			if (RooNumber::isInfinite(ratio)) continue;
			if (ratio>maxRatio) { maxRatio=ratio;  }
		}
		if (VERBOSE) printf("maxRatio: %.2f\n",maxRatio);		

		return maxRatio;
	}
	vector<float> *getPeaks() {
		vector<float> *clone = new vector<float>;
		for (vector<float>::iterator it=peaks->begin(); it!=peaks->end(); it++) {
			if (TMath::Abs(*it-peakZ1)<0.001 || TMath::Abs(*it-peakZ2)<0.001) clone->push_back(-*it);
			else clone->push_back(*it);
		}
		return clone;
		// return peaks;
	}

	NullPointerErr *checkNullPointers() {
		
		if (!namePostfix || !initQ || !bgOfQ || !approxQ || !qa || !fitOfQ || !timeVsQ || !peaks || !spectr) return new NullPointerErr(id,this->GetName(),day);
		return NULL;
	}
	ClassDef(Quantity,1);
};

#endif
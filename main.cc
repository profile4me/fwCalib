// #include "TestClass.h"
// #include "constants.h"
#include "Calibration.h"
#include "TPRegexp.h"

Calibration *cal;

void changeYrange(TList *stList) {
	for (auto *obj1 : *stList) {
		for (auto *obj2 : *(((THStack*)obj1)->GetHists())) {
			TH1F *h = (TH1F*)obj2;
			h->GetYaxis()->SetRangeUser(-0.2*h->GetMaximum(),h->GetMaximum());
		}
	}
}

void printWidthPack(int day,  int status) {
	const char *prefix = cal->getBeamPrefix(day);
	TList *l_init=cal->getStacks(prefix, "width", 0, status);
	changeYrange(l_init);
	TList *l_approx=cal->getStacks(prefix, "width", 1, status);
	TList *l_qa=cal->getStacks(prefix, "width", 2, status);
	TCanvas canva;
	const char *pdfname = Form("pdfs/%03d/width_status%d.pdf",day,status); 
	canva.Print(Form("%s[",pdfname));
	for (int i=0; i<l_init->GetEntries(); i++) {
		( (THStack*)l_init->At(i) )->Draw("PADS");
		( (THStack*)l_approx->At(i) )->Draw("PADS,same");
		( (THStack*)l_qa->At(i) )->Draw("PADS,same");
		for (int pad=1; pad<=Calibration::N_PADS; pad++) canva.cd(pad)->BuildLegend();
		canva.Update();
		canva.Print(pdfname);
	}
	canva.Print(Form("%s]",pdfname));
}

void writePanaramas(int day)	 {
	const char *prefix = cal->getBeamPrefix(day);
	TList *l = cal->getPanaramas(prefix);
	// const char *fname = Form("");
	TFile outF("test.root","update");
	TCanvas *canvases[3];
	TH2F* pan;
	for (int i=0; i<3; i++) {
		pan=(TH2F*)l->At(i);
		canvases[i]=new TCanvas(Form("%s_day%d",pan->GetName(),day),"");
		pan->Draw("colz");
		canvases[i]->SetLogz();
		canvases[i]->Write("",TObject::kOverwrite);
	}
	outF.Close();
}

void showMessage(vector<float> *peaks, int activeCell, int day, int status) {

	printf("Now current active cell: %d(status=%d). Peaks:\n",activeCell,status);
	float z1=0;
	float z2=0;
	for (int i=0; i<peaks->size(); i++) printf("\t\t%d",i+1);
	printf("\n");
	for (float &peak : *peaks) {
		if (peak<0) {
			if (z1==0) z1=-peak;
			else z2=-peak;
		}  
		printf("\t\t%.2f",TMath::Abs(peak));
	}
	printf("\nZ1: %.2f\tZ2: %.2f\n",z1,z2);
	printf("you could choose values for z1/z2 from upper row. For example: \'z1<-1 z2<-2\'\n");
	printf("to change active cell: \'cell=<id>\'\n");
	printf(">");
}

void isWritable() {
	Quantity::NullPointerErr *err = cal->checkNullPointers();
	printf("----->isWritable(): checking on null pointers...\n");
	if (err) printf("\tNULL POINTER DETECTED!!! {id=%d,qname=%s,day=%d}\n",err->id,err->qname,err->day);
	else printf("\tchecked.\n");
}

int main(int argc, char const *argv[])
{
	TPRegexp fModeStr("fmode=([[:digit:]]+)");
	TPRegexp runStr("run=([[:alnum:]]+)");
	TPRegexp inStr("in=([[:graph:]]+)");
	TPRegexp dayStr("day=([[:digit:]]+)");
	TPRegexp cellStr("cell=([[:digit:]]+)");
	int fMode=0, day, cell;
	TString runName;
	TString input;

	TString initCalStr("-initCal"), fill1Str("-fillCells"), recFillStr("-recalibFill"),fill2Str("-fillEp"),  fitStr("-fit"), printWidthStr("-printW"), printQaStr("-printQa"), writeEpStr("-writeEp"), genEPparsStr("-genEPpars"), testStr("-test");
	TString calStr("-calibrate"), writeAdcStr("-adc"), printChargeStr("-printCh"), panaramaStr("-panaramas"), rRefilStr("-rRefil"), printZ1Z2Str("-printZ1Z2"), fixPeaksStr("-fixPeaks"), printCellStr("-printCell");
	for (int i=0; i<argc; i++) {
		if (fModeStr.Match(argv[i])) {
			fMode=( (TObjString*)fModeStr.MatchS(argv[i])->At(1) )->GetString().Atoi();
		}
		if (runStr.Match(argv[i])) {
			runName =( (TObjString*)runStr.MatchS(argv[i])->At(1) )->GetString();
		}
		if (inStr.Match(argv[i])) {
			input =( (TObjString*)inStr.MatchS(argv[i])->At(1) )->GetString();
		}
		if (dayStr.Match(argv[i])) {
			day =( (TObjString*)dayStr.MatchS(argv[i])->At(1) )->GetString().Atoi();
		}
		if (cellStr.Match(argv[i])) {
			cell =( (TObjString*)cellStr.MatchS(argv[i])->At(1) )->GetString().Atoi();
		}
	}
	for (int i=0; i<argc; i++) {
		
		if (!initCalStr.CompareTo(argv[i])) {
			cal = new Calibration(runName.Data());
			TFile ioF("calibration.root","recreate");
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!fill1Str.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->setRawUsage(true);
			cal->setInput(input.Data());
			// cal->resetCells();
			cal->fillCells();
			ioF.cd();
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!recFillStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->setRawUsage(true);
			cal->setInput(input.Data());
			cal->setFillMode(1);
			cal->fillCells();
			ioF.cd();
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!calStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->doCal();
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!writeAdcStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->writePars(63);
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!fill2Str.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			
			cal->setInput(input.Data());
			cal->resetEP();
			cal->setRawUsage(false);
			cal->fillEP();
			ioF.cd();
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!fitStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->fitCells();
			ioF.cd();
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}
		if (!printWidthStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			printWidthPack(day,-3);
			printWidthPack(day,-2);
			printWidthPack(day,-1);
			printWidthPack(day,1);
		}
		if (!printChargeStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			TList *stList = cal->getStacks("be19063","charge");				
			TCanvas canva;
			const char *pdfname = "pdfs/charge_be19063.pdf";
			canva.Print(Form("%s[",pdfname));
			for (auto *obj : *stList) {
				canva.Clear();
				canva.Divide(4,3);
				( (THStack*)obj)->Draw("PADS");
				canva.Update();
				for (int p=1; p<=Calibration::N_PADS; p++) canva.cd(p)->SetLogy();
				canva.Update();
				canva.Modified();
				canva.Print(pdfname);
			}
			canva.Print(Form("%s]",pdfname));
		}
		if (!panaramaStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			writePanaramas(day);
		}
		if (!printQaStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->printRatioHists();
		}
		if (!writeEpStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->writeEP();
		}
		
		if (!genEPparsStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->generateEPpars();
			
		}
		if (!testStr.CompareTo(argv[i])) {
		}
		if (!rRefilStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			cal->resetRDists();
			cal->setRawUsage(false);
			cal->setInput(input.Data());
			cal->fillCells();
			cal->writeRDists();
			ioF.cd();
			isWritable();
			cal->Write("",TObject::kOverwrite);
			ioF.Close();
		}		
		if (!printZ1Z2Str.CompareTo(argv[i])) {
			TFile ioF("calibration.root");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			TCanvas canva;
			canva.Divide(2,1);
			canva.cd(1);
			cal->getZ1dist(day)->Draw();
			canva.cd(2);
			cal->getZ2dist(day)->Draw();
			const char *pdfname = Form("pdfs/%03d/Z1Z2.pdf)",day); 
			canva.Print(pdfname);
		}

		if (!fixPeaksStr.CompareTo(argv[i])){
			const char *qstr="q";
			TPRegexp setZ1pattern("z1<-([[:digit:]]+)");
			TPRegexp setZ2pattern("z2<-([[:digit:]]+)");
			TPRegexp changeCellpattern("cell=([[:digit:]]+)");

			int activeCell=0;
			TFile ioF("calibration.root","update");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());
			string buf;
			vector<float> *peaks = cal->getWidthPeaks(activeCell,day);
			showMessage(peaks, activeCell, day, cal->getFitStatus(activeCell,day));
			while (getline(cin,buf)) {
				if (!buf.compare(qstr)) break;
				if (setZ1pattern.Match(buf.data())) {
					int newPos = ( (TObjString*)setZ1pattern.MatchS(buf.data())->At(1) )->GetString().Atoi();	
					if (newPos>=1&& newPos<=peaks->size()) cal->setPeakZ1(activeCell,day, TMath::Abs(peaks->at(newPos-1)) );
				}
				if (setZ2pattern.Match(buf.data())) {
					int newPos = ( (TObjString*)setZ2pattern.MatchS(buf.data())->At(1) )->GetString().Atoi();	
					if (newPos>=1&& newPos<=peaks->size()) cal->setPeakZ2(activeCell,day, TMath::Abs(peaks->at(newPos-1)) );
				}
				if (changeCellpattern.Match(buf.data())) {
					int newId = ( (TObjString*)changeCellpattern.MatchS(buf.data())->At(1) )->GetString().Atoi();
					if (newId>=0 && newId<Calibration::N_CELLS && !Calibration::isEmpty(newId)) activeCell=newId;
				}
				peaks=cal->getWidthPeaks(activeCell,day);
				showMessage(peaks, activeCell, day, cal->getFitStatus(activeCell,day));
			}
		}
		if (!printCellStr.CompareTo(argv[i])) {
			TFile ioF("calibration.root","");
			cal=(Calibration*)ioF.FindObjectAny(runName.Data());

			THStack st;
			const char *prefix = cal->getBeamPrefix(day);
			for (int t=0;t<3; t++) st.Add(cal->getWidthForCell(cell,prefix,t));	
			TPolyMarker *spectr=cal->getSpectr(cell,prefix);

			TCanvas canva;
			st.Draw("nostack");
			spectr->Draw("same");
			const char *pdfname = Form("pdfs/%03d/width_cell%d.pdf)",day,cell); 
			canva.Print(pdfname);
		}
	}
	
	return 0;
}
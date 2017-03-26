#include "FilteringPanel.h"
#include "MagicDisplay.h"
#include "FilterOperation.h"

ClassImp(FilteringPanel)

FilteringPanel*  FilteringPanel::fgInstance = 0;


FilteringPanel::FilteringPanel()
{
  //Default constructor 
  fgInstance=this;  
  //MagicDisplay Stuff

  fMainFrame = new TGMainFrame(gClient->GetRoot(),200,300,kVerticalFrame);

  fCombo = new TGComboBox(fMainFrame,100);
  selectMagicDisplayFilterInComboBox();
  fCombo->Connect("Selected(Int_t)", "FilteringPanel", this, "updateTextAndSetFilter()"); // then connect selection to updating MagicDisplay
  fCombo->Resize(150, 20); // needed to set reasonable height.
  
  int id=0;
  // fSelectedTextView = new TGTextView(fMainFrame, 500, 94, id, kFixedWidth | kFixedHeight);
  fSelectedTextView = new TGTextView(fMainFrame, 500, 94, id, kLHintsExpandX | kLHintsExpandY);  

  TColor* yellow = gROOT->GetColor(kYellow - 9); // -9 tones down the yellow, which is a bit in-your-face otherwise
  fSelectedTextView->SetBackgroundColor(yellow->GetPixel());  
  
  // fMainFrame->AddFrame(fCombo, new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2));
  fMainFrame->AddFrame(fCombo, new TGLayoutHints(kLHintsTop | kLHintsExpandX,2,2,2,2));  
  fMainFrame->AddFrame(fSelectedTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2));  

  fMainFrame->SetWindowName("FilteringPanel");
  fMainFrame->MapSubwindows();  
  fMainFrame->Connect("CloseWindow()", "FilteringPanel", this, "closeWindow()");  

  fMainFrame->Resize();
  fMainFrame->MapRaised();

  updateTextAndSetFilter();

}


//______________________________________________________________________________
FilteringPanel*  FilteringPanel::Instance()
{
  //static function
  FilteringPanel* fp = (fgInstance) ? (FilteringPanel*) fgInstance : new FilteringPanel();
  // fp->fMainFrame->cd();
  // fp->fMainFrame->RequestFocus();
  return fp;  
}


FilteringPanel::~FilteringPanel()
{
  fgInstance = 0;  
}

void FilteringPanel::closeWindow()
{
  delete this;
}



void FilteringPanel::selectMagicDisplayFilterInComboBox(){

  MagicDisplay *md = MagicDisplay::Instance();
  FilterStrategy* currentStrat = md->getStrategy();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();

  int entry=1;
  int thisEntry = entry;
  TString stratName;  
  for(; it!=filterStrats.end(); ++it){

    fCombo->AddEntry(it->first.Data(), entry);

    if(it->second == currentStrat){
      thisEntry = entry;
      stratName = it->first;
    }
    entry++;
  }  
  fCombo->Select(thisEntry); // select the current filter

}





void FilteringPanel::updateTextAndSetFilter(){//TGTextView* tv, std::map<TString, FilterStrategy*>::iterator it){

  fSelectedTextView->Clear();

  int selectedEntry = fCombo->GetSelected();

  MagicDisplay* md = MagicDisplay::Instance();
  std::map<TString, FilterStrategy*> filterStrats = md->getFilterStrats();
  std::map<TString, FilterStrategy*>::iterator it = filterStrats.begin();
  
  int entry=1;
  for(; it!=filterStrats.end(); ++it){
    if(entry==selectedEntry){      
      md->setFilterStrategy(it->second);
      // setText(fAppliedTexfSelectedTextViewiew, it);
      break;
      
    }
    entry++;
  }  
  
   // Set an appropriate title, showing the filter strategy name in MagicDisplay

  TString firstLine = it->first + "\n\n";

  fSelectedTextView->AddLineFast(firstLine.Data());
  
  for(unsigned i=0; i < it->second->nOperations(); i++){
    const FilterOperation * op = it->second->getOperation(i);
    TString thisLine = TString::Format("Step %u: %s \n", i+1, op->description());
    fSelectedTextView->AddLineFast(thisLine.Data());
  }

  fSelectedTextView->Update();

}

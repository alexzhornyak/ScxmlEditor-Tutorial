//---------------------------------------------------------------------------

#ifndef UnitTreeClipboardExH
#define UnitTreeClipboardExH
//---------------------------------------------------------------------------

#include "TeeTree.hpp"

class TTreeClipboardEx : public TCustomTree {
private:
	TCustomTree *FIsCut;
protected:

public:
	__fastcall TTreeClipboardEx(Classes::TComponent* AOwner);

	void __fastcall Copy(TCustomTree *SourceTree);
	void __fastcall Cut(TCustomTree *SourceTree);
	void __fastcall Paste(TCustomTree *DestTree);

};

#endif

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitScxmlFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvToolBtn"
#pragma link "TeeNavigator"
#pragma link "TeeProcs"
#pragma link "TeeTree"
#pragma link "TreeNavigator"
#pragma resource "*.dfm"
TFrameScxmlBase *FrameScxmlBase;
//---------------------------------------------------------------------------
__fastcall TFrameScxmlBase::TFrameScxmlBase(TComponent* Owner)
	: TFrame(Owner)
{
}
//---------------------------------------------------------------------------

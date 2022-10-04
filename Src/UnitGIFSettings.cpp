/***********************************************************************************
	BSD 3-Clause License

	Copyright (c) 2018, https://github.com/alexzhornyak
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

	* Neither the name of the copyright holder nor the names of its
	  contributors may be used to endorse or promote products derived from
	  this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/
#include <vcl.h>
#pragma hdrstop

#include "UnitGIFSettings.h"

#include <memory>

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterGifMakerSettings() {
#pragma startup RegisterGifMakerSettings

	Classes::RegisterClassA(__classid(TGifMakerSettings));
}

class TFontPropEditor : public TLMDFontPropEditor {
protected:

	virtual Lmdtypes::TLMDString __fastcall GetValue(void) {
		if (this->AllEqual()) {

			TFont *AFont = dynamic_cast<TFont*>(this->GetObjectValue(0));
			if (AFont) {

				return UnicodeString().sprintf(L"%s, %d, %s", AFont->Name.c_str(), AFont->Size, ColorToString(AFont->Color).c_str());

			}
		}
		return L"";
	}

};

// ---------------------------------------------------------------------------
// ---------------------------- TGifMakerSettings ---------------------------
// ---------------------------------------------------------------------------
__fastcall TGifMakerSettings::TGifMakerSettings(TComponent* Owner) : TSettingsBase(Owner) {
	FIgnoreDuplicates = true;
	FSnapshotOnStartRecord = true;
	FClearAllOnStartRecord = true;
	FTextMarginLeftPercent = 50;
	FTextMarginTopPercent = 50;
	FTextHorizAlign = thaCenter;
	FDelayBetweenFramesMSec = 1000;
	FLoopsNumber = 0;

	FTextFont = new TFont();
	FTextFont->Size = 18;
	FTextFont->Color = clRed;

	FCaptureType = TCaptureType::ctScxmlRoot;
	FCaptureRect = new TPersistentRect;

	FTextDisplayTypes = TTextDisplayTypes() << tdtEnter << tdtTransition << tdtCustom;
}

// ---------------------------------------------------------------------------
__fastcall TGifMakerSettings::~TGifMakerSettings() {
	delete FTextFont;
	delete FCaptureRect;
}

// ---------------------------------------------------------------------------
bool __fastcall TGifMakerSettings::OnFilterPropEvent(const UnicodeString &sPropName) {

	/* TRUE == SHOW */

	std::auto_ptr<TStringList>AStringList(new TStringList());
	AStringList->CaseSensitive = true;
	AStringList->Add("Name");
	AStringList->Add("Tag");

	return /*Show*/AStringList->IndexOf(sPropName) == -1;
}

// ---------------------------------------------------------------------------
void __fastcall TGifMakerSettings::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass)
{
	const UnicodeString sPropName = APropInfo->Name();
	if (sPropName == L"CaptureRect") {
		AEditorClass = __classid(TPersistentRectEditor);
	}
	else if (sPropName == L"TextFont") {
		AEditorClass = __classid(TFontPropEditor);
	}
}

// ---------------------------------------------------------------------------
TComponent * __fastcall TGifMakerSettings::DoCreateDefaultInstance() {
	return new TGifMakerSettings(this->Owner);
}

// ---------------------------------------------------------------------------
void __fastcall TGifMakerSettings::Assign(TPersistent* Source) {
	if (Source) {
		TGifMakerSettings * AGifMakerSettings = dynamic_cast<TGifMakerSettings*>(Source);
		if (AGifMakerSettings) {
			FCaptureType = AGifMakerSettings->FCaptureType;
			FTextDisplayTypes = AGifMakerSettings->FTextDisplayTypes;
			FIgnoreDuplicates = AGifMakerSettings->FIgnoreDuplicates;
			FSnapshotOnStartRecord = AGifMakerSettings->FSnapshotOnStartRecord;
			FClearAllOnStartRecord = AGifMakerSettings->FClearAllOnStartRecord;
			FTextMarginLeftPercent = AGifMakerSettings->FTextMarginLeftPercent;
			FTextMarginTopPercent = AGifMakerSettings->FTextMarginTopPercent;
			FTextHorizAlign = AGifMakerSettings->FTextHorizAlign;
			FDelayBetweenFramesMSec = AGifMakerSettings->FDelayBetweenFramesMSec;
			FLoopsNumber = AGifMakerSettings->FLoopsNumber;

			FTextFont->Assign(AGifMakerSettings->FTextFont);
			FCaptureRect->Assign(AGifMakerSettings->FCaptureRect);
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");
	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

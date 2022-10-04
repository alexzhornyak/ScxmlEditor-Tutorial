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

#ifndef UnitGIFSettingsH
#define UnitGIFSettingsH
//---------------------------------------------------------------------------

#include "UnitSettingsBase.h"
#include "PersistentRect.h"

typedef enum {thaLeft, thaCenter, thaRight} TTextHorizAlign;

typedef enum {ctScxmlRoot, ctMainForm, ctScreen} TCaptureType;

typedef enum {tdtEnter, tdtTransition, tdtCustom} TTextDisplayType;
typedef Set<TTextDisplayType,tdtEnter,tdtCustom> TTextDisplayTypes;

class TGifMakerSettings: public TSettingsBase
{
__published:
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
									TLMDPropEditorClass &AEditorClass);
private:

	TCaptureType FCaptureType;

	TFont *	FTextFont;
	void __fastcall SetTextFont(TFont *val) { FTextFont->Assign(val); }

	TTextDisplayTypes FTextDisplayTypes;
	bool FIgnoreDuplicates;
	bool FSnapshotOnStartRecord;
	bool FClearAllOnStartRecord;
	TColor FTextColor;

	int FTextMarginLeftPercent;
	int FTextMarginTopPercent;
	int FTextSizePx;
	TTextHorizAlign FTextHorizAlign;

	int FDelayBetweenFramesMSec;
	int FLoopsNumber;

	TPersistentRect *FCaptureRect;
	void __fastcall SetCaptureRect(TPersistentRect *val) { FCaptureRect->Assign(val); }
	bool __fastcall IsCaptureRectStored(void) { return !FCaptureRect->IsEmpty(); }

protected:

public:		// User declarations

	__fastcall TGifMakerSettings(TComponent* Owner);
	virtual __fastcall ~TGifMakerSettings();

	virtual void __fastcall Assign(TPersistent* Source);

	// функци€ должна создать новый экземпл€р текущего класса
	virtual TComponent *__fastcall DoCreateDefaultInstance();

__published:

	__property TCaptureType			CaptureType = {read=FCaptureType, write=FCaptureType, default=ctScxmlRoot};
	__property TPersistentRect *	CaptureRect = {read=FCaptureRect, write=SetCaptureRect, stored=IsCaptureRectStored};
	__property bool 				SnapshotOnStartRecord = {read=FSnapshotOnStartRecord, write=FSnapshotOnStartRecord, default=true };
	__property bool 				ClearAllOnStartRecord = {read=FClearAllOnStartRecord, write=FClearAllOnStartRecord, default=true };

	__property int 					DelayBetweenFramesMSec = {read=FDelayBetweenFramesMSec, write=FDelayBetweenFramesMSec, default=1000 };
	__property int 					LoopsNumber = {read=FLoopsNumber, write=FLoopsNumber, default=0 };

	__property TTextDisplayTypes 	TextDisplayTypes = {read=FTextDisplayTypes, write=FTextDisplayTypes, default=7 };
	__property bool 				IgnoreDuplicates = {read=FIgnoreDuplicates, write=FIgnoreDuplicates, default=true };

	__property TFont *				TextFont = {read=FTextFont, write=SetTextFont};
	__property int 					TextMarginLeftPercent = {read=FTextMarginLeftPercent, write=FTextMarginLeftPercent, default=50 };
	__property int 					TextMarginTopPercent = {read=FTextMarginTopPercent, write=FTextMarginTopPercent, default=50 };
	__property TTextHorizAlign 		TextHorizAlign = {read=FTextHorizAlign, write=FTextHorizAlign, default=thaCenter};

};

#endif

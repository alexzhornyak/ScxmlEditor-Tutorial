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

#ifndef UnitFrameGifMakerH
#define UnitFrameGifMakerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include <ActnList.hpp>
#include <Buttons.hpp>
#include <StdActns.hpp>
#include <ExtActns.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------

#include "UnitGIFSettings.h"
#include "ScxmlMsgTypes.h"
#include "UnitScxmlBaseShape.h"

struct TGifMakerData {
	// оригинальное изображение
	Graphics::TBitmap *Bitmap;
	// редактируемое изображение
	Graphics::TBitmap *BitmapEdit;
	int Delay;

	TGifMakerData();
	~TGifMakerData();

    TGifMakerData *Clone(void);

	static Graphics::TBitmap *GetBitmap(TListItem *AItem);
	static Graphics::TBitmap *GetEditBitmap(TListItem *AItem);
	static void DeleteGifData(TListItem *AItem);
	static void RevertBitmap(TListItem *AItem);
	static void SetDelay(TListItem *AItem, const int iDelay);
};

class TFrameGifMaker : public TFrame
{
__published:	// IDE-managed Components
	TListView *ListView1;
	TPanel *Panel1;
	TImageList *ImageListToolBar;
	TSpeedButton *SpeedButton1;
	TActionList *ActionList1;
	TAction *ActionGifStartRecord;
	TAction *ActionGifPause;
	TAction *ActionGifStop;
	TSpeedButton *SpeedButtonPause;
	TSpeedButton *SpeedButton3;
	TSplitter *Splitter1;
	TFileSaveAs *FileSaveAs1;
	TLabel *Label1;
	TLabel *Label2;
	TButton *Button1;
	TPopupMenu *PopupMenu1;
	TButton *Button2;
	TAction *ActionGifSettings;
	TLabel *LabelDelay;
	TLabel *LabelLoops;
	TScrollBox *ScrollBox1;
	TImage *Image1;
	TImageList *ImageListItems;
	TAction *ActionGifEdit;
	TMenuItem *Edit1;
	TAction *ActionGifCapture;
	TButton *Button3;
	TAction *ActionGifClearAll;
	TButton *Button4;
	TAction *ActionGifMoveUp;
	TMenuItem *MoveUp1;
	TAction *ActionGifMoveDown;
	TMenuItem *MoveDown1;
	TAction *ActionGifClone;
	TMenuItem *Clone1;
	TMenuItem *Replace1;
	TAction *ActionReplaceAll;
	TSpeedButton *SpeedUseAlignOffset;
	TAction *ActionGifTextTransitions;
	TAction *ActionGifTextCustom;
	TAction *ActionGifTextEnter;
	TSpeedButton *SpeedButton2;
	TSpeedButton *SpeedButton4;
	TSpeedButton *SpeedButton5;
	TAction *ActionGifIgnoreDuplicates;
	TMenuItem *Rename1;
	TMenuItem *Delete1;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *Revert1;
	TMenuItem *ChangeDelay1;
	void __fastcall ActionGifStartRecordExecute(TObject *Sender);
	void __fastcall ActionGifPauseExecute(TObject *Sender);
	void __fastcall ActionGifStopExecute(TObject *Sender);
	void __fastcall ListView1SelectItem(TObject *Sender, TListItem *Item, bool Selected);
	void __fastcall FileSaveAs1Accept(TObject *Sender);
	void __fastcall FileSaveAs1Update(TObject *Sender);
	void __fastcall ListView1Edited(TObject *Sender, TListItem *Item, UnicodeString &S);
	void __fastcall ActionGifEditExecute(TObject *Sender);
	void __fastcall ActionGifCaptureExecute(TObject *Sender);
	void __fastcall ActionGifClearAllExecute(TObject *Sender);
	void __fastcall ActionGifClearAllUpdate(TObject *Sender);
	void __fastcall ActionGifCaptureUpdate(TObject *Sender);
	void __fastcall ActionGifMoveUpExecute(TObject *Sender);
	void __fastcall ActionGifMoveDownExecute(TObject *Sender);
	void __fastcall ActionGifCloneExecute(TObject *Sender);
	void __fastcall ActionReplaceAllExecute(TObject *Sender);
	void __fastcall ActionGifTextTransitionsUpdate(TObject *Sender);
	void __fastcall ActionGifTextTransitionsExecute(TObject *Sender);
	void __fastcall ActionGifTextCustomExecute(TObject *Sender);
	void __fastcall ActionGifTextCustomUpdate(TObject *Sender);
	void __fastcall ActionGifTextEnterExecute(TObject *Sender);
	void __fastcall ActionGifTextEnterUpdate(TObject *Sender);
	void __fastcall ActionGifIgnoreDuplicatesExecute(TObject *Sender);
	void __fastcall ActionGifIgnoreDuplicatesUpdate(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall Delete1Click(TObject *Sender);
	void __fastcall Revert1Click(TObject *Sender);
	void __fastcall ChangeDelay1Click(TObject *Sender);
	void __fastcall ActionGifSettingsExecute(TObject *Sender);
private:	// User declarations

	std::auto_ptr<TLMDPropPageStateObject> FPropInspStateSettingsPtr;

	void ClearAll(void);

    void SaveToFile(const UnicodeString &sFileName);

	void AddText(Graphics::TBitmap *ABitmap, const UnicodeString &sText);

	void AssignImageFromList(TListItem *Item, const UnicodeString &sCaption);

    void DoSaveSettings(void);

	int GetItemDelay(TListItem *Item);

	void UpdateExtraDataItem(TListItem *AItem);
	void UpdateExtraData(void);

	static int TypeToImageIndex(const TScxmlMsgType AType);
	static TTextDisplayType ImageIndexToTextDisplayType(const int iImageIndex);

protected:

	TGifMakerSettings *FSettings;

    void __fastcall DoEditSettings(const UnicodeString sItemName = L"");

    friend class TDialogInsight;

public:		// User declarations
	__fastcall TFrameGifMaker(TComponent* Owner);
	virtual __fastcall ~TFrameGifMaker();

	bool IsRecordMode() { return ActionGifPause->Enabled && !SpeedButtonPause->Down; }

	void AddImage(const TScxmlMsgType AType, const UnicodeString &sName,
		Graphics::TBitmap *ABitmap, const TRect &ARectShape, const int iWidth, const int iHeight);

	void CaptureImage(TCustomTree *ATree, TTreeNodeShape *AScxmlRootShape,
		const TScxmlMsgType AType, const UnicodeString &sImageName);

	__property TGifMakerSettings *Settings = {read=FSettings};
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameGifMaker *FrameGifMaker;
//---------------------------------------------------------------------------
#endif

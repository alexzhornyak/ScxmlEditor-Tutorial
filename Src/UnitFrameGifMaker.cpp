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

#include "UnitFrameGifMaker.h"

#include <memory>
#include <vector>
#include <GifImg.hpp>
#include <IOUtils.hpp>
#include "Log4cpp_VCL.hpp"

#include "VISUAL_GLOBALS.h"
#include "UnitSettingsEditor.h"
#include "UnitSettings.h"
#include "UnitDialogWait.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitScxmlShape.h"
#include "UnitSimpleReplaceDialog.h"
#include "UnitTreeEx.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrameGifMaker *FrameGifMaker = NULL;

//---------------------------------------------------------------------------
//---------------------------- TGifMakerData --------------------------------
//---------------------------------------------------------------------------
TGifMakerData::TGifMakerData() : Delay(-1), BitmapEdit(NULL) {
	Bitmap = new Graphics::TBitmap;
}

//---------------------------------------------------------------------------
TGifMakerData::~TGifMakerData() {
	if (Bitmap) {
		delete Bitmap;
		Bitmap = NULL;
	}
	if (BitmapEdit) {
		delete BitmapEdit;
		BitmapEdit = NULL;
	}
}

//---------------------------------------------------------------------------
TGifMakerData *TGifMakerData::Clone(void) {
	TGifMakerData *AGifData = new TGifMakerData();
	AGifData->Delay = this->Delay;
	if (this->Bitmap) {
		AGifData->Bitmap->Assign(this->Bitmap);
	}
	if (this->BitmapEdit) {
		AGifData->BitmapEdit = new Graphics::TBitmap;
		AGifData->BitmapEdit->Assign(this->BitmapEdit);
	}
	return AGifData;
}

//---------------------------------------------------------------------------
Graphics::TBitmap *TGifMakerData::GetBitmap(TListItem *AItem) {
	if (AItem && AItem->Data) {
		TGifMakerData *AGifData = static_cast<TGifMakerData*>(AItem->Data);
		// если что-то редактировалось
		if (AGifData->BitmapEdit) {
			return AGifData->BitmapEdit;
		}
		// оригинальное
		return AGifData->Bitmap;
	}
	return NULL;
}

//---------------------------------------------------------------------------
Graphics::TBitmap *TGifMakerData::GetEditBitmap(TListItem *AItem) {
	if (AItem && AItem->Data) {
		TGifMakerData *AGifData = static_cast<TGifMakerData*>(AItem->Data);
		// предполагаетс€, что не может быть 'BitmapEdit' без 'Bitmap'
		if (AGifData->Bitmap) {
			if (!AGifData->BitmapEdit) {
				AGifData->BitmapEdit = new Graphics::TBitmap;
				AGifData->BitmapEdit->Assign(AGifData->Bitmap);
			}
			return AGifData->BitmapEdit;
		}
	}
	return NULL;
}

//---------------------------------------------------------------------------
void TGifMakerData::DeleteGifData(TListItem *AItem) {
	if (AItem && AItem->Data) {
		TGifMakerData *ANodeData = static_cast<TGifMakerData*>(AItem->Data);
		delete ANodeData;
		AItem->Data = NULL;
	}
}

//---------------------------------------------------------------------------
void TGifMakerData::RevertBitmap(TListItem *AItem) {
	if (AItem && AItem->Data) {
		TGifMakerData *ANodeData = static_cast<TGifMakerData*>(AItem->Data);
		if (ANodeData->BitmapEdit) {
			delete ANodeData->BitmapEdit;
			ANodeData->BitmapEdit = NULL;
		}
	}
}

//---------------------------------------------------------------------------
void TGifMakerData::SetDelay(TListItem *AItem, const int iDelay) {
	if (AItem && AItem->Data) {
		TGifMakerData *ANodeData = static_cast<TGifMakerData*>(AItem->Data);
		ANodeData->Delay = iDelay;
	}
}

//---------------------------------------------------------------------------
//-----------------------------  TFrameGifMaker  ----------------------------
//---------------------------------------------------------------------------
__fastcall TFrameGifMaker::TFrameGifMaker(TComponent* Owner) : TFrame(Owner) {
	FSettings = new TGifMakerSettings(this);

	if (SettingsData) {
		TSettingsRegistryItem *AItem = SettingsData->TempRegistryItems->Map[this->Name];
		if (AItem) {
			FSettings->LoadFromStrings(AItem->Data);

			LabelDelay->Caption = FSettings->DelayBetweenFramesMSec;
			LabelLoops->Caption = FSettings->LoopsNumber;
		}
	}
}

//---------------------------------------------------------------------------
__fastcall TFrameGifMaker::~TFrameGifMaker() {
	ClearAll();

	delete FSettings;
}

//---------------------------------------------------------------------------
void TFrameGifMaker::ClearAll(void) {
	for (int i = 0; i < ListView1->Items->Count; i++) {
		TGifMakerData::DeleteGifData(ListView1->Items->Item[i]);
	}
	ListView1->Items->Clear();
	Image1->Picture->Assign(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifStartRecordExecute(TObject *Sender) {
	if (FSettings->ClearAllOnStartRecord) {
		ClearAll();
	}

	ActionGifStartRecord->Enabled = false;
	ActionGifPause->Enabled = true;
	ActionGifStop->Enabled = true;

	if (FSettings->SnapshotOnStartRecord) {
		ActionGifCapture->Execute();
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifPauseExecute(TObject *Sender) {
	//
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifStopExecute(TObject *Sender) {
	ActionGifStartRecord->Enabled = true;
	ActionGifPause->Enabled = false;
	ActionGifStop->Enabled = false;
	SpeedButtonPause->Down = false;

	if (ListView1->Items->Count) {
		ListView1->ClearSelection();
		ListView1->ItemIndex = 0;
	}
}

//---------------------------------------------------------------------------
int TFrameGifMaker::TypeToImageIndex(const TScxmlMsgType AType) {
	switch(AType) {
	case TScxmlMsgType::
		smttBeforeEnter : return 0;
	case TScxmlMsgType::
		smttBeforeTakingTransition : return 2;
	case TScxmlMsgType::
		smttUnknown : return 4;
	}
	return -1;
}

//---------------------------------------------------------------------------
TTextDisplayType TFrameGifMaker::ImageIndexToTextDisplayType(const int iImageIndex) {
	switch(iImageIndex) {
	case 0:
		return tdtEnter;
	case 2:
		return tdtTransition;
	case 4:
		return tdtCustom;
	}
	throw Exception("Can not convert ImageIndex:[" + UnicodeString(iImageIndex) + "] to TextDisplayType!");
}

//---------------------------------------------------------------------------
void TFrameGifMaker::AddImage(const TScxmlMsgType AType, const UnicodeString &sName, Graphics::TBitmap *ABitmap, const TRect &ARectShape,
	const int iWidth, const int iHeight) {

	// не добавл€ем дубликаты
	if (ListView1->Items->Count && FSettings->IgnoreDuplicates && ListView1->Items->Item[ListView1->Items->Count - 1]->Caption == sName) {
		return;
	}

	TRect ADestSource(0, 0, iWidth, iHeight);

	TGifMakerData *AGifData = new TGifMakerData();
	AGifData->Bitmap->PixelFormat = ABitmap->PixelFormat;
	AGifData->Bitmap->Width = ADestSource.Width();
	AGifData->Bitmap->Height = ADestSource.Height();
	AGifData->Bitmap->Canvas->CopyRect(ADestSource, ABitmap->Canvas, ARectShape);

	TListItem* AItem = ListView1->Items->Add();
	AItem->Caption = sName;
	AItem->Data = AGifData;
	AItem->SubItems->Add(GetItemDelay(AItem));
	ListView1->Items->Item[ListView1->Items->Count - 1]->ImageIndex = TypeToImageIndex(AType);
}

//---------------------------------------------------------------------------
void TFrameGifMaker::AddText(Graphics::TBitmap *ABitmap, const UnicodeString &sText) {
	ABitmap->Canvas->Font->Assign(FSettings->TextFont);

	int iAlign = TA_LEFT;
	switch(FSettings->TextHorizAlign) {
	case thaCenter:
		iAlign = TA_CENTER;
		break;
	case thaRight:
		iAlign = TA_RIGHT;
		break;
	}
	SetTextAlign(ABitmap->Canvas->Handle, iAlign);

	const int iOffsetX = (double)ABitmap->Width * (double)FSettings->TextMarginLeftPercent * 0.01f;
	const int iOffsetY = (double)ABitmap->Height * (double)FSettings->TextMarginTopPercent * 0.01f;
	ABitmap->Canvas->TextOut(iOffsetX, iOffsetY, sText);
}

//---------------------------------------------------------------------------
void TFrameGifMaker::AssignImageFromList(TListItem *Item, const UnicodeString &sCaption) {
	try {
		if (Graphics::TBitmap * ABmp = TGifMakerData::GetBitmap(Item)) {

			this->Image1->Width = ABmp->Width;
			this->Image1->Height = ABmp->Height;

			this->Image1->Picture->Assign(ABmp);

			if (FSettings->TextDisplayTypes.Contains(ImageIndexToTextDisplayType(Item->ImageIndex))) {
				AddText(this->Image1->Picture->Bitmap, sCaption);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ListView1SelectItem(TObject *Sender, TListItem *Item, bool Selected) {
	if (Selected && Item) {
		AssignImageFromList(Item, Item->Caption);
	}
}

//---------------------------------------------------------------------------
void TFrameGifMaker::SaveToFile(const UnicodeString &sFileName) {
	try {

		Screen->Cursor = crHourGlass;

		std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(this));
		ADialogWaitPtr->ShowDialog(L"GIF file [" + sFileName + "] is creating ...");

		std::auto_ptr<TGIFImage>AGifImagePtr(new TGIFImage());

		{ /* все расширени€ должны удалитьс€ до удалени€ GIF Image */
			std::auto_ptr<TGIFAppExtNSLoop>AGIFAppExtNSLoopPtr;
			std::vector<TGIFGraphicControlExtension*>AGifExtensions;

			for (int i = 0; i < ListView1->Items->Count; i++) {
				if (ListView1->Items->Item[i]->Data) {

					TGifMakerData *AGifData = static_cast<TGifMakerData*>(ListView1->Items->Item[i]->Data);

					const int iImgIndex = ListView1->Items->Item[i]->ImageIndex;

					ListView1->Items->Item[i]->ImageIndex = 6;
					ListView1->Refresh();

					Graphics::TBitmap * ABmp = TGifMakerData::GetBitmap(ListView1->Items->Item[i]);
					if (!ABmp)
						throw Exception("GIF Image is NULL!");

					TGIFFrame *AFrame = NULL;

					if (FSettings->TextDisplayTypes.Contains(ImageIndexToTextDisplayType(iImgIndex))) {
						std::auto_ptr<Graphics::TBitmap>ABitmapPtr(new Graphics::TBitmap());
						ABitmapPtr->Assign(ABmp);
						AddText(ABitmapPtr.get(), ListView1->Items->Item[i]->Caption);
						AFrame = AGifImagePtr->Add(ABitmapPtr.get());
					}
					else {
						AFrame = AGifImagePtr->Add(ABmp);
					}

					if (i == 0) {
						AGIFAppExtNSLoopPtr.reset(new TGIFAppExtNSLoop(AFrame));
						AGIFAppExtNSLoopPtr->Loops = FSettings->LoopsNumber;
					}

					TGIFGraphicControlExtension *AGifExt = new TGIFGraphicControlExtension(AFrame);
					AGifExt->Delay = (AGifData->Delay == -1 ? FSettings->DelayBetweenFramesMSec : AGifData->Delay) / 10;
					AGifExtensions.push_back(AGifExt);

					ListView1->Items->Item[i]->ImageIndex = iImgIndex;
				}
			}

			AGifImagePtr->SaveToFile(sFileName);

			const int iFrameCount = AGifExtensions.size();

			for (std::size_t i = 0; i < AGifExtensions.size(); i++) {
				delete AGifExtensions[i];
			}
			AGifExtensions.clear();

			WLOG_INFO(L"GIF file with %d frames [%s] was successfully saved!", iFrameCount, sFileName.c_str());
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	Screen->Cursor = crDefault;
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::FileSaveAs1Accept(TObject *Sender) {
	SaveToFile(FileSaveAs1->Dialog->FileName);
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::FileSaveAs1Update(TObject *Sender) {
	FileSaveAs1->Enabled = ListView1->Items->Count && !IsRecordMode();
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifSettingsExecute(TObject *Sender) {
	DoEditSettings();
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::DoEditSettings(const UnicodeString sItemName/* = L""*/) {
	try {
		// копируем объект настроек
		std::auto_ptr<TMemoryStream>AMemoryStream(new TMemoryStream());
		AMemoryStream->WriteComponent(FSettings);
		AMemoryStream->Position = 0;

		// отдаем смарт указателю дл€ дальнейшего удалени€
		std::auto_ptr<TComponent>AComponentPtr(FSettings->DoCreateDefaultInstance());
		AMemoryStream->ReadComponent(AComponentPtr.get());
		AComponentPtr->Assign(FSettings);

		std::auto_ptr<TFormSettingsEditor>AFormSettingsEditor(new TFormSettingsEditor(this, AComponentPtr.get()));
		AFormSettingsEditor->LabelCaption->Caption = "Edit [GIF Maker] settings";

		AFormSettingsEditor->SelectItemOrRestoreState(sItemName, FPropInspStateSettingsPtr.get());

		if (AFormSettingsEditor->ShowModal() == mrOk) {
			FSettings->Assign(AComponentPtr.get());

			UpdateExtraData();

			FPropInspStateSettingsPtr.reset(AFormSettingsEditor->PropSettingsInspector->SaveState());

			DoSaveSettings();
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void TFrameGifMaker::DoSaveSettings(void) {
	try {
		if (SettingsData) {
			TSettingsRegistryItem *AItem = SettingsData->TempRegistryItems->Map[this->Name];
			if (!AItem) {
				AItem = SettingsData->TempRegistryItems->Add();
				AItem->Key = this->Name;
			}
			FSettings->SaveToStrings(AItem->Data);

			LabelDelay->Caption = FSettings->DelayBetweenFramesMSec;
			LabelLoops->Caption = FSettings->LoopsNumber;
		}

		if (ListView1->Selected) {
			// обновл€ем
			ListView1SelectItem(ListView1, ListView1->Selected, true);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ListView1Edited(TObject *Sender, TListItem *Item, UnicodeString &S) {
	AssignImageFromList(Item, S);
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifEditExecute(TObject *Sender) {
	try {
		if (ListView1->Selected) {
			if (Graphics::TBitmap * ABmp = TGifMakerData::GetEditBitmap(ListView1->Selected)) {

				const UnicodeString sTempFile = TPath::Combine(TPath::GetTempPath(), "ScxmlEditor.bmp~");
				ABmp->SaveToFile(sTempFile);

				TFileAttributes AFileAttr = TFile::GetAttributes(sTempFile);
				TFile::SetAttributes(sTempFile, AFileAttr << Ioutils::faHidden);

				SpawnInfinite("mspaint " + AnsiQuotedStr(sTempFile, L'"'));

				ABmp->LoadFromFile(sTempFile);

				DeleteFile(sTempFile);

				// обновл€ем
				ListView1SelectItem(ListView1, ListView1->Selected, true);
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void TFrameGifMaker::CaptureImage(TCustomTree *ATree, TTreeNodeShape *AScxmlRootShape, const TScxmlMsgType AType,
	const UnicodeString &sImageName) {
	try {
		if (!ATree)
			throw Exception(L"Tree is not defined!");

		TTreeEx *ATreeEx = dynamic_cast<TTreeEx*>(ATree);
		if (!ATreeEx)
			throw Exception(L"Only TreeEx is supported!");

		switch(this->Settings->CaptureType) {
		case ctScxmlRoot: {

				if (!AScxmlRootShape)
					throw Exception(L"No root SCXML is found!");

				const int i_OFFSET = 3;

				const int iOldHoriz = ATreeEx->View3DOptions->HorizOffset;
				const int iOldVert = ATreeEx->View3DOptions->VertOffset;

				const bool bWasFocusDisabled = ATreeEx->DisableFocus;
				ATreeEx->DisableFocus = true;

				try {
					ATreeEx->View3DOptions->HorizOffset = 0;
					ATreeEx->View3DOptions->VertOffset = 0;
					const TRect ARectSource(0, 0, AScxmlRootShape->X1 + i_OFFSET, AScxmlRootShape->Y1 + i_OFFSET);

					const TRect ARectShape(AScxmlRootShape->X0, AScxmlRootShape->Y0, //
						AScxmlRootShape->X1 + i_OFFSET, AScxmlRootShape->Y1 + i_OFFSET);

					std::auto_ptr<Graphics::TBitmap>ABmpSourcePtr(ATreeEx->TeeCreateBitmap(clWhite, ARectSource));

					this->AddImage(AType, sImageName, ABmpSourcePtr.get(), ARectShape, AScxmlRootShape->Width + i_OFFSET,
						AScxmlRootShape->Height + i_OFFSET);

				}
				__finally {
					ATreeEx->DisableFocus = bWasFocusDisabled;
					ATreeEx->View3DOptions->HorizOffset = iOldHoriz;
					ATreeEx->View3DOptions->VertOffset = iOldVert;
				}
			}break;
		case ctMainForm: {
				// без этого не обновл€ет чарт
				if (AType == smttBeforeEnter) {
					ATree->Refresh();
				}

				TWinControl *AMainFormArea = GlobalDockSite();

				std::auto_ptr<Graphics::TBitmap>ABmpSourcePtr(new Graphics::TBitmap);
				ABmpSourcePtr->Width = AMainFormArea->Width;
				ABmpSourcePtr->Height = AMainFormArea->Height;
				ABmpSourcePtr->Canvas->CopyRect(AMainFormArea->ClientRect, GlobalMainForm()->Canvas, AMainFormArea->BoundsRect);

				this->AddImage(AType, sImageName, ABmpSourcePtr.get(), AMainFormArea->ClientRect, AMainFormArea->ClientRect.Width(),
					AMainFormArea->ClientRect.Height());

			}break;
		case ctScreen: {

				const TRect AScreenRect = this->Settings->CaptureRect->IsEmpty() ? Screen->DesktopRect : this->Settings->CaptureRect->Rect;

				// без этого не обновл€ет чарт
				if (AType == smttBeforeEnter) {
					ATree->Refresh();
				}

				std::auto_ptr<Graphics::TBitmap>ABmpSourcePtr(new Graphics::TBitmap);
				ABmpSourcePtr->Width = AScreenRect.Width();
				ABmpSourcePtr->Height = AScreenRect.Height();

				HWND hWnd = GetDesktopWindow();

				HDC hDc = GetDC(hWnd);
				if (hDc) {
					BitBlt(ABmpSourcePtr->Canvas->Handle, 0, 0, AScreenRect.Width(), AScreenRect.Height(), hDc, AScreenRect.Left,
						AScreenRect.Top, SRCCOPY);
					ReleaseDC(hWnd, hDc);
				}

				this->AddImage(AType, sImageName, ABmpSourcePtr.get(), TRect(0, 0, AScreenRect.Width(), AScreenRect.Height()),
					AScreenRect.Width(), AScreenRect.Height());

			}break;
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionGifCaptureExecute(TObject *Sender) {
	CaptureImage(GlobalActiveEditorTree(), GlobalActiveEditorRootScxml(), smttUnknown, Now().FormatString("hh:mm:ss.zzz"));
	// выдел€ем последний
	if (ListView1->Items->Count) {
		ListView1->ClearSelection();
		ListView1->ItemIndex = ListView1->Items->Count - 1;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifClearAllExecute(TObject *Sender) {
	ClearAll();
}
//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionGifClearAllUpdate(TObject *Sender) {
	ActionGifClearAll->Enabled = ListView1->Items->Count;
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifCaptureUpdate(TObject *Sender) {
	ActionGifCapture->Enabled = GlobalActiveEditorTree();
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifMoveUpExecute(TObject *Sender) {
	if (ListView1->Items->Count && ListView1->Selected && ListView1->Selected->Index > 0) {
		TListItem *ATempItem = ListView1->Selected;
		TListItem *AItem = ListView1->Items->Insert(ATempItem->Index - 1);
		AItem->Assign(ATempItem);

		ATempItem->Data = 0;
		ListView1->Items->Delete(ATempItem->Index);

		ListView1->ItemIndex = AItem->Index;
		ListView1->ItemFocused = AItem;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifMoveDownExecute(TObject *Sender) {
	if (ListView1->Items->Count && ListView1->Selected && ListView1->Selected->Index < (ListView1->Items->Count - 1)) {
		TListItem *ATempItem = ListView1->Selected;
		TListItem *AItem = ListView1->Items->Insert(ATempItem->Index + 2);
		AItem->Assign(ATempItem);

		ATempItem->Data = 0;
		ListView1->Items->Delete(ATempItem->Index);

		ListView1->ItemIndex = AItem->Index;
		ListView1->ItemFocused = AItem;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifCloneExecute(TObject *Sender) {
	if (ListView1->Items->Count && ListView1->Selected) {
		TListItem *ATempItem = ListView1->Selected;

		// необходимо об€зательно убрать выделение, так как при Assign оно также назначитс€
		ListView1->ClearSelection();

		TListItem *AItem = ListView1->Items->Insert(ATempItem->Index + 1);
		AItem->Assign(ATempItem);

		if (ATempItem->Data) {

			TGifMakerData * AGifDataSrc = static_cast<TGifMakerData*>(ATempItem->Data);

			AItem->Data = AGifDataSrc->Clone();

			UpdateExtraDataItem(AItem);
		}

		ListView1->ItemIndex = AItem->Index;
		ListView1->ItemFocused = AItem;
	}
}

//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionReplaceAllExecute(TObject *Sender) {

	std::auto_ptr<TSimpleReplaceDialog>AReplaceDlgPtr(new TSimpleReplaceDialog(this));
	AReplaceDlgPtr->ComboSearch->Text = SettingsData->TempRegistry->Values[this->Name + ".Search"];
	AReplaceDlgPtr->ComboReplace->Text = SettingsData->TempRegistry->Values[this->Name + ".Replace"];
	AReplaceDlgPtr->CheckReplaceAll->Visible = false;
	if (AReplaceDlgPtr->ShowModal() == mrOk) {
		for (int i = 0; i < ListView1->Items->Count; i++) {
			TReplaceFlags AReplaceFlags = TReplaceFlags() << rfReplaceAll;
			if (AReplaceDlgPtr->CheckIgnoreCase->Checked) {
				AReplaceFlags = AReplaceFlags << rfIgnoreCase;
			}
			ListView1->Items->Item[i]->Caption = StringReplace(ListView1->Items->Item[i]->Caption, AReplaceDlgPtr->ComboSearch->Text,
				AReplaceDlgPtr->ComboReplace->Text, AReplaceFlags);
		}

		SettingsData->TempRegistry->Values[this->Name + ".Search"] = AReplaceDlgPtr->ComboSearch->Text;
		SettingsData->TempRegistry->Values[this->Name + ".Replace"] = AReplaceDlgPtr->ComboReplace->Text;

		if (ListView1->Selected) {
			// обновл€ем
			ListView1SelectItem(ListView1, ListView1->Selected, true);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifTextTransitionsExecute(TObject *Sender) {
	FSettings->TextDisplayTypes = FSettings->TextDisplayTypes.Contains(tdtTransition) ? (FSettings->TextDisplayTypes >> tdtTransition) :
		(FSettings->TextDisplayTypes << tdtTransition);
	DoSaveSettings();
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifTextTransitionsUpdate(TObject *Sender) {
	ActionGifTextTransitions->Checked = FSettings->TextDisplayTypes.Contains(tdtTransition);
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifTextCustomExecute(TObject *Sender) {
	FSettings->TextDisplayTypes = FSettings->TextDisplayTypes.Contains(tdtCustom) ? (FSettings->TextDisplayTypes >> tdtCustom) :
		(FSettings->TextDisplayTypes << tdtCustom);
	DoSaveSettings();
}
//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionGifTextCustomUpdate(TObject *Sender) {
	ActionGifTextCustom->Checked = FSettings->TextDisplayTypes.Contains(tdtCustom);
}
//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionGifTextEnterExecute(TObject *Sender) {
	FSettings->TextDisplayTypes = FSettings->TextDisplayTypes.Contains(tdtEnter) ? (FSettings->TextDisplayTypes >> tdtEnter) :
		(FSettings->TextDisplayTypes << tdtEnter);
	DoSaveSettings();
}
//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionGifTextEnterUpdate(TObject *Sender) {
	ActionGifTextEnter->Checked = FSettings->TextDisplayTypes.Contains(tdtEnter);
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ActionGifIgnoreDuplicatesExecute(TObject *Sender) {
	FSettings->IgnoreDuplicates = !FSettings->IgnoreDuplicates;
}
//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::ActionGifIgnoreDuplicatesUpdate(TObject *Sender) {
	ActionGifIgnoreDuplicates->ImageIndex = FSettings->IgnoreDuplicates ? 24 : 23;
	ActionGifIgnoreDuplicates->Checked = FSettings->IgnoreDuplicates;
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::Rename1Click(TObject *Sender) {
	if (ListView1->Selected) {
		ListView1->Selected->EditCaption();
	}

}
//---------------------------------------------------------------------------

void __fastcall TFrameGifMaker::Delete1Click(TObject *Sender) {
	// провер€ем, чтобы не было в режиме редактировани€,
	// так как при удалении текста, будет и элемент удал€ть
	if (!ListView1->IsEditing()) {

		ListView1->Items->BeginUpdate();
		try {
			for (int i = ListView1->Items->Count - 1; i >= 0; i--) {
				if (ListView1->Items->Item[i]->Selected) {
					TGifMakerData::DeleteGifData(ListView1->Items->Item[i]);
					ListView1->Items->Delete(i);
				}
			}
		}
		__finally {
			ListView1->Items->EndUpdate();
		}

		for (int i = 0; i < ListView1->Items->Count; i++) {
			if (ListView1->Items->Item[i]->Focused) {
				ListView1->Items->Item[i]->Selected = true;
				break;
			}
		}

		// в этом случае не очиститс€ картинка
		if (!ListView1->Items->Count) {
			Image1->Picture->Assign(NULL);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::Revert1Click(TObject *Sender) {
	ListView1->Items->BeginUpdate();
	try {
		for (int i = ListView1->Items->Count - 1; i >= 0; i--) {
			if (ListView1->Items->Item[i]->Selected) {
				TGifMakerData::RevertBitmap(ListView1->Items->Item[i]);
			}
		}
	}
	__finally {
		ListView1->Items->EndUpdate();
	}

	if (ListView1->Selected) {
		AssignImageFromList(ListView1->Selected, ListView1->Selected->Caption);
	}

}

//---------------------------------------------------------------------------
int TFrameGifMaker::GetItemDelay(TListItem *Item) {
	if (Item && Item->Data) {
		TGifMakerData *AGifData = static_cast<TGifMakerData*>(Item->Data);
		return AGifData->Delay == -1 ? FSettings->DelayBetweenFramesMSec : AGifData->Delay;
	}
	return -1;
}

//---------------------------------------------------------------------------
void __fastcall TFrameGifMaker::ChangeDelay1Click(TObject *Sender) {
	try {
		if (ListView1->Selected && ListView1->Selected->Data) {
			TGifMakerData *AGifData = static_cast<TGifMakerData*>(ListView1->Selected->Data);
			UnicodeString sDelay = AGifData->Delay == -1 ? FSettings->DelayBetweenFramesMSec : AGifData->Delay;
			if (InputQuery("Edit Delay", "Enter delay value in milliseconds", sDelay)) {
				const int iDelay = sDelay.ToInt();
				if (iDelay < 10)
					throw Exception("Delay value:[" + sDelay + "] must not be less than 10ms");

				const int iResultDelay = iDelay == FSettings->DelayBetweenFramesMSec ? -1 : iDelay; ;

				ListView1->Items->BeginUpdate();
				try {
					for (int i = ListView1->Items->Count - 1; i >= 0; i--) {
						if (ListView1->Items->Item[i]->Selected) {
							TGifMakerData::SetDelay(ListView1->Items->Item[i], iResultDelay);
							UpdateExtraDataItem(ListView1->Items->Item[i]);
						}
					}
				}
				__finally {
					ListView1->Items->EndUpdate();
				}
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"CHANGE DELAY> %s", E->Message.c_str());
	}
}

//---------------------------------------------------------------------------
void TFrameGifMaker::UpdateExtraDataItem(TListItem *AItem) {
	if (AItem) {
		const int iDelay = GetItemDelay(AItem);

		if (AItem->SubItems->Count) {
			AItem->SubItems->Strings[0] = iDelay;
		}
		else {
			AItem->SubItems->Add(iDelay);
		}
	}
}

//---------------------------------------------------------------------------
void TFrameGifMaker::UpdateExtraData(void) {
	for (int i = 0; i < ListView1->Items->Count; i++) {
		UpdateExtraDataItem(ListView1->Items->Item[i]);
	}
}

//---------------------------------------------------------------------------

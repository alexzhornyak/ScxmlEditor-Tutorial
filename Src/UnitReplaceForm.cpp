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

#include "UnitReplaceForm.h"

#include <IOUtils.hpp>

#include <boost/regex.hpp>

#include "UnitSettings.h"
#include "UnitReplaceRefactorForm.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitScxmlBaseShape.h"
#include "UnitStateMachineConnection.h"
#include "UnitStateMachineUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

typedef enum {
	rfeReplaceAll, rfeIgnoreCase, rfeUseRegex, rfeRegexLiteral
}TReplaceFlagExt;
typedef Set<TReplaceFlagExt, rfeReplaceAll, rfeRegexLiteral>TReplaceFlagsExt;

UnicodeString StringReplaceExt(const UnicodeString &sText, const UnicodeString &sWhat, const UnicodeString &sTo,
	const TReplaceFlagsExt AFlagsExt) {
	if (AFlagsExt.Contains(rfeUseRegex)) {
		try {
			boost::wregex e = AFlagsExt.Contains(rfeIgnoreCase) ? //
			boost::wregex(sWhat.c_str(), boost::regex::icase) : //
			boost::wregex(sWhat.c_str());

			std::wstring wsText = sText.c_str();
			std::wstring wsWhat = sTo.c_str();

			boost::match_flag_type flags = boost::match_default;

			if (!AFlagsExt.Contains(rfeReplaceAll)) {
				flags = flags | boost::format_first_only;
			}
			if (AFlagsExt.Contains(rfeRegexLiteral)) {
				flags = flags | boost::regex_constants::match_flags::format_literal;
			}

			return boost::regex_replace(wsText, e, wsWhat, flags).c_str();
		}
		catch(std::exception & e) {
			throw Exception(e.what());
		}
		return sText;
	}
	else {
		TReplaceFlags AFlags;
		if (AFlagsExt.Contains(rfeReplaceAll)) {
			AFlags = AFlags << rfReplaceAll;
		}
		if (AFlagsExt.Contains(rfeIgnoreCase)) {
			AFlags = AFlags << rfIgnoreCase;
		}
		return StringReplace(sText, sWhat, sTo, AFlags);
	}
}

// ---------------------------------------------------------------------------
void __fastcall ReplaceTextInObject(TObject *AObject, TMetaClass *AIgnoredClass, const UnicodeString &sText, const UnicodeString &sWhat,
	const TReplaceFlagsExt AFlags, const UnicodeString &sObjectPath, TListView *AReplacePropsList, UnicodeString sSimpleText) {
	if (!AObject) {
		return;
	}

	TCustomTreeElement * ACustomTreeElement = dynamic_cast<TCustomTreeElement*>(AObject);
	if (ACustomTreeElement) {
		sSimpleText = ACustomTreeElement->SimpleText;
	}
	TPropList APropList;

	TTypeKinds TypeKinds = TTypeKinds() << tkString << tkLString << tkWString << tkUString << tkClass;

	const int iRes = GetPropList(PTypeInfo(AObject->ClassInfo()), TypeKinds, (TPropList*)(&APropList), false);

	for (int i = 0; i < iRes; i++) {
		TTypeInfo *ATypeInfo = *APropList[i]->PropType;

		const UnicodeString sPropName = AnsiString(APropList[i]->Name);
		const UnicodeString sTypeKindName = AnsiString(ATypeInfo->Name);

		bool bIsIgnored = AIgnoredClass && AObject->InheritsFrom(AIgnoredClass) && IsPublishedProp(AIgnoredClass, sPropName);

		if (!bIsIgnored && APropList[i]->SetProc) {

			switch(ATypeInfo->Kind) {
			case tkClass: {
					TObject *AGetObject = GetObjectProp(AObject, sPropName);
					if (AGetObject) {
						if (AGetObject->InheritsFrom(__classid(TCollection))) {
							TCollection * ACollection = dynamic_cast<TCollection*>(AGetObject);

							const UnicodeString sGetObjectPath = TPath::Combine(sObjectPath, sPropName);
							ReplaceTextInObject(ACollection, AIgnoredClass, sText, sWhat, AFlags, sGetObjectPath, AReplacePropsList,
								sSimpleText);

							for (int k = 0; k < ACollection->Count; k++) {
								ReplaceTextInObject(ACollection->Items[k], AIgnoredClass, sText, sWhat, AFlags,
									TPath::Combine(sGetObjectPath, "Item[" + UnicodeString(k) + "]"), AReplacePropsList, sSimpleText);
							}
						}
						else if (AGetObject->InheritsFrom(__classid(TStrings))) {
							TStrings *AStrings = dynamic_cast<TStrings*>(AGetObject);
							if (AStrings) {
								const UnicodeString sGetObjectPath = TPath::Combine(sObjectPath, sPropName);

								const UnicodeString sValue = StringReplaceExt(AStrings->Text, sText, sWhat, AFlags);
								if (sValue != AStrings->Text) {
									TListItem *AItem = AReplacePropsList->Items->Add();
									AItem->Checked = true;
									AItem->Data = AGetObject;
									AItem->Caption = sGetObjectPath;
									AItem->SubItems->Add(sSimpleText);
									AItem->SubItems->Add("Text");
									AItem->SubItems->Add(sText);
									AItem->SubItems->Add(sWhat);
									AItem->SubItems->Add(AStrings->Text);
									AItem->SubItems->Add(sValue);
								}
							}
						}
						else if (AGetObject->InheritsFrom(__classid(TPersistent))) {
							const UnicodeString sGetObjectPath = TPath::Combine(sObjectPath, sPropName);
							ReplaceTextInObject(AGetObject, AIgnoredClass, sText, sWhat, AFlags, sGetObjectPath, AReplacePropsList,
								sSimpleText);
						}
					}
					else {
						// объект пустой
					}

				}break;
			default: {
					const UnicodeString sOldValue = GetPropValue(AObject, sPropName);
					const UnicodeString sValue = StringReplaceExt(sOldValue, sText, sWhat, AFlags);
					if (sValue != sOldValue) {
						const UnicodeString sGetObjectPath = TPath::Combine(sObjectPath, sPropName);

						TListItem *AItem = AReplacePropsList->Items->Add();
						AItem->Checked = true;
						AItem->Data = AObject;
						AItem->Caption = sObjectPath;
						AItem->SubItems->Add(sSimpleText);
						AItem->SubItems->Add(sPropName);
						AItem->SubItems->Add(sText);
						AItem->SubItems->Add(sWhat);
						AItem->SubItems->Add(sOldValue);
						AItem->SubItems->Add(sValue);
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
void ReplaceTextInScxmlBaseShape(TScxmlBaseShape *AShape, const UnicodeString &sText, const UnicodeString &sWhat,
	const TReplaceFlagsExt AFlags, TListView *AReplacePropsList) {
	if (!AShape)
		return;

	ReplaceTextInObject(AShape, __classid(TTreeNodeShape), sText, sWhat, AFlags, AShape->Name, AReplacePropsList, "");

	for (int i = 0; i < AShape->Connections->Count; i++) {
		ReplaceTextInObject(dynamic_cast<TStateMachineConnection*>(AShape->Connections->Items[i]), __classid(TTreeConnection), sText,
			sWhat, AFlags, AShape->Connections->Items[i]->Name, AReplacePropsList, "");
	}

	for (int i = 0; i < AShape->Children->Count; i++) {
		ReplaceTextInScxmlBaseShape(dynamic_cast<TScxmlBaseShape*>(AShape->Children->Items[i]), sText, sWhat, AFlags, AReplacePropsList);
	}
}

//---------------------------------------------------------------------------
__fastcall TFormReplaceDlg::TFormReplaceDlg(TStateMachineEditor* Owner, const UnicodeString &sOld, const UnicodeString &sNew) : TForm
	(Owner) {
	FStateMachineEditor = Owner;
	FReplaceShape = NULL;

	ComboSearch->Text = sOld;
	ComboReplace->Text = sNew;

	for (int i = 0; i < this->ComponentCount; i++) {
		TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(this->Components[i]);
		if (ACheckBox) {
			FDefaultValues.insert(std::make_pair(ACheckBox, ACheckBox->Checked));
		}
	}

	this->CheckUseRegExp->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["FormReplace.CheckUseRegExp.Checked"],
		this->CheckUseRegExp->Checked);
	this->CheckIgnoreCase->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["FormReplace.CheckIgnoreCase.Checked"],
		this->CheckIgnoreCase->Checked);
	this->CheckReplaceAll->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["FormReplace.CheckReplaceAll.Checked"],
		this->CheckReplaceAll->Checked);
	this->CheckRegexLiteral->Checked = StrToBoolDef(SettingsData->TempRegistry->Values["FormReplace.CheckRegexLiteral.Checked"],
		this->CheckRegexLiteral->Checked);

}

//---------------------------------------------------------------------------

void __fastcall TFormReplaceDlg::ExecuteDialog(Teetree::TTreeNodeShape *AReplaceShape) {

	FReplaceShape = AReplaceShape;

	this->ComboSearch->Items->Assign(SettingsData->SearchLines);
	this->ComboReplace->Items->Assign(SettingsData->ReplaceLines);

	if (FStateMachineEditor && FStateMachineEditor->StateMachineEditorUnit) {
		this->Caption = UnicodeString().sprintf(L"Replace dialog [%s] [%s]", FStateMachineEditor->StateMachineEditorUnit->FileName.c_str(),
			FReplaceShape->SimpleText.c_str());
	}

	this->Show();
}

//---------------------------------------------------------------------------
void __fastcall TFormReplaceDlg::BtnReplaceClick(TObject *Sender) {
	try {
		if (!FStateMachineEditor)
			throw Exception("FStateMachineEditor==NULL!");

		if (FStateMachineEditor->TheTree->Items->IndexOf(FReplaceShape) == -1)
			throw Exception("Shape for replace is no more longer valid!");

		const int i_MAX_ITEMS_COUNT = 20;

		if (SettingsData->SearchLines->IndexOf(this->ComboSearch->Text) == -1) {
			SettingsData->SearchLines->Insert(0, this->ComboSearch->Text);
		}
		if (SettingsData->SearchLines->Count > i_MAX_ITEMS_COUNT) {
			SettingsData->SearchLines->Delete(SettingsData->SearchLines->Count - 1);
		}
		if (SettingsData->ReplaceLines->IndexOf(this->ComboReplace->Text) == -1) {
			SettingsData->ReplaceLines->Insert(0, this->ComboReplace->Text);
		}
		if (SettingsData->ReplaceLines->Count > i_MAX_ITEMS_COUNT) {
			SettingsData->ReplaceLines->Delete(SettingsData->ReplaceLines->Count - 1);
		}

		TReplaceFlagsExt AFlags;
		if (this->CheckReplaceAll->Checked) {
			AFlags = AFlags << rfeReplaceAll;
		}
		if (this->CheckIgnoreCase->Checked) {
			AFlags = AFlags << rfeIgnoreCase;
		}
		if (this->CheckUseRegExp->Checked) {
			AFlags = AFlags << rfeUseRegex;
		}
		if (this->CheckRegexLiteral->Checked) {
			AFlags = AFlags << rfeRegexLiteral;
		}

		std::auto_ptr<TReplaceRefactor>AReplaceRefactorPtr(new TReplaceRefactor(this));

		ReplaceTextInScxmlBaseShape(dynamic_cast<TScxmlBaseShape*>(FReplaceShape), this->ComboSearch->Text, this->ComboReplace->Text,
			AFlags, AReplaceRefactorPtr->ListViewReplace);

		if (AReplaceRefactorPtr->ListViewReplace->Items->Count && AReplaceRefactorPtr->ShowModal() == mrOk) {

			for (int i = 0; i < AReplaceRefactorPtr->ListViewReplace->Items->Count; i++) {
				if (!AReplaceRefactorPtr->ListViewReplace->Items->Item[i]->Checked)
					continue;

				const UnicodeString &sPropName = AReplaceRefactorPtr->ListViewReplace->Items->Item[i]->SubItems->Strings[1];

				TObject *AObject = reinterpret_cast<TObject*>(AReplaceRefactorPtr->ListViewReplace->Items->Item[i]->Data);
				if (AObject) {
					if (AObject->InheritsFrom(__classid(TStrings))) {
						TStrings * AStrings = dynamic_cast<TStrings*>(AObject);
						if (AStrings) {
							AStrings->Text = StringReplaceExt(AStrings->Text, this->ComboSearch->Text, this->ComboReplace->Text, AFlags);
						}
					}
					else {
						const UnicodeString sOldValue = GetPropValue(AObject, sPropName);
						const UnicodeString sValue = StringReplaceExt(sOldValue, this->ComboSearch->Text, this->ComboReplace->Text, AFlags);
						SetPropValue(AObject, sPropName, sValue);
					}
				}
			}

			FStateMachineEditor->FillNodeTree();
			FStateMachineEditor->TeeModified(True, 1, UnicodeString().sprintf(L"Replace [%s] by [%s]", this->ComboSearch->Text.c_str(),
					this->ComboReplace->Text.c_str()));
			if (FStateMachineEditor->PropSettingsInspector) {
				FStateMachineEditor->PropSettingsInspector->UpdateContent();
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	this->Close();
}

//---------------------------------------------------------------------------
void __fastcall TFormReplaceDlg::BtnCancelClick(TObject *Sender) {
	this->Close();
}

//---------------------------------------------------------------------------
void __fastcall TFormReplaceDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	switch(Key) {
	case VK_RETURN: {
			if (Shift.Contains(ssCtrl)) {
				Key = 0;
				BtnReplaceClick(BtnReplace);
			}
		}break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormReplaceDlg::FormClose(TObject *Sender, TCloseAction &Action) {
	try {
		SettingsData->TempRegistry->Values["FormReplace.CheckUseRegExp.Checked"] = BoolToStr(this->CheckUseRegExp->Checked);
		SettingsData->TempRegistry->Values["FormReplace.CheckIgnoreCase.Checked"] = BoolToStr(this->CheckIgnoreCase->Checked);
		SettingsData->TempRegistry->Values["FormReplace.CheckReplaceAll.Checked"] = BoolToStr(this->CheckReplaceAll->Checked);
		SettingsData->TempRegistry->Values["FormReplace.CheckRegexLiteral.Checked"] = BoolToStr(this->CheckRegexLiteral->Checked);
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormReplaceDlg::OnChangeValue(TObject *Sender) {
	std::map<TCheckBox*, bool>::iterator it = FDefaultValues.find(dynamic_cast<TCheckBox*>(Sender));
	if (it != FDefaultValues.end()) {
		it->first->Font->Style = (it->first->Checked == it->second ? TFontStyles() : (TFontStyles() << fsBold));
	}
}
//---------------------------------------------------------------------------

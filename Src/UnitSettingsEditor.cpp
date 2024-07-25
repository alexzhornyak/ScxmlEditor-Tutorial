/** *********************************************************************************
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
 ************************************************************************************** */

#include <vcl.h>
#pragma hdrstop

// header
#include "UnitSettingsEditor.h"

#include "Log4cpp_VCL.hpp"

// local
#include "UnitRegisterDefaultEditors.h"
#include "UnitSettings.h"
#include "UnitPropInspectorExtensions.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDDsgObjects"
#pragma link "LMDDsgPropInsp"
#pragma link "LMDInsPropInsp"
#pragma resource "*.dfm"

// ---------------------------------------------------------------------------
__fastcall TFormSettingsEditor::TFormSettingsEditor(TComponent* Owner, TPersistent *APersistent) : TForm(Owner),
FPersistent(APersistent), m_b_modified(false) {
	if (!FPersistent)
		throw Exception("Can not edit NULL object!");

	LMDDesignObjects1->Add(FPersistent);

	RegisterDefaultPropertyEditors(PropSettingsInspector);

}

// ---------------------------------------------------------------------------
__fastcall TFormSettingsEditor::~TFormSettingsEditor() {
	LMDDesignObjects1->Clear();
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::SpBtnCollapseClick(TObject *Sender) {
	if (PropSettingsInspector->ArrangeKind == akByCategory) {
		PropSettingsInspector->CollapseAllCategories();
	}
	else {
		for (int i = 0; i < PropSettingsInspector->Items->Count; i++) {
			PropSettingsInspector->Items->Items[i]->Collapse();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::SpBtnExpandClick(TObject *Sender) {
	if (PropSettingsInspector->ArrangeKind == akByCategory) {
		PropSettingsInspector->ExpandAllCategories();
	}
	else {
		for (int i = 0; i < PropSettingsInspector->Items->Count; i++) {
			PropSettingsInspector->Items->Items[i]->Expand();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::BtnArrangeTypeClick(TObject *Sender) {
	PropSettingsInspector->ArrangeKind = BtnArrangeType->Down ? akByCategory : akByName;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::FormCloseQuery(TObject *Sender, bool &CanClose) {
	if (m_b_modified) {
		String Caption = "WARNING!";
		String Text = "Settings are modified! Are you sure to quit?";
		CanClose = Application->MessageBoxA(Text.c_str(), Caption.c_str(),
			MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2) == IDOK;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFormSettingsEditor::PropSettingsInspectorChange(TObject *Sender) {
	m_b_modified = true;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::BtnOkClick(TObject *Sender) {
	m_b_modified = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::FileOpen1Accept(TObject *Sender) {
	try {
		TSettingsBase * ASettingsBase = dynamic_cast<TSettingsBase*>(FPersistent);
		if (ASettingsBase) {

			ASettingsBase->LoadFromFile(FileOpen1->Dialog->FileName);

			PropSettingsInspector->UpdateItems();
		}
		else
			throw Exception("Can not cast <" + FPersistent->ClassName() + "> to <TSettingsBase>");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::FileSaveAs1Accept(TObject *Sender) {
	try {
		TSettingsBase * ASettingsBase = dynamic_cast<TSettingsBase*>(FPersistent);
		if (ASettingsBase) {

			ASettingsBase->SaveToFile(FileSaveAs1->Dialog->FileName);

			WLOG_DEBUG(L"Successfully saved to <%s>", FileSaveAs1->Dialog->FileName.c_str());
		}
		else
			throw Exception("Can not cast <" + FPersistent->ClassName() + "> to <TSettingsBase>");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::FileImportAccept(TObject *Sender) {
	try {
		TSettingsBase * ASettingsBase = dynamic_cast<TSettingsBase*>(FPersistent);
		if (ASettingsBase) {

			ASettingsBase->ImportFromFile(FileImport->Dialog->FileName);

			PropSettingsInspector->UpdateItems();
		}
		else
			throw Exception("Can not cast <" + FPersistent->ClassName() + "> to <TSettingsBase>");
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::ActionResetToDefaultExecute(TObject *Sender) {
	String Caption = "WARNING";
	String Text = "Are you sure to reset all settings to default?";
	if (Application->MessageBoxA(Text.c_str(), Caption.c_str(), MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
		try {
			TSettingsBase * ASettingsBase = dynamic_cast<TSettingsBase*>(FPersistent);
			if (ASettingsBase) {

				ASettingsBase->ResetToDefault();

				PropSettingsInspector->UpdateItems();
			}
			else
				throw Exception("Can not cast <" + FPersistent->ClassName() + "> to <TSettingsBase>");
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::ActionResetSelectedToDefaultExecute(TObject *Sender) {
	if (PropSettingsInspector->ActiveItem) {

		TLMDPropertyInspectorItem *AItem = PropSettingsInspector->ActiveItem;

		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->StrictDelimiter = true;
		AStringListPtr->Delimiter = L'.';

		while (AItem) {
			AStringListPtr->Insert(0, AItem->PropName);
			AItem = AItem->Parent;
		}

		for (int i = 0; i < PropSettingsInspector->Selection->Count; i++) {
			TPersistent *AInstance = PropSettingsInspector->Selection->Item[i];

			TSettingsBase * ASettingsBase = dynamic_cast<TSettingsBase*>(AInstance);
			if (ASettingsBase) {
				ASettingsBase->ResetPropertyToDefault(AStringListPtr->DelimitedText);
			}
		}

		PropSettingsInspector->UpdateContent();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::PropSettingsInspectorFilterProp(TObject *Sender, TPersistent *AInstance,
	ILMDProperty *APropInfo, bool AIsSubProp, bool &AIncludeProp) {

	Propinspext::PropSettingsInspectorFilterProp(Sender, AInstance, APropInfo, AIsSubProp, AIncludeProp);

}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::PropSettingsInspectorGetCaptionColor(TObject *Sender,
	TLMDPropertyInspectorItem *AItem, TColor &AColor) {

	Propinspext::PropSettingsInspectorGetCaptionColor(Sender, AItem, AColor);
}

// ---------------------------------------------------------------------------

void __fastcall TFormSettingsEditor::PropSettingsInspectorGetEditorClass(TObject *Sender, TPersistent *AInstance,
	ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {

	Propinspext::PropSettingsInspectorGetEditorClass(Sender, AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
void __fastcall TFormSettingsEditor::SelectItemOrRestoreState(const UnicodeString &sItemName,
	TLMDPropPageStateObject *APropStateObject) {
	if (!sItemName.IsEmpty()) {
		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->Delimiter = L'.';
		AStringListPtr->StrictDelimiter = true;
		AStringListPtr->DelimitedText = sItemName;

		TLMDPropertyInspectorItem* AItem = NULL;
		for (int i = 0; i < AStringListPtr->Count; i++) {

			AItem = i == 0 ? this->PropSettingsInspector->Items->Find(AStringListPtr->Strings[i]) : AItem->Find
				(AStringListPtr->Strings[i]);
			if (!AItem) {
				WLOG_WARNING(L"Can not find property item:[%s]. Full path:[%s]", AStringListPtr->Strings[i].c_str(),
					sItemName.c_str());
				break; // надо обязательно выйти, так как вверху без проверки используется AItem
			}
			else {
				// необходимо разворачивать, иначе попросту не будет видеть свёрнутых свойств
				AItem->Expand();
			}
		}

		if (AItem) {
			AItem->Active = true;
		}
	}
	else {
		if (APropStateObject) {
			this->PropSettingsInspector->RestoreState(APropStateObject, false);
		}
	}

    // HACK: scroll can be broken if there are no active items yet
	if (!this->PropSettingsInspector->ActiveItem) {
		for (int i = 0; i < this->PropSettingsInspector->Items->Count; i++) {
			this->PropSettingsInspector->Items->Items[i]->Active = true;
			break;
		}
	}
}

// ---------------------------------------------------------------------------

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

#include "UnitSearchForm.h"

#include <vector>

#include <boost/regex.hpp>

#include <DateUtils.hpp>

#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitMain.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitStateMachineUnit.h"
#include "UnitFrameSearchResults.h"
#include "UnitTreeEx.h"
#include "UnitDialogRegExp.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElCLabel"
#pragma resource "*.dfm"
TFormSearch *FormSearch = 0;

// ---------------------------------------------------------------------------
bool StrMatchWhat(const UnicodeString &sOriginalText, const UnicodeString &sFindText, const TEditorSearchTypes ATypes,
	TMatchFind &sMatchOut) {
	bool bFind = false;
	UnicodeString sText = sOriginalText;
	UnicodeString sFindTextTemp = sFindText;

	if (ATypes.Contains(estTrim)) {
		sText = sText.Trim();
	}

	int iIndex = 0;

	if (ATypes.Contains(estUseRegexp)) {

		try {
			boost::wregex e = ATypes.Contains(estCaseSensitive) ? //
			boost::wregex(sFindTextTemp.c_str()) : //
			boost::wregex(sFindTextTemp.c_str(), boost::regex::icase);

			boost::wsmatch what;

			std::wstring wstr = sText.c_str();
			if (boost::regex_search(wstr, what, e)) {
				iIndex = what.position() + 1;

				if (what.length()) {
					sFindTextTemp = sText.SubString(iIndex, what.length());
				}

				bFind = true;
			}
		}
		catch(std::exception & e) {
			LOG_ERROR("ERROR> %s. RegExp:[%s]", e.what(), sFindTextTemp.c_str());
		}

	}
	else {
		if (!ATypes.Contains(estCaseSensitive)) {
			sText = sText.UpperCase();
		}
		if (ATypes.Contains(estWholeWord)) {
			bFind = sText == sFindTextTemp;
			iIndex = sText.IsEmpty() ? 0 : 1;
		}
		else {
			iIndex = sText.Pos(sFindTextTemp);
			bFind = iIndex;
		}
	}

	if (bFind) {

		const int i_MAX_OFFSET = 3;
		const int i_MAX_OUT_LENGTH = 60;

		UnicodeString sOriginalWhat = sOriginalText.SubString(iIndex, sFindTextTemp.Length());
		UnicodeString sLeftPart = iIndex > 1 ? sOriginalText.SubString(1, iIndex - 1) : UnicodeString(L"");
		UnicodeString sRightPart = sOriginalText.SubString(iIndex + sFindTextTemp.Length(),
			sOriginalText.Length() - (iIndex + sFindTextTemp.Length() - 1));

		int iOffset = (i_MAX_OUT_LENGTH - sOriginalWhat.Length()) / 2;
		if (iOffset < i_MAX_OFFSET) {
			iOffset = i_MAX_OFFSET;
		}

		if (sLeftPart.Length() > iOffset) {
			sLeftPart = "..." + sLeftPart.Delete(1, sLeftPart.Length() - iOffset);
		}

		if (sRightPart.Length() > iOffset) {
			sRightPart = sRightPart.SubString(1, iOffset) + "...";
		}

		sMatchOut.sLeft = sLeftPart;
		sMatchOut.sWhat = sOriginalWhat;
		sMatchOut.sRight = sRightPart;

		sMatchOut.sHTMLOut = LMDTextToXML(sLeftPart) + L"<B>" + LMDTextToXML(sOriginalWhat) + L"</B>" + LMDTextToXML(sRightPart);
	}
	return bFind;
}

// ---------------------------------------------------------------------------
// -------------------------------- TFormSearch ------------------------------
// ---------------------------------------------------------------------------
__fastcall TFormSearch::TFormSearch(TComponent* Owner) : TForm(Owner), m_b_FromBeginning(true), m_iDockPanel(0), m_iUnit(0) {
	try {

		for (int i = 0; i < this->ComponentCount; i++) {
			if (TRadioButton * ARadioButton = dynamic_cast<TRadioButton*>(this->Components[i])) {
				FDefaultValues.insert(std::make_pair(ARadioButton, ARadioButton->Checked));
			}
			else if (TCheckBox * ACheckSource = dynamic_cast<TCheckBox*>(this->Components[i])) {
				FDefaultValues.insert(std::make_pair(ACheckSource, ACheckSource->Checked));
			}
		}

		if (SettingsData) {
			this->CheckCloseTabIfNotFound->Checked = StrToBoolDef
				(SettingsData->TempRegistry->Values["FormSearch.CheckCloseTabIfNotFound.Checked"], this->CheckCloseTabIfNotFound->Checked);
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::EditSearchChange(TObject *Sender) {

	m_b_FromBeginning = true;
	m_iDockPanel = 0;
	m_iUnit = 0;

	BtnNext->Enabled = !EditSearch->Text.IsEmpty();
	if (FOnSearchTextChanged) {
		FOnSearchTextChanged(Sender);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::EditSearchKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {
	if (Key == VK_RETURN) {
		BtnNext->Click();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::FormShow(TObject *Sender) {
	EditSearch->SelectAll();
	EditSearch->SetFocus();

	m_b_FromBeginning = true;
	m_iDockPanel = 0;
	m_iUnit = 0;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::BtnCancelClick(TObject *Sender) {
	this->Close();
}

// ---------------------------------------------------------------------------
TEditorSearchTypes TFormSearch::ToSearchTypes() {
	TEditorSearchTypes ATypes;
	if (CheckMatchCase->Checked) {
		ATypes = ATypes << estCaseSensitive;
	}
	if (CheckBoxWholeWord->Checked) {
		ATypes = ATypes << estWholeWord;
	}
	if (CheckStateText->Checked) {
		ATypes = ATypes << estStateText;
	}
	if (CheckAllProperties->Checked) {
		ATypes = ATypes << estAllProperties;
	}
	if (CheckTransitionText->Checked) {
		ATypes = ATypes << estTransitionText;
	}
	if (CheckTrim->Checked) {
		ATypes = ATypes << estTrim;
	}
	if (RadioModeSelectedStates->Checked) {
		ATypes = ATypes << estSelectedStatesOnly;
	}
	if (CheckUseRegExp->Checked) {
		ATypes = ATypes << estUseRegexp;
	}
	return ATypes;
}

// ---------------------------------------------------------------------------
void TFormSearch::FromSearchTypes(const TEditorSearchTypes ATypes) {

	CheckMatchCase->Checked = ATypes.Contains(estCaseSensitive);

	CheckBoxWholeWord->Checked = ATypes.Contains(estWholeWord);

	CheckStateText->Checked = ATypes.Contains(estStateText);

	CheckAllProperties->Checked = ATypes.Contains(estAllProperties);

	CheckTransitionText->Checked = ATypes.Contains(estTransitionText);

	CheckTrim->Checked = ATypes.Contains(estTrim);

	CheckUseRegExp->Checked = ATypes.Contains(estUseRegexp);

	RadioModeSelectedStates->Checked = ATypes.Contains(estSelectedStatesOnly);
}

// ---------------------------------------------------------------------------
TEditorSearchDocumentType TFormSearch::GetEditorSearchType(void) {
	TEditorSearchDocumentType ADocType = TEditorSearchDocumentType::esdtCurrentDoc;
	if (RadioDocTypeOpened->Checked) {
		ADocType = TEditorSearchDocumentType::esdtOpenedDocs;
	}
	else if (RadioDocTypeWholeProject->Checked) {
		ADocType = TEditorSearchDocumentType::esdtProject;
	}
	return ADocType;
}

// ---------------------------------------------------------------------------
void TFormSearch::SetEditorSearchType(const TEditorSearchDocumentType val) {
	RadioDocTypeCurrent->Checked = val == esdtCurrentDoc;
	RadioDocTypeOpened->Checked = val == esdtOpenedDocs;
	RadioDocTypeWholeProject->Checked = val == esdtProject;
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::BtnNextClick(TObject *Sender) {

	BtnNext->Enabled = false;

	FrameSearchResults->HTMLSearch->Clear();

	const bool bSelectAndStop = !CheckShowResultsInSeparateWindow->Checked;

	try {
		bool bFound = false;

		if (!bSelectAndStop) {
			if (!FormScxmlGui->DockPanelSearch->Parent) {
				FormScxmlGui->LMDDockSite1->DockControl(FormScxmlGui->DockPanelSearch, FormScxmlGui->LMDDockSite1->RootZone, alBottom);
				WLOG_DEBUG(L"Dock panel [%s] is undocked! Dock alBottom!", FormScxmlGui->DockPanelSearch->Caption.c_str());
			}

			FormScxmlGui->DockPanelSearch->Show();
		}

		this->Caption = L"Search";

		switch(this->GetEditorSearchType()) {
		case TEditorSearchDocumentType::
			esdtCurrentDoc : {

				TStateMachineDockPanel *APanel = FormScxmlGui->ActiveEditorDockPanel;
				if (APanel) {

					this->Caption = L"Search [" + APanel->StateMachineEditorUnit->DisplayName + "] ...";

					if (APanel->StateMachineEditor->FindAndSelect(EditSearch->Text, this->ToSearchTypes(), m_b_FromBeginning,
							bSelectAndStop)) {
						bFound = true;
					}
				}
			}break;
		case TEditorSearchDocumentType::
			esdtOpenedDocs : {

				for (int i = m_iDockPanel; i < FormScxmlGui->LMDDockSite1->PanelCount; i++) {
					TStateMachineDockPanel *ACurrentDockPanel = dynamic_cast<TStateMachineDockPanel*>
						(FormScxmlGui->LMDDockSite1->Panels[i]);
					if (ACurrentDockPanel) {

						this->Caption = L"Search [" + ACurrentDockPanel->StateMachineEditorUnit->DisplayName + "] ...";

						if (ACurrentDockPanel->StateMachineEditor->FindAndSelect(EditSearch->Text, this->ToSearchTypes(),
								m_b_FromBeginning, bSelectAndStop)) {
							if (ACurrentDockPanel->Zone && ACurrentDockPanel->Zone->Parent) {
								ACurrentDockPanel->Zone->Parent->ActivePage = ACurrentDockPanel->Zone;
							}

							bFound = true;
							m_iDockPanel = i;

							if (bSelectAndStop) {
								break;
							}

						}

						if (!IsWindowVisible(this->Handle)) {
							break;
						}
						Application->ProcessMessages();
					}
				}

			}break;
		case TEditorSearchDocumentType::
			esdtProject : {

				std::vector<TStateMachineEditorUnit*>AVecUnits;
				CollectAllEnabledUnits(FormScxmlGui->ProjectManager1->Root, AVecUnits);

				for (std::size_t i = m_iUnit; i < AVecUnits.size(); i++) {
					bool bShouldCloseEditor = false;
					if (!AVecUnits[i]->Opened) {
						AVecUnits[i]->Open(true);
						bShouldCloseEditor = true && CheckCloseTabIfNotFound->Checked;
					}

					if (AVecUnits[i]->StateMachineDockPanel) {

						this->Caption = L"Search [" + AVecUnits[i]->DisplayName + "] ...";

						if (AVecUnits[i]->StateMachineDockPanel->StateMachineEditor->FindAndSelect(EditSearch->Text, this->ToSearchTypes(),
								m_b_FromBeginning, bSelectAndStop)) {

							AVecUnits[i]->StateMachineDockPanel->ActivateZonePage();

							bFound = true;
							m_iUnit = i;

							if (bSelectAndStop) {
								break;
							}
						}
					}

					if (!IsWindowVisible(this->Handle)) {
						break;
					}

					if (bShouldCloseEditor && AVecUnits[i]->Opened) {
						AVecUnits[i]->Close();
					}

					Application->ProcessMessages();
				}

			}break;
		}

		if (!bFound) {
			this->LabelInfo->Caption = L"";

			if (bSelectAndStop && IsWindowVisible(this->Handle)) {
				const UnicodeString Caption = L"INFORMATION";
				UnicodeString Text = L"Search completed!";
				MessageBoxW(this->Handle, Text.c_str(), Caption.c_str(), MB_OK | MB_ICONINFORMATION);
			}

			m_b_FromBeginning = true;
			m_iDockPanel = 0;
			m_iUnit = 0;
		}
		else {
			m_b_FromBeginning = false;
		}

		if (bSelectAndStop && IsWindowVisible(this->Handle)) {
			this->EditSearch->SelectAll();
			this->EditSearch->SetFocus();
		}

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

	BtnNext->Enabled = true;

	if (!bSelectAndStop) {
		this->Close();
		FrameSearchResults->HTMLSearch->Selected->Clear();
		TTreeNodeShape * ASelected = GetSafeTreeListFirst(FrameSearchResults->HTMLSearch->Items);
		if (ASelected) {
			ASelected->Selected = true;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::FormClose(TObject *Sender, TCloseAction &Action) {
	if (SettingsData) {
		SettingsData->SearchTypes = this->ToSearchTypes();
		SettingsData->SearchDocumentType = this->GetEditorSearchType();

		try {
			SettingsData->TempRegistry->Values["FormSearch.CheckCloseTabIfNotFound.Checked"] = BoolToStr
				(this->CheckCloseTabIfNotFound->Checked);
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormSearch::RadioGroupDocTypeClick(TObject *Sender) {
	CheckCloseTabIfNotFound->Visible = RadioDocTypeWholeProject->Checked;

	if (RadioDocTypeOpened->Checked || RadioDocTypeWholeProject->Checked) {
		if (RadioModeSelectedStates->Checked) {
			RadioModeNormal->Checked = true;
		}
	}

	OnChangeValue(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TFormSearch::BtnDefaultClick(TObject *Sender) {
	for (std::map<TComponent*, Variant>::iterator it = FDefaultValues.begin(); it != FDefaultValues.end(); ++it) {
		if (TRadioButton * ARadioButton = dynamic_cast<TRadioButton*>(it->first)) {
			const bool bChecked = it->second;
			if (bChecked != ARadioButton->Checked) {
				ARadioButton->Checked = bChecked;
			}
		}
		else if (TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(it->first)) {
			const bool bChecked = it->second;
			if (bChecked != ACheckBox->Checked) {
				ACheckBox->Checked = bChecked;
			}
		}
	}
}

//---------------------------------------------------------------------------

template<typename T> //
void CheckDefaultFont(T *AControl, const TFontStyles AFontStyles) {
	if (AControl->Font->Style != AFontStyles) {
		AControl->Font->Style = AFontStyles;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormSearch::OnChangeValue(TObject *Sender) {
	for (std::map<TComponent*, Variant>::iterator it = FDefaultValues.begin(); it != FDefaultValues.end(); ++it) {
		TFontStyles AStyles;
		if (TRadioButton * ARadioButton = dynamic_cast<TRadioButton*>(it->first)) {
			const bool bChecked = it->second;
			if (ARadioButton->Checked && bChecked != ARadioButton->Checked) {
				AStyles = AStyles << fsBold;
			}
			CheckDefaultFont<TRadioButton>(ARadioButton, AStyles);
		}
		else if (TCheckBox * ACheckBox = dynamic_cast<TCheckBox*>(it->first)) {
			const bool bChecked = it->second;
			if (bChecked != ACheckBox->Checked) {
				AStyles = AStyles << fsBold;
			}
			CheckDefaultFont<TCheckBox>(ACheckBox, AStyles);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormSearch::BtnRegExpValidateClick(TObject *Sender) {
	std::auto_ptr<TDialogRegExp>ADialogRegExpPtr(new TDialogRegExp(this));
	ADialogRegExpPtr->EditRegExp->Text = this->EditSearch->Text;
	ADialogRegExpPtr->SetRegExecType(retRegexSearch);
	ADialogRegExpPtr->CheckBox1->Checked = CheckMatchCase->Checked;
	ADialogRegExpPtr->CheckBox1->Enabled = true;
	if (ADialogRegExpPtr->ShowModal() == mrOk) {
		CheckMatchCase->Checked = ADialogRegExpPtr->CheckBox1->Checked;
		this->EditSearch->Text = ADialogRegExpPtr->EditRegExp->Text.Trim();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormSearch::RadioModeSelectedStatesClick(TObject *Sender) {
	CheckShowResultsInSeparateWindow->Checked = true;
	RadioDocTypeCurrent->Checked = true;

	OnChangeValue(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TFormSearch::CheckShowResultsInSeparateWindowClick(TObject *Sender) {
	if (RadioModeSelectedStates->Checked && !CheckShowResultsInSeparateWindow->Checked) {
		RadioModeNormal->Checked = true;
	}

	OnChangeValue(Sender);
}
//---------------------------------------------------------------------------

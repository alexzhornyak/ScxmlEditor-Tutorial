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

#include "UnitFrameTrigger.h"

#include "Log4cpp_VCL.hpp"
#include "UnitSettings.h"
#include "UnitCustomUtils.h"
#include "UnitUniqueUtils.h"

/* https://github.com/badunius/myJSON */
/* MIT License */
#include "uJSON.hpp"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElBtnEdit"
#pragma link "ElEdits"
#pragma link "ElXPThemedControl"
#pragma resource "*.dfm"
TFrameTrigger *FrameTrigger;

// ---------------------------------------------------------------------------
__fastcall TFrameTrigger::TFrameTrigger(TComponent * Owner, TContentTrigger * AContentTrigger, TParamTriggerItems * AParams,
	const UnicodeString & sEventName) : TFrame(Owner), FContentTrigger(new TContentTrigger(this)), FEventName(sEventName),
Precision(1.0f), FParams(new TParamTriggerItems(this)), FOriginalContentTrigger(new TContentTrigger(this)),
FOriginalParams(new TParamTriggerItems(this))
//
{
	assert(SettingsData != NULL);

	EditBtn1->Align = alTop;

	// вот эти менться не будут
	FOriginalContentTrigger->Assign(AContentTrigger);
	FOriginalParams->Assign(AParams);

	/* эти дальше будут меняться */
	FContentTrigger->Assign(AContentTrigger);
	FParams->Assign(AParams);

	// убираем неоднозначность между 'Expr' и 'XMLText'
	if (FContentTrigger->XMLText->Text.IsEmpty()) {
		FContentTrigger->XMLText->Text = FContentTrigger->Expr;
		FContentTrigger->Expr = L"";
	}

	/* CONTENT */

	if (FContentTrigger->Enabled) {
		switch(FContentTrigger->Appearance) {
		case ctaButton: {
				Button1->Caption = FEventName;
			}break;
		case ctaCheckBox: {
				CheckBox1->Caption = FEventName + "[" + AContentTrigger->XMLText->Text + "]";
				CheckBox1->Checked = FContentTrigger->XMLText->Text == FContentTrigger->On;
			}break;
		case ctaCombo: {
				LabelCombo->Caption = FEventName;
				ComboBox1->Items->Assign(FContentTrigger->Values);

				if (FContentTrigger->Values->Count == 0) {
					LOG_WARNING("Values are not defined for type ComboBox!");
				}

				const int iIndex = ComboBox1->Items->IndexOf(FContentTrigger->XMLText->Text);
				if (iIndex != -1) {
					ComboBox1->Tag = 1;
					ComboBox1->ItemIndex = iIndex;
					ComboBox1->Tag = 0;
				}
			}break;
		case ctaEdit: {
				LabelEdit->Caption = FEventName;
				EditBtn1->Text = FContentTrigger->XMLText->Text;

				LabelEdit->Font->Color = clWindowText;
				EditBtn1->Font->Color = clWindowText;

			}break;
		case ctaMemo: {
				LabelMemo->Caption = FEventName;

				Memo1->Lines->Text = FContentTrigger->XMLText->Text;

				PanelMemo->Font->Color = clWindowText;
				Memo1->Font->Color = clWindowText;

			}break;
		case ctaTrackBar: {

				TrackBar1->Tag = 1;

				if (FContentTrigger->TriggerType == cttDouble) {
					const double dMax = StrToFloatDef(FContentTrigger->Max, 0.0f);
					const double dMin = StrToFloatDef(FContentTrigger->Min, 0.0f);

					const double dStep = (dMax - dMin) / 100.0f;

					if (dStep < 1) {
						UnicodeString sPrecision = FormatFloat("0.#########", dStep);
						sPrecision = StringReplace(sPrecision, "0.", "", TReplaceFlags());
						int iCount = 1;
						for (int i = 1; i <= sPrecision.Length(); i++) {
							if (sPrecision[i] == '0') {
								iCount++;
							}
						}
						Precision = 1.0 / (Power(10.0f, iCount));
					}

					TrackBar1->Min = dMin / Precision;
					TrackBar1->Max = dMax / Precision;
					TrackBar1->Frequency = dStep / Precision;
					TrackBar1->Position = StrToFloatDef(FContentTrigger->XMLText->Text, 0.0f) / Precision;
				}
				else {
					TrackBar1->Min = FContentTrigger->Min.ToIntDef(0);
					TrackBar1->Max = FContentTrigger->Max.ToIntDef(0);
					TrackBar1->Frequency = (TrackBar1->Max - TrackBar1->Min) / 100;
					TrackBar1->Position = FContentTrigger->XMLText->Text.ToIntDef(0);
				}

				TrackBar1->Tag = 0;

				LabelTrack->Caption = FEventName;
			}break;
		}
	}

	if (FContentTrigger->Appearance != ctaButton || !FContentTrigger->Enabled) {
		Button1->Visible = false;
	}
	if (FContentTrigger->Appearance != ctaCheckBox || !FContentTrigger->Enabled) {
		CheckBox1->Visible = false;
	}
	if (FContentTrigger->Appearance != ctaCombo || !FContentTrigger->Enabled) {
		LabelCombo->Visible = false;
		ComboBox1->Visible = false;
	}
	if (FContentTrigger->Appearance != ctaMemo || !FContentTrigger->Enabled) {
		PanelMemo->Visible = false;
		Memo1->Visible = false;
	}
	if (FContentTrigger->Appearance != ctaTrackBar || !FContentTrigger->Enabled) {
		LabelTrack->Visible = false;
		TrackBar1->Visible = false;
	}
	if (FContentTrigger->Appearance != ctaEdit || !FContentTrigger->Enabled) {
		LabelEdit->Visible = false;
		EditBtn1->Visible = false;
	}

	/* PARAMS */
	if (FParams->Enabled) {
		ScrollBoxParams->Tag = 1;
		LabelParams->Caption = "Params:[" + FEventName + "]";

		if (FParams->Count) {
			ScrollBoxParams->DisableAlign();
			try {

				std::auto_ptr<Graphics::TBitmap>ABitmapPtr(new Graphics::TBitmap());
				ABitmapPtr->Canvas->Font->Assign(ScrollBoxParams->Font);

				for (int i = 0; i < FParams->Count; i++) {

					TElButtonEdit *ABtn = new TElButtonEdit(ScrollBoxParams);
					ABtn->Parent = ScrollBoxParams;
					ABtn->Align = alLeft;
					ABtn->AlignWithMargins = true;
					ABtn->Constraints->MaxHeight = 22;

					if (FParams->Items[i]->DisplayWidth > 10) {
						ABtn->Width = FParams->Items[i]->DisplayWidth;
					}
					else {
						WLOG_ERROR(L"Param:[%s] width:[%d] can not be less 10!", FParams->Items[i]->ParamName.c_str(),
							FParams->Items[i]->DisplayWidth);
					}

					ABtn->ButtonCaption = FParams->Items[i]->ParamName;
					const int iWasWidth = 22;
					const int iBtnWidth = std::max(iWasWidth, ABitmapPtr->Canvas->TextWidth(ABtn->ButtonCaption) + 10);
					ABtn->Width = ABtn->Width + (iBtnWidth - iWasWidth);
					ABtn->ButtonWidth = iBtnWidth;

					ABtn->Text = FParams->Items[i]->Location.IsEmpty() ? FParams->Items[i]->Expr : FParams->Items[i]->Location;
					ABtn->OnButtonClick = ScrollBoxParamsClickBtn;
					ABtn->OnKeyPress = EditBtnKeyPress;
					ABtn->OnChange = EditBtn1Change;
					ABtn->Tag = FParams->Items[i]->TriggerType;

				}
			}
			__finally {
				ScrollBoxParams->EnableAlign();
			}
		}

		ScrollBoxParams->Tag = 0;
	}
	else {
		LabelParams->Visible = false;
		ScrollBoxParams->Visible = false;
	}
}

// ---------------------------------------------------------------------------
__fastcall TFrameTrigger::~TFrameTrigger() {
	if (FParams) {
		delete FParams;
	}
	if (FContentTrigger) {
		delete FContentTrigger;
	}
	if (FOriginalParams) {
		delete FOriginalParams;
	}
	if (FOriginalContentTrigger) {
		delete FOriginalContentTrigger;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::ValidateXMLTriggerValueThrow(const UnicodeString &sExpr, const TContentTriggerType ATriggerType) {
	switch(ATriggerType) {
	case cttBool: {
			bool bVal = false;
			if (!TryStrToBool(sExpr, bVal))
				throw Exception(UnicodeString().sprintf(L"Can not convert event:[%s] value:[%s] to Boolean!", FEventName.c_str(),
					sExpr.c_str()));
		}break;
	case cttInteger: {
			int iVal = 0;
			if (!TryStrToInt(sExpr, iVal))
				throw Exception(UnicodeString().sprintf(L"Can not convert event:[%s] value:[%s] to Integer!", FEventName.c_str(),
					sExpr.c_str()));
		}break;
	case cttDouble: {
			double dVal = false;
			if (!TryStrToFloat(sExpr, dVal))
				throw Exception(UnicodeString().sprintf(L"Can not convert event:[%s] value:[%s] to Double!", FEventName.c_str(),
					sExpr.c_str()));
		}break;
	}
}

// ---------------------------------------------------------------------------
Ujson::myJSONItem * __fastcall TFrameTrigger::AssignJSONTriggerValue(Ujson::myJSONItem *AObj, const UnicodeString &sKey,
	const UnicodeString &sExpr, const TContentTriggerType ATriggerType) {

	myJSONItem *ADataItem = NULL;

	if (AObj) {
		switch(ATriggerType) {
		case cttBool: {
				bool bVal = false;
				if (TryStrToBool(sExpr, bVal)) {
					ADataItem = AObj->Item[sKey];
					ADataItem->setBool(bVal);
				}
				else
					throw Exception(UnicodeString().sprintf(L"Can not convert event:[%s] value:[%s] to Boolean!", FEventName.c_str(),
						sExpr.c_str()));
			}break;
		case cttInteger: {
				int iVal = 0;
				if (TryStrToInt(sExpr, iVal)) {
					ADataItem = AObj->Item[sKey];
					ADataItem->setInt(iVal);
				}
				else
					throw Exception(UnicodeString().sprintf(L"Can not convert event:[%s] value:[%s] to Integer!", FEventName.c_str(),
						sExpr.c_str()));
			}break;
		case cttDouble: {
				double dVal = 0.0f;
				if (TryStrToFloat(sExpr, dVal)) {
					ADataItem = AObj->Item[sKey];
					ADataItem->setNum(dVal);
				}
				else
					throw Exception(UnicodeString().sprintf(L"Can not convert event:[%s] value:[%s] to Double!", FEventName.c_str(),
						sExpr.c_str()));
			}break;
		case cttString: {
				ADataItem = AObj->Item[sKey];
				ADataItem->setStr(sExpr);
			}break;
		default: {
				if (!sExpr.IsEmpty()) {
					ADataItem = AObj->Item[sKey];
					ADataItem->setStr(sExpr);
				}
			}break;
		}
	}

	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::CreateSendText(void) {
	try {
		if (SettingsData->IsTesterWorking) {

			switch(SettingsData->ActiveTestSettings->TriggersMsgFormat) {
			case tmfJSON: {
					std::unique_ptr<myJSONItem>AJSONObjPtr(new myJSONItem);
					AJSONObjPtr->setType(dtObject);
					myJSONItem *AItem = AJSONObjPtr->Item[L"name"];
					AItem->setStr(FEventName);

					myJSONItem *ADataItem = NULL;

					if (FContentTrigger->Enabled) {

						const UnicodeString sExpr = !FContentTrigger->Expr.IsEmpty()
							? FContentTrigger->Expr : FContentTrigger->XMLText->Text;
						ADataItem = this->AssignJSONTriggerValue(AJSONObjPtr.get(), L"data", sExpr, FContentTrigger->TriggerType);
					}
					if (FParams->Enabled) {
						if (ADataItem)
							throw Exception("Trigger:[" + FEventName + "] 'Params.Enabled' must not occur with 'Content.Enabled'!");

						ADataItem = AJSONObjPtr->Item["data"];
						ADataItem->setType(dtObject);

						for (int i = 0; i < ScrollBoxParams->ComponentCount; i++) {
							TElButtonEdit *ABtn = dynamic_cast<TElButtonEdit*>(ScrollBoxParams->Components[i]);
							if (ABtn) {
								if (ABtn->ButtonCaption.IsEmpty())
									throw Exception(UnicodeString().sprintf(L"Event:[%s] param:[%d] name MUST NOT be empty!", //
										FEventName.c_str(), i));

								this->AssignJSONTriggerValue(ADataItem, ABtn->ButtonCaption, ABtn->Text, TContentTriggerType(ABtn->Tag));
							}
						}
					}

					SettingsData->SendEvent(AJSONObjPtr->Code);

				}break;
			default: {
					_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML("<EVENT/>");
					if (ADoc && ADoc->DocumentElement) {
						ADoc->DocumentElement->SetAttr("name", FEventName);

						ILMDXmlElement *AContent = NULL;
						if (FContentTrigger->Enabled) {
							if (!FContentTrigger->XMLText->Text.IsEmpty()) {

								ValidateXMLTriggerValueThrow(FContentTrigger->XMLText->Text, FContentTrigger->TriggerType);

								AContent = ADoc->DocumentElement->AppendElement("content");
								FContentTrigger->XMLText->AppendToElement(AContent);
							}
							else if (!FContentTrigger->Expr.IsEmpty()) {

								ValidateXMLTriggerValueThrow(FContentTrigger->Expr, FContentTrigger->TriggerType);

								AContent = ADoc->DocumentElement->AppendElement("content");
								AContent->AppendText(FContentTrigger->Expr);
							}
							if (AContent) {
								AContent->SetAttr("type", FContentTrigger->TriggerType);
							}
						}
						if (FParams->Enabled) {
							if (AContent)
								throw Exception("Trigger:[" + FEventName + "] 'Params.Enabled' must not occur with 'Content.Enabled'!");

							for (int i = 0; i < ScrollBoxParams->ComponentCount; i++) {
								TElButtonEdit *ABtn = dynamic_cast<TElButtonEdit*>(ScrollBoxParams->Components[i]);
								if (ABtn) {
									if (ABtn->ButtonCaption.IsEmpty())
										throw Exception(UnicodeString().sprintf(L"Event:[%s] param:[%d] name MUST NOT be empty!", //
											FEventName.c_str(), i));

									const TContentTriggerType ATriggerType = TContentTriggerType(ABtn->Tag);
									ValidateXMLTriggerValueThrow(ABtn->Text, ATriggerType);

									ILMDXmlElement *AParam = ADoc->DocumentElement->AppendElement("param");
									AParam->SetAttr("name", ABtn->ButtonCaption);
									AParam->SetAttr("expr", ABtn->Text);
									AParam->SetAttr("type", ATriggerType);

									ABtn->Font->Color = clWindowText;
								}
							}
						}

						SettingsData->SendEvent(ADoc->XML);
					}
				}break;
			}

			LabelEdit->Font->Color = clWindowText;
			EditBtn1->Font->Color = clWindowText;

			PanelMemo->Font->Color = clWindowText;
			Memo1->Font->Color = clWindowText;
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"TRIGGER> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::CheckBox1Click(TObject *Sender) {
	FContentTrigger->XMLText->Text = CheckBox1->Checked ? FContentTrigger->On : FContentTrigger->Off;
	CreateSendText();
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::EditBtnKeyPress(TObject *Sender, wchar_t &Key) {
	if (Key == VK_RETURN) {
		Key = 0;
		TElButtonEdit * AEditBtn = dynamic_cast<TElButtonEdit*>(Sender);
		if (AEditBtn) {
			AEditBtn->OnButtonClick(Sender);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameTrigger::ComboBox1Change(TObject *Sender) {
	if (ComboBox1->ItemIndex != -1 && ComboBox1->Tag == 0) {
		FContentTrigger->XMLText->Text = ComboBox1->Text;
		CreateSendText();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::Button1Click(TObject *Sender) {
	// простая единоразовая посылка
	if (!IsOnOffDeclared()) {
		CreateSendText();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::TrackBar1Change(TObject *Sender) {
	if (TrackBar1->Tag == 0) {
		if (FContentTrigger->TriggerType == cttDouble) {
			const double dVal = static_cast<double>(TrackBar1->Position) * Precision;
			FContentTrigger->XMLText->Text = FormatFloat("0.000", dVal);
		}
		else {
			FContentTrigger->XMLText->Text = TrackBar1->Position;
		}
		LabelTrack->Caption = FEventName + ":" + FContentTrigger->XMLText->Text;
		CreateSendText();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::Button1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	if (IsOnOffDeclared()) {
		FContentTrigger->XMLText->Text = FContentTrigger->On;
		CreateSendText();
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameTrigger::Button1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y) {
	if (IsOnOffDeclared()) {
		FContentTrigger->XMLText->Text = FContentTrigger->Off;
		CreateSendText();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrameTrigger::ScrollBoxParamsClickBtn(TObject *Sender) {
	CreateSendText();
}

//---------------------------------------------------------------------------

void __fastcall TFrameTrigger::EditBtn1Change(TObject *Sender) {
	TElButtonEdit * ABtnEdit = dynamic_cast<TElButtonEdit*>(Sender);
	if (ABtnEdit) {
		ABtnEdit->Font->Color = clBlue;

		if (ABtnEdit == EditBtn1) {
			LabelEdit->Font->Color = clBlue;
		}

	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameTrigger::EditBtn1ButtonClick(TObject *Sender) {
	TElButtonEdit * ABtn = dynamic_cast<TElButtonEdit*>(Sender);
	if (ABtn) {
		FContentTrigger->XMLText->Text = EditBtn1->Text;
		CreateSendText();
	}
}

//---------------------------------------------------------------------------
void __fastcall TFrameTrigger::BtnMemoClick(TObject *Sender) {
	FContentTrigger->XMLText->Text = Memo1->Lines->Text;
	CreateSendText();
}
//---------------------------------------------------------------------------

void __fastcall TFrameTrigger::Memo1Change(TObject *Sender) {
	Memo1->Font->Color = clBlue;
	PanelMemo->Font->Color = clBlue;
}

//---------------------------------------------------------------------------
void __fastcall TFrameTrigger::Memo1KeyPress(TObject *Sender, wchar_t &Key) {
	if (((GetKeyState(VK_CONTROL) & 0x8000) != 0) && Key == 10) {
		Key = 0;
		CreateSendText();
	}
}
//---------------------------------------------------------------------------

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

#include "UnitStateMachineConnection.h"

#include <queue>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <LMDXML.hpp>
#include <StrUtils.hpp>

#include "Log4cpp_VCL.hpp"

#include "UnitTransitionEditor.h"
#include "UnitScxmlBaseShape.h"
#include "UnitSyntaxUtils.h"
#include "UnitStringEditorEx.h"
#include "UnitSettings.h"
#include "UnitSyntaxEditorForm.h"
#include "TreeNodeListAccess.h"
#include "UnitTreeEditorUtils.h"
#include "UnitStateMachineUnit.h"
#include "UnitTreeEx.h"
#include "UnitStateMachineParser.h"
#include "UnitDummyShape.h"
#include "UnitSVGExporter.h"
#include "ScxmlShapes.hpp"

// --------------------------------------------------------------------------- 

#pragma package(smart_init)

void RegisterStateMachineConnection() {
#pragma startup RegisterStateMachineConnection

	Classes::RegisterClassA(__classid(TStateMachineConnection));
	Classes::RegisterClassA(__classid(TContentTrigger));
	Classes::RegisterClassA(__classid(TParamTriggerItem));
	Classes::RegisterClassA(__classid(TParamTriggerItems));
}

// --------------------------------------------------------------------------- 
UnicodeString ClipText(const UnicodeString &sText, TClipTextType AType, int iClipMax) {
	if (sText.Length() > iClipMax) {
		switch(AType) {
		case TClipTextType::
			cttLeft : {
				return AnsiLeftStr(sText, iClipMax) + L" ...";
			}break;
		case TClipTextType::
			cttRight : {
				return L"... " + AnsiRightStr(sText, iClipMax);
			}break;
		case TClipTextType::
			cttMiddle : {
				return AnsiLeftStr(sText, iClipMax / 2) + L" ... " + AnsiRightStr(sText, iClipMax / 2);
			}break;
		}
	}
	return sText;
}

// --------------------------------------------------------------------------- 
bool IsStateMachineConnectionClipboardFormat(const UnicodeString &sText) {
	return sText.Pos(L"object TStateMachineConnection") == 1;
}

// --------------------------------------------------------------------------- 
void PasteStateMachineConnectionFromText(TCustomTree *ADestTree, const UnicodeString &sText) {
	try {
		if (!ADestTree)
			throw Exception(L"ADestTree==NULL!!!");

		std::auto_ptr<TStringList>AStringListPtr(new TStringList());
		AStringListPtr->Text = sText;

		// так как у нас будут новые родители 
		UnicodeString sFromShape = L"";
		UnicodeString sToShape = L"";

		for (int i = AStringListPtr->Count - 1; i >= 0; i--) {
			const UnicodeString sLine = AStringListPtr->Strings[i].Trim();
			if (sLine.Pos(L"FromShape =") == 1) {
				sFromShape = StringReplace(sLine, L"FromShape =", "", TReplaceFlags());
				AStringListPtr->Delete(i);
			}
			else if (sLine.Pos(L"ToShape =") == 1) {
				sToShape = StringReplace(sLine, L"ToShape =", "", TReplaceFlags());
				AStringListPtr->Delete(i);
			}
		}

		const bool bIsSelfConnection = !sFromShape.IsEmpty() && !sToShape.IsEmpty() && sFromShape == sToShape;

		std::auto_ptr<TStringStream>AInStreamPtr(new TStringStream(AStringListPtr->Text));
		AInStreamPtr->Position = 0;
		std::auto_ptr<TMemoryStream>AOutStreamPtr(new TMemoryStream());
		ObjectTextToBinary(AInStreamPtr.get(), AOutStreamPtr.get());
		AOutStreamPtr->Position = 0;

		std::auto_ptr<TComponent>AClipboardComponentPtr(AOutStreamPtr->ReadComponent(NULL));
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(AClipboardComponentPtr.get());
		if (AConnection) {
			// если выбрано соединение, тогда назначаем его тем, которое в буфере 
			if (ADestTree->Connections->Selected) {

				// стиль соединения должен остаться прежним 
				TStateMachineConnection *ASelectedConnection = dynamic_cast<TStateMachineConnection*>(ADestTree->Connections->Selected);
				if (ASelectedConnection) {
					ASelectedConnection->AssignStateMachineConnection(AConnection);

					if (ASelectedConnection->IsSelfConnection) {
						TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
							(ASelectedConnection->FromShape);
						if (AVisualScxmlBaseShape) {
							AVisualScxmlBaseShape->ArrangeSelfConnections();
						}
					}
				}
			}
			else {

				if (bIsSelfConnection) {
					if (ADestTree->Selected->Count()) {
						TStateMachineConnection * ANewConnection = new TStateMachineConnection(ADestTree->Owner);

						ANewConnection->Tree = ADestTree;

						ANewConnection->FromShape = ADestTree->Selected->Items[0];
						ANewConnection->ToShape = ADestTree->Selected->Items[0];

						ANewConnection->Assign(AConnection);

						// bug fix: какого-то хрена не назначаются точки при множественном назначении 
						ANewConnection->Style = AConnection->Style;
						ANewConnection->Points->Clear();

						for (int i = 0; i < AConnection->Points->Count(); i++) {
							ANewConnection->Points->Add(AConnection->Points->Item[i].XStyle, AConnection->Points->Item[i].XValue,
								AConnection->Points->Item[i].YStyle, AConnection->Points->Item[i].YValue);
						}

						// необходимо удалить временное соединение, чтобы его уже не было на момент формирования нового имени 
						AClipboardComponentPtr.reset(0);

						const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(ADestTree->Owner, __classid(TStateMachineConnection));
						ANewConnection->Name = AUniqueName.second;

						TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(ADestTree->Selected->Items[0]);
						if (AVisualScxmlBaseShape) {
							AVisualScxmlBaseShape->ArrangeSelfConnections();
						}

						ADestTree->Selected->Clear();

						Editorutils::ProtectConnectionToBeInSamePoints(ANewConnection);

						ADestTree->SelectConnection(ANewConnection);
					}
				}
				else {
					if (ADestTree->Selected->Count() >= 2) {
						TStateMachineConnection * ANewConnection = new TStateMachineConnection(ADestTree->Owner);
						ANewConnection->Tree = ADestTree;

						ANewConnection->FromShape = ADestTree->Selected->Items[0];
						ANewConnection->ToShape = ADestTree->Selected->Items[1];

						ANewConnection->Assign(AConnection);
						// bug fix: какого-то хрена не назначаются точки при множественном назначении 
						ANewConnection->Style = AConnection->Style;
						ANewConnection->Points->Clear();

						for (int i = 0; i < AConnection->Points->Count(); i++) {
							ANewConnection->Points->Add(AConnection->Points->Item[i].XStyle, AConnection->Points->Item[i].XValue,
								AConnection->Points->Item[i].YStyle, AConnection->Points->Item[i].YValue);
						}

						// необходимо удалить временное соединение, чтобы его уже не было на момент формирования нового имени 
						AClipboardComponentPtr.reset(0);

						const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(ADestTree->Owner, __classid(TStateMachineConnection));
						ANewConnection->Name = AUniqueName.second;

						ADestTree->Selected->Clear();

						if (ANewConnection->AdjustableSides) {

							ANewConnection->Draw(); // даём команду отрисовать, чтобы обновились все значения в точках 

							Editorutils::AdjustSidesConnection(ANewConnection);
						}

						Editorutils::ProtectConnectionToBeInSamePoints(ANewConnection);

						ADestTree->SelectConnection(ANewConnection);
					}
				}
			}
		}
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// --------------------------------------------------------------------------- 
// ----------------------- EStateMachineConnectionException ------------------ 
// --------------------------------------------------------------------------- 
__fastcall EStateMachineConnectionException::EStateMachineConnectionException(TStateMachineConnection *AStateMachineConnection,
	const UnicodeString &sCategory, const UnicodeString &sMsg) : Exception(sMsg), FStateMachineConnection(AStateMachineConnection),
FCategory(sCategory) {
}

// --------------------------------------------------------------------------- 
// ------------------------- TStateMachineConnection ------------------------- 
// --------------------------------------------------------------------------- 
__fastcall TStateMachineConnection::TStateMachineConnection(Classes::TComponent* AOwner) : TTreeConnection(AOwner),
FTransitionEditor(NULL), FXML(L""), FCondition(L""), FConditionBack(L""), FEvent(L""), FAdjustableSides(true),
FContentTrigger(new TContentTrigger(this)), FParams(new TParamTriggerItems(this)), FGuid(new GUID), FSwitch(tstNONE) {
	FBreakpointSet = false;
	FLocked = false;
	FInheritanceMismatch = false;
	FInheritanceResolver = new TInheritanceResolver(&this->GetStateMachineEditorUnit);

	memset(FGuid, 0, sizeof(GUID));

	memset(&FDrawTextData, 0, sizeof(FDrawTextData));

	FGuidStr = L"";
	if (CoCreateGuid(FGuid) != S_OK) {
		LOG_ERROR_SS << "Can not create GUID! Some operations may not work!";
	}
	else {
		FGuidStr = Sysutils::GUIDToString(*FGuid);
	}

	FProtocolControlBinding = new TProtocolControlBinding(this);
	FProtocolControlBinding->BindTypes = TProtocolBindTypes() << pbtConsumer;

	FScxmlTransitionDisplayTypes = TScxmlTransitionDisplayTypes() << stdtMAXSIZE;

	this->Text->Angle = 90;
	this->ArrowTo->Size = 15;

	this->ArrowFrom->Size = 10;
	this->ArrowFrom->Style = casCircle;
	this->ArrowFrom->BackColor = clWhite;

	FIsTargetStored = false;

	FClipOutCondition = false;
	FClipOutXML = true;
	FClipTypeCondition = cttMiddle;
	FClipTypeXML = cttLeft;

	FClipOutMax = MAX_SIMPLETEXT_LENGTH;

	FExtraParams = L"";

	FType = external;

	FDescription = L"";

	FVerticalCentering = false;

	FSkipAutolayout = false;
}

// --------------------------------------------------------------------------- 
__fastcall TStateMachineConnection::~TStateMachineConnection(void) {
	if (FTransitionEditor) {
		delete FTransitionEditor;
		FTransitionEditor = NULL;
	}
	if (FContentTrigger) {
		delete FContentTrigger;
		FContentTrigger = NULL;
	}
	if (FParams) {
		delete FParams;
		FParams = NULL;
	}
	if (FProtocolControlBinding) {
		delete FProtocolControlBinding;
		FProtocolControlBinding = NULL;
	}
	if (FInheritanceResolver) {
		delete FInheritanceResolver;
		FInheritanceResolver = NULL;
	}
	if (FGuid) {
		delete FGuid;
		FGuid = NULL;
	}
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::OnFilterPropEvent(const UnicodeString &sPropName) {
	std::auto_ptr<TStringList>ABlackListPtr(new TStringList());
	ABlackListPtr->CaseSensitive = true;
	if (!SettingsData->AviaExtensionEnabled) {
		ABlackListPtr->Add(L"ProtocolControlBinding");
	}
	if (!StateMachineEditorUnit || !StateMachineEditorUnit->IsInherited) {
		ABlackListPtr->Add(L"InheritanceMismatch");
		ABlackListPtr->Add(L"InheritanceResolver");
	}

	if (this->SWITCH != tstINVERT_CONDITION) {
		ABlackListPtr->Add(L"ConditionBack");
	}

	if (ABlackListPtr->IndexOf(sPropName) != -1)
		return false;

	return IsPublishedProp(__classid(TTreeConnection), sPropName) == false;
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem,
	TColor &AColor) {

	// специальный случай 
	if (AItem->PropName == L"InheritanceResolver") {
		AColor = clTeal;
		return;
	}

	if (AItem->TypeInfo->Kind != tkClass && !AItem->Parent && AItem->PropInfo[0]->IsWritable()) {
		std::auto_ptr<TStringList>ANavyColorPtr(new TStringList());
		ANavyColorPtr->CaseSensitive = true;
		ANavyColorPtr->Add("Event");
		ANavyColorPtr->Add("Condition");
		ANavyColorPtr->Add("XML");

		std::auto_ptr<TStringList>ATealColorPtr(new TStringList());
		ATealColorPtr->CaseSensitive = true;
		ATealColorPtr->Add("ExtraParams");
		ATealColorPtr->Add("Target");
		ATealColorPtr->Add("IsTargetStored");
		ATealColorPtr->Add("Type");
		ATealColorPtr->Add("Locked");
		ATealColorPtr->Add("InheritanceMismatch");
		ATealColorPtr->Add("ConditionBack");

		std::auto_ptr<TStringList>ABlueColorPtr(new TStringList());
		ABlueColorPtr->CaseSensitive = true;
		ABlueColorPtr->Add("SWITCH");

		if (ANavyColorPtr->IndexOf(AItem->PropName) != -1) {
			AColor = clNavy;
		}
		else if (ATealColorPtr->IndexOf(AItem->PropName) != -1) {
			AColor = clTeal;
		}
		else if (ABlueColorPtr->IndexOf(AItem->PropName) != -1) {
			AColor = clBlue;
		}
		else if (SameText(AItem->PropName, L"Description")) {
			AColor = clGreen;
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {
	if (APropInfo) {
		const UnicodeString sPropName = APropInfo->Name();
		if (sPropName == L"XML") {
			AEditorClass = __classid(TXMLStringPropEditor);
		}
		else if (sPropName == L"Params") {
			AEditorClass = __classid(TParamTriggersPropEditor);
		}
		else if (sPropName == L"ContentTrigger") {
			AEditorClass = __classid(TContentPropEditor);
		}
		// указываем флаг AviaExtensionEnabled иначе редактор отменит фильтр 
		else if (SettingsData->AviaExtensionEnabled && sPropName == L"ProtocolControlBinding") {
			AEditorClass = __classid(TProtocolControlBindingPropertyEditor);
		}
		else if (sPropName == L"Condition") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}
		else if (sPropName == L"ConditionBack") {
			AEditorClass = __classid(TUnicodeStringSyntaxDialogEditor);
		}
		else if (sPropName == L"ExtraParams") {
			AEditorClass = __classid(TStringTextPairsEditor);
		}
		else if (sPropName == L"Event") {
			AEditorClass = __classid(TTransitionEventPropEditor);
		}
		else if (sPropName == L"InheritanceResolver") {
			AEditorClass = __classid(TInheritanceResolverPropEditor);
		}
	}
}

// --------------------------------------------------------------------------- 
UnicodeString __fastcall TStateMachineConnection::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	if (sPropName == L"Event") {
		return //
		"A space-separated list of designators of events that trigger this transition.\n" //
		;
	}

	if (sPropName == L"Condition") {
		return //
		"Any boolean expression.\n" //
		"The guard condition for this transition.\n" //
		;
	}

	if (sPropName == L"ConditionBack") {
		return //
		"<b>Back condition in INVERTED SWITCH</b>\n" //
		"Any boolean expression.\n" //
		"The guard condition for this transition.\n" //
		;
	}

	if (sPropName == L"Target") {
		return //
		"The identifier(s) of the state or parallel region to transition to.\n" //
		;
	}

	if (sPropName == L"Type") {
		return //
		"Determines whether the source state is exited in transitions whose target state is a <b>descendant of the source state</b>.\n" //
		;
	}

	if (sPropName == L"ContentTrigger") {
		return //
		"The trigger is used for sending events to the SCXML Processor\n"//
		"P.S. Do not forget to enable it for use!";
	}

	if (sPropName == L"Params") {
		return //
		"Params of the event\n"//
		"For example: you are planning to send <b>'_event.data.nParam==1'</b>\n"//
		"In this case you should create a param with the name <b>'nParam'</b> and set the expr <b>'1'</b>\n"//
		"P.S. <b>ContentTrigger</b> - should be <b>enabled</b> to activate trigger itself\n" //
		;
	}

	if (sPropName == L"AdjustableSides") {
		return //
		"if connection style is <b>Square</b> or <b>Lines</b>\n"//
		"then connection points will be adjusted to optimal position between <b>To</b> and <b>From</b> shapes\n"//
		;
	}

	if (sPropName == L"DisplayTypes") {
		return //
		"Select what do you want to see on the transition\n" //
		"<B>stdtEvent<B> - event name (this option must be present always)\n" //
		"<B>stdtCondition<B> - condition\n" //
		"<B>stdtXML<B> - xml block with the executable content\n" //
		"<B>stdtBindingName<B> - name of the bind (used only in custom extensions)\n" //
		"<B>stdtDescription<B> - description\n" //
		;
	}

	if (sPropName == L"TransitionIndex") {
		return //
		"<b>Lowest index has highest execution priority!</b>\n" //
		"Enumeration starts from <B>0(zero)</B>,\n"//
		"but <B>0 index</B> is reserved for child elements!\n" //
		"So, if the shape that owns the connecton <B>has children</B>, then use counting from <B>1 index</B>, otherwise start from <B>0 index</B>"
		//
		;
	}

	if (sPropName == L"IsTargetStored") {
		return //
		"If transition is <B>self connected</B> and parameter <B>'IsTargetStored'</B> is set to <B>true</B> " //
		"then during processing the event with the corresponding name and condition " //
		"will be provided <B>exit from shape and enter to shape (re-enter)</B>\n" //
		;
	}
	if (sPropName == L"XML") {
		return //
		"executable content that is run after all the 'onexit' handlers and before the all 'onentry' handlers " //
		"that are triggered by this transition.\n" //
		;
	}
	if (sPropName == L"ExtraParams") {
		return //
		L"<B>ExtraParams</B> - gives an opportunity to user to add custom attributes\n" //
		"<B>For example:</B>\n" //
		"<I>&lt;transition cond='IsTrue' <B>myAttr='myValue'</B>/&gt;</I>";
	}
	if (sPropName == L"ClipOutMax") {
		return //
		"If <B>ClipOutCondition</B> or <B>ClipOutXML</B> are set to <B>true</B> then the text will be cut until <B>ClipOutMax</B> position\n"
		//
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long condition'</B>\n" //
		"    ClipOutCondition=true\n" //
		"    ClipOutMax=5\n" //
		"    ClipTypeCondition=cttLeft\n" //
		"Result: Text=<B>'Very'</B>\n";
	}
	if (sPropName == L"ClipOutCondition") {
		return //
		"If it is set to <B>true</B> then the condition text will be cut until <B>ClipOutMax</B> position\n" //
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long condition'</B>\n" //
		"    ClipOutCondition=true\n" //
		"    ClipOutMax=5\n" //
		"    ClipTypeCondition=cttLeft\n" //
		"Result: Text=<B>'Very'</B>\n";
	}
	if (sPropName == L"ClipOutXML") {
		return //
		"If it is set to <B>true</B> then the XML text will be cut until <B>ClipOutMax</B> position\n" //
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long transition text'</B>\n" //
		"    ClipOutXML=true\n" //
		"    ClipOutMax=5\n" //
		"    ClipTypeXML=cttLeft\n" //
		"Result: Text=<B>'Very'</B>\n";
	}
	if (sPropName == L"ClipTypeCondition") {
		return //
		"<B>cttLeft</B> - condition text will be counted for clipping from left\n" //
		"<B>cttRight</B> - condition text will be counted for clipping from right\n" //
		"<B>cttMiddle</B> - condition text will be counted for clipping from center\n" //
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long condition'</B>\n" //
		"    ClipOutCondition=true\n" //
		"    ClipOutMax=5\n" //
		"    ClipTypeCondition=cttLeft\n" //
		"Result: Text=<B>'Very'</B>\n";
	}
	if (sPropName == L"ClipTypeXML") {
		return //
		"<B>cttLeft</B> - XML text will be counted for clipping from left\n" //
		"<B>cttRight</B> - XML text will be counted for clipping from right\n" //
		"<B>cttMiddle</B> - XML text will be counted for clipping from center\n" //
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long condition'</B>\n" //
		"    ClipOutXML=true\n" //
		"    ClipOutMax=5\n" //
		"    ClipTypeXML=cttLeft\n" //
		"Result: Text=<B>'Very'</B>\n";
	}
	if (sPropName == L"Locked") {
		return L"1) if the element is locked you <b>are not able to delete it</b>\n" //
		"2) if <b>the unit is inherited</b> then attribute <b>Locked=true</b> means the element be inherited\n" //
		;
	}
	if (sPropName == L"InheritanceMismatch") {
		return L"if the value is <b>true</b> then <b>the element differs</b> from those in origin unit\n" //
		"otherwise it seems to be <b>the same</b>\n" //
		;
	}
	if (sPropName == L"InheritanceResolver") {
		return L"String list of properties which will be skipped during inheritance comparison\n"//
		;
	}
	if (sPropName == L"Description") {
		return //
		"The description will be displayed at the bottom of connection\n" //
		;
	}
	if (sPropName == L"SWITCH") {
		return //
		"<B>tstNONE</B> - default behaviour of transition\n" //
		"<B>tstINVERT_CONDITION</B> - will be created reverse transition with <b>inverted condition</b>\n" //
		"For example: 'Condition'='_event.data==1'\n" //
		"'Datamodel'='lua' Macro='not (%COND%)'\n" //
		"Output: 'Condition'='not (_event.data==1)\n" //
		"<B>tstSIMPLE</B> - will be created reverse transition with <b>the same attributes</b>\n" //
		;
	}
	if (sPropName == L"VerticalCentering") {
		return //
		"Multiline text is <b>centered by top line by default</b>\n" //
		"You may override it by setting <b>'VerticalCentering'</b> option to <b>'true'</b>\n";
	}
	if (sPropName == L"SkipAutolayout") {
		return //
		"If it is set to <B>true</B>, then <b>AutoArrangeLayout</b> option will not be applied to this connection\n";
	}
	return L"";
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetSwitch(TTransitionSwitchType val) {
	FSwitch = val;

	UpdateAppearance();

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetVerticalCentering(bool val) {
	if (FVerticalCentering != val) {
		FVerticalCentering = val;

		if (!this->ComponentState.Contains(csLoading)) {
			this->Draw();
			this->UpdateDummyText();
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::UpdateDummyText(void) {
	if (Tree) {
		for (int i = 0; i < Tree->Shapes->Count; i++) {
			TTextDummyShape * ATextDummyShape = dynamic_cast<TTextDummyShape*>(Tree->Shapes->Items[i]);
			if (ATextDummyShape && ATextDummyShape->Connection == this) {
				this->AssignTextPolygonToPoints(ATextDummyShape->Points);
			}
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::UpdateAppearance() {
	if (!this->ComponentState.Contains(csLoading)) {

		if (IsSelfConnection || (Type == external && IsTargetDescendant)) {
			this->Border->Style = IsTargetStored ? psDash : psDot;
			this->Border->Color = IsTargetStored ? clRed : clBlue;
			this->Border->Width = IsTargetStored ? 2 : 1;
			this->Border->SmallDots = false;
		}
		else {
			this->Border->Style = IsTargetDescendant ? psDot : psSolid;
			this->Border->Color = clBlack;
			this->Border->Width = 1;
			this->Border->SmallDots = IsTargetDescendant == false;
		}
		this->Format->Visible = ContentTrigger->Enabled == true;
		this->Format->Color = clInfoBk;
		this->Format->Gradient->Visible = false;
		this->Format->Shadow->Visible = false;
		this->Format->Pen->Color = TColor(0x004080FF);

		/* INHERITANCE CHECK */
		if (this->Locked && !this->InheritanceMismatch && StateMachineEditorUnit && StateMachineEditorUnit->IsInherited &&
			StateMachineEditor) {

			StateMachineEditor->EnqueeScxmlConnectionForInheritanceCheck(this);
		}

		TColor AFromBackColor = Locked ? (InheritanceMismatch ? clRed : (InheritanceResolver->ResolvedProps->Count ? TColor(RGB(255, 150,
						50)) : clYellow)) : (InheritanceMismatch ? clFuchsia : clWhite);
		TColor AToBackColor = Locked ? (InheritanceMismatch ? clRed : (InheritanceResolver->ResolvedProps->Count ? TColor(RGB(255, 150,
						50)) : clYellow)) : (InheritanceMismatch ? clFuchsia : clBlack);

		switch(SWITCH) {
		case tstINVERT_CONDITION: {
				this->ArrowFrom->Style = casSolid;
				this->ArrowFrom->Size = 15;
				this->Border->SmallDots = false;
				this->Border->Style = psDash;
				this->Border->Color = TColor(RGB(200, 0, 0));

				// так как стрелки выглядят одинаково по размеру, то необходимо указать направление откуда 
				if (Locked || InheritanceMismatch) {
					USHORT ushHue, ushLuminance, ushSaturation;
					ColorToHLS(AFromBackColor, ushHue, ushLuminance, ushSaturation);
					AFromBackColor = HLSToColor(ushHue, 220, ushSaturation);
				}
			}break;
		case tstSIMPLE: {
				this->ArrowFrom->Style = casSolid;
				this->ArrowFrom->Size = 15;
				this->Border->SmallDots = false;
				this->Border->Style = psDash;
				this->Border->Color = clGray;

				// так как стрелки выглядят одинаково по размеру, то необходимо указать направление откуда 
				if (Locked || InheritanceMismatch) {
					USHORT ushHue, ushLuminance, ushSaturation;
					ColorToHLS(AFromBackColor, ushHue, ushLuminance, ushSaturation);
					AFromBackColor = HLSToColor(ushHue, 220, ushSaturation);
				}
			}break;
		case tstNONE:
		default: {
				this->ArrowFrom->Style = casCircle;
				this->ArrowFrom->Size = 10;
			}break;
		}

		this->ArrowFrom->BackColor = AFromBackColor;
		this->ArrowTo->BackColor = AToBackColor;

		if (StateMachineEditor) {
			if (this == StateMachineEditor->FActiveDebugConnection) {
				this->Border->Color = clRed;
				this->Border->Style = psSolid;
				this->Border->Width = 3;
				this->Border->SmallDots = false;
			}
			else {
				if (StateMachineEditor->WasEnteredConnection(this)) {
					this->Border->Color = SettingsData->ChartTestCoverageColor;
					this->Border->Style = psSolid;
					this->Border->Width = 3;
					this->Border->SmallDots = false;

				}
			}
		}
	}
}

// --------------------------------------------------------------------------- 
UnicodeString __fastcall TStateMachineConnection::GetTarget(void) {
	if (this->ToShape) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(this->ToShape);
		if (AShape) {
			return AShape->SimpleText;
		}
	}
	return L"";
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetEvent(UnicodeString val) {
	FEvent = val;

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetCondition(UnicodeString val) {
	FCondition = val.Trim();

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetBreakpointSet(bool val) {
	if (FBreakpointSet != val) {
		FBreakpointSet = val;
		if (!this->ComponentState.Contains(csLoading)) {
			this->Draw();
		}
	}
}

// --------------------------------------------------------------------------- 
UnicodeString __fastcall TStateMachineConnection::GetConditionBack(void) {
	if (FConditionBack.IsEmpty()) {
		try {
			return GetConditionBackFromSettingsThrow(GetSyntaxSchemeThrow());
		}
		catch(Exception * E) {
			// safe for inspector 
		}
	}

	return FConditionBack;
}

// --------------------------------------------------------------------------- 
UnicodeString __fastcall TStateMachineConnection::GetSyntaxSchemeThrow(void) {
	UnicodeString sSyntaxScheme = L"";
	if (this->StateMachineEditor) {
		sSyntaxScheme = this->StateMachineEditor->SyntaxScheme;
	}
	if (sSyntaxScheme.IsEmpty())
		throw EStateMachineConnectionException(this, L"Datamodel not supported", L"SWITCH INVERSE does not support 'null' datamodel!");

	return sSyntaxScheme;
}

// --------------------------------------------------------------------------- 
UnicodeString __fastcall TStateMachineConnection::GetConditionBackFromSettingsThrow(const UnicodeString &sSyntaxScheme) {

	const UnicodeString sInvertedConditionMacro = SettingsData->ConnectionInvertedConditionMacro->Values[sSyntaxScheme];
	if (sInvertedConditionMacro.IsEmpty())
		throw EStateMachineConnectionException(this, L"Conversion macro not found",
		L"SWITCH INVERSE can not find conversion macro for [" + sSyntaxScheme + "] datamodel!");

	return sInvertedConditionMacro;
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetConditionBack(UnicodeString val) {
	FConditionBack = val.Trim();

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetDescription(UnicodeString val) {
	FDescription = val.Trim();

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TTreeConnection *AConnection = dynamic_cast<TTreeConnection*>(Source);
		if (Source) {
			// ********************************************* // 
			// сначала все назначения из базового класса !!! // 
			// ********************************************* // 
			TTreeConnection::Assign(AConnection);
			// ********************************************* // 

			TStateMachineConnection * ATreeConnection = dynamic_cast<TStateMachineConnection*>(Source);
			if (ATreeConnection) {
				this->AssignStateMachineConnection(ATreeConnection);
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::AssignStateMachineConnection(TStateMachineConnection *AStateMachineConnection) {
	if (AStateMachineConnection) {
		FXML = AStateMachineConnection->FXML;
		FEvent = AStateMachineConnection->FEvent;
		FCondition = AStateMachineConnection->FCondition;
		FConditionBack = AStateMachineConnection->FConditionBack;
		FAdjustableSides = AStateMachineConnection->FAdjustableSides;
		FContentTrigger->Assign(AStateMachineConnection->FContentTrigger);
		FParams->Assign(AStateMachineConnection->FParams);
		FScxmlTransitionDisplayTypes = AStateMachineConnection->FScxmlTransitionDisplayTypes;
		FIsTargetStored = AStateMachineConnection->FIsTargetStored;
		FClipOutCondition = AStateMachineConnection->FClipOutCondition;
		FClipOutXML = AStateMachineConnection->FClipOutXML;
		FClipTypeCondition = AStateMachineConnection->FClipTypeCondition;
		FClipTypeXML = AStateMachineConnection->FClipTypeXML;
		FClipOutMax = AStateMachineConnection->FClipOutMax;
		FExtraParams = AStateMachineConnection->FExtraParams;
		FLocked = AStateMachineConnection->FLocked;
		FDescription = AStateMachineConnection->FDescription;
		FSwitch = AStateMachineConnection->FSwitch;
		FVerticalCentering = AStateMachineConnection->FVerticalCentering;
		FInheritanceResolver->Assign(AStateMachineConnection->FInheritanceResolver);
		FSkipAutolayout = AStateMachineConnection->FSkipAutolayout;
		FBreakpointSet = AStateMachineConnection->FBreakpointSet;

		// здесь произойдет UpdateTransitionText 
		FProtocolControlBinding->Assign(AStateMachineConnection->FProtocolControlBinding);

		// параметры с применением Set 
		InheritanceMismatch = AStateMachineConnection->InheritanceMismatch;

		UpdateAppearance();
	}
}

// --------------------------------------------------------------------------- 
TStateMachineEditorUnit *__fastcall TStateMachineConnection::GetStateMachineEditorUnit(void) {
	if (this->Tree) {
		TStateMachineEditor * AStateMachineEditor = StateMachineEditor;
		if (AStateMachineEditor) {
			return AStateMachineEditor->StateMachineEditorUnit;
		}
	}
	return NULL;
}

// --------------------------------------------------------------------------- 
TStateMachineEditor * __fastcall TStateMachineConnection::GetStateMachineEditor(void) {
	if (this->Tree) {
		return dynamic_cast<TStateMachineEditor*>(this->Tree->Owner);
	}
	return NULL;
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::ShowEditor(void) {
	if (!FTransitionEditor) {
		FTransitionEditor = new TFormTransitionEditor(this, this);
	}
	FTransitionEditor->Show();
	// восстанавливаем свёрнутое 
	if (FTransitionEditor->WindowState == wsMinimized) {
		FTransitionEditor->WindowState = wsNormal;
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SaveEditor(void) {
	if (FTransitionEditor) {
		FTransitionEditor->ActionSave->Execute();
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::UpdateTransitionText(void) {
	this->Text->Clear();
	if ((FScxmlTransitionDisplayTypes.Contains(stdtEvent) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE)) && !FEvent.IsEmpty()) {
		this->Text->Add(FEvent);
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtCondition) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE)) && !FCondition.IsEmpty
		()) {

		if (this->ClipOutCondition) {
			this->Text->Add(ClipText(Condition, ClipTypeCondition, ClipOutMax));
		}
		else {
			std::auto_ptr<TStringList>AConditionList(new TStringList());
			AConditionList->Text = Condition;

			if (this->SWITCH == tstINVERT_CONDITION && !FConditionBack.IsEmpty()) {
				std::auto_ptr<TStringList>AConditionBackListPtr(new TStringList());
				AConditionBackListPtr->Text = L"◄- " + StringReplace(FConditionBack, L"%COND%", FCondition,
					TReplaceFlags() << rfReplaceAll);

				AConditionList->AddStrings(AConditionBackListPtr.get());
			}

			this->Text->AddStrings(AConditionList.get());
		}

	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtXML) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE)) && !XML.Trim().IsEmpty()) {
		std::auto_ptr<TStringList>AXMLList(new TStringList());
		AXMLList->Text = XML.Trim();
		for (int i = AXMLList->Count - 1; i >= 0; i--) {
			AXMLList->Strings[i] = AXMLList->Strings[i].Trim();
			if (AXMLList->Strings[i].IsEmpty()) {
				AXMLList->Delete(i);
			}
		}
		if (this->ClipOutXML) {
			// обязательно Trim в этом месте, так как иначе будут добавлены '\r\n' 
			this->Text->Add(ClipText(AXMLList->Text.Trim(), ClipTypeXML, ClipOutMax));
		}
		else {
			this->Text->AddStrings(AXMLList.get());
		}
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtBindingName) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE))
		&& ProtocolControlBinding->Active) {
		this->Text->Add(ProtocolControlBinding->ControlName);
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtDescription) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE))
		&& !FDescription.IsEmpty()) {
		this->Text->Add(FDescription);
	}

	if (!this->ComponentState.Contains(csLoading) && !this->ComponentState.Contains(csReading)) {
		if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
			StateMachineEditor->MarkUpdateNodeTreeElement(this);
		}
	}
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::IsScxmlTransitionDisplayTypesStored(void) {
	return !FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE);
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetScxmlTransitionDisplayTypes(TScxmlTransitionDisplayTypes ATypes) {
	FScxmlTransitionDisplayTypes = ATypes << stdtEvent;

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetXML(UnicodeString val) {
	FXML = val.Trim();

	UpdateTransitionText();
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DoAppendExtraParamsAndChildren(Lmdxml::ILMDXmlElement *AElement,
	Lmdxml::ILMDXmlElement *AParentNode) {
	// дополнительные аттрибуты 
	std::auto_ptr<TStringList>AExtraParamsList(new TStringList());
	AExtraParamsList->Text = this->ExtraParams;
	for (int i = 0; i < AExtraParamsList->Count; i++) {
		if (!AExtraParamsList->Names[i].IsEmpty()) {
			AElement->SetAttr(AExtraParamsList->Names[i], AExtraParamsList->ValueFromIndex[i]);
		}
	}

	if (!this->XML.IsEmpty()) {
		_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML(this->XML);
		if (ADoc) {
			for (int i = 0; i < ADoc->ChildNodes->Count; i++) {
				AElement->AppendChild(ADoc->ChildNodes->Item[i]);
			}
		}
		else
			WLOG_ERROR(L"Transition body XML format is invalid[%s]", this->XML.c_str());
	}

	if (AParentNode) {

		for (int i = 0; i < AParentNode->ChildNodes->Count; i++) {
			if (AParentNode->ChildNodes->Item[i]->XML == AElement->XML) {
				throw EStateMachineConnectionException(this, L"Transition duplicated",
					"Transition duplicated! Full XML: " + AElement->XML.Trim());
			}
		}

		AParentNode->AppendChild((Lmdxml::ILMDXmlElement*)AElement);
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::AppendTransitionElement(Lmdxml::ILMDXmlElement *AParentNode) {
	try {
		// только интерфейс должен быть для пристыковки дальше !!! 
		_di_ILMDXmlElement AElement = LMDCreateXmlElement("transition");

		bool bTransitionIsEmpty = true;

		if (!this->Condition.IsEmpty()) {
			AElement->SetAttr("cond", this->Condition);
			bTransitionIsEmpty = false;
		}
		if (!this->Event.IsEmpty()) {
			AElement->SetAttr("event", this->Event);
			bTransitionIsEmpty = false;
		}

		if (bTransitionIsEmpty && IsSelfConnection && SettingsData->ValidateEmptyTransitions) {
			throw EStateMachineConnectionException(this, L"No 'condition', 'event' or 'target'",
				L"SELF-TRANSITION must not have both empty 'Condition' and 'Event'!");
		}

		if (!this->Target.IsEmpty()) {
			if (this->IsTargetStored) {
				AElement->SetAttr("target", this->Target);
			}
		}
		else {
			if (!this->IsSelfConnection && this->ToShape) {
				throw EStateMachineConnectionException(this, L"Target 'ID' empty", L"Can not create target to <state> with empty ID!");
			}
		}

		if (this->Type != TScxmlTransitionType::external || SettingsData->ForceSaveDefaultScxmlValues) {
			AElement->SetAttr("type", this->Type == TScxmlTransitionType::external ? L"external" : L"internal");
		}

		DoAppendExtraParamsAndChildren(AElement, AParentNode);
	}
	catch(EStateMachineConnectionException * E) {
		throw E;
	}
	catch(Exception * E) {
		throw EStateMachineConnectionException(this, L"Invalid syntax",
			UnicodeString().sprintf(L"Transition [%s->%s]: %s", this->FromShape ? this->FromShape->SimpleText.c_str() : L"",
				this->ToShape ? this->ToShape->SimpleText.c_str() : L"", E->Message.c_str()));
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::AppendTransitionSwitchElement(Lmdxml::ILMDXmlElement *AParentNode, const UnicodeString &sTarget) {
	try {
		if (sTarget.IsEmpty())
			throw EStateMachineConnectionException(this, L"'Target' not defined", L"SWITCH Target Not Defined!");

		if (IsSelfConnection && SWITCH != tstNONE) {

			this->SWITCH = tstNONE;

			throw EStateMachineConnectionException(this, L"SWITCH Not applicable for SELF-CONNECTION!",
				this->Name + L"> SWITCH Not applicable for SELF-CONNECTION!");
		}

		// только интерфейс должен быть для пристыковки дальше !!! 
		_di_ILMDXmlElement AElement = LMDCreateXmlElement(L"transition");

		switch(SWITCH) {
		case tstINVERT_CONDITION: {
				if (this->Condition.IsEmpty())
					throw EStateMachineConnectionException(this, L"'Condition' not defined",
					L"SWITCH INVERSE requires non-empty CONDITION!");

				const UnicodeString sSyntaxScheme = GetSyntaxSchemeThrow();

				const UnicodeString sInvertedConditionMacro = FConditionBack.IsEmpty() ? GetConditionBackFromSettingsThrow(sSyntaxScheme)
					: FConditionBack;

				AElement->SetAttr("cond", StringReplace(sInvertedConditionMacro, L"%COND%", this->Condition,
						TReplaceFlags() << rfReplaceAll));

				if (!this->Event.IsEmpty()) {
					AElement->SetAttr("event", this->Event);
				}
			}break;
		case tstSIMPLE: {
				if (!this->Condition.IsEmpty()) {
					AElement->SetAttr("cond", this->Condition);
				}

				if (this->Event.IsEmpty())
					throw EStateMachineConnectionException(this, L"'Event' not defined", L"SWITCH SIMPLE requires non-empty EVENT!");

				AElement->SetAttr("event", this->Event);
			}break;
		default:
			throw EStateMachineConnectionException(this, "Invalid Program Logic",
				"'AppendTransitionSwitchElement' Can not be applied for SWITCH==tstNONE!");
		}

		AElement->SetAttr("target", sTarget);

		DoAppendExtraParamsAndChildren(AElement, AParentNode);
	}
	catch(EStateMachineConnectionException * E) {
		throw E;
	}
	catch(Exception * E) {
		throw Exception(UnicodeString().sprintf(L"Connection [%s->%s]: %s", this->FromShape ? this->FromShape->SimpleText.c_str() : L"",
				this->ToShape ? this->ToShape->SimpleText.c_str() : L"", E->Message.c_str()));
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::MakeSelfConnection(void) {
	try {
		if (!this->IsSelfConnection)
			throw Exception(UnicodeString().sprintf(L"Transition [%s] [%s] is not a self-connection!!!", this->Name.c_str(),
				this->SimpleText.c_str()));

		this->AlignSelfConnection(scaTop);

		UpdateAppearance();
	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}

}

// --------------------------------------------------------------------------- 
TSelfConnectionInfo __fastcall TStateMachineConnection::GetSelfConnectionInfo() {
	TSelfConnectionInfo AInfo;
	memset(&AInfo, 0, sizeof(TSelfConnectionInfo));
	AInfo.Align = scaTop;

	if (this->IsSelfConnection) {

		TVisualScxmlBaseShape * AVisShape = dynamic_cast<TVisualScxmlBaseShape*>(this->FromShape);
		if (!AVisShape)
			throw Exception("FromShape is NULL or can not cast to <TVisualScxmlBaseShape>!");

		if (this->Points->Count() == 4) {

			const int tmpAngle = RoundTo(RadToDeg(ArcTan2((this->Points->Item[2].X -this->Points->Item[0].X), //
						(this->Points->Item[2].Y -this->Points->Item[0].Y))), 0);

			WLOG_DEBUG(L"%d", tmpAngle);

			//			throw Exception("Break"); 

			// пока ориентируемся по этой точке, предполагаем, что она параллельна со второй точкой 
			const int iStartPointX = this->Points->Item[0].X;
			const int iStartPointY = this->Points->Item[0].Y;

			const int iCheckPointX = this->Points->Item[1].X;
			const int iCheckPointY = this->Points->Item[1].Y;
			const int iCheckPointX2 = this->Points->Item[2].X;
			const int iCheckPointY2 = this->Points->Item[2].Y;

			// определение верха 
			if ((iCheckPointX2 > iStartPointX) && //
				(iCheckPointY2 < iStartPointY && iCheckPointY < iStartPointY)) {
				AInfo.Align = AVisShape->SelfConnectionInside ? scaBottom : scaTop;

				switch(this->Points->Item[1].XStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetBegin = this->Points->Item[1].XValue * 100 / AVisShape->Width;
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetBegin = this->Points->Item[1].XValue;
					break;
				default:
					AInfo.LengthOffsetBegin = 0;
				}

				switch(this->Points->Item[2].XStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetEnd = 100 - (this->Points->Item[2].XValue * 100 / AVisShape->Width);
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetEnd = 100-this->Points->Item[2].XValue;
					break;
				default:
					AInfo.LengthOffsetEnd = 0;
				}

				AInfo.TextOffset = AVisShape->SelfConnectionTextOffsets->Top;
			}
			else
			// определение низа 
				if ((iCheckPointX2 > iStartPointX) && //
				(iCheckPointY2 > iStartPointY && iCheckPointY > iStartPointY)) {
				AInfo.Align = AVisShape->SelfConnectionInside ? scaTop : scaBottom;

				switch(this->Points->Item[1].XStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetBegin = this->Points->Item[1].XValue * 100 / AVisShape->Width;
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetBegin = this->Points->Item[1].XValue;
					break;
				default:
					AInfo.LengthOffsetBegin = 0;
				}

				switch(this->Points->Item[2].XStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetEnd = 100 - (this->Points->Item[2].XValue * 100 / AVisShape->Width);
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetEnd = 100-this->Points->Item[2].XValue;
					break;
				default:
					AInfo.LengthOffsetEnd = 0;
				}

				AInfo.TextOffset = AVisShape->SelfConnectionTextOffsets->Bottom;
			}
			else
			// определение влево 
				if ((iCheckPointY2 > iStartPointY) && //
				(iCheckPointX2 < iStartPointX && iCheckPointX < iStartPointX)) {
				AInfo.Align = AVisShape->SelfConnectionInside ? scaRight : scaLeft;

				switch(this->Points->Item[1].YStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetBegin = this->Points->Item[1].YValue * 100 / AVisShape->Height;
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetBegin = this->Points->Item[1].YValue;
					break;
				default:
					AInfo.LengthOffsetBegin = 0;
				}

				switch(this->Points->Item[2].YStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetEnd = 100 - (this->Points->Item[2].YValue * 100 / AVisShape->Height);
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetEnd = 100-this->Points->Item[2].YValue;
					break;
				default:
					AInfo.LengthOffsetEnd = 0;
				}

				AInfo.TextOffset = AVisShape->SelfConnectionTextOffsets->Left;
			}
			else
			// определение вправо 
				if ((iCheckPointY2 > iStartPointY) && //
				(iCheckPointX2 > iStartPointX && iCheckPointX > iStartPointX)) {
				AInfo.Align = AVisShape->SelfConnectionInside ? scaLeft : scaRight;

				switch(this->Points->Item[1].YStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetBegin = this->Points->Item[1].YValue * 100 / AVisShape->Height;
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetBegin = this->Points->Item[1].YValue;
					break;
				default:
					AInfo.LengthOffsetBegin = 0;
				}

				switch(this->Points->Item[2].YStyle) {
				case cpsFromRel:
					AInfo.LengthOffsetEnd = 100 - (this->Points->Item[2].YValue * 100 / AVisShape->Height);
					break;
				case cpsFromPercent:
					AInfo.LengthOffsetEnd = 100-this->Points->Item[2].YValue;
					break;
				default:
					AInfo.LengthOffsetEnd = 0;
				}
				AInfo.TextOffset = AVisShape->SelfConnectionTextOffsets->Right;
			}

		}
	}
	return AInfo;
}

// --------------------------------------------------------------------------- 
struct TCorner {
	TCorner() {
		iOffset = 0;
		iCount = 0;
		iIndex = 0;
	}

	int iOffset;
	int iCount;
	int iIndex;
};

// --------------------------------------------------------------------------- 
void GetSelfConnectionCorners(TTreeConnection * AConnection, const bool bIsSelf, const bool bIsReverseOrder, TCorner &ATop,
	TCorner &ABottom, TCorner &ALeft, TCorner &ARight) {
	if (AConnection->FromShape == AConnection->ToShape) {
		if (AConnection->Points->Count() == 4) {

			// ориентируемся по первой точке, она является ключевой в Self-Connection 
			const int iStartPointX = AConnection->Points->Item[0].X;
			const int iStartPointY = AConnection->Points->Item[0].Y;

			const int iCheckPointX = AConnection->Points->Item[1].X;
			const int iCheckPointY = AConnection->Points->Item[1].Y;
			const int iCheckPointX2 = AConnection->Points->Item[2].X;
			const int iCheckPointY2 = AConnection->Points->Item[2].Y;

			// определение верха 
			if ((iCheckPointX2 > iStartPointX) && //
				(iCheckPointY2 < iStartPointY && iCheckPointY < iStartPointY)) {

				if (!bIsSelf) {
					const int iStartY = bIsReverseOrder ? AConnection->FromShape->Y1 : AConnection->FromShape->Y0;
					ATop.iOffset = abs(iStartY - iCheckPointY);
					ATop.iIndex++;
				}

				ATop.iCount++;
			}

			// определение низа 
			if ((iCheckPointX2 > iStartPointX) && //
				(iCheckPointY2 > iStartPointY && iCheckPointY > iStartPointY)) {

				if (!bIsSelf) {
					const int iStartY = bIsReverseOrder ? AConnection->FromShape->Y0 : AConnection->FromShape->Y1;
					ABottom.iOffset = abs(iStartY - iCheckPointY);
					ABottom.iIndex++;
				}

				ABottom.iCount++;
			}

			// определение влево 
			if ((iCheckPointY2 > iStartPointY) && //
				(iCheckPointX2 < iStartPointX && iCheckPointX < iStartPointX)) {

				if (!bIsSelf) {
					const int iStartX = bIsReverseOrder ? AConnection->FromShape->X1 : AConnection->FromShape->X0;
					ALeft.iOffset = abs(iStartX - iCheckPointX);
					ALeft.iIndex++;
				}

				ALeft.iCount++;
			}

			// определение вправо 
			if ((iCheckPointY2 > iStartPointY) && //
				(iCheckPointX2 > iStartPointX && iCheckPointX > iStartPointX)) {

				if (!bIsSelf) {
					const int iStartX = bIsReverseOrder ? AConnection->FromShape->X0 : AConnection->FromShape->X1;
					ARight.iOffset = abs(iStartX - iCheckPointX);
					ARight.iIndex++;
				}

				ARight.iCount++;
			}
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::AlignSelfConnection(const TSelfConnectionAlign AConnectionAlign,
	const int iLengthOffsetBegin/* = 0*/, const int iLengthOffsetEnd/* = 0*/, const int iTextOffset/* = 0*/) {
	try {
		if (!this->IsSelfConnection)
			throw Exception(UnicodeString().sprintf(L"Transition [%s] [%s] is not a self-connection!!!", this->Name.c_str(),
				this->SimpleText.c_str()));

		const int iTextLinesCount = this->TextLinesCount();

		this->Tree->Canvas->Font->Assign(this->Font);

		const int iCharHeight = this->Tree->Canvas->TextHeight(TeeCharForHeight);
		const int iTextHeight = iTextLinesCount * iCharHeight + 6;
		int iSelfTextHeight = (iTextHeight < SettingsData->ConnectionSelfDefaultHeight ?
			SettingsData->ConnectionSelfDefaultHeight : iTextHeight) + iTextOffset;

#if 0 // для глубокой отладки
		WLOG_DEBUG(L"Self-connection:[%s] iTextLinesCount=%d iCharHeight=%d iTextHeight=%d iSelfTextHeight:[%d]", this->SimpleText.c_str(),
			iTextLinesCount, iCharHeight, iTextHeight, iSelfTextHeight);
#endif

		TCorner ATop, ABottom, ALeft, ARight;

		bool bFindSelf = false;

		/* https://github.com/alexzhornyak/ScxmlEditor-Tutorial/issues/94 */
		bool bReverse = false;

		TVisualScxmlBaseShape * AVisualFrom = this->GetVisualFrom();
		if (AVisualFrom) {
			bReverse = AVisualFrom->SelfConnectionInside;
		}

		for (int i = 0; i < this->FromShape->Connections->Count; i++) {
			if (this->FromShape->Connections->Items[i] == this) {
				bFindSelf = true;
			}

			GetSelfConnectionCorners(this->FromShape->Connections->Items[i], bFindSelf, bReverse, ATop, ABottom, ALeft, ARight);
		}

		this->Points->Clear();

		const int iShapeWidth = this->FromShape->Width;
		const int iShapeHeight = this->FromShape->Height;

		int iQuoterWidth = iShapeWidth / 4;
		int iQuoterHeight = iShapeHeight / 4;

		TSelfConnectionAlign AResultAlign = AConnectionAlign;

		if (bReverse) {
			switch(AConnectionAlign) {
			case scaTop:
				AResultAlign = scaBottom;
				break;
			case scaBottom:
				AResultAlign = scaTop;
				break;
			case scaLeft:
				AResultAlign = scaRight;
				break;
			case scaRight:
				AResultAlign = scaLeft;
				break;
			}
		}

		switch(AResultAlign) {
		case scaTop: {
				iQuoterWidth = iQuoterWidth / (ATop.iCount + 1) * (ATop.iIndex + 1);
				iQuoterWidth = iQuoterWidth * 100 / iShapeWidth;

				const int iStartY = bReverse ? 100 : 0;
				const TConnectionPointStyle AMiddleStyleY = bReverse ? cpsPrevious : cpsFromRel;

				this->Points->Add(cpsFromPercent, 0 + iQuoterWidth + iLengthOffsetBegin, cpsFromPercent, iStartY);
				this->Points->Add(cpsFromPercent, 0 + iLengthOffsetBegin, AMiddleStyleY, 0 - iSelfTextHeight - ATop.iOffset);
				this->Points->Add(cpsFromPercent, 100 - iLengthOffsetEnd, cpsPrevious, 0);
				this->Points->Add(cpsFromPercent, 100 - iQuoterWidth - iLengthOffsetEnd, cpsFromPercent, iStartY);
			}break;
		case scaBottom: {
				iQuoterWidth = iQuoterWidth / (ABottom.iCount + 1) * (ABottom.iIndex + 1);
				iQuoterWidth = iQuoterWidth * 100 / iShapeWidth;

				const int iStartY = bReverse ? 0 : 100;

				this->Points->Add(cpsFromPercent, 0 + iQuoterWidth + iLengthOffsetBegin, cpsFromPercent, iStartY);
				this->Points->Add(cpsFromPercent, 0 + iLengthOffsetBegin, cpsPrevious, iSelfTextHeight + ABottom.iOffset);
				this->Points->Add(cpsFromPercent, 100 - iLengthOffsetEnd, cpsPrevious, 0);
				this->Points->Add(cpsFromPercent, 100 - iQuoterWidth - iLengthOffsetEnd, cpsFromPercent, iStartY);
			}break;
		case scaLeft: {
				iQuoterHeight = iQuoterHeight / (ALeft.iCount + 1) * (ALeft.iIndex + 1);
				iQuoterHeight = iQuoterHeight * 100 / iShapeHeight;

				const int iStartX = bReverse ? 100 : 0;
				const TConnectionPointStyle AMiddleStyleX = bReverse ? cpsPrevious : cpsFromRel;

				this->Points->Add(cpsFromPercent, iStartX, cpsFromPercent, 0 + iQuoterHeight + iLengthOffsetBegin);
				this->Points->Add(AMiddleStyleX, 0 - iSelfTextHeight - ALeft.iOffset, cpsFromPercent, 0 + iLengthOffsetBegin);
				this->Points->Add(cpsPrevious, 0, cpsFromPercent, 100 - iLengthOffsetEnd);
				this->Points->Add(cpsFromPercent, iStartX, cpsFromPercent, 100 - iQuoterHeight - iLengthOffsetEnd);
			}break;
		case scaRight: {
				iQuoterHeight = iQuoterHeight / (ARight.iCount + 1) * (ARight.iIndex + 1);
				iQuoterHeight = iQuoterHeight * 100 / iShapeHeight;

				const int iStartX = bReverse ? 0 : 100;

				this->Points->Add(cpsFromPercent, iStartX, cpsFromPercent, 0 + iQuoterHeight + iLengthOffsetBegin);
				this->Points->Add(cpsPrevious, iSelfTextHeight + ARight.iOffset, cpsFromPercent, 0 + iLengthOffsetBegin);
				this->Points->Add(cpsPrevious, 0, cpsFromPercent, 100 - iLengthOffsetEnd);
				this->Points->Add(cpsFromPercent, iStartX, cpsFromPercent, 100 - iQuoterHeight - iLengthOffsetEnd);
			}break;
		}

		// для того, чтобы сформировались координаты X и Y 
		Editorutils::CalculatePointsPosition(this);

	}
	catch(Exception * E) {
		LOG_ERROR(LOG_ERROR_MSG);
	}
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::Draw(void) {

	bool result = false;

	if (Tree && Tree->Canvas) {
		/* если рисуем SVG, то открываем группу */
		TTreeSVGCanvasEx * ATreeSVGCanvasEx = dynamic_cast<TTreeSVGCanvasEx*>(Tree->Canvas);
		if (ATreeSVGCanvasEx) {

			if (this->ExcludeFromSave) {
				/* Must return 'true' to stop further draw */
				return true;
			}

			ATreeSVGCanvasEx->StartGroup(this->Name, this->ClassName());

			/* Description */
			std::auto_ptr<TStringList>ADescListPtr(new TStringList());

			/* Process FromShape */ {
				int iTransitionIterIndex = 0;

				// 1) Smart Transitions 
				Editorutils::TConnectionFromToMap AInConnections;
				Editorutils::CollectInConnections(this->FromShape, AInConnections);

				for (Editorutils::TConnectionFromToMap::iterator it = AInConnections.begin(); it != AInConnections.end(); ++it) {
					TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(it->first);
					/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
					if (AConnection && !AConnection->ExcludeFromSave && AConnection->SWITCH != tstNONE && AConnection != this) {
						iTransitionIterIndex++;
					}
				}

				// 2) Regular Transitions 
				for (int k = 0; k < this->FromShape->Connections->Count; k++) {
					TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(this->FromShape->Connections->Items[k]);
					/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
					if (AConnection && !AConnection->ExcludeFromSave) {

						if (AConnection == this) {
							break;
						}

						iTransitionIterIndex++;
					}
				}

				ADescListPtr->Add(this->FromShape->SimpleText + L"|" + UnicodeString(iTransitionIterIndex));
			}

			/* Process ToShape */
			if (!this->IsSelfConnection && this->SWITCH != tstNONE) {
				// 1) Smart Transitions 
				Editorutils::TConnectionFromToMap AInConnections;
				Editorutils::CollectInConnections(this->ToShape, AInConnections);

				int iTransitionIterIndex = 0;

				for (Editorutils::TConnectionFromToMap::iterator it = AInConnections.begin(); it != AInConnections.end(); ++it) {
					TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(it->first);
					/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
					if (AConnection && !AConnection->ExcludeFromSave && AConnection->SWITCH != tstNONE) {

						if (AConnection == this) {

							ADescListPtr->Add(this->ToShape->SimpleText + L"|" + UnicodeString(iTransitionIterIndex));

							break;
						}

						iTransitionIterIndex++;
					}
				}
			}

			ATreeSVGCanvasEx->AddDesc(ADescListPtr->Text);
		}

		/* ОТРИСОВКА */
		try {

			result = true;

			// раньше вызывалось только один раз, 
			// 26.04.2019 тестово устанавливаем постоянно 
			UpdateAppearance();

			if (Visible()) {
				// use manager to draw connection line(s)... 
				if (Style == csAuto) {
					if (Tree && Tree->GlobalFormat.ChildManager) {
						result = Tree->GlobalFormat.ChildManager->DrawConnection(this);
					}
				}
				else {
					DoInternalDrawEx();
				}
			}
		}
		/* ФИНАЛЬНЫЕ ДЕЙСТВИЯ ПОСЛЕ ОТРИСОКИ */
		__finally {
			if (ATreeSVGCanvasEx) {
				ATreeSVGCanvasEx->EndGroup();
			}
		}
	}

	return result;
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DoInternalDrawEx(void) {
	TTreeEx *ATree = dynamic_cast<TTreeEx*>(Tree);
	if (!ATree) {
		InternalDraw();
		return;
	}

	DoPreparePoints();

	// не делаем 'const', так как для Curve может поменяться их кол-во 
	int tmpNum = Points->Count();
	if (!tmpNum)
		return;

	DoPrepareIBounds();

	if (Style == csAuto || Intersect(IBounds, ATree->Bounds2D)) {
		PrepareCanvas();

		std::vector<TPoint>ARealPoints;

		/* Draw Lines */
		if (Style == csCurve) {
			TCurvePoints tmpCurve;
			GetCurvePoints(tmpCurve);

			// Curves need 4 points, or more than 4 in groups of 3 
			if (tmpNum > 4) {
				while ((tmpNum - 4) % 3 != 0) {
					tmpCurve[tmpNum] = tmpCurve[tmpNum - 1];
					tmpNum++;
				}
			}

			/* fix: https://github.com/alexzhornyak/ScxmlEditor-Tutorial/issues/90 */
			if (TTreeSVGCanvasEx * ATreeSVGCanvasEx = dynamic_cast<TTreeSVGCanvasEx*>(Tree->Canvas)) {
				// Draw as SVG cubic bezier path 
				ATreeSVGCanvasEx->DrawPolyBezier(tmpCurve, tmpNum);
			}
			else {
				PolyBezier(Tree->Canvas->Handle, tmpCurve, tmpNum);
			}

			for (int i = 0; i < tmpNum; i++) {
				ARealPoints.push_back(tmpCurve[i]);
			}
		}
		else {
			if (tmpNum > 1) {
				int tmpX = Points->Item[0].X;
				int tmpY = Points->Item[0].Y;

				Tree->Canvas->MoveTo3D(tmpX, tmpY, TeeTreeZ);
				ARealPoints.push_back(TPoint(Points->Item[0].X, Points->Item[0].Y));

				for (int i = 1; i < Points->Count(); i++) {
					switch(Style) {
					case csSides: {
							const double dDistance = Editorutils::DistanceBetweenPoints(tmpX, tmpY, tmpX, Points->Item[i].Y);
							if (dDistance <= 1.0f) {
								// WLOG_DEBUG(L"Too small distance point=%d dist=%f", i, dDistance); 
								ARealPoints.push_back(TPoint(Points->Item[i].X, Points->Item[i].Y));
							}
							else {
								ARealPoints.push_back(TPoint(tmpX, Points->Item[i].Y));
								if (tmpX - Points->Item[i].X) {
									ARealPoints.push_back(TPoint(Points->Item[i].X, Points->Item[i].Y));
								}
							}
						}break;
					case csInvertedSides: {
							const double dDistance = Editorutils::DistanceBetweenPoints(tmpX, tmpY, Points->Item[i].X, tmpY);
							if (dDistance <= 1.0f) {
								// WLOG_DEBUG(L"Too small distance point=%d dist=%f", i, dDistance); 
								ARealPoints.push_back(TPoint(Points->Item[i].X, Points->Item[i].Y));
							}
							else {
								ARealPoints.push_back(TPoint(Points->Item[i].X, tmpY));
								if (tmpY - Points->Item[i].Y) {
									ARealPoints.push_back(TPoint(Points->Item[i].X, Points->Item[i].Y));
								}
							}
						}break;
					default:
						ARealPoints.push_back(TPoint(Points->Item[i].X, Points->Item[i].Y));
					}

					tmpX = Points->Item[i].X;
					tmpY = Points->Item[i].Y;
				}

				for (std::size_t i = 1; i < ARealPoints.size(); i++) {
					Tree->Canvas->LineTo3D(ARealPoints[i].x, ARealPoints[i].y, TeeTreeZ);
				}

			}
			else {
				// в этом месте только одна точка может быть! 
				ARealPoints.push_back(TPoint(Points->Item[0].X, Points->Item[0].Y));
			}
		}

		double tmpAngle = 0;

		int tmpX0 = 0;
		int tmpY0 = 0;
		int tmpX1 = 0;
		int tmpY1 = 0;

		/* Draw Arrows */
		// 1) Arrow From 
		if (ARealPoints.size() > 1) {
			tmpX1 = ARealPoints[1].x;
			tmpY1 = ARealPoints[1].y;
			tmpX0 = ARealPoints[0].x;
			tmpY0 = ARealPoints[0].y;
			tmpAngle = RoundTo(RadToDeg(ArcTan2((tmpX1 - tmpX0), (tmpY1 - tmpY0))), 0);
		}

		if (ArrowFrom->Style != casNone) {
			ArrowFrom->Draw(Points->Item[0], 270 - tmpAngle);
		}

		// 2) Arrow To 
		tmpAngle = 0;
		const int iLast = tmpNum - 1; // last point index 
		if (iLast > 0) {
			tmpX1 = Points->Item[iLast].X;
			tmpY1 = Points->Item[iLast].Y;
			tmpX0 = Points->Item[iLast - 1].X;
			tmpY0 = Points->Item[iLast - 1].Y;
			switch(Style) {
			case csSides:
				tmpY0 = tmpY1;
				break;
			case csInvertedSides:
				tmpX0 = tmpX1;
				break;
			}
			tmpAngle = RoundTo(RadToDeg(ArcTan2((tmpX1 - tmpX0), (tmpY1 - tmpY0))), 0);
		}

		const double dArrowToAngle = tmpAngle;

		if (ArrowTo->Style != casNone) {
			ArrowTo->Draw(Points->Item[iLast], 90 - dArrowToAngle);
		}

		if (FText)
			if (FText->Angle)
				tmpAngle = FText->Angle;

		tmpAngle -= 90.0f;
		if (((tmpAngle < -90.0f) && (tmpAngle > -180.0f)) || ((tmpAngle < -180.0f) && (tmpAngle > -270.0f))) {
			tmpAngle += 180;
		}

		if (this->BreakpointSet && iLast > 0) {
			TColor ABrushColor = clRed;
			TColor APenColor = clBlack;
			int iPenWidth = 1;

			if (StateMachineEditor) {
				if (StateMachineEditor->FActiveDebugConnection == this) {
					ABrushColor = clInfoBk;
					APenColor = clRed;
					iPenWidth = 3;
				}
			}

			std::auto_ptr<TBrush>ABrushPtr(new TBrush());
			this->Tree->Canvas->Brush->Assign(ABrushPtr.get());
			this->Tree->Canvas->Brush->Color = ABrushColor;

			TPoint AFrom = Point(Points->Item[iLast].X, Points->Item[iLast].Y);
			AFrom = Editorutils::GetRemotePoint(AFrom, -25.0, dArrowToAngle);

			std::auto_ptr<TPen>APenPtr(new TPen());
			this->Tree->Canvas->Pen->Assign(APenPtr.get());
			this->Tree->Canvas->Pen->Color = APenColor;
			this->Tree->Canvas->Pen->Width = iPenWidth;

			this->Tree->Canvas->EllipseWithZ(AFrom.x - 7, AFrom.y - 7, AFrom.x + 7, AFrom.y + 7, TeeTreeZ);
		}

		ISimpleTextBold = !this->Event.Trim().IsEmpty();

		/* Text */
		DrawText(tmpAngle);

		/* Отрисовка индексов соединений */
		if (this->Points->Count() && this->FromShape) {
			// проверяем сколько переходов типа Transition, если больше одного, тогда покажем нумерацию 
			int iStateConnectionsCount = 0;
			int iIndex = 0;
			for (int i = 0; i < this->FromShape->Connections->Count; i++) {
				TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(this->FromShape->Connections->Items[i]);
				if (AConnection) {
					iStateConnectionsCount++;
					if (this == AConnection) {
						iIndex = iStateConnectionsCount;
					}
				}
			}

			if (iStateConnectionsCount > 1) {
				Tree->Canvas->Brush->Style = bsClear;
				Tree->Canvas->Font->Style = TFontStyles() << fsBold;
				Tree->Canvas->Font->Color = clBlack;
				Tree->Canvas->TextOut3D(this->Points->Item[0].X + 10, this->Points->Item[0].Y - Tree->Canvas->Font->Size, TeeTreeZ,
					UnicodeString().sprintf(L"%d)", iIndex));
			}
		}
	}
}

// --------------------------------------------------------------------------- 
TPoint __fastcall TStateMachineConnection::GetTextAbsolutePosWithoutCentering(void) {
	int tmpX = 0;
	int tmpY = 0;
	const int iPointsCount = this->Points->Count();
	if (iPointsCount) {
		int tmpL = iPointsCount / 2;
		TConnectionPoint *tmpB = &(this->Points->Item[tmpL]);
		TConnectionPoint *tmpA = &(this->Points->Item[0]);
		if (tmpL > 0) {
			tmpL--;
			tmpA = &(this->Points->Item[tmpL]);
		}

		switch(this->HorizTextAlign) {
		case htaCenter:
			tmpX = tmpA->X + ((tmpB->X - tmpA->X) / 2);
			break;
		case htaLeft:
			tmpX = tmpA->X;
			break;
		default:
			tmpX = tmpB->X;
		}

		switch(this->VertTextAlign) {
		case vtaCenter:
			tmpY = tmpA->Y + ((tmpB->Y - tmpA->Y) / 2);
			break;
		case vtaTop:
			tmpY = tmpA->Y;
			break;
		default:
			tmpY = tmpB->Y;
		}
	}
	return Point(tmpX, tmpY);
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::CalculateTextPosWithoutDraw(void) {
	if (!Tree)
		return;

	TTreeEx *ATree = dynamic_cast<TTreeEx*>(Tree);
	if (!ATree) {
		return;
	}

	UpdateAppearance();

	DoPreparePoints();

	// не делаем 'const', так как для Curve может поменяться их кол-во 
	int tmpNum = Points->Count();
	if (!tmpNum)
		return;

	DoPrepareIBounds();

	PrepareCanvas();

	double tmpAngle = 0;
	if (FText)
		if (FText->Angle)
			tmpAngle = FText->Angle;

	tmpAngle -= 90.0f;
	if (((tmpAngle < -90.0f) && (tmpAngle > -180.0f)) || ((tmpAngle < -180.0f) && (tmpAngle > -270.0f))) {
		tmpAngle += 180;
	}

	/* Text */ {
		int tmpCount = TextLinesCount();
		bool isVisible = true;
		if (FText) {
			isVisible = FText->Visible;
		}

		if (tmpCount > 0 && isVisible) {
			Tree->Canvas->TextAlign = TA_CENTER;

			Tree->Canvas->AssignFont(InternalFont());

			Tree->Canvas->BackMode = cbmTransparent;

			/* CalcTextPosition */
			TPoint ATextPos = GetTextAbsolutePosWithoutCentering();

			const int tmpOffX = FText ? FText->HorizOffset : 0;
			const int tmpOffY = FText ? FText->VertOffset : 0;

			const int tmpH = Tree->Canvas->TextHeight(TeeCharForHeight);

			// центруем, если текст только по Центру 
			if (FVerticalCentering && !tmpAngle) {
				ATextPos.y -= tmpH * tmpCount / 2;
			}

			DoDrawTextInternal(false, tmpCount, ATextPos.x, tmpOffX, ATextPos.y, tmpOffY, tmpAngle, tmpH);
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DrawText(int Angle) {
	if (!Tree)
		return;

	int tmpCount = TextLinesCount();
	bool isVisible = true;
	if (FText) {
		isVisible = FText->Visible;
	}

	if (tmpCount > 0 && isVisible) {
		Tree->Canvas->TextAlign = TA_CENTER;

		Tree->Canvas->AssignFont(InternalFont());

		Tree->Canvas->BackMode = cbmTransparent;

		/* CalcTextPosition */
		TPoint ATextPos = GetTextAbsolutePosWithoutCentering();

		const int tmpOffX = FText ? FText->HorizOffset : 0;
		const int tmpOffY = FText ? FText->VertOffset : 0;

		const int tmpH = Tree->Canvas->TextHeight(TeeCharForHeight);

		// центруем, если текст только по Центру 
		if (FVerticalCentering && !Angle) {
			ATextPos.y -= tmpH * tmpCount / 2;
		}

		DoDrawText(tmpCount, ATextPos.x, tmpOffX, ATextPos.y, tmpOffY, Angle, tmpH);
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::FillTextColorQueue(std::deque<TColor> &AColorQueue) {
	if (!FEvent.IsEmpty()) {
		AColorQueue.push_back(clBlack);
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtCondition) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE)) && !FCondition.IsEmpty
		()) {
		if (ClipOutCondition) {
			AColorQueue.push_back(clBlue);
		}
		else {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = FCondition;
			for (int i = 0; i < AStringListPtr->Count; i++) {
				AColorQueue.push_back(clBlue);
			}

			if (this->SWITCH == tstINVERT_CONDITION && !FConditionBack.IsEmpty()) {
				std::auto_ptr<TStringList>AConditionBackListPtr(new TStringList());
				AConditionBackListPtr->Text = FConditionBack;

				for (int i = 0; i < AConditionBackListPtr->Count; i++) {
					AColorQueue.push_back(clTeal);
				}
			}
		}
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtXML) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE)) && !FXML.IsEmpty()) {
		if (ClipOutXML) {
			AColorQueue.push_back(clMaroon);
		}
		else {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = FXML.Trim();
			for (int i = AStringListPtr->Count - 1; i >= 0; i--) {
				AStringListPtr->Strings[i] = AStringListPtr->Strings[i].Trim();
				if (AStringListPtr->Strings[i].IsEmpty()) {
					AStringListPtr->Delete(i);
				}
			}
			for (int i = 0; i < AStringListPtr->Count; i++) {
				AColorQueue.push_back(clMaroon);
			}
		}
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtBindingName) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE))
		&& FProtocolControlBinding->Active) {
		AColorQueue.push_back(clPurple);
	}
	if ((FScxmlTransitionDisplayTypes.Contains(stdtDescription) || FScxmlTransitionDisplayTypes.Contains(stdtMAXSIZE))
		&& !FDescription.IsEmpty()) {
		AColorQueue.push_back(clGreen);
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DoDrawText(int tmpCount, int tmpX, int tmpOffX, int tmpY, int tmpOffY, int Angle, int tmpH) {

	DoDrawTextInternal(true, tmpCount, tmpX, tmpOffX, tmpY, tmpOffY, Angle, tmpH);
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DoDrawTextInternal(const bool bDraw, int tmpCount, int tmpX, int tmpOffX, int tmpY, int tmpOffY,
	int Angle, int tmpH) {
	std::deque<TColor>AColorQueue;
	FillTextColorQueue(AColorQueue);

	memset(&FDrawTextData, 0, sizeof(FDrawTextData));

	FDrawTextData.TextX = tmpX + tmpOffX;
	FDrawTextData.TextY = tmpY + tmpOffY;

	const bool bNeedToDrawBackground = this->FFormat && this->Format->Visible && (!this->Format->Transparent);

	std::vector<boost::tuple<bool/*bold*/, TColor, UnicodeString/*text*/, int/*x*/, int/*y*/ > >AVecText;

	for (int t = 0; t < tmpCount; t++) {
		UnicodeString tmpS = SimpleText;
		if (FText)
			tmpS = FText->Strings[t];

		Tree->Canvas->Font->Style = TFontStyles();

		if (!AColorQueue.empty()) {
			Tree->Canvas->Font->Color = AColorQueue.front();

			switch(Tree->Canvas->Font->Color) {
			case clBlack:
				Tree->Canvas->Font->Style = TFontStyles() << fsBold;
				break;
			}

			AColorQueue.pop_front();
		}

		const int iTmpH = this->Tree->Canvas->TextHeight(TeeCharForHeight);

		const int iTextWidth = this->Tree->Canvas->TextWidth(tmpS);
		const int iAddTextWidth = this->Tree->Canvas->TextWidth(TeeCharForHeight);

		const int iWidth = iTextWidth + iAddTextWidth;
		if (FDrawTextData.MaxWidth < iWidth) {
			FDrawTextData.MaxWidth = iWidth;
		}
		FDrawTextData.MaxHeight += iTmpH;

		const int iTextX = tmpX + tmpOffX;
		const int iTextY = tmpY + tmpOffY;

		if (bDraw) {
			// если бэкграунд, тогда только просчитываем данные 
			if (bNeedToDrawBackground) {
				AVecText.push_back(boost::make_tuple(Tree->Canvas->Font->Style.Contains(fsBold), Tree->Canvas->Font->Color, tmpS, iTextX,
						iTextY));
			}
			else {
				if (Angle == 0) {
					Tree->Canvas->TextOut3D(iTextX, iTextY, TeeTreeZ, tmpS);
				}
				else {
					Tree->Canvas->RotateLabel3D(iTextX, iTextY, TeeTreeZ, tmpS, Angle);
				}
			}
		}

		TPoint ARemote = Editorutils::GetRemotePoint(TPoint(tmpX, tmpY), iTmpH, Angle);
		tmpX = ARemote.x;
		tmpY = ARemote.y;
	}

	TPoint ARemote = Editorutils::GetRemotePoint(TPoint(tmpX, tmpY), -FDrawTextData.MaxHeight / 2, Angle);
	ARemote.x += tmpOffX;
	ARemote.y += tmpOffY;

	const TRect ABoundsRect(ARemote.x - FDrawTextData.MaxWidth / 2, ARemote.y - FDrawTextData.MaxHeight / 2, //
		ARemote.x + FDrawTextData.MaxWidth / 2, ARemote.y + FDrawTextData.MaxHeight / 2);

	RectToFourPoints(ABoundsRect, Angle, FDrawTextData.Polygon);
	FDrawTextData.Bounds = ABoundsRect;

	if (bDraw && bNeedToDrawBackground) {

		TTeeBlend*tmpBlend = NULL;

		if (this->Format->Transparency > 0) {
			TRect ABlendRect = Angle ? RectFromPolygon(FDrawTextData.Polygon, 3, 4) : ABoundsRect;

			TTreeEx * ATreeEx = dynamic_cast<TTreeEx*>(Tree);
			if (ATreeEx) {
				TPoint APtLeftTop = ATreeEx->ConvertToPixels(ABlendRect.Left, ABlendRect.Top);
				TPoint APtRightBottom = ATreeEx->ConvertToPixels(ABlendRect.Right, ABlendRect.Bottom);

				ABlendRect = TRect(APtLeftTop, APtRightBottom);
			}

			tmpBlend = Tree->Canvas->BeginBlending(ABlendRect, this->Format->Transparency);
		}

		this->Tree->Canvas->AssignBrush(this->Format->Brush, this->Format->Color);
		this->Tree->Canvas->AssignVisiblePen(this->Format->Pen);

		if (Angle) {
			this->Tree->Canvas->PolygonWithZ(FDrawTextData.Polygon, 3, TeeTreeZ);
		}
		else {
			this->Tree->Canvas->RectangleWithZ(ABoundsRect, TeeTreeZ);
		}

		this->Tree->Canvas->Brush->Style = bsClear;

		if (this->Format->Transparency > 0 && tmpBlend) {
			Tree->Canvas->EndBlending(tmpBlend);
		}

		for (std::size_t i = 0; i < AVecText.size(); i++) {

			Tree->Canvas->Font->Style = AVecText[i].get<0>() ? (TFontStyles() << fsBold) : (TFontStyles());
			Tree->Canvas->Font->Color = AVecText[i].get<1>();

			if (Angle == 0) {
				Tree->Canvas->TextOut3D(AVecText[i].get<3>(), AVecText[i].get<4>(), TeeTreeZ, AVecText[i].get<2>());
			}
			else {
				Tree->Canvas->RotateLabel3D(AVecText[i].get<3>(), AVecText[i].get<4>(), TeeTreeZ, AVecText[i].get<2>(), Angle);
			}
		}
	}
}

// --------------------------------------------------------------------------- 
int __fastcall TStateMachineConnection::GetTransitionIndex(void) {
	return Editorutils::GetConnectionIndex(this);
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetTransitionIndex(int val) {
	if (Editorutils::SetConnectionIndex(this, val)) {
		if (!this->ComponentState.Contains(csLoading) && !this->ComponentState.Contains(csReading)) {
			if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
				StateMachineEditor->FillNodeTree();
			}
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetConnectionOverStyle(const TConnectionOverType AType) {
	if (this->FromShape && this->ToShape) {

		this->AdjustableSides = false;

		switch(AType) {
		case TConnectionOverType::
			cotOverTop : {

				this->Style = csInvertedSides;

				this->Points->Clear();

				// 1 точка: привязана к FromShape 
				this->Points->Add(cpsFromPercent, 50, cpsFromPercent, 0);

				// 2 точка: вне фигуры 
				const int iPrevious = (this->FromShape->Y0 > this->ToShape->Y0) ? (-(this->FromShape->Y0 -this->ToShape->Y0) - 50) : -50;
				this->Points->Add(cpsFromPercent, 50, cpsPrevious, iPrevious);

				// 3 точка: привязана к ToShape 
				this->Points->Add(cpsToPercent, 50, cpsToPercent, 0);

				this->Text->VertAlign = TVertTextAlign::vtaBottom;
				this->Text->HorizAlign = THorizTextAlign::htaCenter;

				const int iX1 = this->FromShape->X0 +this->FromShape->Width / 2;
				const int iX2 = this->ToShape->X0 +this->ToShape->Width / 2;
				this->Text->HorizOffset = (iX1 > iX2 ? -1 : 1) * abs(iX1 - iX2) / 2;
				this->Text->VertOffset = 0;

			}break;
		case TConnectionOverType::
			cotOverBottom : {
				this->Style = csInvertedSides;

				this->Points->Clear();

				this->Points->Add(cpsFromPercent, 50, cpsFromPercent, 100);

				this->Points->Add(cpsFromPercent, 50, cpsPrevious, //
					(this->ToShape->Y1 > this->FromShape->Y1) ? (this->ToShape->Y1 -this->FromShape->Y1 + 50) : 50);

				this->Points->Add(cpsToPercent, 50, cpsToPercent, 100);

				this->Text->VertAlign = TVertTextAlign::vtaBottom;
				this->Text->HorizAlign = THorizTextAlign::htaCenter;

				const int iX1 = this->FromShape->X0 +this->FromShape->Width / 2;
				const int iX2 = this->ToShape->X0 +this->ToShape->Width / 2;
				this->Text->HorizOffset = (iX1 > iX2 ? -1 : 1) * abs(iX1 - iX2) / 2;
				this->Text->VertOffset = 0;
			}break;

		case TConnectionOverType::
			cotOverLeft : {
				this->Style = csSides;

				this->Points->Clear();

				this->Points->Add(cpsFromPercent, 0, cpsFromPercent, 50);

				this->Points->Add(cpsPrevious, //
					(this->FromShape->X0 > this->ToShape->X0) ? (-(this->FromShape->X0 -this->ToShape->X0) - 50) : -50, //
					cpsFromPercent, 50);

				this->Points->Add(cpsToPercent, 0, cpsToPercent, 50);

				this->Text->VertAlign = TVertTextAlign::vtaCenter;
				this->Text->HorizAlign = THorizTextAlign::htaRight;

				const int iY1 = this->FromShape->Y0 +this->FromShape->Height / 2;
				const int iY2 = this->ToShape->Y0 +this->ToShape->Height / 2;
				this->Text->VertOffset = (iY1 > iY2 ? -1 : 1) * abs(iY1 - iY2) / 2+this->TextCount*this->Font->Height / 2;
				this->Text->HorizOffset = 0;
			}break;
		case TConnectionOverType::
			cotOverRight : {
				this->Style = csSides;

				this->Points->Clear();

				this->Points->Add(cpsFromPercent, 100, cpsFromPercent, 50);

				this->Points->Add(cpsPrevious, //
					(this->ToShape->X1 > this->FromShape->X1) ? (this->ToShape->X1 -this->FromShape->X1 + 50) : 50, //
					cpsFromPercent, 50);

				this->Points->Add(cpsToPercent, 100, cpsToPercent, 50);

				this->Text->VertAlign = TVertTextAlign::vtaCenter;
				this->Text->HorizAlign = THorizTextAlign::htaRight;

				const int iY1 = this->FromShape->Y0 +this->FromShape->Height / 2;
				const int iY2 = this->ToShape->Y0 +this->ToShape->Height / 2;
				this->Text->VertOffset = (iY1 > iY2 ? -1 : 1) * abs(iY1 - iY2) / 2+this->TextCount*this->Font->Height / 2;
				this->Text->HorizOffset = 0;
			}break;
		}

		Editorutils::CalculatePointsPosition(this);

		this->Tree->Invalidate();
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetInheritanceMismatch(bool val) {
	if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {
		FInheritanceMismatch = val;
	}
	else {
		FInheritanceMismatch = false;
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::SetSkipAutolayout(bool val) {
	FSkipAutolayout = val;

	//this->Brush->Style = FSkipDebugging ? bsCross : bsSolid; 
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DoPreparePoints(void) {
	const int tmpNum = Points->Count();

	if (Style != csAuto) {
		if (Style == csCurve) {
			// If curve with less than 4 points, call Setup 
			if (tmpNum < 4) {
				SetupPoints();
			}
		}
		else {
			// If line with less than 2 points, call Setup 
			if (tmpNum < 2) {
#ifdef _PANIC_DEBUG_
				WLOG_DEBUG(L"%s> Setup points. Current point count:[%d]", this->SimpleText.c_str(), tmpNum);
#endif
				SetupPoints();
			}
			else {

				const double d_MIN_AVAILABLE_POINTS_DISTANCE = 5.0f;

				Editorutils::CalculatePointsPosition(this);

				// убираем точки, которые накладываются друг на друга 
				for (int i = tmpNum - 2; i >= 0; i--) {
					const double dDistanceToPrev = Editorutils::DistanceBetweenPoints(Points->Item[i].X, Points->Item[i].Y,
						Points->Item[i + 1].X, Points->Item[i + 1].Y);
					if (dDistanceToPrev <= d_MIN_AVAILABLE_POINTS_DISTANCE) {
#ifdef _PANIC_DEBUG_
						LOG_DEBUG_SS << "-----------------------------";
						Editorutils::DumpConnectionPoints(this);
						WLOG_DEBUG(L"%s> Point[%d] of [%d] is to close to point[%d] - dist[%f]", this->SimpleText.c_str(), i,
							this->Points->Count(), i + 1, dDistanceToPrev);
#endif
						if (i > 0) {
							Points->Delete(i);
						}
						else if (Points->Count() > 2) {
							Points->Delete(i + 1);
						}

#ifdef _PANIC_DEBUG_
						Editorutils::DumpConnectionPoints(this);
#endif
					}
				}
			}
		}
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::DoPrepareIBounds(void) {
	TRect tmpBounds(0, 0, 0, 0);
	const int tmpNum = Points->Count();

	for (int t = 0; t < tmpNum; t++) {
		Points->CalculatePosition(t);

		if (Style != csAuto) {
			if (t == 0) {
				tmpBounds.Left = Points->Item[t].X;
				tmpBounds.Right = Points->Item[t].X;
				tmpBounds.Top = Points->Item[t].Y;
				tmpBounds.Bottom = Points->Item[t].Y;
			}
			else {
				if (Points->Item[t].X < tmpBounds.Left)
					tmpBounds.Left = Points->Item[t].X;
				else if (Points->Item[t].X > tmpBounds.Right)
					tmpBounds.Right = Points->Item[t].X;
				if (Points->Item[t].Y < tmpBounds.Top)
					tmpBounds.Top = Points->Item[t].Y;
				else if (Points->Item[t].Y > tmpBounds.Bottom)
					tmpBounds.Bottom = Points->Item[t].Y;
			}
		}

		IBounds = tmpBounds;
	}
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::ForceCalculateBounds(void) {
	UpdateAppearance();

	DoPreparePoints();

	// не делаем 'const', так как для Curve может поменяться их кол-во 
	int tmpNum = Points->Count();
	if (!tmpNum)
		return;

	DoPrepareIBounds();
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::IsPointInTextPolygon(const TPoint &pt) {
	return PointInPolygon(pt, FDrawTextData.Polygon, 3);
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::IsPointInTextPolygon(const int iX, const int iY) {
	return IsPointInTextPolygon(TPoint(iX, iY));
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::AssignTextPolygonToPoints(TPointCollection * APoints) {
	if (!APoints)
		return;

	APoints->BeginUpdate();
	APoints->Clear();

	for (int i = 0; i < 4; i++) {
		TPointItem *AItem = APoints->Add();
		AItem->X = FDrawTextData.Polygon[i].x;
		AItem->Y = FDrawTextData.Polygon[i].y;
	}

	APoints->EndUpdate();
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::IsTextEmpty(void) {
	return !FText || FText->Text.IsEmpty();
}

// --------------------------------------------------------------------------- 
UnicodeString __fastcall TStateMachineConnection::ToClipboardText(void) {
	std::auto_ptr<TMemoryStream>AInStreamPtr(new TMemoryStream());
	AInStreamPtr->WriteComponent(this);
	AInStreamPtr->Position = 0;
	std::auto_ptr<TStringStream>AOutStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
	ObjectBinaryToText(AInStreamPtr.get(), AOutStreamPtr.get());
	AOutStreamPtr->Position = 0;

	std::auto_ptr<TStringList>AConnectionListPtr(new TStringList());
	AConnectionListPtr->LoadFromStream(AOutStreamPtr.get());
	if (AConnectionListPtr->Count) {
		AConnectionListPtr->Strings[0] = L"object " + ClassName(); // чтобы имя компонента не записывалось 
		return AConnectionListPtr->Text;
	}
	return L"";
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::IsEditorModified(void) {
	if (FTransitionEditor) {
		return FTransitionEditor->IsModified();
	}
	return false;
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::IsEditorOpened(void) {
	if (FTransitionEditor) {
		return FTransitionEditor->Showing;
	}
	return false;
}

// --------------------------------------------------------------------------- 
TVisualScxmlBaseShape * __fastcall TStateMachineConnection::GetVisualFrom(void) {
	return dynamic_cast<TVisualScxmlBaseShape*>(this->FromShape);
}

// --------------------------------------------------------------------------- 
TVisualScxmlBaseShape * __fastcall TStateMachineConnection::GetVisualTo(void) {
	return dynamic_cast<TVisualScxmlBaseShape*>(this->ToShape);
}

// --------------------------------------------------------------------------- 
bool __fastcall TStateMachineConnection::GetExcludeFromSave(void) {
	if (this->VisualFrom && this->VisualTo && !this->VisualFrom->ExcludeFromSave && !this->VisualTo->ExcludeFromSave) {
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------- 
void __fastcall TStateMachineConnection::FillEventNamesList(TStrings *AList) {
	if (this->Tree) {
		for (int i = 0; i < this->Tree->Shapes->Count; i++) {
			if (TSendShape * ASendShape = dynamic_cast<TSendShape*>(this->Tree->Shapes->Items[i])) {
				if (!ASendShape->Event.IsEmpty()) {
					AList->Add(ASendShape->Event);
				}
			}
			else if (TRaiseShape * ARaiseShape = dynamic_cast<TRaiseShape*>(this->Tree->Shapes->Items[i])) {
				if (!ARaiseShape->Event.IsEmpty()) {
					AList->Add(ARaiseShape->Event);
				}
			}
			else if (TProtocolWeakBindingShape * AWeakBindingShape = dynamic_cast<TProtocolWeakBindingShape*>(this->Tree->Shapes->Items[i])
				) {
				if (!AWeakBindingShape->ScxmlName.IsEmpty()) {
					AList->Add(AWeakBindingShape->ScxmlName);
				}
			}
			else if (TProtocolBindingShape * AProtocolBindingShape = dynamic_cast<TProtocolBindingShape*>(this->Tree->Shapes->Items[i])) {
				if (!AProtocolBindingShape->ProtocolControlBinding->ScxmlName.IsEmpty()) {
					AList->Add(AProtocolBindingShape->ProtocolControlBinding->ScxmlName);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------       

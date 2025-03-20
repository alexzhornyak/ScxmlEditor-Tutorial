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

#include "UnitStateShape.h"

#include <IOUtils.hpp>

#include "UnitDataModelStateShape.h"
#include "UnitChildScxmlBaseShapes.h"
#include "UnitStateMachineUnit.h"
#include "UnitScxmlShape.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitDialogFileNotFound.h"
#include "UnitSettings.h"
#include "UnitStringEditorEx.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

/* PROP EDITORS */
class TVirtualSrcDialogPropEditor : public TLMDStringPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs() {
		return TLMDStringPropEditor::GetAttrs() << praMultiSelect << praDialog;
	}

	virtual void __fastcall Edit(void) {
		try {
			if (Propinspext::IsWritablePropertyOfPropEditor(this)) {
				const UnicodeString sValue = this->GetStrValue(0);

				TVirtualShape * AShape = dynamic_cast<TVirtualShape*>(this->GetInstance(0));
				if (AShape) {
					AShape->EditSrc();
				}
			}
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}
};

// ---------------------------------------------------------------------------
// ---------------------------- TStateShape -------------------------------
// ---------------------------------------------------------------------------
__fastcall TStateShape::TStateShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner) {

	FAvailableTypes.insert(sctDatamodel);
	FAvailableTypes.insert(sctOnentry);
	FAvailableTypes.insert(sctOnexit);
	FAvailableTypes.insert(sctInitial);
	FAvailableTypes.insert(sctState);
	FAvailableTypes.insert(sctParallel);
	FAvailableTypes.insert(sctFinal);
	FAvailableTypes.insert(sctHistory);
	FAvailableTypes.insert(sctInvoke);
	FAvailableTypes.insert(sctVirtual);

	this->FStateAttrMap["Id"] = false;
	this->FStateAttrMap["Initial"] = false;

	FConvertibleTypes.insert(sctParallel);
	FConvertibleTypes.insert(sctInitial);
	FConvertibleTypes.insert(sctFinal);
	FConvertibleTypes.insert(sctHistory);
	FConvertibleTypes.insert(sctVirtual);

}

// ---------------------------------------------------------------------------
__fastcall TStateShape::~TStateShape() {

}

// ---------------------------------------------------------------------------
void __fastcall TStateShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *ASourceShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (ASourceShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(ASourceShape);
			// ********************************************* //

			if (Source->InheritsFrom(__classid(TFinalShape)) || //
				Source->InheritsFrom(__classid(THistoryShape)) || //
				Source->InheritsFrom(__classid(TInitialShape))) {

				this->SetDefaultSize();

				this->MoveToOtherShapeCenter(ASourceShape);
			}

			// если мы конвертируемся из других фигур, то включаем отображение текста
			if (!ASourceShape->InheritsFrom(this->ClassType())) {
				this->Text->Visible = true;

				this->Text->VertOffset = 0;
				this->Text->HorizOffset = 0;

				this->Text->VertAlign = vtaCenter;
				this->Text->HorizAlign = htaCenter;
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
// ---------------------------- TParallelShape -------------------------------
// ---------------------------------------------------------------------------
__fastcall TParallelShape::TParallelShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner) {

	FAvailableTypes.insert(sctDatamodel);
	FAvailableTypes.insert(sctOnentry);
	FAvailableTypes.insert(sctOnexit);
	FAvailableTypes.insert(sctState);
	FAvailableTypes.insert(sctParallel);
	FAvailableTypes.insert(sctHistory);
	FAvailableTypes.insert(sctInvoke);
	FAvailableTypes.insert(sctVirtual);

	FConvertibleTypes.insert(sctState);
	FConvertibleTypes.insert(sctInitial);
	FConvertibleTypes.insert(sctFinal);
	FConvertibleTypes.insert(sctHistory);
	FConvertibleTypes.insert(sctVirtual);

	this->FStateAttrMap["Id"] = false;
}

// ---------------------------------------------------------------------------
TColor __fastcall TParallelShape::GetHeadColor(void) {
	return SettingsData->ThemeSettings->ParallelNormalColor;
}

// ---------------------------------------------------------------------------
TColor __fastcall TParallelShape::GetNormalColor(void) {
	return SettingsData->ThemeSettings->ParallelNormalColor;
}

// ---------------------------------------------------------------------------
void __fastcall TParallelShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *ASourceShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (ASourceShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(ASourceShape);
			// ********************************************* //

			if (Source->InheritsFrom(__classid(TFinalShape)) || //
				Source->InheritsFrom(__classid(THistoryShape)) || //
				Source->InheritsFrom(__classid(TInitialShape))) {

				this->SetDefaultSize();

				this->MoveToOtherShapeCenter(ASourceShape);
			}

			// если мы конвертируемся из других фигур, то включаем отображение текста
			if (!ASourceShape->InheritsFrom(this->ClassType())) {
				this->Text->Visible = true;

				this->Text->VertOffset = 0;
				this->Text->HorizOffset = 0;

				this->Text->VertAlign = vtaCenter;
				this->Text->HorizAlign = htaCenter;
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
// ------------------------------ TFinalShape --------------------------------
// ---------------------------------------------------------------------------

__fastcall TFinalShape::TFinalShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner) {
	FAvailableTypes.insert(sctDonedata);
	FAvailableTypes.insert(sctOnentry);
	FAvailableTypes.insert(sctOnexit);

	FConvertibleTypes.insert(sctState);
	FConvertibleTypes.insert(sctInitial);
	FConvertibleTypes.insert(sctParallel);
	FConvertibleTypes.insert(sctHistory);

	this->Style = TTreeShapeStyle::tssCircle;
	this->AutoSize = false;

	this->FStateAttrMap["Id"] = false;

	this->SetBounds(TRect(X0, Y0, X0 + 25, Y0 + 25));
}

// ---------------------------------------------------------------------------
void __fastcall TFinalShape::SetDefaultSize() {
	this->SetBounds(TRect(X0, Y0, X0 + 25, Y0 + 25));
	this->Text->VertOffset = this->Height;
}

// ---------------------------------------------------------------------------
void __fastcall TFinalShape::DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R) {
	TVisualScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	const int tmpX = R.Width() / 4;
	const int tmpY = R.Height() / 4;

	ACanvas->Brush->Color = clBlack;
	ACanvas->EllipseWithZ(R.Left + tmpX, R.Top + tmpY, R.Right - tmpX, R.Bottom - tmpY, TeeTreeZ);
}

// ---------------------------------------------------------------------------
TColor __fastcall TFinalShape::GetNormalColor(void) {
	return clWhite;
}

// ---------------------------------------------------------------------------
TTreeShapeStyle __fastcall TFinalShape::GetNormalStyle(void) {
	return TTreeShapeStyle::tssCircle;
}

// ---------------------------------------------------------------------------
void __fastcall TFinalShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *ASourceShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (ASourceShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(ASourceShape);
			// ********************************************* //

			// если мы конвертируемся из других фигур
			if (!ASourceShape->InheritsFrom(this->ClassType())) {

				this->Text->Visible = true;

				// сбрасываем, если раньше указывал Initial
				if (this->IsInitial) {
					this->VisualParent->Initial = L"";
				}

				this->SetDefaultSize();

				this->MoveToOtherShapeCenter(ASourceShape);
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ------------------------------ THistoryShape --------------------------------
// ---------------------------------------------------------------------------

__fastcall THistoryShape::THistoryShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner),
FHistoryType(THistoryStateType::shallow) {

	FConvertibleTypes.insert(sctState);
	FConvertibleTypes.insert(sctInitial);
	FConvertibleTypes.insert(sctParallel);
	FConvertibleTypes.insert(sctFinal);

	this->Style = TTreeShapeStyle::tssCircle;
	this->AutoSize = false;

	this->FStateAttrMap["Id"] = false;
	this->FStateAttrMap["Type"] = false;

	this->SetBounds(TRect(X0, Y0, X0 + 25, Y0 + 25));
}

// ---------------------------------------------------------------------------
void __fastcall THistoryShape::SetDefaultSize() {
	this->SetBounds(TRect(X0, Y0, X0 + 25, Y0 + 25));
	this->Text->VertOffset = this->Height;
}

// ---------------------------------------------------------------------------
bool __fastcall THistoryShape::IsCluster(void) {
	return false;
}

// ---------------------------------------------------------------------------
TColor __fastcall THistoryShape::GetNormalColor(void) {
	return clWhite;
}

// ---------------------------------------------------------------------------
TTreeShapeStyle __fastcall THistoryShape::GetNormalStyle(void) {
	return TTreeShapeStyle::tssCircle;
}

// ---------------------------------------------------------------------------
void __fastcall THistoryShape::SetHistoryType(THistoryStateType val) {
	FHistoryType = val;

	// иначе будет access violation
	if (!this->ComponentState.Contains(csReading) && !this->ComponentState.Contains(csLoading)) {
		this->Draw();
	}

}

// ---------------------------------------------------------------------------
void __fastcall THistoryShape::DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R) {
	TVisualScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
	ACanvas->Font->Style = TFontStyles();
	ACanvas->Font->Size = RoundTo((double)R.Height() / 2.0f, 0);
	ACanvas->Font->Color = this->Font->Color;
	ACanvas->Font->Name = L"Tahoma";
	ACanvas->TextAlign = TA_CENTER;
	const UnicodeString sText = FHistoryType == THistoryStateType::shallow ? L"H" : L"H*";

	const int iHeight = ACanvas->TextHeight(sText);

	/* При размере 0,0,16,16 фактически рисует 0,0,17,17, поэтому текст не совсем правильно позиционируется */
	const int iLeft = R.Left + R.Width() / 2 + 1;
	const int iTop = R.Top + R.Height() / 2 - iHeight / 2;

	ACanvas->TextOut3D(iLeft, iTop, TeeTreeZ, sText);
}

// ---------------------------------------------------------------------------
void __fastcall THistoryShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *ASourceShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (ASourceShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(ASourceShape);
			// ********************************************* //

			if (THistoryShape * AHistoryShape = dynamic_cast<THistoryShape*>(Source)) {
				FHistoryType = AHistoryShape->FHistoryType;
			}

			// если мы конвертируемся из других фигур
			if (!ASourceShape->InheritsFrom(this->ClassType())) {

				this->Text->Visible = true;

				// сбрасываем, если раньше указывал Initial
				if (this->IsInitial) {
					this->VisualParent->Initial = L"";
				}

				this->SetDefaultSize();

				this->MoveToOtherShapeCenter(ASourceShape);
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall THistoryShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TVisualScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Type") {
		return //
		"Determines whether the active atomic substate(s) of the current state or only its immediate active substate(s) are recorded.\n" //
		"<B>shallow</B> - the SCXML processor must record the immediately active children of its parent before taking any transition that exits the parent.\n"
		//
		"<B>deep</B> - the SCXML processor must record the active atomic descendants of the parent before taking any transition that exits the parent.\n"
		//
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
// ------------------------------ TInitialShape ------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
__fastcall TInitialShape::TInitialShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner) {

	FConvertibleTypes.insert(sctState);
	FConvertibleTypes.insert(sctFinal);
	FConvertibleTypes.insert(sctParallel);
	FConvertibleTypes.insert(sctHistory);

	this->AutoSize = false;
	this->Text->Visible = false;
	this->Style = TTreeShapeStyle::tssCircle;
	this->Color = clBlack;

	this->SetBounds(TRect(X0, Y0, X0 + 25, Y0 + 25));
}

// ---------------------------------------------------------------------------
__fastcall TInitialShape::~TInitialShape() {

}

// ---------------------------------------------------------------------------
bool __fastcall TInitialShape::IsCluster(void) {
	return false;
}

// ---------------------------------------------------------------------------
TColor __fastcall TInitialShape::GetNormalColor(void) {
	return clBlack;
}

// ---------------------------------------------------------------------------
TTreeShapeStyle __fastcall TInitialShape::GetNormalStyle(void) {
	return TTreeShapeStyle::tssCircle;
}

// ---------------------------------------------------------------------------
void __fastcall TInitialShape::SetDefaultSize(void) {
	this->SetBounds(TRect(X0, Y0, X0 + 25, Y0 + 25));
}

// ---------------------------------------------------------------------------
void __fastcall TInitialShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *ASourceShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (ASourceShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(ASourceShape);
			// ********************************************* //

			// если мы конвертируемся из других фигур
			if (!ASourceShape->InheritsFrom(this->ClassType())) {

				this->Text->Visible = false;

				// сбрасываем, если раньше указывал Initial
				if (this->IsInitial) {
					this->VisualParent->Initial = L"";
				}

				this->SetDefaultSize();

				this->MoveToOtherShapeCenter(ASourceShape);
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
// ---------------------------- TVirtualShape -------------------------------
// ---------------------------------------------------------------------------
__fastcall TVirtualShape::TVirtualShape(Classes::TComponent* AOwner) : TVisualScxmlBaseShape(AOwner) {

	FAvailableTypes.insert(sctDatamodel);
	FAvailableTypes.insert(sctOnentry);
	FAvailableTypes.insert(sctOnexit);
	FAvailableTypes.insert(sctInvoke);

	FConvertibleTypes.insert(sctParallel);
	FConvertibleTypes.insert(sctInitial);
	FConvertibleTypes.insert(sctFinal);
	FConvertibleTypes.insert(sctHistory);
	FConvertibleTypes.insert(sctState);

	FAlias = L"";
	FAliasParams = L"";
}

// ---------------------------------------------------------------------------
__fastcall TVirtualShape::~TVirtualShape() {

}

// ---------------------------------------------------------------------------
void __fastcall TVirtualShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	const UnicodeString sPropName = APropInfo->Name();
	if (sPropName == L"Src") {
		AEditorClass = __classid(TVirtualSrcDialogPropEditor);
		return;
	}
	else if (sPropName == L"AliasParams") {
		AEditorClass = __classid(TStringTextPairsEditor);
		return;
	}

	/* inherited */
	TVisualScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
bool __fastcall TVirtualShape::OnFilterPropEvent(const UnicodeString & sPropName) {
	if (sPropName == L"AliasVar" || sPropName == L"AliasParams") {
		return !this->Alias.IsEmpty();
	}

	return TVisualScxmlBaseShape::OnFilterPropEvent(sPropName);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVirtualShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TVisualScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"AliasVar") {
		return //
		L"Alias varaible that will be replaced by <b>Alias</b> value\n" //
		"Can be changed in Application Settings: <b>VirtualAliasVariable</b>\n"//
		;
	}

	if (sPropName == L"Alias") {
		return //
		L"Alias value that will be used for replacement of <b>AliasVar</b>\n" //
		"<b>For example:<b>\n"//
		"    State ID = State%1\n"//
		"    AliasVar = %1\n"//
		"    Alias = Test\n"//
		"<b>Result:</b> StateTest\n"//
		;
	}

	if (sPropName == L"AliasParams") {
		return //
		L"Format: <b>name</b>=<b>value</b>\n" //
		"Param name will be replaced by value in virtual scxml\n"//
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVirtualShape::GetAliasedText(UnicodeString sText) {
	return StringReplace(sText, this->AliasVar, this->Alias, TReplaceFlags() << rfReplaceAll);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVirtualShape::GetAliasedParamsText(UnicodeString sText) {
	std::auto_ptr<TStringList>AParamsListPtr(new TStringList());
	AParamsListPtr->Text = this->AliasParams.Trim();
	for (int i = 0; i < AParamsListPtr->Count; i++) {
		sText = StringReplace(sText, AParamsListPtr->Names[i], AParamsListPtr->ValueFromIndex[i], TReplaceFlags() << rfReplaceAll);
	}
	return sText;
}

// ---------------------------------------------------------------------------
TColor __fastcall TVirtualShape::GetHeadColor(void) {
	return SettingsData->ThemeSettings->VirtualHeadColor;
}

// ---------------------------------------------------------------------------
TColor __fastcall TVirtualShape::GetNormalColor(void) {
	return SettingsData->ThemeSettings->VirtualNormalColor;
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualShape::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TVisualScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(Source);
			if (AVirtualShape) {
				FSrc = AVirtualShape->FSrc;

				FAlias = AVirtualShape->FAlias;
				FAliasParams = AVirtualShape->FAliasParams;

				UpdateView();
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVirtualShape::GetFullPath(void) {
	if (this->Src.IsEmpty())
		return L"";

	TStateMachineEditorUnit *AEditorUnit = this->StateMachineEditorUnit;
	if (!AEditorUnit)
		return this->Src;

	return TPath::Combine(ExtractFilePath(AEditorUnit->FilePath), this->Src);
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit * __fastcall TVirtualShape::GetSrcUnit(void) {
	TStateMachineEditorUnit *AEditorUnit = this->StateMachineEditorUnit;
	if (AEditorUnit && AEditorUnit->StateMachineProject) {

		if (!FileName.IsEmpty()) {
			return FindUnitByFileName(AEditorUnit->StateMachineProject, FileName);
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVirtualShape::GetAliasVar(void) {
	return SettingsData->VirtualAliasVariable;
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit * __fastcall TVirtualShape::NewVirtualUnit(void) {
	TStateMachineEditorUnit *ASrcUnit = this->SrcUnit;
	if (!ASrcUnit) {
		TStateMachineEditorUnit *ACurrentUnit = this->StateMachineEditorUnit;
		if (ACurrentUnit && ACurrentUnit->ProjMgr) {
			if (this->Src.IsEmpty()) {
				GlobalVirtualUnitLock ALock;
				ASrcUnit = dynamic_cast<TStateMachineEditorUnit*>(ACurrentUnit->ProjMgr->New(ACurrentUnit,
						__classid(TStateMachineEditorUnit)));

				if (ASrcUnit) {
					ASrcUnit->StateMachineUnitType = smutVirtual;
					this->Src = ASrcUnit->FileName;
					GlobalFullExpandProjectView(); // bugfix: нода скрыта при добавлении
					return ASrcUnit;
				}
			}
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
bool __fastcall TVirtualShape::IsSrcValid() {
	if (this->SrcUnit) {
		const UnicodeString sUnitFilePath = this->SrcUnit->FilePath;
		const UnicodeString sFullFilePath = this->FullPath;

		return SameText(sUnitFilePath, sFullFilePath) || SameText(ExpandFileName(sUnitFilePath), ExpandFileName(sFullFilePath));
	}

	const UnicodeString sFullPath = this->FullPath;
	if (!sFullPath.IsEmpty() && FileExists(sFullPath))
		return true;

	return false;
}

// ---------------------------------------------------------------------------
bool __fastcall TVirtualShape::EditSrc(void) {
	bool bIsModified = false;

	TStateMachineEditorUnit *ASrcUnit = this->SrcUnit;
	if (!ASrcUnit) {

		TStateMachineEditorUnit *ACurrentUnit = this->StateMachineEditorUnit;
		if (ACurrentUnit && ACurrentUnit->ProjMgr) {

			bool bProjectModified = false;

			if (this->Src.IsEmpty()) {

				GlobalVirtualUnitLock ALock;
				ASrcUnit = dynamic_cast<TStateMachineEditorUnit*>(ACurrentUnit->ProjMgr->New(ACurrentUnit,
						__classid(TStateMachineEditorUnit)));

				if (ASrcUnit) {
					ASrcUnit->StateMachineUnitType = smutVirtual;
					this->Src = ASrcUnit->FileName;
					bIsModified = true;

					GlobalFullExpandProjectView(); // bugfix: нода скрыта при добавлении
				}

				if (ACurrentUnit->StateMachineProject) {
					ACurrentUnit->StateMachineProject->MarkModified();
				}
			}
			else {
				if (FileExists(this->FullPath)) {
					ASrcUnit = GlobalAddExistingStateMachineUnit(ACurrentUnit, this->FullPath, true, true);
					if (ASrcUnit) {
						ASrcUnit->StateMachineUnitType = smutVirtual;
					}
				}
				else {
					const UnicodeString sText = L"Virtual file ["+this->FullPath + "] is not found!\n" //
					"'Create' - will create a new unit\n" //
					"'Find' - will try to find the unit";

					std::auto_ptr<TDialogFileNotFound>ADialogFileNotFoundPtr(new TDialogFileNotFound(this->Owner));
					ADialogFileNotFoundPtr->LabelText->Caption = sText;
					switch(ADialogFileNotFoundPtr->ShowModal()) {
					case CUSTOM_MODAL_RESULT_CREATE: {
							std::auto_ptr<TStringList>ATempListPtr(new TStringList());
							ATempListPtr->Text = //
							"<scxml name=\"VIRTUAL\" version=\"1.0\" xmlns=\"http://www.w3.org/2005/07/scxml\">" //
							"	<state id=" + AnsiQuotedStr(this->SimpleText, L'"') + "/>" //
							"</scxml>";
							ATempListPtr->SaveToFile(this->FullPath, TEncoding::UTF8);

							ASrcUnit = GlobalAddExistingStateMachineUnit(ACurrentUnit, this->FullPath, true, true);
							if (ASrcUnit) {
								ASrcUnit->StateMachineUnitType = smutVirtual;
							}
						}break;
					case CUSTOM_MODAL_RESULT_FIND: {
							UnicodeString sFileName = this->FullPath;
							if (PromptForFileName(sFileName, L"SCXML files(*.scxml)|*.scxml", L"scxml", L"Find virtual unit",
									ExtractFileDir(this->FullPath), false)) {
								ASrcUnit = GlobalAddExistingStateMachineUnit(ACurrentUnit, sFileName, true, true);
								if (ASrcUnit) {
									ASrcUnit->StateMachineUnitType = smutVirtual;
									this->Src = ExtractRelativePath(ExtractFilePath(ACurrentUnit->FilePath), sFileName);
									bIsModified = true;
								}
							}
						}break;
					}

				}
			}
		}
	}

	if (ASrcUnit) {

		ASrcUnit->Open(true);

		TStateMachineDockPanel *ADockPanel = ASrcUnit->StateMachineDockPanel;
		if (ADockPanel) {
			TScxmlShape *AShape = ADockPanel->StateMachineEditor->RootScxml;
			if (AShape) {
				TVisualScxmlBaseShape *AVisualRoot = AShape->FirstVisualChild;
				if (!AVisualRoot) {
					TStateShape *ARootShape = dynamic_cast<TStateShape*>(AShape->AddStateChild(sctState, true));
					ARootShape->Id = this->SimpleText;
					ARootShape->Y0 = ARootShape->Y0 + 50;
					ARootShape->X1 = ARootShape->X0 + 400;
					ARootShape->Y1 = ARootShape->Y0 + 200;
					ADockPanel->StateMachineEditor->MarkModified(1, "Virtual Root:" + ARootShape->SimpleText, true);
				}
				else {

					const UnicodeString sVisRootID = this->Alias.IsEmpty() ? AVisualRoot->SimpleText : this->GetAliasedText
						(AVisualRoot->SimpleText);

					if (sVisRootID != this->SimpleText) {
						WLOG_ERROR(L"VIRTUAL> Submachine root ID:[%s] mismatches to Parent:[%s]", sVisRootID.c_str(),
							this->SimpleText.c_str());
					}
				}
			}
		}

		// перерисовываем, чтобы обновить статус, если было в состоянии ошибки
		this->Draw();
	}
	return bIsModified;
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualShape::ValidateShape() {
	TScxmlBaseShape::ValidateShape();

	if (this->BaseParent) {
		TStateErrorMap::iterator it = FErrorMap.find(SCXML_ERROR_SRC_NOT_FOUND);
		if (IsSrcValid()) {
			if (it != FErrorMap.end()) {
				FErrorMap.erase(it);
			}
		}
		else {
			const UnicodeString sMsg = UnicodeString().sprintf(L"E%d:%s> text:[%s] name:[%s] src:[%s]", SCXML_ERROR_SRC_NOT_FOUND,
				GetErrorDescription(SCXML_ERROR_SRC_NOT_FOUND).c_str(), this->SimpleText.c_str(), this->Name.c_str(), this->Src.c_str());

			if (it == FErrorMap.end()) {
				WLOG_WARNING(sMsg.c_str());
			}

			FErrorMap[SCXML_ERROR_SRC_NOT_FOUND] = sMsg;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVirtualShape::DrawShapeCanvas(Tecanvas::TCanvas3D * ACanvas, const Types::TRect & R) {
	TVisualScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	ACanvas->BackMode = cbmTransparent;
	ACanvas->TextAlign = TA_RIGHT;
	ACanvas->Font->Color = clMaroon;
	ACanvas->Font->Style = TFontStyles();
	ACanvas->TextOut3D(this->X1 - 10, this->Y0 + 3, TeeTreeZ, !this->Src.IsEmpty() ? this->Src : UnicodeString("(NONE) ?"));
}
// ---------------------------------------------------------------------------

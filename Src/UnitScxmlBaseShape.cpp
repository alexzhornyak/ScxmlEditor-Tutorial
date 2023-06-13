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

#include "UnitScxmlBaseShape.h"

#include <Character.hpp>

#include "UnitScxmlShape.h"
#include "UnitChildScxmlBaseShapes.h"
#include "UnitStateShape.h"
#include "UnitDataModelStateShape.h"
#include "UnitExecutableShapes.h"
#include "UnitExternalCommunicationShapes.h"
#include "UnitStringEditorEx.h"
#include "UnitStateMachineParser.h"
#include "UnitSyntaxEditorForm.h"
#include "TreeNodeListAccess.h"
#include "UnitTreeEx.h"
#include "UnitStateMachineUnit.h"
#include "UnitTreeEditorUtils.h"
#include "TreeEditorEx.h"
#include "UnitPropInspectorExtensions.h"
#include "UnitSettings.h"
#include "UnitStateMachineConnection.h"
#include "UnitTriggerShape.h"
#include "UnitWatchShape.h"
#include "UnitCustomUtils.h"
#include "UnitSVGExporter.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

void RegisterScxmlShapes() {
#pragma startup RegisterScxmlShapes

	Classes::RegisterClassA(__classid(TScxmlShape));
	Classes::RegisterClassA(__classid(TStateShape));
	Classes::RegisterClassA(__classid(TParallelShape));
	Classes::RegisterClassA(__classid(TFinalShape));
	Classes::RegisterClassA(__classid(THistoryShape));
	Classes::RegisterClassA(__classid(TInitialShape));
	Classes::RegisterClassA(__classid(TVirtualShape));
	Classes::RegisterClassA(__classid(TTriggerShape));
	Classes::RegisterClassA(__classid(TWatchShape));
	Classes::RegisterClassA(__classid(TTransitionXMLShape));

	RegisterCustomTreeShape("SCXML", "Scxml", __classid(TScxmlShape));
	RegisterCustomTreeShape("SCXML", "State", __classid(TStateShape));
	RegisterCustomTreeShape("SCXML", "Initial", __classid(TInitialShape));
	RegisterCustomTreeShape("SCXML", "Final", __classid(TFinalShape));
	RegisterCustomTreeShape("SCXML", "Parallel", __classid(TParallelShape));
	RegisterCustomTreeShape("SCXML", "History", __classid(THistoryShape));
	RegisterCustomTreeShape("SCXML", "Virtual", __classid(TVirtualShape));
}

// ---------------------------------------------------------------------------
class TEditByDefaultPropEditor : public TLMDStringPropEditor {
protected:
	virtual Lmdinspropinsp::TLMDPropAttrs __fastcall GetAttrs(void) {
		return TLMDStringPropEditor::GetAttrs() << praValueList;
	}

	virtual void __fastcall GetValues(Classes::TStrings* AValues) {
		TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(this->GetInstance(0));
		if (AChildScxmlBaseShape) {
			AChildScxmlBaseShape->SetDefaultEditableProps(AValues);
		}
	}
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
int GetMaxBrotherRight(TVisualScxmlBaseShape *AVisualParentShape) {
	int iX = 0;
	for (int i = 0; i < AVisualParentShape->Children->Count; i++) {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AVisualParentShape->Children->Items[i]);
		if (AVisualShape) {
			if (AVisualShape->X1 > iX) {
				iX = AVisualShape->X1;
			}
		}
	}
	return iX;
}

// ---------------------------------------------------------------------------
int GetMaxBrotherBottom(TVisualScxmlBaseShape *AVisualParentShape, TVisualScxmlBaseShape *ACurrentShape) {
	int iY = 0;
	for (int i = 0; i < AVisualParentShape->Children->Count; i++) {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AVisualParentShape->Children->Items[i]);
		// чтобы не смещать на собственную высоту
		if (AVisualShape && (ACurrentShape != AVisualShape)) {
			WLOG_DEBUG(L"Current child is <%s>", AVisualShape->SimpleText.c_str());
			if (AVisualShape->Y1 > iY) {
				iY = AVisualShape->Y1;
			}
		}
	}
	return iY;
}

// ---------------------------------------------------------------------------
bool IsOverlappedWithSomebody(TVisualScxmlBaseShape *AVisualParentShape, TVisualScxmlBaseShape *ACurrentShape) {
	bool bOverlapped = false;
	for (int i = 0; i < AVisualParentShape->Children->Count; i++) {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AVisualParentShape->Children->Items[i]);
		// чтобы не сравнивать с собой
		if (AVisualShape && (ACurrentShape != AVisualShape)) {
			TRect AIntersect;

			//			WLOG_DEBUG(L"Comparing overlapping between [%s] and [%d][%s]", ACurrentShape->SimpleText.c_str(), i,
			//				AVisualShape->SimpleText.c_str());

			bOverlapped = IntersectRect(AIntersect, TRect(AVisualShape->X0, AVisualShape->Y0, AVisualShape->X1, AVisualShape->Y1),
				TRect(ACurrentShape->X0, ACurrentShape->Y0, ACurrentShape->X1, ACurrentShape->Y1));

			if (bOverlapped) {
				WLOG_DEBUG(L"Intersect %s=[%d,%d,%d,%d] with %s=[%d,%d,%d,%d]", AVisualShape->SimpleText.c_str(),
					AVisualShape->Bounds().left, AVisualShape->Bounds().top, AVisualShape->Bounds().right, AVisualShape->Bounds().bottom,
					ACurrentShape->SimpleText.c_str(), ACurrentShape->Bounds().left, ACurrentShape->Bounds().top,
					ACurrentShape->Bounds().right, ACurrentShape->Bounds().bottom);
				break;
			}
		}
	}
	return bOverlapped;
}

// ---------------------------------------------------------------------------
void CheckVisualScxmlShapePos(TCustomTree *ATree, TVisualScxmlBaseShape *AVisualShape, int iVisualIndex) {
	if (AVisualShape) {
		TVisualScxmlBaseShape *AVisualShapeParent = dynamic_cast<TVisualScxmlBaseShape*>(AVisualShape->Parent);
		if (AVisualShapeParent) {
			if (AVisualShape->X0 <= AVisualShapeParent->X0) {
				AVisualShape->X0 = AVisualShapeParent->X0 + 30;

				WLOG_DEBUG(L"Moved %s on X axis %d,%d,%d,%d", AVisualShape->SimpleText.c_str(), AVisualShape->Bounds().left,
					AVisualShape->Bounds().top, AVisualShape->Bounds().right, AVisualShape->Bounds().bottom);
			}
			if (AVisualShape->Y0 <= AVisualShapeParent->Y0) {
				AVisualShape->Y0 = AVisualShapeParent->Y0 + 30;

				WLOG_DEBUG(L"Moved %s on Y axis %d,%d,%d,%d", AVisualShape->SimpleText.c_str(), AVisualShape->Bounds().left,
					AVisualShape->Bounds().top, AVisualShape->Bounds().right, AVisualShape->Bounds().bottom);
			}

			if (iVisualIndex > 0 && IsOverlappedWithSomebody(AVisualShapeParent, AVisualShape)) {
				int iY1 = GetMaxBrotherBottom(AVisualShapeParent, AVisualShape);
				if (AVisualShape->Y0 <= iY1) {
					AVisualShape->Y0 = iY1 == 0 ? AVisualShape->Y0 : iY1 + 50;
				}
			}
		}

		// итерация дочерних
		int iIndex = 0;
		for (int i = 0; i < AVisualShape->Children->Count; i++) {
			TVisualScxmlBaseShape *AVisualChild = dynamic_cast<TVisualScxmlBaseShape*>(AVisualShape->Children->Items[i]);
			if (AVisualChild) {
				CheckVisualScxmlShapePos(ATree, AVisualChild, iIndex);
				iIndex++;
			}
		}

		if (AVisualShapeParent) {
			if (AVisualShapeParent->Y1 < AVisualShape->Y1) {
				AVisualShapeParent->Y1 = AVisualShape->Y1 + 10;
			}

			if (AVisualShapeParent->X1 < AVisualShape->X1) {
				AVisualShapeParent->X1 = AVisualShape->X1 + 10;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void CheckVisualScxmlStateBounds(TCustomTree *ATree) {
	if (!ATree)
		throw Exception(__FUNCTION__ "> ATree==NULL");
	TTreeNodeShape *AShape = GetSafeTreeListFirst(ATree->Items);
	if (AShape) {
		CheckVisualScxmlShapePos(ATree, dynamic_cast<TVisualScxmlBaseShape*>(AShape), 0);
	}
}

// ---------------------------------------------------------------------------
// TScxmlBaseShape
// ---------------------------------------------------------------------------

__fastcall TScxmlBaseShape::TScxmlBaseShape(Classes::TComponent* AOwner) : TTreeNodeShape(AOwner),
FXMLText(new TSyntaxString(this, L"XMLText")), FExtraParams(L""),
FExtraContent(new TSyntaxString(this, L"ExtraContent", true, L"XML", xttXML)),
FComments(new TSyntaxString(this, L"Comments", true, L"Text")), FGuid(new GUID) {

	memset(FGuid, 0, sizeof(GUID));

	FLocked = false;
	FInheritanceMismatch = false;
	FInheritanceResolver = new TInheritanceResolver(&this->GetStateMachineEditorUnit);

	FAvailableTypes.insert(sctComment);
	FAvailableTypes.insert(sctExtraContent);

	FXMLText->OnTextChanged = OnXMLTextChanged;

	this->Shadow->Visible = false;
	FExcludeFromSave = false;

	FGuidStr = L"";
	if (CoCreateGuid(FGuid) != S_OK) {
		LOG_ERROR_SS << "Can not create GUID! Some operations may not work!";
	}
	else {
		FGuidStr = Sysutils::GUIDToString(*FGuid);
	}
}

// ---------------------------------------------------------------------------
__fastcall TScxmlBaseShape::~TScxmlBaseShape() {
	delete FComments;
	delete FExtraContent;
	delete FXMLText;
	delete FGuid;
	delete FInheritanceResolver;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::DrawShapeCanvas(Tecanvas::TCanvas3D * ACanvas, const Types::TRect & R) {
	/* INHERITANCE */
	if (this->Locked && !this->InheritanceMismatch && StateMachineEditorUnit && StateMachineEditorUnit->IsInherited && StateMachineEditor) {
		StateMachineEditor->EnqueeScxmlShapeForInheritanceCheck(this);
	}

	/* base class inherited method */
	TTreeNodeShape::DrawShapeCanvas(ACanvas, R);

	/* BOOKMARKS */
	if (this->Tree) {
		TTreeEditorEx * ATreeEditorEx = dynamic_cast<TTreeEditorEx*>(this->Tree->Owner);
		if (ATreeEditorEx) {
			for (int i = 0; i < ATreeEditorEx->PanelBookmarks->ControlCount; i++) {
				TSpeedButton * ASpeedButton = dynamic_cast<TSpeedButton*>(ATreeEditorEx->PanelBookmarks->Controls[i]);
				if (ASpeedButton && ASpeedButton->Tag == reinterpret_cast<int>(this->Guid)) {
					ACanvas->TextAlign = 0;
					ACanvas->BackMode = cbmTransparent;
					ACanvas->Font->Color = clGreen;
					ACanvas->Font->Style = ACanvas->Font->Style << fsBold;
					const UnicodeString sBookmarkText = "[" + UnicodeString(ASpeedButton->HelpContext) + "]";
					const int iTextWidth = ACanvas->TextWidth(sBookmarkText);

					int iOffsetX = iTextWidth;
					int iOffsetY = 10;
					if (this->InheritsFrom(__classid(TChildScxmlBaseShape))) {
						iOffsetX = iOffsetX + 42;
						iOffsetY = 0;
					}

					ACanvas->TextOut3D(R.Left - iOffsetX, R.Top - iOffsetY, TeeTreeZ, sBookmarkText);
				}
			}
		}
	}

	/* Locked view */
	if (this->Locked || this->InheritanceMismatch) {
		ACanvas->Pen->Color = clBlack;
		ACanvas->Pen->Width = 1;
		ACanvas->Brush->Style = bsSolid;
		ACanvas->Brush->Color = InheritanceMismatch ? (Locked ? clRed : clFuchsia) :
			(InheritanceResolver->ResolvedProps->Count ? TColor(RGB(255, 150, 50)) : clYellow);
		TRect AEllipseRect = R;
		AEllipseRect.Left -= 10;
		AEllipseRect.Right = AEllipseRect.Left + 9;
		AEllipseRect.Bottom = AEllipseRect.Top + 9;
		ACanvas->EllipseWithZ(AEllipseRect, TeeTreeZ);
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::GetHasSelfConnections(void) {
	for (int i = 0; i < this->Connections->Count; i++) {
		if (this->Connections->Items[i]->FromShape == this && this->Connections->Items[i]->ToShape == this) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::GetHasNonVisualChildren(void) {
	for (int i = 0; i < this->Children->Count; i++) {
		if (!this->Children->Items[i]->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::DeleteAllToConnections(void) {

	/* bugfix: родной метод DeleteAllTo вызывает AccessViolation в определенных случаях
	насколько я понимаю, возникают перекрестные удаления, связанные с понятием 'Parents'
	 */

	std::set<TTreeConnection*>AToConnections;

	if (_GetFParents()) {
		for (int k = 0; k < this->_GetFParents()->Count; k++) {
			TTreeNodeShape *AFromConnectionShape = this->_GetFParents()->Items[k];

			for (int n = 0; n < AFromConnectionShape->_GetFConnections()->Count; n++) {
				TTreeConnection *AConnection = AFromConnectionShape->_GetFConnections()->Items[n];

				if (AConnection->ToShape == this) {
					AToConnections.insert(AConnection);
				}
			}
		}
	}
	else if (this->_GetIParents0() && this->_GetIParents0()->_GetFConnections()) {
		for (int n = 0; n < this->_GetIParents0()->_GetFConnections()->Count; n++) {
			TTreeConnection *AConnection = this->_GetIParents0()->_GetFConnections()->Items[n];

			if (AConnection->ToShape == this) {
				AToConnections.insert(AConnection);
			}
		}
	}

	for (std::set<TTreeConnection*>::iterator it = AToConnections.begin(); it != AToConnections.end(); ++it) {
		delete(*it);
	}
}

// ---------------------------------------------------------------------------
bool AtHandle(TTreeNodeShape *AShape, int x, int y, int Ax, int Ay, int tmpSize) {
	return(abs(x - Ax) <= tmpSize) && (abs(y - Ay) <= tmpSize);
}

// ---------------------------------------------------------------------------
TTreeShapeHandle __fastcall TScxmlBaseShape::GetResizingHandle(int x, int y) {
	TTreeShapeHandle AShapeHandle = rcNone;
	TRect ABounds = Bounds();
	int tmpSize = Tree->Selected->HandleSize;
	// bugfix: при маленьких зумах невозможно попасть в Handle, поэтому увеличиваем область в зависимости от зума
	if (Tree->View3DOptions->Zoom < 100) {
		tmpSize = tmpSize * 100 / ((Tree->View3DOptions->Zoom > 0) ? Tree->View3DOptions->Zoom : 1);
	}

	if (AtHandle(this, x, y, ABounds.Left, ABounds.Top, tmpSize))
		AShapeHandle = rcLeftTop;
	else if (AtHandle(this, x, y, ABounds.Right, ABounds.Bottom, tmpSize))
		AShapeHandle = rcRightBottom;
	else if (AtHandle(this, x, y, ABounds.Right, ABounds.Top, tmpSize))
		AShapeHandle = rcRightTop;
	else if (AtHandle(this, x, y, ABounds.Left, ABounds.Bottom, tmpSize))
		AShapeHandle = rcLeftBottom;
	else {
		int MidX = 1 + ((ABounds.Left + ABounds.Right) / 2);
		int MidY = 1 + ((ABounds.Top + ABounds.Bottom) / 2);

		if (AtHandle(this, x, y, ABounds.Left, MidY, tmpSize))
			AShapeHandle = rcLeft;
		else if (AtHandle(this, x, y, ABounds.Right, MidY, tmpSize))
			AShapeHandle = rcRight;
		else if (AtHandle(this, x, y, MidX, ABounds.Top, tmpSize))
			AShapeHandle = rcTop;
		else if (AtHandle(this, x, y, MidX, ABounds.Bottom, tmpSize))
			AShapeHandle = rcBottom;
	}

	// call DoDrawHandle, if it returns "false" in tmpOk,
	// then it means the handle is not visible...
	bool tmpOk = True;
	DoDrawHandle(AShapeHandle, x, y, tmpOk);
	if (!tmpOk)
		AShapeHandle = rcNone;
	return AShapeHandle;
}

// ---------------------------------------------------------------------------
TStateMachineEditorUnit *__fastcall TScxmlBaseShape::GetStateMachineEditorUnit(void) {
	if (this->Tree) {
		TStateMachineEditor * AStateMachineEditor = StateMachineEditor;
		if (AStateMachineEditor) {
			return AStateMachineEditor->StateMachineEditorUnit;
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
TStateMachineEditor * __fastcall TScxmlBaseShape::GetStateMachineEditor(void) {
	if (this->Tree) {
		return dynamic_cast<TStateMachineEditor*>(this->Tree->Owner);
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
	const UnicodeString sPropName = APropInfo->Name();
	if (APropInfo && !IsPublishedProp(__classid(TTreeNodeShape), sPropName)) {

		const AnsiString sTypeName(APropInfo->PropType()->Name);
		const Typinfo::TTypeKind ATypeKind = APropInfo->PropType()->Kind;
		switch(ATypeKind) {
		case tkUString: {
				if (sPropName == L"ExtraParams") {
					AEditorClass = __classid(TStringTextPairsEditor);
				}

			}break;
		case tkClass: {
				if (sTypeName == "TSyntaxString") {
					AEditorClass = __classid(TSyntaxStringClassEditor);
				}
				else if (sPropName == L"InheritanceResolver") {
					AEditorClass = __classid(TInheritanceResolverPropEditor);
				}
			}break;
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::OnFilterPropEvent(const UnicodeString & sPropName) {
	std::auto_ptr<TStringList>ABlackList(new TStringList());
	ABlackList->CaseSensitive = true;
	std::auto_ptr<TStringList>AWhiteList(new TStringList());
	AWhiteList->CaseSensitive = true;

	// черный список
	if (!SettingsData->AviaExtensionEnabled) {
		ABlackList->Add("ProtocolControlBinding");
	}
	if (!StateMachineEditorUnit || !StateMachineEditorUnit->IsInherited) {
		ABlackList->Add("InheritanceMismatch");
		ABlackList->Add("InheritanceResolver");
	}

	// белый список
	AWhiteList->Add("Visible");

	return ABlackList->IndexOf(sPropName) == -1 && //
	(AWhiteList->IndexOf(sPropName) != -1 || !IsPublishedProp(__classid(TTreeNodeShape), sPropName));
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor) {
	// специальный случай
	if (AItem->PropName == L"InheritanceResolver") {
		AColor = clTeal;
		return;
	}

	if (AItem->TypeInfo->Kind != tkClass && !AItem->Parent && AItem->PropInfo[0]->IsWritable()) {

		if (!IsPublishedProp(__classid(TTreeNodeShape), AItem->PropName) && IsPublishedProp(__classid(TScxmlBaseShape), AItem->PropName)) {
			AColor = clTeal;
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	if (sPropName == L"ChildIndex") {
		return //
		L"<B>ChildIndex</B> - indicates the position in the tree of elements<BR>" //
		"enumeration starts from <B>0</B>!!!";
	}

	if (sPropName == L"ExtraParams") {
		return //
		L"<B>ExtraParams</B> - gives an opportunity to user to add custom attributes\n" //
		"<B>For example:</B>\n" //
		"<I>&lt;state id='Test' <B>myAttr='myValue'</B>/&gt;</I>";
	}

	if (sPropName == L"ExcludeFromSave") {
		return L"The element will not be saved to <B>the resulting scxml file</B>\n" //
		"P.S. You can exclude the element by <b>ExcludeConditions</b> also (see <b>ExcludeConditions</b> for more details)" //
		;
	}

	if (sPropName == L"X0") {
		return L"<B>Left</B> position of the element in pixels" //
		;
	}

	if (sPropName == L"Y0") {
		return L"<B>Top</B> position of the element in pixels" //
		;
	}

	if (sPropName == L"X1") {
		return L"<B>Right</B> position of the element in pixels" //
		;
	}

	if (sPropName == L"Y1") {
		return L"<B>Bottom</B> position of the element in pixels" //
		;
	}

	if (sPropName == L"Visible") {
		return L"<b>Displays or hides</b> the element\n" //
		"P.S.If you want to find hidden shape use <b>Nodes tree</b>" //
		;
	}

	if (sPropName == L"SelfConnectionTextOffsets") {
		return L"Text offsets for self-connections\n" //
		"<B>Left</B> - vertical text offset for self-connections over left edge of the shape\n" //
		"<B>Right</B> - vertical text offset for self-connections over right edge of the shape\n" //
		"<B>Top</B> - vertical text offset for self-connections over top edge of the shape\n" //
		"<B>Bottom</B> - vertical text offset for self-connections over bottom edge of the shape\n" //
		;
	}

	if (sPropName == L"XMLText") {
		return //
		"Represents <b>child content</b> of an element\n" //
		"The usage of XMLText depends on its Type:\n" //
		"1) <b>xttText</b> - simple XML text\n" //
		"2) <b>xttCDATA</b> - XML text in CDATA representation\n" //
		"3) <b>xttXML</b> - nested XML nodes\n" //
		"4) <b>xttSCXML</b> - SCXML stream\n" //
		;
	}

	if (sPropName == L"ExcludeConditions") {
		return L"<b>Semicolon-separated list of project defines</b>, which lead to exclude this element from save\n" //
		"you can use <b>'~'</b> to invert result\n" //
		"<b>For example:</b>\n" //
		"<B>ProjectDefines</B>==<b>DEBUG;...</b>\n" //
		"if <b>ExcludeConditions</b>==<b>~DEBUG;...</b> then <b>ExcludeFromSave</b>==<b>False</b>\n" //
		"if <b>ExcludeConditions</b>==<b>DEBUG;...</b> then <b>ExcludeFromSave</b>==<b>True</b>\n" //
		;
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

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::Assign(Classes::TPersistent * Source) {
	if (Source) {
		TTreeNodeShape *AShape = dynamic_cast<TTreeNodeShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TTreeNodeShape::Assign(AShape);
			// ********************************************* //

			TScxmlBaseShape * ABaseShape = dynamic_cast<TScxmlBaseShape*>(Source);
			if (ABaseShape) {
				FExtraParams = ABaseShape->FExtraParams;

				FXMLText->Assign(ABaseShape->FXMLText);
				FExtraContent->Assign(ABaseShape->FExtraContent);
				FComments->Assign(ABaseShape->FComments);
				FExcludeConditions = ABaseShape->FExcludeConditions;
				FExcludeFromSave = ABaseShape->FExcludeFromSave;
				Locked = ABaseShape->Locked;
				FInheritanceResolver->Assign(ABaseShape->FInheritanceResolver);

				// обязательно применение с вызовом Set
				InheritanceMismatch = ABaseShape->InheritanceMismatch;
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::EditRawXMLContent(void) {
	bool bRetVal = false;
	try {
		std::auto_ptr<TFormSyntaxEditorForm>ASyntaxForm(new TFormSyntaxEditorForm(Application, "XML",
				Statemachine::InvisibleShapeChildrenToXMLString(this)));
		ASyntaxForm->CheckSwitchToDocPanel->Visible = false;

		if (ASyntaxForm->ShowModal() == mrOk) {
			Statemachine::XMLStringToInvisibleShapeChildren(this, ASyntaxForm->EditDoc1->Lines->Text);
			bRetVal = true;
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"EDIT RAW XML> %s", E->Message.c_str());
	}
	return bRetVal;
}

// ---------------------------------------------------------------------------
TScxmlBaseShape * __fastcall TScxmlBaseShape::AddStateChild(TStateChildType AChildType, bool bCheckBounds) {
	if (FAvailableTypes.find(AChildType) == FAvailableTypes.end())
		return NULL;

	int iTop = this->Top + 30;
	TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(this);
	if (AVisualShape) {
		for (int i = 0; i < AVisualShape->Count(); i++) {
			TVisualScxmlBaseShape *AChildVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AVisualShape->Children->Items[i]);
			if (AChildVisualShape) {
				if (AChildVisualShape->Bounds().Bottom > iTop) {
					iTop = AChildVisualShape->Bounds().Bottom;
				}
			}
		}
	}

	TScxmlBaseShape *AShape = NULL;
	switch(AChildType) {
	case sctScxml:
		AShape = AddScxmlClassShape<TScxmlShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	case sctState:
		AShape = AddScxmlClassShape<TStateShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctParallel:
		AShape = AddScxmlClassShape<TParallelShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctTransition:

		break;
	case sctInitial:
		AShape = AddScxmlClassShape<TInitialShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	case sctFinal:
		AShape = AddScxmlClassShape<TFinalShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctOnentry:
		AShape = AddScxmlClassShape<TOnEntryStateShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctOnexit:
		AShape = AddScxmlClassShape<TOnExitStateShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctHistory:
		AShape = AddScxmlClassShape<THistoryShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctDatamodel:
		AShape = AddScxmlClassShape<TDatamodelStateShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	case sctData:
		AShape = AddScxmlClassShape<TDataChildShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctAssign:
		AShape = AddScxmlClassShape<TAssignShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctDonedata:
		AShape = AddScxmlClassShape<TDonedataShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	case sctContent:
		AShape = AddScxmlClassShape<TContentShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	case sctParam:
		AShape = AddScxmlClassShape<TParamShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctScript:
		AShape = AddScxmlClassShape<TScriptShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctRaise:
		AShape = AddScxmlClassShape<TRaiseShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctIf:
		AShape = AddScxmlClassShape<TIfShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctElseif:
		AShape = AddScxmlClassShape<TElseIfShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctElse:
		AShape = AddScxmlClassShape<TElseShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctForeach:
		AShape = AddScxmlClassShape<TForeachShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctLog:
		AShape = AddScxmlClassShape<TLogShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctSend:
		AShape = AddScxmlClassShape<TSendShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctCancel:
		AShape = AddScxmlClassShape<TCancelShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctInvoke:
		AShape = AddScxmlClassShape<TInvokeShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctFinalize:
		AShape = AddScxmlClassShape<TFinalizeShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	case sctComment:
		AShape = AddScxmlClassShape<TCommentShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctExtraContent:
		AShape = AddScxmlClassShape<TExtraContentShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctProtocolBinding:
		AShape = AddScxmlClassShape<TProtocolBindingShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctWeakProtocolBinding:
		AShape = AddScxmlClassShape<TProtocolWeakBindingShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctSetValue:
		AShape = AddScxmlClassShape<TSetValueShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctSessionState:
		AShape = AddScxmlClassShape<TSessionStateShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctVirtualFolder:
		AShape = AddScxmlClassShape<TVirtualFolderShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctVirtual:
		AShape = AddScxmlClassShape<TVirtualShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctTrigger:
		AShape = AddScxmlClassShape<TTriggerShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctWatch:
		AShape = AddScxmlClassShape<TWatchShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), false);
		break;
	case sctTransitionXML:
		AShape = AddScxmlClassShape<TTransitionXMLShape>(this, TScxmlBaseShape::TypeToXMLNodeName(AChildType), true);
		break;
	}

	// if (AVisualShape) {
	// AVisualShape->UpdateStateChildren();
	// }

	TVisualScxmlBaseShape *AVisualNewShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
	if (AVisualNewShape) {

		AVisualNewShape->SetDefaultSize();

		if (AVisualShape && bCheckBounds) {

			AVisualNewShape->Top = iTop + 10;

			if (AVisualShape->Y1 < AVisualNewShape->Y1) {
				AVisualShape->Y1 = AVisualNewShape->Y1 + 10;
			}

			AVisualNewShape->X0 = AVisualShape->X0 + 30;

			if (AVisualShape->X1 < AVisualNewShape->X1) {
				AVisualShape->X1 = AVisualNewShape->X1 + 10;
			}
		}
	}

	if (AShape) {
		if (AShape->Image) {
			AShape->Image->Repaint();
		}
	}

	return AShape;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::XMLAttrNameToPropName(UnicodeString sAttrName) {
	assert(!sAttrName.IsEmpty());
	sAttrName[1] = ToUpper(sAttrName[1]);
	return sAttrName;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::PropNameToXMLAttrName(UnicodeString sPropName) {
	return sPropName.LowerCase();
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::AttributesToProps(ILMDXmlElement * ANode) {
	assert(ANode != NULL);
	// обработка возможных аттрибутов
	std::auto_ptr<TStringList>AUnsupportedList(new TStringList());
	for (int k = 0; k < ANode->AttrCount; k++) {
		const UnicodeString sPropName = this->XMLAttrNameToPropName(ANode->AttrNames[k]);

		// провереям, чтобы это не было свойством базового класса
		if (!IsPublishedProp(__classid(TTreeNodeShape), sPropName) && IsPublishedProp(this, sPropName)) {
			PPropInfo APropInfo = GetPropInfo(this, sPropName);
			if (APropInfo) {
				if (APropInfo->SetProc) {
					SetPropValue(this, sPropName, ANode->AttrValues[k]);
				}
			}
		}
		else {
			AUnsupportedList->Values[ANode->AttrNames[k]] = ANode->AttrValues[k];
			LOG_WARNING("%s> Can not set property <%s> to <%s>", __FUNCTION__, AnsiString(sPropName).c_str(),
				AnsiString(this->ClassName()).c_str());
		}
	}
	this->ExtraParams = AUnsupportedList->Text;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::PropsToAttributes(ILMDXmlElement * ANode) {
	assert(ANode != NULL);
	for (TStateAttributesMap::iterator it = FStateAttrMap.begin(); it != FStateAttrMap.end(); ++it) {
		const UnicodeString sAttrName = it->first;
		const bool bRequiredOrDefault = it->second;
		assert(IsPublishedProp(this->ClassType(), sAttrName) != NULL);

		const UnicodeString sAttrValue = GetPropValue(this, sAttrName);

		if (bRequiredOrDefault && sAttrValue.IsEmpty())
			throw EScxmlBaseShapeException(this, L"Empty '" + sAttrName + "'",
			L"Required attribute '" + sAttrName + "' can not be empty! Shape id:["+this->SimpleText + "] name:["+this->Name + "]");

		bool bDoSave = true;

		// значения по умолчанию и те, которые не требуются, записывать не будем
		if (!bRequiredOrDefault && !SettingsData->ForceSaveDefaultScxmlValues) {
			PPropInfo APropInfo = GetPropInfo(this, sAttrName);
			if (APropInfo) {
				switch((*APropInfo->PropType)->Kind) {
				case tkInteger:
				case tkEnumeration:
				case tkSet: {
						const int iDefault = APropInfo->Default;
						const int iOrdProp = GetOrdProp(this, sAttrName);
						if (iDefault == iOrdProp) {
							// равно значению по умолчанию
							bDoSave = false;
						}
					}break;
				}
			}
		}

		if (!sAttrValue.IsEmpty() && bDoSave) {
			ANode->SetAttr(PropNameToXMLAttrName(sAttrName), sAttrValue);
		}
	}

	std::auto_ptr<TStringList>AExtraParamsList(new TStringList());
	AExtraParamsList->Text = this->ExtraParams;
	for (int i = 0; i < AExtraParamsList->Count; i++) {
		if (!AExtraParamsList->Names[i].IsEmpty()) {
			ANode->SetAttr(AExtraParamsList->Names[i], AExtraParamsList->ValueFromIndex[i]);
		}
	}
}

// ---------------------------------------------------------------------------
TStateChildType __fastcall TScxmlBaseShape::XMLNodeNameToType(const UnicodeString & sNodeName) {
	for (int i = 0; i < sctMAXSIZE; i++) {
		PPropInfo APropInfo = GetPropInfo(__classid(TScxmlBaseShape), "StateChildType");
		assert(APropInfo != NULL);
		const TStateChildType AType = TStateChildType(i);
		const UnicodeString sEnumName = StringReplace(GetEnumName(*(APropInfo->PropType), AType), L"sct", L"", TReplaceFlags());
		if (sEnumName.UpperCase() == sNodeName.UpperCase()) {
			return AType;
		}
	}
	return sctMAXSIZE;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::TypeToXMLNodeName(const TStateChildType AType) {
	PPropInfo APropInfo = GetPropInfo(__classid(TScxmlBaseShape), "StateChildType");
	if (!APropInfo)
		throw Exception("GetPropInfo(__classid(TScxmlBaseShape), \"StateChildType\"==NULL)");
	return StringReplace(GetEnumName(*(APropInfo->PropType), AType), L"sct", L"", TReplaceFlags()).LowerCase();
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::GetXMLName(void) {
	return TypeToXMLNodeName(this->StateChildType);
}

// ---------------------------------------------------------------------------
TScxmlBaseShape *__fastcall TScxmlBaseShape::GetBaseParent() {
	return dynamic_cast<TScxmlBaseShape*>(this->Parent);
}

// ---------------------------------------------------------------------------
TVisualScxmlBaseShape *__fastcall TScxmlBaseShape::GetVisualParent(void) {
	return dynamic_cast<TVisualScxmlBaseShape*>(this->Parent);
}

// ---------------------------------------------------------------------------
TVisualScxmlBaseShape *__fastcall TScxmlBaseShape::GetFirstVisualParent(void) {
	TTreeNodeShape *AParent = dynamic_cast<TTreeNodeShape*>(this->Parent);
	while (AParent) {

		TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(AParent);
		if (AVisualScxmlBaseShape)
			return AVisualScxmlBaseShape;

		AParent = AParent->Parent;
	}

	return NULL;
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::HasAsParent(TTreeNodeShape *AShape) {
	if (AShape && AShape != this) {
		TTreeNodeShape *AParent = dynamic_cast<TTreeNodeShape*>(this->Parent);
		while (AParent) {

			if (AParent == AShape)
				return true;

			AParent = AParent->Parent;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
TVisualScxmlBaseShape *__fastcall TScxmlBaseShape::GetFirstVisualChild(void) {
	for (int i = 0; i < this->Children->Count; i++) {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(this->Children->Items[i]);
		if (AVisualShape && !AVisualShape->ExcludeFromSave) {
			return AVisualShape;
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::ValidateChild(TScxmlBaseShape *AChild) {
	if (!AChild)
		return;

	if (this->AvailableTypes.find(AChild->StateChildType) == this->AvailableTypes.end())
		throw EScxmlBaseShapeException(AChild, L"Not valid child", UnicodeString().sprintf(L"<%s> is not a valid child of <%s>", //
			AChild->XMLName.c_str(), this->XMLName.c_str()));

	if (AChild->OccursOnce) {
		if (AChild->Parent != this)
			throw EScxmlBaseShapeException(AChild, L"Program logic", UnicodeString().sprintf
			(L"ValidateChild> Program logic error! [%s] is not a child of [%s]", //
				AChild->SimpleText.c_str(), this->SimpleText.c_str()));

		if (AChild->ChildIndex >= this->Children->Count)
			throw EScxmlBaseShapeException(AChild, L"Program logic", //
			UnicodeString().sprintf(L"ValidateChild> Program logic error! ChildIndex:[%s]:[%d] is out of range [%s]:[0...%d]", //
				AChild->SimpleText.c_str(), AChild->ChildIndex, this->SimpleText.c_str(), this->Children->Count));

		// проверяем все предыдущие
		for (int i = 0; i < AChild->ChildIndex; i++) {
			if (this->Children->Items[i]->ClassType() == AChild->ClassType()) {
				throw EScxmlBaseShapeException(AChild, L"Duplicated", L"<" + AChild->XMLName + "> must occur only 1 time!");
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::ValidateShape() {
	// проверки, когда уже назначен родитель
	if (this->BaseParent) {

		TStateErrorMap::iterator it = FErrorMap.find(SCXML_ERROR_WRONG_CHILD_TYPE);
		if (this->BaseParent->AvailableTypes.find(this->StateChildType) != this->BaseParent->AvailableTypes.end()) {
			if (it != FErrorMap.end()) {
				FErrorMap.erase(it);
			}
		}
		else {
			const UnicodeString sMsg = UnicodeString().sprintf(L"E%d:%s> text=[%s] name=[%s]", SCXML_ERROR_WRONG_CHILD_TYPE,
				GetErrorDescription(SCXML_ERROR_WRONG_CHILD_TYPE).c_str(), this->SimpleText.c_str(), this->Name.c_str());

			if (it == FErrorMap.end()) {
				WLOG_WARNING(L"%s", sMsg.c_str());
			}

			FErrorMap[SCXML_ERROR_WRONG_CHILD_TYPE] = sMsg;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::ValidateSimpleText(TCustomTree *ATree) {
	if (ATree) {

		std::auto_ptr<TStringList>ANamesList(new TStringList());

		bool bValid = true;

		for (int i = 0; i < ATree->Items->Count; i++) {
			TScxmlBaseShape * ABaseShape = dynamic_cast<TScxmlBaseShape*>(ATree->Items->Items[i]);
			if (ABaseShape && !ABaseShape->ExcludeFromSave && ABaseShape->MustSimpleTextBeUnique()) {

				/* ID может быть пустым для визуальных фигур */
				TVisualScxmlBaseShape * AVisuaShape = dynamic_cast<TVisualScxmlBaseShape*>(ABaseShape);
				if (AVisuaShape && AVisuaShape->SimpleText.IsEmpty()) {
					continue;
				}

				TStateErrorMap::iterator it = ABaseShape->FErrorMap.find(SCXML_ERROR_NAME_DUPLICATED);

				if (ANamesList->IndexOf(ABaseShape->SimpleText) != -1) {
					const UnicodeString sMsg = UnicodeString().sprintf(L"E%d:%s> text=[%s] name=[%s]", SCXML_ERROR_NAME_DUPLICATED,
						ABaseShape->GetErrorDescription(SCXML_ERROR_NAME_DUPLICATED).c_str(), ABaseShape->SimpleText.c_str(),
						ABaseShape->Name.c_str());
					if (it == ABaseShape->FErrorMap.end()) {
						WLOG_WARNING(sMsg.c_str());
						bValid = false;
					}
					ABaseShape->FErrorMap[SCXML_ERROR_NAME_DUPLICATED] = sMsg;
				}
				else {
					if (it != ABaseShape->FErrorMap.end()) {
						ABaseShape->FErrorMap.erase(it);
						bValid = false;
					}

					ANamesList->Add(ABaseShape->SimpleText);
				}

				/* Virtual */
				// проверяется на выходе на уровне парсера, собирающего всё в scxml
			}
		}

		if (!bValid) {
			ATree->Invalidate();
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::GetErrorDescription(const int iErrCode) {
	switch(iErrCode) {
	case SCXML_ERROR_NAME_DUPLICATED:
		return L"Id is duplicated";
	case SCXML_ERROR_WRONG_CHILD_TYPE:
		return L"Wrong child type";
	case SCXML_ERROR_SRC_NOT_FOUND:
		return L"'Src' not found";
	}
	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::SetBrotherIndexEx(int val) {
	if (this->Parent && this->Tree) {
		const int iShapeIndex = this->Parent->Children->IndexOf(this);

		if (val != iShapeIndex && val >= 0 && val < this->Parent->Children->Count) {

			const Editorutils::TreeUpdateLock ATreeLock(this->Tree);

			TTreeNodeShape *AShapeNext = this->Parent->Children->Items[val];

			const int iIndex1 = this->Tree->Shapes->IndexOf(this);
			const int iIndex2 = this->Tree->Shapes->IndexOf(AShapeNext);

			TNodeListAccess *AListAccess = reinterpret_cast<TNodeListAccess*>(this->Tree->Shapes);
			AListAccess->Move(iIndex1, iIndex2);

			while (this->BrotherIndex != val) {
				this->BrotherIndex = val < iShapeIndex ? (this->BrotherIndex - 1) : (this->BrotherIndex + 1);
			}

			if (!this->ComponentState.Contains(csLoading) && !this->ComponentState.Contains(csReading)) {
				if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
					StateMachineEditor->FillNodeTree();
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::GetExcludeFromSave(void) {
	// если хотя бы один родитель исключен из записи
	TScxmlBaseShape *AParent = dynamic_cast<TScxmlBaseShape*>(this->Parent);
	while (AParent) {
		if (AParent->ExcludeFromSave)
			return true;
		AParent = dynamic_cast<TScxmlBaseShape*>(AParent->Parent);
	}
	return FExcludeFromSave || IsExcludedByCondition();
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::IsExcludedByCondition() {
	if (!ExcludeConditions.IsEmpty() && StateMachineEditorUnit && StateMachineEditorUnit->StateMachineProject) {
		std::auto_ptr<TStringList>ADefinesListPtr(new TStringList());
		ADefinesListPtr->StrictDelimiter = true;
		ADefinesListPtr->Delimiter = L';';
		ADefinesListPtr->DelimitedText = StateMachineEditorUnit->StateMachineProject->ProjectProperties->ProjectDefines;
		ADefinesListPtr->CaseSensitive = true;

		std::auto_ptr<TStringList>AConditionsListPtr(new TStringList());
		AConditionsListPtr->StrictDelimiter = true;
		AConditionsListPtr->Delimiter = L';';
		AConditionsListPtr->DelimitedText = ExcludeConditions;
		AConditionsListPtr->CaseSensitive = true;

		for (int i = 0; i < AConditionsListPtr->Count; i++) {
			if (AConditionsListPtr->Strings[i].Pos("~") == 1) {
				AConditionsListPtr->Strings[i] = AConditionsListPtr->Strings[i].Delete(1, 1);

				if (ADefinesListPtr->IndexOf(AConditionsListPtr->Strings[i]) == -1)
					return true;
			}
			else {
				if (ADefinesListPtr->IndexOf(AConditionsListPtr->Strings[i]) != -1)
					return true;
			}
		}

	}
	return false;
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::AreConditionalDefinesUsed(TStrings *ADefines) {
	if (!ADefines)
		throw Exception(L"ADefines==NULL!");

	if (!ExcludeConditions.IsEmpty()) {
		std::auto_ptr<TStringList>AConditionsListPtr(new TStringList());
		AConditionsListPtr->StrictDelimiter = true;
		AConditionsListPtr->Delimiter = L';';
		AConditionsListPtr->DelimitedText = ExcludeConditions;
		AConditionsListPtr->CaseSensitive = true;

		for (int i = 0; i < AConditionsListPtr->Count; i++) {
			if (AConditionsListPtr->Strings[i].Pos("~") == 1) {
				AConditionsListPtr->Strings[i] = AConditionsListPtr->Strings[i].Delete(1, 1);
			}

			if (ADefines->IndexOf(AConditionsListPtr->Strings[i]) != -1)
				return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::SetInheritanceMismatch(bool val) {
	if (StateMachineEditorUnit && StateMachineEditorUnit->IsInherited) {
		FInheritanceMismatch = val;
	}
	else {
		FInheritanceMismatch = false;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::StoreInheritedVisualsParentOffsets(void) // сохранит положение дочерних относительно родителя
{
	// запомним дочерние не наследованные
	for (int i = 0; i < this->Children->Count; i++) {
		TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(this->Children->Items[i]);
		if (AVisualScxmlBaseShape && !AVisualScxmlBaseShape->Locked) {
			AVisualScxmlBaseShape->ParentOffsetStored->X = AVisualScxmlBaseShape->ParentOffsetX;
			AVisualScxmlBaseShape->ParentOffsetStored->Y = AVisualScxmlBaseShape->ParentOffsetY;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::RestoreInheritedVisualsParentOffsets(void) // восстановит
{
	// сдвинем дочерние не наследованные
	for (int i = 0; i < this->Children->Count; i++) {
		TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(this->Children->Items[i]);
		if (AVisualScxmlBaseShape && !AVisualScxmlBaseShape->Locked && !AVisualScxmlBaseShape->ParentOffsetStored->IsEmpty()) {

			const int iOffsetX = AVisualScxmlBaseShape->ParentOffsetStored->X - AVisualScxmlBaseShape->ParentOffsetX;
			const int iOffsetY = AVisualScxmlBaseShape->ParentOffsetStored->Y - AVisualScxmlBaseShape->ParentOffsetY;

			if (iOffsetX || iOffsetY) {
				AVisualScxmlBaseShape->MoveRelative(iOffsetX, iOffsetY, true);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void TScxmlBaseShape::EditUnicodeStringSyntaxProperty(const UnicodeString &sPropName) {
	if (__EditUnicodeStringSyntaxProperty(this, sPropName)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit '%s':[%s]", sPropName.c_str(),
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
void TScxmlBaseShape::EditUnicodeStringLineProperty(const UnicodeString &sPropName) {
	if (__EditUnicodeStringLineProperty(this, sPropName)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit '%s':[%s]", sPropName.c_str(),
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
void TScxmlBaseShape::EditEventIDLineProperty(const UnicodeString &sPropName) {
	if (__EditEventIDLineProperty(this, sPropName)) {
		if (StateMachineEditor) {
			StateMachineEditor->MarkModified(1, UnicodeString().sprintf(L"Edit '%s':[%s]", sPropName.c_str(),
					Customutils::ClippedText(this->SimpleText, 40).c_str()), false);
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TScxmlBaseShape::GetScxmlSyntaxScheme(void) {
	if (StateMachineEditor) {
		return StateMachineEditor->SyntaxScheme;
	}
	return SettingsData->ActiveSyntaxScheme;
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::ShouldDrawCross(void) {
	if (Tree && Tree->Canvas) {
		TTreeSVGCanvasEx * ATreeSVGCanvasEx = dynamic_cast<TTreeSVGCanvasEx*>(Tree->Canvas);
		if (ATreeSVGCanvasEx) {

			if (this->ExcludeFromSave) {
				return false;
			}
		}
	}

	return TTreeNodeShape::ShouldDrawCross();
}

// ---------------------------------------------------------------------------
void __fastcall TScxmlBaseShape::Draw(void) {
	if (Tree && Tree->Canvas) {
		TTreeSVGCanvasEx * ATreeSVGCanvasEx = dynamic_cast<TTreeSVGCanvasEx*>(Tree->Canvas);
		if (ATreeSVGCanvasEx) {

			if (this->ExcludeFromSave) {
				return;
			}

			UnicodeString sID = L"";
			TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(this);
			if (AVisualScxmlBaseShape && AVisualScxmlBaseShape->MustSimpleTextBeUnique()) {
				sID = SimpleText.Trim();
			}
			else {
				sID = this->Name;
			}

			ATreeSVGCanvasEx->StartGroup(sID, this->ClassName());
		}

		try {
			/* inherited */
			TTreeNodeShape::Draw();
		}
		__finally {
			if (ATreeSVGCanvasEx) {
				ATreeSVGCanvasEx->EndGroup();
			}
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::IsCanvasSVG(void) {
	return Tree && dynamic_cast<TTreeSVGCanvasEx*>(Tree->Canvas);
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlBaseShape::ShouldDrawVectorImages(void) {
	return SettingsData->UseVectorImages || IsCanvasSVG();
}

// ---------------------------------------------------------------------------
// TVisualScxmlBaseShape
// ---------------------------------------------------------------------------
__fastcall TVisualScxmlBaseShape::TVisualScxmlBaseShape(Classes::TComponent * AOwner) : TScxmlBaseShape(AOwner), FEntered(false),
FInitial(L""), FExamined(false), FSkipDebugging(false), FBreakpointSet(false) {

	FAvailableTypes.insert(sctProtocolBinding);
	FAvailableTypes.insert(sctWeakProtocolBinding);
	FAvailableTypes.insert(sctVirtualFolder);
	FAvailableTypes.insert(sctTrigger);
	FAvailableTypes.insert(sctWatch);

	FSelfConnectionTextOffsets = new TPersistentRect;
	FSelfConnectionInside = false;

	FAliasID = L"";

	FParentOffsetStored = new TPersistentPoint;

	FChildrenAlignX = scaxLeft;
	FChildrenAlignY = scayClusterTop;
	FChildrenAlignXOffset = 0;

	ImageIndex = tiNone;
	AutoSize = false;
	AutoPosition->Left = false;
	AutoPosition->Top = false;

	X0 = 0;
	Y0 = 0;
	X1 = 100;
	Y1 = 50;

	BackColor = TColor(RGB(230, 230, 230));

	/* переопределяем для того, чтобы не вылазило за верх */
	this->ImageAlignment = iaLeftTop;
}

// ---------------------------------------------------------------------------
__fastcall TVisualScxmlBaseShape::~TVisualScxmlBaseShape() {
	delete FSelfConnectionTextOffsets;
	delete FParentOffsetStored;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	const UnicodeString sPropName = APropInfo->Name();
	if (sPropName == L"SelfConnectionTextOffsets") {
		AEditorClass = __classid(TPersistentRectEditor);
		return;
	}

	if (sPropName == L"Id") {
		AEditorClass = __classid(TLineEditStringPropEditor);
		return;
	}

	/* inherited */
	TScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
}

// ---------------------------------------------------------------------------
bool __fastcall TVisualScxmlBaseShape::OnFilterPropEvent(const UnicodeString & sPropName) {

	std::auto_ptr<TStringList>ASelfConnStringList(new TStringList());
	ASelfConnStringList->Add(L"SelfConnectionTextOffsets");
	ASelfConnStringList->Add(L"SelfConnectionInside");
	if (ASelfConnStringList->IndexOf(sPropName) != -1 && !HasSelfConnections) {
		return false;
	}

	// для внутреннего сохранения
	if (sPropName == L"ParentOffsetStored") {
		return false;
	}

	if (sPropName == L"AliasID") {
		return !this->AliasID.IsEmpty();
	}

	std::auto_ptr<TStringList>AChildrenPropsListPtr(new TStringList());
	AChildrenPropsListPtr->Add(L"ChildrenAlignX");
	AChildrenPropsListPtr->Add(L"ChildrenAlignY");
	AChildrenPropsListPtr->Add(L"ChildrenAlignXOffset");
	if (AChildrenPropsListPtr->IndexOf(sPropName) != -1) {
		return this->HasNonVisualChildren;
	}

	std::auto_ptr<TStringList>AStringList(new TStringList());
	AStringList->CaseSensitive = true;
	AStringList->Add("X0");
	AStringList->Add("X1");
	AStringList->Add("Y0");
	AStringList->Add("Y1");

	if (!this->IsCluster()) {
		AStringList->Add("VertTextAlign");
	}
	AStringList->Add("HorizTextAlign");

	const int iIndex = AStringList->IndexOf(sPropName);
	const bool bShow = iIndex == -1 ? TScxmlBaseShape::OnFilterPropEvent(sPropName) : true;

	return bShow;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem,
	TColor &AColor) {

	TScxmlBaseShape::OnGetPropCaptionColor(Sender, AItem, AColor);

	if (AItem->TypeInfo->Kind != tkClass && !AItem->Parent && AItem->PropInfo[0]->IsWritable()) {
		if (IsPublishedProp(__classid(TVisualScxmlBaseShape), AItem->PropName)) {
			//			if (!IsPublishedProp(__classid(TScxmlBaseShape), AItem->PropName)) {
			//				AColor = RGB(90,90,0);
			//			}
		}
		else {
			AColor = clNavy;
		}
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVisualScxmlBaseShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Id") {
		return //
		"The identifier for this state." //
		;
	}

	if (sPropName == L"Initial") {
		return //
		"The id of the default initial state (or states) for this state.\n" //
		"MUST NOT be specified in conjunction with the &lt;initial&gt; element.\n" //
		"MUST NOT occur in atomic states." //
		;
	}

	if (sPropName == L"IsInitial") {
		return //
		"Whether the element is initial state for its parent\n" //
		;
	}

	if (sPropName == L"SelfConnectionTextOffsets") {
		return //
		"Text offsets for self-connections\n" //
		"<B>Left</B> - for connections from the left side of the shape\n" //
		"<B>Right</B> - for connections from the right side of the shape\n" //
		"<B>Top</B> - for connections from the top side of the shape\n" //
		"<B>Bottom</B> - for connections from the top side of the shape\n" //
		;
	}

	if (sPropName == L"SelfConnectionInside") {
		return L"Self-connections are distributed inside of a state\n" //
		"Call 'Arrange Self-Connections' operation to see the result\n" //
		;
	}

	if (sPropName == L"VertTextAlign") {
		return L"Vertical text alignment\n" //
		;
	}

	if (sPropName == L"AliasID") {
		return L"Displays interpreted name based on <b>Virtual State Alias</b>\n" //
		;
	}

	if (sPropName == L"HorizTextAlign") {
		return L"Horizontal text alignment\n" //
		;
	}

	if (sPropName == L"SkipDebugging") {
		return //
		"If it is set to <B>true</B>, then <B>OnEnter</B> and <B>OnExit</B> actions will not be applied to this shape during Debug Session.\n"
		//
		"Shape will not be highlighted and msg will be filtered in CallStack view\n";
	}

	if (sPropName == L"ChildrenAlignX") {
		return //
		L"<B>scaxLeft</B> - children are aligned to the <b>left</b> corner of the shape + 'ChildrenAlignXOffset'\n" //
		"<B>scaxRight</B> - children are aligned to the <b>right</b> corner of the shape + 'ChildrenAlignXOffset'\n" //
		"<B>scaxCenter</B> - children are aligned to the <b>center</b> of the shape + 'ChildrenAlignXOffset'\n" //
		;
	}

	if (sPropName == L"ChildrenAlignXOffset") {
		return //
		L"children are aligned to the <b>left</b>, <b>right</b> or <b>center</b> of the shape + 'ChildrenAlignXOffset'\n" //
		;
	}

	if (sPropName == L"ChildrenAlignY") {
		return //
		L"<B>scayClusterTop</B> - children are aligned to the <b>top</b> corner of the shape in <b>cluster mode</b>\n" //
		"<B>scayAlwaysBottom</B> - children are aligned to the <b>bottom</b> corner of the shape\n" //
		"<B>scayAlwaysTop</B> - children are aligned to the <b>top</b> corner of the shape\n" //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetSelfConnectionTextOffsets(TPersistentRect *val) {
	FSelfConnectionTextOffsets->Assign(val);
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetSelfConnectionInside(bool val) {
	FSelfConnectionInside = val;
}

// ---------------------------------------------------------------------------
int __fastcall TVisualScxmlBaseShape::GetParentOffsetX(void) {
	if (this->Parent) {
		return this->X0 -this->Parent->X0;
	}
	return 0;
}

// ---------------------------------------------------------------------------
int __fastcall TVisualScxmlBaseShape::GetParentOffsetY(void) {
	if (this->Parent) {
		return this->Y0 -this->Parent->Y0;
	}
	return 0;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetParentOffsetStored(TPersistentPoint *val) {
	FParentOffsetStored->Assign(val);
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::UpdateStateChildren(void) {
	int iBrotherIndex = 0;

	for (int i = 0; i < this->Count(); i++) {
		TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(this->Children->Items[i]);
		if (AChildShape) {
			const int iWasIndex = AChildShape->ChildIndex;
			if (iWasIndex != iBrotherIndex) {
				AChildShape->ChildIndex = iBrotherIndex;
				WLOG_DEBUG(L"[%s] Reassigned brother index from[%d] to[%d]", AChildShape->SimpleText.c_str(), iWasIndex, iBrotherIndex);
			}
			iBrotherIndex++;
			// применяем автопозицию
			AChildShape->AutoPosition->Left = true;
			AChildShape->AutoPosition->Top = true;
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TVisualScxmlBaseShape::IsCluster(void) {
	// если есть хотя бы один дочерний элемент
	for (int i = 0; i < this->Count(); i++) {
		TVisualScxmlBaseShape *ABaseShape = dynamic_cast<TVisualScxmlBaseShape*>(this->Children->Items[i]);
		if (ABaseShape) {
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TVisualScxmlBaseShape::GetInitial(void) {
	// проверка, является ли валидным указатель на начальный элемент
	if (!FInitial.IsEmpty()) {
		// если классический Initial с указанием только одного элемента
		if (!FInitial.Pos(" ")) {
			for (int i = 0; i < this->Children->Count; i++) {
				TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(this->Children->Items[i]);
				if (AVisualScxmlBaseShape && AVisualScxmlBaseShape->SimpleText == FInitial) {
					return FInitial;
				}
			}
			WLOG_WARNING(L"Initial element [%s] is not a valid child element of [%s]!", FInitial.c_str(), this->SimpleText.c_str());

			FInitial = L""; // не знаю, правильно или нет
		}
		else {
			// иначе просто возвращаем список без всяких проверок
			return FInitial;
		}

	}
	// в этом случае невозвращаем начальный элемент, но пока сохраняем FInitial
	return L"";
}

// ---------------------------------------------------------------------------
bool __fastcall TVisualScxmlBaseShape::GetIsInitial(void) {
	TVisualScxmlBaseShape *AVisualParent = dynamic_cast<TVisualScxmlBaseShape*>(this->Parent);
	if (AVisualParent) {
		return !AVisualParent->FInitial.IsEmpty() && (AVisualParent->FInitial == this->SimpleText);
	}
	return false;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetIsInitial(bool val) {
	bool bNeedToRedraw = false;
	TVisualScxmlBaseShape *AVisualParent = dynamic_cast<TVisualScxmlBaseShape*>(this->Parent);
	if (AVisualParent) {
		if (val) {
			if (AVisualParent->FInitial != this->SimpleText) {
				AVisualParent->Initial = this->SimpleText;
				bNeedToRedraw = true;
			}
		}
		else if (AVisualParent->FInitial == this->SimpleText) {
			AVisualParent->Initial = L"";
			bNeedToRedraw = true;
		}
	}

	// иначе будет access violation
	if (!this->ComponentState.Contains(csReading) && !this->ComponentState.Contains(csLoading) && bNeedToRedraw) {
		this->Draw();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetStateId(UnicodeString sVal) {
	// эта нода была initial до этого
	if (this->VisualParent && !this->VisualParent->FInitial.IsEmpty() && this->VisualParent->FInitial == SimpleText) {
		this->VisualParent->Initial = sVal;
	}

	SimpleText = sVal;

	if (StateMachineEditor) {
		StateMachineEditor->MarkValidateSimpleText();

		if (!this->ComponentState.Contains(csLoading) && !this->ComponentState.Contains(csReading)) {
			if (StateMachineEditor->NodeTheTree->Checked) {
				StateMachineEditor->MarkUpdateNodeTreeElement(this);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::Assign(Classes::TPersistent * Source) {
	if (Source) {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TVisualScxmlBaseShape * ABaseShape = dynamic_cast<TVisualScxmlBaseShape*>(Source);
			if (ABaseShape) {
				FErrorMap = ABaseShape->FErrorMap;
				FInitial = ABaseShape->FInitial;
				FEntered = ABaseShape->FEntered;
				FAliasID = ABaseShape->FAliasID;
				FExamined = ABaseShape->FExamined;
				FBreakpointSet = ABaseShape->FBreakpointSet;
				FChildrenAlignX = ABaseShape->FChildrenAlignX;
				FChildrenAlignY = ABaseShape->FChildrenAlignY;
				FChildrenAlignXOffset = ABaseShape->FChildrenAlignXOffset;
				FSelfConnectionInside = ABaseShape->FSelfConnectionInside;

				SkipDebugging = ABaseShape->SkipDebugging; // чтобы сработал Update стиля

				FSelfConnectionTextOffsets->Assign(ABaseShape->FSelfConnectionTextOffsets);
				FParentOffsetStored->Assign(ABaseShape->FParentOffsetStored);

				this->SetStateId(ABaseShape->SimpleText);
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
TColor __fastcall TVisualScxmlBaseShape::GetNormalColor(void) {
	return SettingsData->ThemeSettings->StateNormalColor;
}

// ---------------------------------------------------------------------------
TColor __fastcall TVisualScxmlBaseShape::GetHeadColor(void) {
	return SettingsData->ThemeSettings->StateNormalColor;
}

// ---------------------------------------------------------------------------
TColor __fastcall TVisualScxmlBaseShape::GetClusterColor(void) {
	return SettingsData->ThemeSettings->StateClusterColor;
}

// ---------------------------------------------------------------------------
TTreeShapeStyle __fastcall TVisualScxmlBaseShape::GetNormalStyle(void) {
	return TTreeShapeStyle::tssRoundRectangle;
}

// ---------------------------------------------------------------------------
TTreeShapeStyle __fastcall TVisualScxmlBaseShape::GetClusterStyle(void) {
	return TTreeShapeStyle::tssRectangle;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::UpdateBreakpointView() {
	this->ImageIndex = FBreakpointSet ? //
	TTreeNodeImageIndex(IsActiveDebugShape() ? g_BreakpointActiveIndex : g_BreakpointInactiveIndex) : //
	tiNone;
}

// ---------------------------------------------------------------------------
TVertTextAlign __fastcall TVisualScxmlBaseShape::GetVertTextAlign(void) {
	return this->IsCluster() ? Teetree::vtaTop : TScxmlBaseShape::GetVertTextAlign();
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetNormalAppearance(void) {
	const bool bIsCluster = this->IsCluster();
	if (bIsCluster) {
		if (this->Style != GetClusterStyle()) {
			this->Style = GetClusterStyle();
		}
		if (this->Color != GetClusterColor()) {
			this->Color = GetClusterColor();
		}
	}
	else {
		if (this->Style != GetNormalStyle()) {
			this->Style = GetNormalStyle();
		}
		if (this->Color != GetNormalColor()) {
			this->Color = GetNormalColor();
		}
	}

	this->Transparent = bIsCluster && ClassType() != __classid(TScxmlShape);

	if (this->Border->Style != SettingsData->ThemeSettings->StateNormalBorderStyle) {
		this->Border->Style = SettingsData->ThemeSettings->StateNormalBorderStyle;
	}

	if (FExamined) {
		this->Border->Width = 3;
		this->Border->Color = SettingsData->ThemeSettings->StateExaminedBorderColor;
	}
	else {

		TColor ABorderColor = this->Border->Color;
		int iBorderWidth = this->Border->Width;
		if (GetIsInitial()) {
			ABorderColor = SettingsData->ThemeSettings->StateInitialBorderColor;
			iBorderWidth = 2;
		}
		else {
			ABorderColor = SettingsData->ThemeSettings->StateNormalBorderColor;
			iBorderWidth = 1;
		}

		if (FEntered) {
			iBorderWidth = 3;

			if (this->IsActiveDebugShape()) {
				ABorderColor = clRed;
			}
			else {
				if (SettingsData->ChartActiveStateColor != clDefault) {
					ABorderColor = SettingsData->ChartActiveStateColor;
				}
			}
		}
		else {
			if (StateMachineEditor && StateMachineEditor->WasEnteredShape(this)) {
				iBorderWidth = 3;
				ABorderColor = SettingsData->ChartTestCoverageColor;
			}
		}

		if (this->Border->Width != iBorderWidth)
			this->Border->Width = iBorderWidth;

		if (this->Border->Color != ABorderColor) {
			this->Border->Color = ABorderColor;
		}
	}

	if (this->Transparency != 0) {
		this->Transparency = 0;
	}

	UpdateBreakpointView();

	TFontStyles AFontStyle = SettingsData->ThemeSettings->StateNormalFontStyle;
	if (!bIsCluster && this->SelfConnectionInside) {
		AFontStyle = SettingsData->ThemeSettings->StateSelfConnectionInsideFontStyle;
	}

	if (FEntered) {
		if (this->Font->Style != AFontStyle << fsBold) {
			this->Font->Style = AFontStyle << fsBold;
		}
	}
	else {
		if (this->Font->Style != AFontStyle) {
			this->Font->Style = AFontStyle;
		}
	}
	if (this->Shadow->Visible != false) {
		this->Shadow->Visible = false;
	}
	if (this->RoundSize != 10) {
		this->RoundSize = 10;
	}

}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetErrorAppearance(void) {
	if (this->Border->Width != 3) {
		this->Border->Width = 3;
	}
	if (this->Border->Style != psDash) {
		this->Border->Style = psDash;
	}
	if (this->Transparency != 50) {
		this->Transparency = 50;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::UpdateAppearance(void) {
	if (!FErrorMap.empty() || ExcludeFromSave) {
		SetErrorAppearance();
	}
	else {
		SetNormalAppearance();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::DrawShapeCanvas(Tecanvas::TCanvas3D * ACanvas, const Types::TRect & R) {
	/* идет проверка фигуры на наличие ошибок */
	ValidateShape();

	UpdateAppearance();

	TScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	if (this->IsCluster()) {
		ACanvas->Brush->Style = bsSolid;
		ACanvas->Brush->Color = this->GetHeadColor();
		ACanvas->FillRect(ACanvas->RectFromRectZ(TRect(R.Left +this->Border->Width, R.Top +this->Border->Width,
					R.Right -this->Border->Width, R.Top + GetClusterHeight()), TeeTreeZ));

		ACanvas->HorizLine3D(R.Left, R.Right, R.Top + GetClusterHeight(), TeeTreeZ);
	}

	if (ExcludeFromSave) {
		ACanvas->Font->Color = clMaroon;
		ACanvas->Font->Style = TFontStyles();
		ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
		ACanvas->TextAlign = 0; // без этого может позиционироваться по центру
		ACanvas->TextOut3D(R.Left + 3, R.Top, TeeTreeZ,
			L"Excluded" + (ExcludeConditions.IsEmpty() ? UnicodeString(L"") : (L" [" + ExcludeConditions + L"]")));
	}
	else {
		int iX = R.Left;
		if (!FErrorMap.empty()) {
			ACanvas->Font->Size = 8;
			ACanvas->Font->Color = clRed;
			ACanvas->Font->Style = TFontStyles();
			ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
			ACanvas->TextAlign = TA_LEFT;
			const UnicodeString sText = L"Error: " + GetErrorDescription(FErrorMap.begin()->first);
			ACanvas->TextOut3D(iX, R.Top - 14, TeeTreeZ, sText);
			iX += ACanvas->TextWidth(sText) + 3;
		}

		if (!FAliasID.IsEmpty()) {
			ACanvas->Font->Size = 8;
			ACanvas->Font->Color = clBlue;
			ACanvas->Font->Style = TFontStyles();
			ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
			ACanvas->TextAlign = TA_LEFT;
			ACanvas->TextOut3D(iX, R.Top - 14, TeeTreeZ, FAliasID);
		}

		if (TVisualScxmlBaseShape * AVisualParent = this->VisualParent) {

			switch(AVisualParent->StateChildType) {
			case sctScxml:
			case sctState: {
					if (AVisualParent->Initial.IsEmpty()) {

						TVisualScxmlBaseShape *AFirstVisual = NULL;

						int iVisCount = 0;

						for (int i = 0; i < this->Parent->Children->Count; i++) {
							TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
								(this->Parent->Children->Items[i]);
							if (AVisualScxmlBaseShape && !AVisualScxmlBaseShape->ExcludeFromSave) {

								if (AVisualScxmlBaseShape->StateChildType == sctInitial) {
									AFirstVisual = NULL;
									break;
								}

								if (!AFirstVisual) {
									AFirstVisual = AVisualScxmlBaseShape;
								}

								iVisCount++;
							}
						}

						if (AFirstVisual == this) {
							ACanvas->Brush->Color = clBlack;
							ACanvas->Pen->Color = clBlack;

							if (this->StateChildType == sctFinal) {
								// имеет смысл показывать, если больше 1
								if (iVisCount > 1) {
									ACanvas->EllipseWithZ(TRect(R.Left - 4, R.Top - 10, R.Left + 6, R.Top), TeeTreeZ);
								}
							}
							else {
								ACanvas->EllipseWithZ(TRect(R.Left + 4, R.Top + 4, R.Left + 14, R.Top + 14), TeeTreeZ);
							}
						}
					}
				}break;
			default:
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetEntered(bool val) {
	if (val == true && this->VisualParent && !this->VisualParent->Expanded) {
		this->VisualParent->Expanded = true;
	}
	FEntered = val;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetChildrenEntered(TNodeShapeList *AShapes, bool bVal, bool bRecursive /* =true */ ) {
	for (int i = 0; i < AShapes->Count; i++) {
		TVisualScxmlBaseShape *AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AShapes->Items[i]);
		if (AVisualShape) {
			AVisualShape->Entered = bVal;
			if (bRecursive) {
				SetChildrenEntered(AVisualShape->Children, bVal, bRecursive);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetBreakpointSet(bool val) {
	FBreakpointSet = val;

	UpdateBreakpointView();
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetSkipDebugging(bool val) {
	FSkipDebugging = val;

	this->Brush->Style = FSkipDebugging ? bsCross : bsSolid;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetExamined(bool val) {
	FExamined = val;
}

// ---------------------------------------------------------------------------
int __fastcall TVisualScxmlBaseShape::GetClusterHeight(void) {
	return this->Font->Size * 2+this->Border->Width;
}

// ---------------------------------------------------------------------------
int __fastcall TVisualScxmlBaseShape::CalculateChildrenTop(int iDefaultTop) {

	return(IsCluster() || this->ChildrenAlignY == scayAlwaysTop) ? this->Top + GetClusterHeight() : iDefaultTop;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetDefaultSize() {
	this->Text->VertOffset = 0;
	this->Text->HorizOffset = 0;

	SetNormalAppearance();

	Width = 100;
	Height = 50;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::MoveToOtherShapeCenter(TVisualScxmlBaseShape *ASourceShape) {
	if (ASourceShape) {
		const int iX = ASourceShape->XCenter();
		const int iY = ASourceShape->YCenter();
		// могут быть случаи, когда размер ещё не просчитан
		if (iX && iY) {
			this->MoveRelative(iX -this->XCenter(), iY -this->YCenter(), true);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::ArrangeSelfConnections(void) {
	Editorutils::TreeUpdateLock ALock(this->Tree);

	std::map<TSelfConnectionAlign, TSelfConnectionInfo>ASelfAlignMap;

#if 0 // для глубокой отладки!
	Editorutils::DumpConnections(this);
#endif

	for (int i = 0; i < this->Connections->Count; i++) {
		if (this->Connections->Items[i]->ToShape == this) {
			TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(this->Connections->Items[i]);
			if (AStateMachineConnection) {

				const TSelfConnectionInfo AInfo = AStateMachineConnection->SelfConnectionInfo;

				std::map<TSelfConnectionAlign, TSelfConnectionInfo>::iterator it = ASelfAlignMap.find(AInfo.Align);
				if (it == ASelfAlignMap.end()) {
					ASelfAlignMap.insert(std::make_pair(AInfo.Align, AInfo));
				}

				// некрасиво, когда вылазит соединение за пределы фигуры,
				// поэтому будем рассматривать варианты только когда оно в пределах
				AStateMachineConnection->AlignSelfConnection(AInfo.Align,
					ASelfAlignMap[AInfo.Align].LengthOffsetBegin < 0 ? 0 : ASelfAlignMap[AInfo.Align].LengthOffsetBegin,
					ASelfAlignMap[AInfo.Align].LengthOffsetEnd < 0 ? 0 : ASelfAlignMap[AInfo.Align].LengthOffsetEnd,
					ASelfAlignMap[AInfo.Align].TextOffset);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::ArrangeDefaultSelfConnections(void) {

	for (int i = 0; i < this->Connections->Count; i++) {

		TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(this->Connections->Items[i]);
		if (AStateMachineConnection && AStateMachineConnection->IsSelfConnection) {

			AStateMachineConnection->AlignSelfConnection(scaTop);

		}

	}
}

// ---------------------------------------------------------------------------
int __fastcall TVisualScxmlBaseShape::MaxHeightExpandedChildsEx(void) {
	if (Expanded && Children->Count) {
		for (int i = Children->Count - 1; i >= 0; i--) {
			if (!Children->Items[i]->InheritsFrom(__classid(TVisualScxmlBaseShape))) {
				return Children->Items[i]->MaxHeightExpandedChilds();
			}
		}
	}
	return(this->ChildrenAlignY == scayClusterTop || this->ChildrenAlignY == scayAlwaysTop) ? CalculateChildrenTop
		(AdjustedRectangle.Bottom) : AdjustedRectangle.Bottom;
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::UpdateView(void) {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {

		if (this->Tree) {
			this->Tree->Invalidate();
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TVisualScxmlBaseShape::SetExcludeFromSave(bool val) {

	TScxmlBaseShape::SetExcludeFromSave(val);

	if (StateMachineEditor) {
		StateMachineEditor->MarkValidateSimpleText();
	}
}

// ---------------------------------------------------------------------------
TStateMachineConnection * __fastcall TVisualScxmlBaseShape::HasInternalInOutConnections(void) {
	if (Tree) {
		for (int i = 0; i < Tree->Connections->Count; i++) {
			TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(Tree->Connections->Items[i]);
			if (AStateMachineConnection && !AStateMachineConnection->IsSelfConnection) {
				TVisualScxmlBaseShape * AVisualFrom = dynamic_cast<TVisualScxmlBaseShape*>(AStateMachineConnection->FromShape);
				TVisualScxmlBaseShape * AVisualTo = dynamic_cast<TVisualScxmlBaseShape*>(AStateMachineConnection->ToShape);
				if (AVisualFrom && AVisualTo) {
					const bool bThisFromParent = AVisualFrom->HasAsParent(this);
					const bool bThisToParent = AVisualTo->HasAsParent(this);
					if ((bThisFromParent && !bThisToParent) || (!bThisFromParent && bThisToParent)) {
						return AStateMachineConnection;
					}
				}
			}
		}
	}
	return NULL;
}

// ---------------------------------------------------------------------------
bool __fastcall TVisualScxmlBaseShape::IsActiveDebugShape(void) {
	if (TStateMachineEditor * AStateMachineEditor = this->StateMachineEditor) {
		return AStateMachineEditor->ActiveDebugShape == this;
	}
	return false;
}

// ---------------------------------------------------------------------------
// -------------------------- TChildScxmlBaseShape ---------------------------
// ---------------------------------------------------------------------------

__fastcall TChildScxmlBaseShape::TChildScxmlBaseShape(Classes::TComponent * AOwner) : TScxmlBaseShape(AOwner) {
	FClipOutMax = MAX_SIMPLETEXT_LENGTH;
	FClipOutValue = true;
	FDescription = L"";
	FEditByDefault = L"";
}

// ---------------------------------------------------------------------------
__fastcall TChildScxmlBaseShape::~TChildScxmlBaseShape() {
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::Assign(Classes::TPersistent * Source) {

	if (Source) {
		TScxmlBaseShape *AShape = dynamic_cast<TScxmlBaseShape*>(Source);
		if (AShape) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TScxmlBaseShape::Assign(AShape);
			// ********************************************* //

			TChildScxmlBaseShape * ABaseShape = dynamic_cast<TChildScxmlBaseShape*>(Source);
			if (ABaseShape) {
				this->FClipOutMax = ABaseShape->ClipOutMax;
				this->FClipOutValue = ABaseShape->ClipOutValue;
				this->FDescription = ABaseShape->FDescription;
				this->BrotherIndex = ABaseShape->BrotherIndex;
				this->FEditByDefault = ABaseShape->FEditByDefault;
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
TColor __fastcall TChildScxmlBaseShape::GetNormalColor(void) {
	return SettingsData->ThemeSettings->ChildNormalColor;
}

// ---------------------------------------------------------------------------
TFontStyles __fastcall TChildScxmlBaseShape::GetNormalFontStyle() {
	return SettingsData->ThemeSettings->ChildNormalFontStyle;
}

// ---------------------------------------------------------------------------
TColor __fastcall TChildScxmlBaseShape::GetNormalBorderColor(void) {
	return SettingsData->ThemeSettings->ChildNormalBorderColor;
}

// ---------------------------------------------------------------------------
TPenStyle __fastcall TChildScxmlBaseShape::GetNormalBorderStyle(void) {
	return SettingsData->ThemeSettings->ChildNormalBorderStyle;
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::DefaultEditProperty(void) {

	std::vector<TDefaultEditorPair>AVecEditors = GetEditByDefaultEditors();

	const bool bHasXMLText = IsPublishedProp(this, "XMLText");

	if (AVecEditors.size()) {
		if (SameText(EditByDefault, SCXML_EDITBYDEFAULT_AUTO)) {
			for (std::size_t i = 0; i < AVecEditors.size(); i++) {
				const UnicodeString sPropValue = GetPropValue(this, AVecEditors[i].first);
				if (!sPropValue.IsEmpty()) {
					AVecEditors[i].second();
					return;
				}
			}

			// вызываем первый редактор по умолчанию
			if (!bHasXMLText && !this->Children->Count) {
				AVecEditors[0].second();
				return;
			}
		}
		else {
			for (std::size_t i = 0; i < AVecEditors.size(); i++) {
				if (SameText(AVecEditors[i].first, EditByDefault)) {
					AVecEditors[i].second();
					return;
				}
			}
		}
	}

	if (bHasXMLText) {
		if (this->XMLText->EditXMLText()) {
			if (StateMachineEditor) {
				StateMachineEditor->MarkModified(1, "Edit XMLText:"+this->SimpleText, false);
			}
		}
	}
	else {
		if (this->EditRawXMLContent()) {
			if (StateMachineEditor) {
				StateMachineEditor->MarkModified(1, "Edit RAW XML:"+this->SimpleText, true);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::SetDefaultEditableProps(TStrings *AList) {
	std::vector<TDefaultEditorPair>AVecEditors = GetEditByDefaultEditors();

	if (AVecEditors.size()) {
		AList->Add(SCXML_EDITBYDEFAULT_AUTO);
	}

	for (std::size_t i = 0; i < AVecEditors.size(); i++) {
		AList->Add(AVecEditors[i].first);
	}

	AList->Add(SCXML_EDITBYDEFAULT_XML);
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TChildScxmlBaseShape::GetEditByDefaultValue(void) {
	return HasDefaultEditableProps() ? SCXML_EDITBYDEFAULT_AUTO : SCXML_EDITBYDEFAULT_XML;
}

// ---------------------------------------------------------------------------
// это событие вызывается для проверки фильтрации свойств
bool __fastcall TChildScxmlBaseShape::OnFilterPropEvent(const UnicodeString &sPropName) {
	if (sPropName == L"EditByDefault" && !HasDefaultEditableProps()) {
		return false;
	}

	return TScxmlBaseShape::OnFilterPropEvent(sPropName);
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::OnGetPropCaptionColor(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem,
	TColor &AColor) {

	TScxmlBaseShape::OnGetPropCaptionColor(Sender, AItem, AColor);

	if (AItem->TypeInfo->Kind != tkClass && !AItem->Parent && AItem->PropInfo[0]->IsWritable()) {
		if (SameText(AItem->PropName, L"Description")) {
			AColor = clGreen;
		}
		else if (!IsPublishedProp(__classid(TChildScxmlBaseShape), AItem->PropName)) {
			AColor = clNavy;
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,
	TLMDPropEditorClass &AEditorClass) {

	const UnicodeString sPropName = APropInfo->Name();
	if (sPropName == L"EditByDefault") {
		AEditorClass = __classid(TEditByDefaultPropEditor);
	}
	else {
		TScxmlBaseShape::OnGetPropEditorClass(AInstance, APropInfo, AEditorClass);
	}
}

// ---------------------------------------------------------------------------
UnicodeString __fastcall TChildScxmlBaseShape::OnGetHTMLPropertyInfo(const UnicodeString &sPropName) {
	const UnicodeString sInfo = TScxmlBaseShape::OnGetHTMLPropertyInfo(sPropName);
	if (!sInfo.IsEmpty()) {
		return sInfo;
	}

	if (sPropName == L"Description") {
		return //
		"The description will be displayed at the right side of the shape\n" //
		;
	}

	if (sPropName == L"ClipOutValue") {
		return //
		"If it is set to <B>true</B> then the shape's text will be cut until <B>ClipOutMax</B> position\n" //
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long string'</B>\n" //
		"    ClipOutValue=true\n" //
		"    ClipOutMax=5\n" //
		"Result: Text=<B>'Very'</B>\n";
	}

	if (sPropName == L"ClipOutMax") {
		return //
		"If <B>ClipOutValue</B> is set to <B>true</B> then the shape's text will be cut until <B>ClipOutMax</B> position\n" //
		"<B>For example:</B>\n" //
		"    Text=<B>'Very long string'</B>\n" //
		"    ClipOutValue=true\n" //
		"    ClipOutMax=5\n" //
		"Result: Text=<B>'Very'</B>\n";
	}

	if (sPropName == L"EditByDefault") {
		return //
		"The name of property which will be edited by default in state chart\n" //
		"<B>For example:</B> - when double click on <B>&lt;if&gt; element</B>\n" //
		"    <b>(AUTO)</b> = if 'Cond' is not empty edit it else edit 'XML'\n" //
		"    Cond = editing of condition\n" //
		"    XML = edititing of xml content\n" //
		;
	}

	return L"";
}

// ---------------------------------------------------------------------------
bool __fastcall TChildScxmlBaseShape::HasDefaultEditableProps(void) {
	return GetEditByDefaultEditors().size();
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::Loaded(void) {
	TScxmlBaseShape::Loaded(); // inherited

	this->AutoPosition->Left = true;
	this->AutoPosition->Top = true;
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::UpdateParentSimpleText() {
	TChildScxmlBaseShape *AShape = dynamic_cast<TChildScxmlBaseShape*>(this->Parent);
	if (AShape) {
		AShape->UpdateSimpleText();
	}
}

// ---------------------------------------------------------------------------
TColor __fastcall TChildScxmlBaseShape::GetNormalFontColor(void) {
	if (IsPublishedProp(this, L"XMLText")) {
		switch(this->XMLText->Type) {
		case xttCDATA:
			return TColor(RGB(221, 128, 0));
		case xttXML:
			return TColor(RGB(128, 0, 255));
		case xttSCXML:
			return TColor(RGB(128, 0, 0));
		}
	}
	return clBlack;
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::UpdateSimpleText() {
	if (!ComponentState.Contains(csLoading) && !ComponentState.Contains(csReading)) {

		UnicodeString sOut = StringReplace(GetOutValue().Trim(), "\r\n", " ", TReplaceFlags() << rfReplaceAll);
		if (ClipOutValue && sOut.Length() > ClipOutMax) {
			sOut = sOut.SetLength(ClipOutMax).Trim() + " ...";
		}

		// почему то без переменной XMLName возвращает NULL !
		const UnicodeString &sXMLName = XMLName;

		UnicodeString sXMLTextInfo = L"";
		if (IsPublishedProp(this, L"XMLText")) {
			switch(this->XMLText->Type) {
			case xttCDATA:
				sXMLTextInfo = L" [CDATA]";
				break;
			case xttXML:
				sXMLTextInfo = L" [XML]";
				break;
			case xttSCXML:
				sXMLTextInfo = L" [SCXML]";
				break;
			}
		}

		SimpleText = sOut.IsEmpty() ? sXMLName : UnicodeString().sprintf(L"%s%s {%s}", sXMLName.c_str(), sXMLTextInfo.c_str(),
			sOut.c_str());

		UpdateParentSimpleText();

		if (StateMachineEditor && StateMachineEditor->NodeTheTree->Checked) {
			StateMachineEditor->MarkUpdateNodeTreeElement(this);
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::SetExcludeFromSave(bool val) {

	TScxmlBaseShape::SetExcludeFromSave(val);

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::SetExcludeConditions(UnicodeString val) {

	TScxmlBaseShape::SetExcludeConditions(val);

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::SetDescription(UnicodeString val) {
	FDescription = val;

	UpdateSimpleText();
}

// ---------------------------------------------------------------------------
TColor __fastcall TChildScxmlBaseShape::GetFinalFontColor(void) {
	return this->ExcludeFromSave ? clLtGray : GetNormalFontColor();
}

// ---------------------------------------------------------------------------
TPenStyle __fastcall TChildScxmlBaseShape::GetFinalBorderStyle(void) {
	return this->ExcludeFromSave ? psDot : psSolid;
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::Draw(void) {
	this->Color = GetFinalColor();
	this->Font->Color = GetFinalFontColor();
	this->Font->Style = GetFinalFontStyle();
	this->Border->Color = GetFinalBorderColor();
	this->Border->Style = GetFinalBorderStyle();

	TScxmlBaseShape::Draw();
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::DrawImage(void) {
	if (Tree) {
		if (HasVectorImage() && ShouldDrawVectorImages()) {

			TPicture *tmp = GetPicture();
			if (tmp) {
				TRect tmpR = ImageRect;
				tmpR.left += Tree->ChartBounds.left;
				tmpR.right += Tree->ChartBounds.left;
				tmpR.top += Tree->ChartBounds.top;
				tmpR.bottom += Tree->ChartBounds.top;

				DrawVectorImage(tmpR);

				return;
			}
		}

		/* inherited */
		TScxmlBaseShape::DrawImage();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TChildScxmlBaseShape::DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R) {

	TScxmlBaseShape::DrawShapeCanvas(ACanvas, R);

	int iStartText = R.Right + 3;
	if (ExcludeFromSave && !ExcludeConditions.IsEmpty()) {
		ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
		ACanvas->Font->Color = clMaroon;
		ACanvas->Font->Style = TFontStyles();
		ACanvas->TextAlign = 0; // без этого может позиционироваться по центру
		const UnicodeString sExcludeText = L"Excluded [" + ExcludeConditions + L"]";
		ACanvas->TextOut3D(iStartText, R.Top, TeeTreeZ, sExcludeText);
		iStartText += ACanvas->TextWidth(sExcludeText) + 3;
	}

	if (!FDescription.IsEmpty()) {
		ACanvas->BackMode = cbmTransparent; // без этого может задний фонт быть непрозрачным
		ACanvas->Font->Color = clGreen;
		ACanvas->Font->Style = TFontStyles();
		ACanvas->TextAlign = 0; // без этого может позиционироваться по центру
		ACanvas->TextOut3D(iStartText, R.Top, TeeTreeZ, FDescription);
	}
}

// ---------------------------------------------------------------------------
// -------------------- EScxmlDuplicateStateIDException ----------------------
// ---------------------------------------------------------------------------
__fastcall EScxmlDuplicateStateIDException::EScxmlDuplicateStateIDException(const UnicodeString &sMsg, const UnicodeString &sScxmlName,
	const UnicodeString &sID) : Exception(sMsg), FScxmlName(sScxmlName), FStateID(sID) {

}

// ---------------------------------------------------------------------------
// ------------------------ EScxmlBaseShapeException -------------------------
// ---------------------------------------------------------------------------

__fastcall EScxmlBaseShapeException::EScxmlBaseShapeException(TScxmlBaseShape *AShape, const UnicodeString &sCategory,
	const UnicodeString &sMsg) : Exception(sMsg), FShape(AShape), FCategory(sCategory) {

}

// ---------------------------------------------------------------------------
UnicodeString GetTreeSyntaxScheme(TCustomTree *ATree) {
	if (ATree) {
		for (int i = 0; i < ATree->Shapes->Count; i++) {
			TScxmlShape * AScxmlShape = dynamic_cast<TScxmlShape*>(ATree->Shapes->Items[i]);
			if (AScxmlShape) {
				return AScxmlShape->SyntaxScheme;
			}
		}
	}
	return SettingsData->ActiveSyntaxScheme;
}

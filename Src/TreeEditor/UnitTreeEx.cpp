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

#include "UnitTreeEx.h"

#include <math.hpp>

#include "Log4cpp_VCL.hpp"
#include "UnitStateMachineConnection.h"
#include "UnitTreeEditorUtils.h"
#include "CursorConsts.h"
#include "UnitSettings.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

class TreeExMouseDownLock {
	TTreeEx *FInstance;

public:
	TreeExMouseDownLock(TTreeEx *AInstance) : FInstance(AInstance) {
		if (FInstance && FInstance->OnBeforeMouseDown)
			FInstance->OnBeforeMouseDown(FInstance);
	}

	~TreeExMouseDownLock() {
		if (FInstance && FInstance->OnAfterMouseDown)
			FInstance->OnAfterMouseDown(FInstance);
	}
};

// ---------------------------------------------------------------------------
__fastcall TTreeEx::TTreeEx(Classes::TComponent* AOwner) : TTree(AOwner) {
	FOnBeforeMouseDown = NULL;
	FOnAfterMouseDown = NULL;

	this->ShowHintShapes = false;
	FBookmarks = new TStringList;
	FLockCursorFlag = false;

	FDisableFocus = false;
}

// ---------------------------------------------------------------------------
__fastcall TTreeEx::~TTreeEx(void) {
	delete FBookmarks;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::DrawConnecting(int x, int y) {
	TTree::DrawConnecting(x, y);

	// выделение фигуры откуда при связывании соединений для большей наглядности
	if (this->ConnectingShape1) {
		const TRect ARect = Editorutils::IncrementRectCopy(this->ConnectingShape1->Bounds(), 15);
		this->Canvas->Pen->Mode = pmCopy;
		this->Canvas->Pen->Color = clRed;
		this->Canvas->Pen->Width = 1;
		this->Canvas->Brush->Style = bsClear;
		this->Canvas->RectangleWithZ(ARect, TeeTreeZ);

		this->Canvas->Font->Color = clRed;
		this->Canvas->Font->Size = 8;
		this->Canvas->Font->Style = TFontStyles();
		this->Canvas->Brush->Style = bsSolid;
		this->Canvas->Brush->Color = clWhite;

		this->Canvas->TextOut3D(ARect.Left, ARect.Top - 14, TeeTreeZ, L"Press 'ESC' to cancel connecting...");
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::Assign(Classes::TPersistent* Source) {
	if (Source) {
		TTree *ASourceAssign = dynamic_cast<TTree*>(Source);
		if (ASourceAssign) {
			// ********************************************* //
			// сначала все назначения из базового класса !!! //
			// ********************************************* //
			TTree::Assign(ASourceAssign);
			// ********************************************* //

			TTreeEx * ASourceTreeEx = dynamic_cast<TTreeEx*>(Source);
			if (ASourceTreeEx) {
				FApplicationVersion = ASourceTreeEx->FApplicationVersion;
				FBookmarks->Assign(ASourceTreeEx->FBookmarks);
			}
		}
		else
			throw Exception(__FUNCTION__ "> Can not cast <" + Source->ClassName() + "> to <"+this->ClassName() + ">");

	}
	else
		throw Exception(__FUNCTION__ "> Source can not be empty!");
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::DragOver(System::TObject* Source, int X, int Y, Controls::TDragState State, bool &Accept) {
	Teeprocs::TCustomTeePanelExtended::DragOver(Source, X, Y, State, Accept);
	// убираем все надстройки из TTree, так как они меняют родителей, могут удалять или клонировать узлы
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::DoEndDrag(System::TObject* Target, int X, int Y) {
	Teeprocs::TCustomTeePanelExtended::DoEndDrag(Target, X, Y);
	// убираем все надстройки из TTree, так как они меняют родителей, могут удалять или клонировать узлы
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::OnEditorBlockReturnKeyPress(System::TObject* Sender, System::WideChar &Key) {
	switch(Key) {
	case VK_RETURN: {
			if (this->Editing) {
				Key == 0;
				this->StopEditing();
			}
		}break;
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::KeyDown(System::Word &Key, Classes::TShiftState Shift) {
	switch(Key) {
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_UP: {
			if (Shift.Contains(ssCtrl)) {
				// если выделены фигуры
				if (this->Selected->Count()) {
					// при движении мышью, без Alt двигается фигура с дочерними элементами,
					// а при движении клавиатурой - нет, поэтому инветируем, чтобы поведение было одинаковым
					if (Shift.Contains(ssAlt)) {
						Shift = Shift >> ssAlt;
					}
					else {
						Shift = Shift << ssAlt;
					}
				}
			}
		}break;
	}

	// inherited
	TTree::KeyDown(Key, Shift);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y) {
	/* генерирует события до начала нажатия и после */
	TreeExMouseDownLock ALock(this);

	const bool bIgnoredShift = Shift.Contains(ssDouble) || Shift.Contains(ssShift) || Shift.Contains(ssCtrl) || Shift.Contains(ssAlt);

	/* этот метод прописан в базовом классе, но как-то муторно, типа, если не OCX */
	if (!ComponentState.Contains(csDesigning) && !this->Focused() && (CanFocus() || !GetParentForm(this))) {
		LOG_DEBUG("Tree is not focused! Manually set focus!");
		try {
			this->SetFocus();
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

	/* исправляем баг, связанный с тем, что нельзя выделить соединение внутри фигуры */
	this->CancelMouse = false;

	if (Button == mbLeft) {
		const bool bZoomCancelCondition = this->IZoomPriority && this->Zoom->Allow && Button == Zoom->MouseButton && Zoom->KeyShift.ToInt()
			<= Shift.ToInt();
		if (!bZoomCancelCondition && !IPolygonMode && !this->Dragged && //
			this->Cursor != TeeConnectionPointCursor /* иначе будет перескакивать на другае выделение */) {
			if (OnClickConnection) {
				TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(Connections->Clicked(X, Y));
				if (AConnection) {
					// если двойной клик, тогда генерируем OnDblClickConnection, где вызовется редактор
					// если одинарный, тогда проверим, что это соединение ещё не выделено,
					// иначе игнорируем так как дальше будет вызван обработчик выделения точек на соединении
					if (AConnection != this->Connections->Selected || Shift.Contains(ssDouble)) {

						/* inherited */
						Teeprocs::TCustomTeePanelExtended::MouseDown(Button, Shift, X, Y);

						this->CancelMouse = true;

						if (Shift.Contains(ssDouble)) {
							OnDblClickConnection(AConnection, Button, Shift, X, Y);
						}
						else {
							OnClickConnection(AConnection, Button, Shift, X, Y);
						}
					}
				}
			}
		}
	}

	if (this->CancelMouse)
		return;

	/* Убираем автоматическое добавление точек */
	// запоминаем, сколько было точек
	TConnectionPointArray FConnSelectedPoints;
	FConnSelectedPoints.Length = 0;

	TTreeConnection *ASelectedConnection = this->Connections->Selected;

	if (ASelectedConnection) {
		FConnSelectedPoints.Length = this->Connections->Selected->Points->Count();
		for (int i = 0; i < FConnSelectedPoints.Length; i++) {
			FConnSelectedPoints[i].XStyle = this->Connections->Selected->Points->Item[i].XStyle;
			FConnSelectedPoints[i].YStyle = this->Connections->Selected->Points->Item[i].YStyle;
			FConnSelectedPoints[i].XValue = this->Connections->Selected->Points->Item[i].XValue;
			FConnSelectedPoints[i].YValue = this->Connections->Selected->Points->Item[i].YValue;
		}
	}

	/* inherited */
	TTree::MouseDown(Button, Shift, X, Y); // в этом месте может произойти добавление точки

	// теперь сравниваем, сколько стало точек и
	// убираем добавленную точку в предыдущем выделении
	if (!Shift.Contains(ssCtrl) && this->Connections->Selected && (ASelectedConnection == this->Connections->Selected)
		&& FConnSelectedPoints.Length) {
		for (int i = this->Connections->Selected->Points->Count() - 1; i >= 0; i--) {
			bool bIsFound = false;
			for (int k = 0; k < FConnSelectedPoints.Length; k++) {
				if (this->Connections->Selected->Points->Item[i].XStyle == FConnSelectedPoints[k]
					.XStyle && this->Connections->Selected->Points->Item[i].YStyle == FConnSelectedPoints[k]
					.YStyle && this->Connections->Selected->Points->Item[i].XValue == FConnSelectedPoints[k]
					.XValue && this->Connections->Selected->Points->Item[i].YValue == FConnSelectedPoints[k].YValue) {
					bIsFound = true;
					break;
				}
			}
			if (!bIsFound) {
				this->Connections->Selected->Points->Delete(i);
				break;
			}
		}
		CancelMouse = true;
	}

	// только в режиме дизайна
	if (this->Designing && !this->Editing) {

		/* Drag 'n Drop */
		if ((Button == mbLeft || Button == mbMiddle) && !bIgnoredShift) {
			const bool bSelectionDragReady = this->Selected->Count() && Editorutils::AreAllShapesOfInheritedClass(this->Selected->Shapes,
				__classid(TChildScxmlBaseShape)) && Editorutils::AreAllShapesBrothers(this->Selected->Shapes);

			if (bSelectionDragReady) {
				this->BeginDrag(false, 5);
			}
		}
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y) {
	// если были в режиме перетаскивания, возвращаем курсор к нормальному
	if (Cursor == crPanningCursor) {
		Cursor = crDefault;
	}

	TTree::MouseUp(Button, Shift, X, Y);
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::WndProc(Messages::TMessage &Message) {
	/* блокируем все изменения курсора внутри функции */
	if (FLockCursorFlag) {
		switch(Message.Msg) {
		case WM_SETCURSOR:
			return;
		}
	}

	/* inherited */
	TTree::WndProc(Message);
}

// ---------------------------------------------------------------------------
TPoint __fastcall TTreeEx::ConvertToPixels(int iX2D, int iY2D) {
	const double dZoomFactor = this->View3DOptions->ZoomFloat * 0.01f;

	const double Xpix = (((double)iX2D -this->Canvas->XCenter -this->IBounds2D.Left) + (double)this->ChartXCenter) * dZoomFactor;
	const double Ypix = (((double)iY2D -this->Canvas->YCenter -this->IBounds2D.Top) + (double)this->ChartYCenter) * dZoomFactor;

	return TPoint(RoundTo(Xpix, 0), RoundTo(Ypix, 0));
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::MouseMove(Classes::TShiftState Shift, int X, int Y) {
#if 0 // для глубокой отладки!
	int X2D = X;
	int Y2D = Y;
	this->Canvas->Calculate2DPosition(X2D, Y2D, TeeTreeZ);

	const TPoint APixels = ConvertToPixels(X2D, Y2D);

	WLOG_DEBUG(L"X=%d, Y=%d, Xpix=%d, Ypix=%d, X2D=%d, Y2D=%d, CalcDiffX=%d, CalcDiffY=%d", //
		X, Y, APixels.x, APixels.y, X2D, Y2D, X - APixels.x, Y - APixels.y);
#endif

	/* необходимо заблокировать все изменения курсора внутри MouseMove,
	так как он может несколько раз меняться и это вызывает мерцания и глюки отрисовки */
	const TCursor APrevCursor = Cursor;

	FLockCursorFlag = true;

	try {

		/* inherited */
		TTree::MouseMove(Shift, X, Y);

		/* устраняем баг, связанный с тем, что курсор на соединении отрисовывается только, если соединение вне фигур */
		TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(Connections->Clicked(X, Y));
		if (AConnection) {
			if (Designing && OnClickConnection) {
				if (Cursor != TeeConnectionPointCursor) {
					Cursor = (Shift.Contains(ssCtrl) && this->Connections->Selected == AConnection)
						? TeeConnectionPointCursor : TeeConnectionCursor;
				}
			}
			else {
				Cursor = AConnection->Cursor;
			}
		}
		else {
			if (this->IPanning->Active) {
				Cursor = crPanningCursor;
			}
		}
	}
	__finally {
		FLockCursorFlag = false;

		if (Cursor != APrevCursor) {
			Perform(CM_CURSORCHANGED, 0, 0);
		}
	}
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeEx::DoMouseWheelDown(Classes::TShiftState Shift, const Types::TPoint &MousePos) {

	bool bRes = Teeprocs::TCustomTeePanelExtended::DoMouseWheelDown(Shift, MousePos);

	if (!bRes) {
		if (Shift.Contains(ssCtrl)) {
			if (View3DOptions->Zoom > 1) {
				const double dIncrease = 0.05 * (double)View3DOptions->Zoom;
				int iOldZoom = View3DOptions->Zoom;
				View3DOptions->Zoom = View3DOptions->Zoom - Max(1, static_cast<int>(dIncrease));
				if (FOnWheelZoom) {
					FOnWheelZoom(this);
				}
			}
		}
		else {
			View3DOptions->VertOffset = View3DOptions->VertOffset - TreePageScrollQuantity;
		}
	}
	return true;
}

// ---------------------------------------------------------------------------
bool __fastcall TTreeEx::DoMouseWheelUp(Classes::TShiftState Shift, const Types::TPoint &MousePos) {
	bool bRes = Teeprocs::TCustomTeePanelExtended::DoMouseWheelDown(Shift, MousePos);

	if (!bRes) {
		if (Shift.Contains(ssCtrl)) {
			const double dIncrease = 0.05 * (double)View3DOptions->Zoom;
			View3DOptions->Zoom = View3DOptions->Zoom + Max(1, static_cast<int>(dIncrease));
			if (FOnWheelZoom) {
				FOnWheelZoom(this);
			}
		}
		else {

			View3DOptions->VertOffset = View3DOptions->VertOffset + TreePageScrollQuantity;
		}
	}
	return true;
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::AssignTextFromEditorToShape(void) {
	try {
		if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(IEditShape)) {
			std::auto_ptr<TStringList>AStringListPtr(new TStringList());
			AStringListPtr->Text = TextEditor->Memo->Lines->Text.Trim();
			if (AStringListPtr->Count == 0) {

				AVisualScxmlBaseShape->SetStateId("");
			}
			else {
				for (int i = 1; i < AStringListPtr->Count; i++) {
					if (!AStringListPtr->Strings[i].IsEmpty())
						throw Exception("Only single line ID is allowed!");
				}

				AVisualScxmlBaseShape->SetStateId(AStringListPtr->Strings[0]);
			}
		}
		else {
			IEditShape->Text->Assign(TextEditor->Memo->Lines);
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"EDITOR> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::AssignTextFromShapeToEditor(void) {
	try {
		if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(IEditShape)) {
			TextEditor->Memo->Lines->Text = IEditShape->Text->Text.Trim();

			TextEditor->Memo->SelectAll();
		}
		else {
			TextEditor->Memo->Lines->Assign(IEditShape->Text);

			TextEditor->Memo->SelectAll();

			/* 'IEditShape.Text' has '\r\n' by default */
			const int iSelLength = TextEditor->Memo->SelText.Length();
			if (iSelLength > 2) {
				int iCutLength = 0;
				if (TextEditor->Memo->SelText[iSelLength] == L'\n') {
					iCutLength++;
				}
				if (TextEditor->Memo->SelText[iSelLength - 1] == L'\r') {
					iCutLength++;
				}
				TextEditor->Memo->SelLength = TextEditor->Memo->SelLength - iCutLength;
			}
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"EDITOR> %s", E->Message.c_str());
	}
}

// ---------------------------------------------------------------------------
void __fastcall TTreeEx::DrawChartFocus(TCustomTree *ACustomTree) {
	if (ACustomTree->Focused()) {
		TTreeEx *ATreeEx = dynamic_cast<TTreeEx*>(ACustomTree);
		if (ATreeEx && ATreeEx->DisableFocus) {
			return;
		}
		if (SettingsData && SettingsData->ChartFocusEnabled) {
			TCanvas3D *ACanvas = ACustomTree->Canvas;
			if (ACanvas && ACanvas->ClassNameIs("TTeeCanvas3D")) {
				std::auto_ptr<TPen>APenPtr(new TPen());
				ACanvas->Pen->Assign(APenPtr.get());
				ACanvas->Pen->Color = SettingsData->ChartFocusLineColor;
				ACanvas->Pen->Width = SettingsData->ChartFocusLineWidth;
				TRect AClientRect = ACustomTree->ClientRect;
				Editorutils::IncrementRect(AClientRect, -SettingsData->ChartFocusLineWidth);
				std::auto_ptr<TBrush>ABrushPtr(new TBrush());
				ACanvas->Brush->Assign(ABrushPtr.get());
				ACanvas->Brush->Style = bsClear;
				ACanvas->Rectangle(AClientRect);
			}
		}
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TTreeNodeShape *GetSafeTreeListFirst(TNodeShapeList *ATreeList) {
	if (ATreeList && ATreeList->Count) {
		return ATreeList->Items[0];
	}
	return NULL;
}

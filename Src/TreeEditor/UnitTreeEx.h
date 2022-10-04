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

#ifndef UnitTreeExH
#define UnitTreeExH
//---------------------------------------------------------------------------

#include <TeeTree.hpp>

class TTreeEx: public TTree {
private:
	bool FLockCursorFlag;
	bool FDisableFocus;

	TNotifyEvent FOnWheelZoom;
	TStringList *FBookmarks;
	inline void __fastcall SetBookmarks(TStringList *val) { FBookmarks->Assign(val); }

	TNotifyEvent FOnBeforeMouseDown;
	TNotifyEvent FOnAfterMouseDown;

	UnicodeString FApplicationVersion;

protected:
	// переопределяем, чтобы сделать запрет по созданию точки по клику
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);

	// не выделяется соединение, которое расположено на фигуре
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);

	// переопределяем для того, чтобы инвертировать Alt и Ctrl - из-за этого разная логике при смещении фигур мышью
	DYNAMIC void __fastcall KeyDown(System::Word &Key, Classes::TShiftState Shift);

	DYNAMIC bool __fastcall DoMouseWheelDown(Classes::TShiftState Shift, const Types::TPoint &MousePos);
	DYNAMIC bool __fastcall DoMouseWheelUp(Classes::TShiftState Shift, const Types::TPoint &MousePos);

    // переопределяем для запрета родных методов перетаскивания
	DYNAMIC void __fastcall DragOver(System::TObject* Source, int X, int Y, Controls::TDragState State, bool &Accept);
	DYNAMIC void __fastcall DoEndDrag(System::TObject* Target, int X, int Y);

	virtual void __fastcall WndProc(Messages::TMessage &Message);

	virtual void __fastcall DrawConnecting(int x, int y); /* override */

	virtual void __fastcall AssignTextFromEditorToShape(void); /* override */
	virtual void __fastcall AssignTextFromShapeToEditor(void); /* override */

public:
	__fastcall virtual TTreeEx(Classes::TComponent* AOwner);
	__fastcall virtual ~TTreeEx(void);

	inline Types::TRect __fastcall PrepareGrid(int &tmpH, int &tmpV) {
		 return TTree::PrepareGrid(tmpH,tmpV);
	}

	TPoint __fastcall ConvertToPixels(int iX2D, int iY2D);

	void __fastcall OnEditorBlockReturnKeyPress(System::TObject* Sender, System::WideChar &Key);

	__property TRect Bounds2D = {read=IBounds2D};
	__property OnKeyUp; // добавил сюда, так как у TTree в Protected

	__property TNotifyEvent OnWheelZoom = {read=FOnWheelZoom,write=FOnWheelZoom};
	__property TNotifyEvent OnBeforeMouseDown = { read=FOnBeforeMouseDown, write=FOnBeforeMouseDown };
	__property TNotifyEvent OnAfterMouseDown = { read=FOnAfterMouseDown, write=FOnAfterMouseDown };

    virtual void __fastcall Assign(Classes::TPersistent* Source);

	static void __fastcall DrawChartFocus(TCustomTree *ACustomTree);
	__property bool DisableFocus = {read=FDisableFocus, write=FDisableFocus, default=false};

__published:
	__property TStringList * Bookmarks = {read=FBookmarks, write=SetBookmarks};
	__property UnicodeString ApplicationVersion = {read=FApplicationVersion, write=FApplicationVersion};
	__property bool PersistZoomCentered = {read=ZoomCentered, write=ZoomCentered, nodefault};
};

extern TTreeNodeShape *GetSafeTreeListFirst(TNodeShapeList *ATreeList);


#endif

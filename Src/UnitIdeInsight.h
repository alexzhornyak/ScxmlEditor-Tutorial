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

#ifndef UnitIdeInsightH
#define UnitIdeInsightH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ExtCtrls.hpp>
#include <LMDInsPropInsp.hpp>
#include "TeeProcs.hpp"
#include "TeeTree.hpp"

//---------------------------------------------------------------------------

typedef enum { indtAppSettings, indtProjectOptions, indtMenuItem, indtLevel, indtAction, indtPropInspector, indtGifMaker }TIdeNodeDataType;

class TStateMachineEditor;

class TDialogInsight : public TForm
{
__published:	// IDE-managed Components
	TEdit *EditInsight;
	TImageList *ImageList1;
	TImageList *ImageConsts;
	TTree *TreeViewInsight;
	void __fastcall EditInsightChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall EditInsightKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall EditInsightKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall TreeViewInsightDblClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations

	TTreeNodeShape * __fastcall DoAddChildNode(TTreeNodeShape *AParent, const TIdeNodeDataType ADataType,
		const UnicodeString &sFullText, const UnicodeString &sSearchText, TCustomImageList *AFromImages = NULL,
		const int iImageIndex = -1, TObject *AItemObject = NULL, const bool bForceAdd = false);

	void __fastcall DoAddMenuItem(TTreeNodeShape *AParent, TMenuItem* AItem, TCustomImageList *AMenuImages);

	void __fastcall DoAddMenuItems(TTreeNodeShape *AParent, TMenuItem* AItems, TCustomImageList *AMenuImages);

	void __fastcall DoAddActionItems(TTreeNodeShape *AParent, TActionList *AListActions);

	void __fastcall DoAddPersistentItems(TTreeNodeShape *AParent, TObject *AInstance, const TIdeNodeDataType ADataType);

	void __fastcall DoAddPropInspItem(TTreeNodeShape *AParent, TLMDPropertyInspectorItem* AItem);

	void __fastcall DoExecuteSelectedNode(void);

	void __fastcall DoExecuteNode(TTreeNodeShape *ANode);

	// позиция курсора на момент вызова
	TPoint FCursorPoint;

	bool FDoNotRestorePropertyInspector;

	// триммированный текст
	UnicodeString __fastcall GetSearchText(void);

	TStateMachineEditor *FStateMachineEditor;

public:		// User declarations
	__fastcall TDialogInsight(TComponent* Owner);

	int __fastcall Execute(TStateMachineEditor *AStateMachineEditor);
};
//---------------------------------------------------------------------------
extern PACKAGE TDialogInsight *DialogInsight;
//---------------------------------------------------------------------------
#endif

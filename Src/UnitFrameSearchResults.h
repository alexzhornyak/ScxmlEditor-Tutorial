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

#ifndef UnitFrameSearchResultsH
#define UnitFrameSearchResultsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include "TeeProcs.hpp"
#include "TeeTree.hpp"
#include <ExtCtrls.hpp>
#include "CommonConsts.h"
//---------------------------------------------------------------------------

class TFrameSearchResults : public TFrame
{
__published:	// IDE-managed Components
	TImageList *ImageList1;
	TPopupMenu *PopupMenu1;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *Expand1;
	TMenuItem *Collapse1;
	TMenuItem *N1;
	TMenuItem *Select1;
	TMenuItem *SelectChildren1;
	TTree *HTMLSearch;
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall Collapse1Click(TObject *Sender);
	void __fastcall Expand1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall Select1Click(TObject *Sender);
	void __fastcall SelectChildren1Click(TObject *Sender);
	void __fastcall HTMLSearchDblClickShape(TTreeNodeShape *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations

	TTreeNodeShape *EnsureRootNode(const UnicodeString &sName);
	TTreeNodeShape *EnsureNode(TTreeNodeShape *ANodeParent, const UnicodeString &sName);
	TTreeNodeShape *EnsureNodeStart(TTreeNodeShape *ANodeParent, const UnicodeString &sName, const UnicodeString &sStartText);
	TTreeNodeShape *FindNode(TTreeNodeShape *ANodeParent, const UnicodeString &sName);
	TTreeNodeShape *FindNodeStart(TTreeNodeShape *ANodeParent, const UnicodeString &sText);

	bool FDoubleClick;

	void __fastcall DoSelectTreeNode(TTreeNodeShape *ANode, const bool bClear, const bool bScrollChart);

public:		// User declarations
	__fastcall TFrameSearchResults(TComponent* Owner);

	void __fastcall AddMatchNode(const UnicodeString &sUnit, const UnicodeString &sComponentName, const UnicodeString &sSimpleText,
						const UnicodeString &sPropName,	const TMatchFind &AMatch);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameSearchResults *FrameSearchResults;
//---------------------------------------------------------------------------
#endif

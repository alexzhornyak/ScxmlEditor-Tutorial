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

#ifndef UnitFrameTransitionTypesH
#define UnitFrameTransitionTypesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>

static const int i_SIDES_LEFT_DIRECTION = 1;
static const int i_SIDES_RIGHT_DIRECTION = 8;

static const int i_INVERTED_SIDES_TOP_DIRECTION = 2;
static const int i_INVERTED_SIDES_BOTTOM_DIRECTION = 9;

//---------------------------------------------------------------------------
class TFrameTransitionTypes : public TFrame
{
__published:	// IDE-managed Components
	TImageList *ImageList1;
	TToolBar *ToolBar1;
	TToolButton *BtnAlignLines;
	TToolButton *BtnAlignSides;
	TToolButton *BtnAlignInvertedSides;
	TToolButton *BtnAlignCurve;
	TToolButton *SliderDivider;
	TToolButton *BtnAlignOverLeft;
	TToolButton *BtnAlignOverTop;
	TToolButton *BtnAlignOverRight;
	TToolButton *BtnAlignOverBottom;
private:	// User declarations
public:		// User declarations
	__fastcall TFrameTransitionTypes(TComponent* Owner);

	void __fastcall SetAlignType(const int iType);

	void __fastcall AdjustWidth(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrameTransitionTypes *FrameTransitionTypes;
//---------------------------------------------------------------------------
#endif

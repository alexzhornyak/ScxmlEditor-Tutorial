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

#ifndef UnitThemeSettingsH
#define UnitThemeSettingsH
//---------------------------------------------------------------------------

#include "LMDInsPropEditors.hpp"

#include "CommonConsts.h"

static const TColor g_CHILD_NORMAL_BORDER_COLOR = TColor(RGB(32, 32, 32));
static const TColor g_STATE_NORMAL_COLOR = TColor(0x00FFD680);
static const TColor g_STATE_INITIAL_BORDER_COLOR = TColor(0x000080FF);
static const TFontStyles g_STATE_SELF_CONNECTION_INSIDE_FONT_STYLE = TFontStyles() << fsUnderline;
static const int g_STATE_SELF_CONNECTION_INSIDE_FONT_STYLE_INT = 4; // Require recalculation from Int if changed !!!
static const TColor g_PARALLEL_NORMAL_COLOR = TColor(0x00CAFFCA);

class TThemeSettings: public TPersistent {
__published:
	virtual bool __fastcall OnFilterPropEvent(const UnicodeString &sPropName);
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	TColor FStateNormalColor;
	TColor FStateClusterColor;
	TColor FStateInitialBorderColor;
	TColor FStateExaminedBorderColor;
	TColor FStateNormalBorderColor;
	TPenStyle FStateNormalBorderStyle;
	TFontStyles FStateNormalFontStyle;
	TFontStyles FStateSelfConnectionInsideFontStyle;

	int FStateDefaultWidth;
	SET_PROPERTY_MIN(StateDefaultWidth, int, 20);
	int FStateDefaultHeight;
	SET_PROPERTY_MIN(StateDefaultHeight, int, 10);

	TColor FScxmlNormalHeadColor;
	TColor FVirtualNormalColor;
	TColor FVirtualHeadColor;
	TColor FScxmlDatamodelFontColor;
	TColor FScxmlVirtualFontColor;

	TColor FParallelNormalColor;

	TColor FChildNormalColor;
	TFontStyles FChildNormalFontStyle;
	TColor FChildNormalBorderColor;
	TPenStyle FChildNormalBorderStyle;

	int FTransitionLineWidth;
	SET_PROPERTY_RANGE(TransitionLineWidth, int, 1, 20);

	int FTransitionFromCircleSize;
	SET_PROPERTY_RANGE(TransitionFromCircleSize, int, 7, 40);

protected:

public:
	__fastcall TThemeSettings();
	virtual __fastcall ~TThemeSettings(void);

	virtual void __fastcall Assign(TPersistent* Source);

__published:
	 __property TColor StateNormalColor = {read=FStateNormalColor, write=FStateNormalColor, default=g_STATE_NORMAL_COLOR};
	 __property TColor StateClusterColor = {read=FStateClusterColor, write=FStateClusterColor, default=clWhite};
	 __property TColor StateInitialBorderColor = {read=FStateInitialBorderColor, write=FStateInitialBorderColor, default=g_STATE_INITIAL_BORDER_COLOR};
	 __property TColor StateExaminedBorderColor = {read=FStateExaminedBorderColor, write=FStateExaminedBorderColor, default=clBlue};
	 __property TColor StateNormalBorderColor = {read=FStateNormalBorderColor, write=FStateNormalBorderColor, default=clBlack};
	 __property TPenStyle StateNormalBorderStyle = {read=FStateNormalBorderStyle, write=FStateNormalBorderStyle, default=psSolid};
	 __property TFontStyles StateNormalFontStyle = {read=FStateNormalFontStyle, write=FStateNormalFontStyle, default=0};
	 __property TFontStyles StateSelfConnectionInsideFontStyle = {read=FStateSelfConnectionInsideFontStyle, write=FStateSelfConnectionInsideFontStyle, default=g_STATE_SELF_CONNECTION_INSIDE_FONT_STYLE_INT};

	 __property int StateDefaultWidth = {read=FStateDefaultWidth, write=SetStateDefaultWidth, default=100};
	 __property int StateDefaultHeight = {read=FStateDefaultHeight, write=SetStateDefaultHeight, default=50};

	 __property TColor ChildNormalColor = {read=FChildNormalColor, write=FChildNormalColor, default=clWhite};
	 __property TFontStyles ChildNormalFontStyle = {read=FChildNormalFontStyle, write=FChildNormalFontStyle, default=0};
	 __property TColor ChildNormalBorderColor = {read=FChildNormalBorderColor, write=FChildNormalBorderColor, default=g_CHILD_NORMAL_BORDER_COLOR};
	 __property TPenStyle ChildNormalBorderStyle = {read=FChildNormalBorderStyle, write=FChildNormalBorderStyle, default=psSolid};

	 __property TColor ScxmlNormalHeadColor = {read=FScxmlNormalHeadColor, write=FScxmlNormalHeadColor, default=clMoneyGreen};
	 __property TColor ScxmlDatamodelFontColor = {read=FScxmlDatamodelFontColor, write=FScxmlDatamodelFontColor, default=clMaroon};
	 __property TColor ScxmlVirtualFontColor = {read=FScxmlVirtualFontColor, write=FScxmlVirtualFontColor, default=clBlue};

	 __property TColor ParallelNormalColor = {read=FParallelNormalColor, write=FParallelNormalColor, default=g_PARALLEL_NORMAL_COLOR};

	 __property TColor VirtualNormalColor = {read=FVirtualNormalColor, write=FVirtualNormalColor, default=clSilver};
	 __property TColor VirtualHeadColor = {read=FVirtualHeadColor, write=FVirtualHeadColor, default=clGray};

	 __property int TransitionLineWidth = {read=FTransitionLineWidth, write=SetTransitionLineWidth, default=1};
	 __property int TransitionFromCircleSize = {read=FTransitionFromCircleSize, write=SetTransitionFromCircleSize, default=10};
};

/* PROP EDITORS */
//---------------------------------------------------------------------------
class TThemeSettingsPropEditor : public TLMDClassPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
};

#endif

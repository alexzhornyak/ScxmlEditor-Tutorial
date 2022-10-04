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

#ifndef UnitSVGSettingsH
#define UnitSVGSettingsH
//---------------------------------------------------------------------------

#include "LMDInsPropEditors.hpp"

typedef enum {ssrAuto, ssrOptimizeSpeed, ssrCrispEdges, ssrGeometricPrecision} TExportSvgShapeRendering;
typedef enum { strAuto, strOptimizeSpeed, strOptimizeLegibility, strGeometricPrecision} TExportSvgTextRendering;

class TExportSvgSettings: public TPersistent {
__published:
	virtual void __fastcall OnGetPropEditorClass(TPersistent *AInstance, ILMDProperty *APropInfo,TLMDPropEditorClass &AEditorClass);
private:
	TExportSvgShapeRendering FExportSvgShapeRendering;
	TExportSvgTextRendering FExportSvgTextRendering;
	bool FExportSvgTextFontUsePoints;
	UnicodeString FExportSvgExtraParams;
	void __fastcall SetExportSvgExtraParams(UnicodeString val);
protected:

public:
	__fastcall TExportSvgSettings();

	virtual void __fastcall Assign(TPersistent* Source);

	UnicodeString __fastcall GetSvgParams(void);

__published:

	__property TExportSvgShapeRendering ExportSvgShapeRendering = {read=FExportSvgShapeRendering, write=FExportSvgShapeRendering, default=TExportSvgShapeRendering::ssrAuto};
	__property TExportSvgTextRendering ExportSvgTextRendering = {read=FExportSvgTextRendering, write=FExportSvgTextRendering, default=TExportSvgTextRendering::strAuto};
	__property bool ExportSvgTextFontUsePoints = {read=FExportSvgTextFontUsePoints, write=FExportSvgTextFontUsePoints, default=false};
	__property UnicodeString ExportSvgExtraParams = {read=FExportSvgExtraParams, write=SetExportSvgExtraParams};

};

//---------------------------------------------------------------------------
class TExportSvgSettingsPropEditor : public TLMDClassPropEditor {
protected:
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
};

#endif

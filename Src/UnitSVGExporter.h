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

#ifndef UnitSVGExporterH
#define UnitSVGExporterH
//---------------------------------------------------------------------------

#include <TeeTree.hpp>
#include "TreeSVGCanvas.hpp"

class TTreeSVGCanvasEx: public TTreeSVGCanvas
{
	TRect FInitWindowRect;

protected:
	virtual System::UnicodeString __fastcall HeaderContents(void);
public:
	__fastcall TTreeSVGCanvasEx(const UnicodeString sUnitName, Classes::TStrings* AStrings);

	const UnicodeString UnitName;

	// начало создания SVG
    Types::TRect __fastcall InitWindow(Graphics::TCanvas* DestCanvas, Tecanvas::TView3DOptions* A3DOptions,
										Graphics::TColor ABackColor, bool Is3D, const Types::TRect &UserRect);

	virtual void __fastcall Draw(int X, int Y, Graphics::TGraphic* Graphic);
	virtual void __fastcall StretchDraw(const Types::TRect &Rect, Graphics::TGraphic* Graphic);
	virtual void __fastcall RotateLabel(int x, int y, const System::UnicodeString St, double RotDegree);

	virtual void __fastcall StartGroup(const UnicodeString &sID, const UnicodeString  &sClass);
	virtual void __fastcall EndGroup(void);

	virtual void __fastcall AddDesc(const UnicodeString &sDescription);

	void __fastcall DrawPolyBezier(const TCurvePoints &tmpCurve, const int tmpNum);

};

extern void __fastcall SaveTreeToSvg(TCustomTeePanel *APanel, const UnicodeString &sUnitName, const TRect &ARect, TStringList *AOutputList);

#endif

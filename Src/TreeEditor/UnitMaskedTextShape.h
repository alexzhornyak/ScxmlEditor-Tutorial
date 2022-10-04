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

#ifndef UnitMaskedTextShapeH
#define UnitMaskedTextShapeH
//---------------------------------------------------------------------------

#undef DrawText

#include <TeeTree.hpp>

class TMaskedTextShape : public TTreeNodeShape {
protected:

	virtual void __fastcall DrawRotatedLabel(Tecanvas::TCanvas3D* ACanvas, int iX, int iY, int iAngle, System::UnicodeString sText);

	UnicodeString FMask;
	int FMaskPos;

	bool FAlwaysBold;

public:
	__fastcall TMaskedTextShape(Classes::TComponent* AOwner);

	void __fastcall SetMask(const UnicodeString &sMask, const int iMaskPos, const bool bAlwaysBold = false);

	__property int MaskPos = {read=FMaskPos};
	__property UnicodeString Mask = {read=FMask};
};

class TAnchorTextShape : public TTreeNodeShape {
protected:

	virtual void __fastcall DrawRotatedLabel(Tecanvas::TCanvas3D* ACanvas, int iX, int iY, int iAngle, System::UnicodeString sText);

	UnicodeString FFilePath;
	int FItemIndex;
	bool FItemSelected;

public:
	__fastcall TAnchorTextShape(Classes::TComponent* AOwner);

	void __fastcall SetFilePath(const UnicodeString &sFilePath, const int iItemIndex, const bool bSelected);

	__property UnicodeString FilePath = {read=FFilePath};
	__property bool ItemSelected = {read=FItemSelected};
};

#endif

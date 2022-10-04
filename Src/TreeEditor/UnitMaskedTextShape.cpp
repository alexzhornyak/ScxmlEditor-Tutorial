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

#include "UnitMaskedTextShape.h"

#include <memory>

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TMaskedTextShape::TMaskedTextShape(Classes::TComponent* AOwner) : TTreeNodeShape(AOwner), FMask(L""), FMaskPos(0),
FAlwaysBold(false) {
}

//---------------------------------------------------------------------------
void __fastcall TMaskedTextShape::DrawRotatedLabel(Tecanvas::TCanvas3D* ACanvas, int iX, int iY, int iAngle, System::UnicodeString sText) {

	if (FMaskPos == 0 || FMask.IsEmpty() || sText.IsEmpty()) {
		/* Do not use native draw!!! Text Antialias problem */

		if (FAlwaysBold) {
			ACanvas->Font->Style = ACanvas->Font->Style << fsBold;
		}

		ACanvas->TextOut3D(iX, iY, TeeTreeZ, sText);
	}
	else {

		const int iMatchLength = FMask.Length();

		int iTextOffset = 0;

		const UnicodeString sPre = (FMaskPos - 1) > 0 ? sText.SubString(1, FMaskPos - 1) : UnicodeString(L"");
		const UnicodeString sMatch = sText.SubString(FMaskPos, iMatchLength);
		const UnicodeString sSuffix = (FMaskPos + iMatchLength - 1) < sText.Length() ? sText.Delete(1, FMaskPos + iMatchLength - 1)
		: UnicodeString(L"");

		if (!sPre.IsEmpty()) {
			ACanvas->Font->Style = TFontStyles();
			if (FAlwaysBold) {
				ACanvas->Font->Style = ACanvas->Font->Style << fsBold;
			}

			ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, sPre);

			iTextOffset += ACanvas->TextWidth(sPre);
		}

		if (!sMatch.IsEmpty()) {
			ACanvas->Font->Style = TFontStyles() << fsBold << fsUnderline;

			ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, sMatch);

			iTextOffset += ACanvas->TextWidth(sMatch);
		}

		if (!sSuffix.IsEmpty()) {
			ACanvas->Font->Style = TFontStyles();
			if (FAlwaysBold) {
				ACanvas->Font->Style = ACanvas->Font->Style << fsBold;
			}

			ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, sSuffix);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TMaskedTextShape::SetMask(const UnicodeString &sMask, const int iMaskPos, const bool bAlwaysBold/* = false*/) {
	FMask = sMask;
	FMaskPos = iMaskPos;
	FAlwaysBold = bAlwaysBold;
}

//---------------------------------------------------------------------------
__fastcall TAnchorTextShape::TAnchorTextShape(Classes::TComponent* AOwner) : TTreeNodeShape(AOwner), FFilePath(L""), FItemIndex(0),
FItemSelected(false) {
}

//---------------------------------------------------------------------------
void __fastcall TAnchorTextShape::DrawRotatedLabel(Tecanvas::TCanvas3D* ACanvas, int iX, int iY, int iAngle, System::UnicodeString sText) {

	if (FFilePath.IsEmpty()) {
		/* Do not use native draw!!! Text Antialias problem */
		ACanvas->TextOut3D(iX, iY, TeeTreeZ, sText);
	}
	else {
		int iTextOffset = 0;

		const UnicodeString sPre = UnicodeString().sprintf(L"%d. ", FItemIndex);

		TFontStyles AFontStyles = TFontStyles();
		if (FItemSelected) {
			AFontStyles << fsBold;
		}

		ACanvas->Font->Style = AFontStyles;
		ACanvas->Font->Color = clWindowText;

		ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, sPre);
		iTextOffset += ACanvas->TextWidth(sPre);

		AFontStyles = AFontStyles << fsUnderline;
		ACanvas->Font->Style = AFontStyles;
		ACanvas->Font->Color = clBlue;

		ACanvas->TextOut3D(iX + iTextOffset, iY, TeeTreeZ, FFilePath);

	}
}

//---------------------------------------------------------------------------
void __fastcall TAnchorTextShape::SetFilePath(const UnicodeString &sFilePath, const int iItemIndex, const bool bSelected) {
	FFilePath = sFilePath;
	FItemIndex = iItemIndex;
	FItemSelected = bSelected;
	this->SimpleText = UnicodeString().sprintf(L"%d. %s", FItemIndex, FFilePath.c_str());
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

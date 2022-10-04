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

#include "VectorDrawUtils.h"

#include <memory>
#include <TeeTree.hpp>
#include "Log4cpp_VCL.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Vectordrawutils {

	void ResetPenBrush(Tecanvas::TCanvas3D* ACanvas) {
		std::auto_ptr<TBrush>ABrushPtr(new TBrush());
		std::auto_ptr<TPen>APenPtr(new TPen());

		ACanvas->Brush->Assign(ABrushPtr.get());
		ACanvas->Pen->Assign(APenPtr.get());
	}

	void DrawScriptBody(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY, const int iTopMargin/* = 0*/,
		const int iBottomMargin/* = 0*/) {

		{
			TPoint APoints[6];
			APoints[0] = Point(iX + 10, iY + 15 - iBottomMargin);
			APoints[1] = Point(iX + 10, iY + 12 - iBottomMargin);
			APoints[2] = Point(iX + 1, iY + 12 - iBottomMargin);
			APoints[3] = Point(iX + 0, iY + 13 - iBottomMargin);
			APoints[4] = Point(iX + 0, iY + 14 - iBottomMargin);
			APoints[5] = Point(iX + 1, iY + 15 - iBottomMargin);
			ACanvas->PolygonWithZ(APoints, 5, TeeTreeZ);
		}

		{
			TPoint APoints[9];
			APoints[0] = Point(iX + 2, iY + 12 - iBottomMargin);
			APoints[1] = Point(iX + 2, iY + iTopMargin + 1);
			APoints[2] = Point(iX + 3, iY + iTopMargin);
			APoints[3] = Point(iX + 12, iY + iTopMargin);
			APoints[4] = Point(iX + 12, iY + 14 - iBottomMargin);
			APoints[5] = Point(iX + 11, iY + 15 - iBottomMargin);
			APoints[6] = Point(iX + 11, iY + 15 - iBottomMargin);
			APoints[7] = Point(iX + 10, iY + 15 - iBottomMargin);
			APoints[8] = Point(iX + 10, iY + 12 - iBottomMargin);
			ACanvas->PolygonWithZ(APoints, 8, TeeTreeZ);
		}

		{
			TPoint APoints[6];
			APoints[0] = Point(iX + 12, iY + iTopMargin);
			APoints[1] = Point(iX + 13, iY + iTopMargin);
			APoints[2] = Point(iX + 14, iY + iTopMargin + 1);
			APoints[3] = Point(iX + 14, iY + iTopMargin + 2);
			APoints[4] = Point(iX + 13, iY + iTopMargin + 3);
			APoints[5] = Point(iX + 12, iY + iTopMargin + 3);
			ACanvas->PolygonWithZ(APoints, 5, TeeTreeZ);
		}
	}

	void DrawComputer(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ACanvas->Pen->Color = clGray;
		ACanvas->Brush->Color = TColor(RGB(219, 222, 216));

		{
			TPoint APoints[8];
			APoints[0] = Point(iX + 0, iY + 1);
			APoints[1] = Point(iX + 1, iY + 0);
			APoints[2] = Point(iX + 10, iY + 0);
			APoints[3] = Point(iX + 11, iY + 1);
			APoints[4] = Point(iX + 11, iY + 7);
			APoints[5] = Point(iX + 10, iY + 8);
			APoints[6] = Point(iX + 1, iY + 8);
			APoints[7] = Point(iX + 0, iY + 7);
			ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);
		}

		{
			TPoint APoints[6];
			APoints[0] = Point(iX + 0, iY + 11);
			APoints[1] = Point(iX + 0, iY + 10);
			APoints[2] = Point(iX + 1, iY + 9);
			APoints[3] = Point(iX + 10, iY + 9);
			APoints[4] = Point(iX + 11, iY + 10);
			APoints[5] = Point(iX + 11, iY + 11);
			ACanvas->PolygonWithZ(APoints, 5, TeeTreeZ);
		}

		ACanvas->Pen->Color = TColor(RGB(144, 87, 0));
		ACanvas->Brush->Color = TColor(RGB(210, 189, 154));
		ACanvas->RectangleWithZ(TRect(iX + 2, iY + 2, iX + 9, iY + 6), TeeTreeZ);
	}

	void DrawDiamond(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY, const int iLength) {

		TPoint APointsEdge[2];
		APointsEdge[0] = Point(iX + 0, iY + 0);
		APointsEdge[1] = Point(iX + iLength, iY + iLength);

		const TPointArray APointArrayEdge = ACanvas->Calc3DPoints(APointsEdge, 1, TeeTreeZ);
		int iEdgeWidth = APointArrayEdge[1].x - APointArrayEdge[0].x;
		int iEdgeHeight = APointArrayEdge[1].y - APointArrayEdge[0].y;
		if (iEdgeWidth % 2 == 0) {
			iEdgeWidth -= 1;
			iEdgeHeight = iEdgeWidth;
		}
		else if (iEdgeHeight % 2 == 0) {
			iEdgeHeight -= 1;
			iEdgeWidth = iEdgeHeight;
		}
		const int iEdge = std::max(iEdgeWidth, iEdgeHeight);

		TPoint APoints[4];
		APoints[0] = Point(APointArrayEdge[0].x + 0, APointArrayEdge[0].y + iEdge);
		APoints[1] = Point(APointArrayEdge[0].x + iEdge, APointArrayEdge[0].y + 0);
		APoints[2] = Point(APointArrayEdge[0].x + iEdge * 2, APointArrayEdge[0].y + iEdge);
		APoints[3] = Point(APointArrayEdge[0].x + iEdge, APointArrayEdge[0].y + iEdge * 2);

		ACanvas->Polygon(APoints, 3);

	}

	void DrawRectangleCut(Tecanvas::TCanvas3D*ACanvas, const TRect &ARect, const int iEdge) {
		TPoint APoints[8];
		APoints[0] = Point(ARect.left, ARect.top + iEdge);
		APoints[1] = Point(ARect.left + iEdge, ARect.top);
		APoints[2] = Point(ARect.right - iEdge, ARect.top);
		APoints[3] = Point(ARect.right, ARect.top + iEdge);
		APoints[4] = Point(ARect.right, ARect.bottom - iEdge);
		APoints[5] = Point(ARect.right - iEdge, ARect.bottom);
		APoints[6] = Point(ARect.left + iEdge, ARect.bottom);
		APoints[7] = Point(ARect.left, ARect.bottom - iEdge);
		ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);
	}

	void DrawCircle(Tecanvas::TCanvas3D*ACanvas, const int iCenterX, const int iCenterY, const int iRadius) {
		ACanvas->EllipseWithZ(iCenterX - iRadius, iCenterY - iRadius, iCenterX + iRadius, iCenterY + iRadius, TeeTreeZ);
	}

	void DrawFolderBig(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {
		TPoint APoints[9];
		APoints[0] = Point(iX + 0, iY + 1);
		APoints[1] = Point(iX + 1, iY + 0);
		APoints[2] = Point(iX + 6, iY + 0);
		APoints[3] = Point(iX + 8, iY + 2);
		APoints[4] = Point(iX + 11, iY + 2);
		APoints[5] = Point(iX + 12, iY + 3);
		APoints[6] = Point(iX + 12, iY + 11);
		APoints[7] = Point(iX + 1, iY + 11);
		APoints[8] = Point(iX + 0, iY + 10);
		ACanvas->PolygonWithZ(APoints, 8, TeeTreeZ);
	}

	void DrawFolderSmall(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {
		TPoint APoints[8];
		APoints[0] = Point(iX + 0, iY + 8);
		APoints[1] = Point(iX + 0, iY + 2);
		APoints[2] = Point(iX + 1, iY + 1);
		APoints[3] = Point(iX + 5, iY + 1);
		APoints[4] = Point(iX + 6, iY + 0);
		APoints[5] = Point(iX + 10, iY + 0);
		APoints[6] = Point(iX + 11, iY + 1);
		APoints[7] = Point(iX + 11, iY + 8);
		ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);
	}

	//---------------------------------------------------------------------------
	//---------------------- ÑÏÅÖÈÀËÈÇÈÐÎÂÀÍÍÛÅ ÔÈÃÓÐÛ --------------------------
	//---------------------------------------------------------------------------

	void DrawDatamodel(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(152, 136, 18));
		ACanvas->Brush->Color = TColor(RGB(240, 227, 141));

		const int iX1 = iX + 1;
		const int iY1 = iY + 1;

		const int iWidth = 13;
		const int iHeight = 4;

		for (int i = 0; i < 3; i++) {
			DrawRectangleCut(ACanvas, TRect(iX1, iY1 + i * iHeight, iX1 + iWidth, iY1 + iHeight + i * iHeight), 1);
		}
	}

	void DrawData(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clSilver;
		ACanvas->Brush->Color = clWhite;

		{
			TPoint APoints[8];
			APoints[0] = Point(iX + 1, iY + 3);
			APoints[1] = Point(iX + 3, iY + 1);
			APoints[2] = Point(iX + 9, iY + 1);
			APoints[3] = Point(iX + 11, iY + 3);
			APoints[4] = Point(iX + 11, iY + 11);
			APoints[5] = Point(iX + 9, iY + 13);
			APoints[6] = Point(iX + 3, iY + 13);
			APoints[7] = Point(iX + 1, iY + 11);
			ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);
		}

		{
			TPoint APoints[6];
			APoints[0] = Point(iX + 1, iY + 3);
			APoints[1] = Point(iX + 3, iY + 1);
			APoints[2] = Point(iX + 9, iY + 1);
			APoints[3] = Point(iX + 11, iY + 3);
			APoints[4] = Point(iX + 9, iY + 5);
			APoints[5] = Point(iX + 3, iY + 5);
			ACanvas->PolygonWithZ(APoints, 5, TeeTreeZ);
		}

		ACanvas->Pen->Color = clGreen;
		ACanvas->Brush->Color = TColor(RGB(177, 211, 144));
		ACanvas->EllipseWithZ(iX + 8, iY + 8, iX + 14, iY + 14, TeeTreeZ);

		ACanvas->Pen->Color = clWhite;
		ACanvas->Brush->Style = bsClear;
		ACanvas->LineWithZ(iX + 9, iY + 11, iX + 13, iY + 11, TeeTreeZ);
		ACanvas->LineWithZ(iX + 11, iY + 9, iX + 11, iY + 13, TeeTreeZ);
	}

	void DrawAssign(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(109, 109, 109));
		ACanvas->Brush->Color = TColor(RGB(234, 234, 234));

		{
			const int iX1 = iX + 1;
			const int iY1 = iY + 1;

			const int iWidth = 11;
			const int iHeight = 4;

			for (int i = 0; i < 3; i++) {
				DrawRectangleCut(ACanvas, TRect(iX1, iY1 + i * iHeight, iX1 + iWidth, iY1 + iHeight + i * iHeight), 1);
			}
		}

		ACanvas->Pen->Color = TColor(RGB(29, 79, 203));
		ACanvas->Brush->Color = TColor(RGB(95, 229, 255));

		TPoint APoints[11];
		APoints[0] = Point(iX + 10, iY + 15);
		APoints[1] = Point(iX + 6, iY + 11);
		APoints[2] = Point(iX + 7, iY + 10);
		APoints[3] = Point(iX + 8, iY + 10);
		APoints[4] = Point(iX + 8, iY + 7);
		APoints[5] = Point(iX + 9, iY + 6);
		APoints[6] = Point(iX + 11, iY + 6);
		APoints[7] = Point(iX + 12, iY + 7);
		APoints[8] = Point(iX + 12, iY + 10);
		APoints[9] = Point(iX + 13, iY + 10);
		APoints[10] = Point(iX + 14, iY + 11);
		ACanvas->PolygonWithZ(APoints, 10, TeeTreeZ);
	}

	void DrawLog(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(237, 176, 11));
		ACanvas->Brush->Color = TColor(RGB(253, 243, 194));
		ACanvas->Rectangle(iX + 5, iY + 8, iX + 9, iY + 12, TeeTreeZ);
		ACanvas->Pixels3D[iX + 7][iY + 10][TeeTreeZ] = ACanvas->Pen->Color;

		ACanvas->Pen->Color = TColor(RGB(86, 127, 189));
		ACanvas->Brush->Color = TColor(RGB(139, 181, 228));

		TPoint APoints[8];
		APoints[0] = Point(iX + 8, iY + 6);
		APoints[1] = Point(iX + 11, iY + 3);
		APoints[2] = Point(iX + 12, iY + 3);
		APoints[3] = Point(iX + 14, iY + 5);
		APoints[4] = Point(iX + 14, iY + 6);
		APoints[5] = Point(iX + 11, iY + 9);
		APoints[6] = Point(iX + 10, iY + 9);
		APoints[7] = Point(iX + 8, iY + 7);
		ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);

		ACanvas->Pixels3D[iX + 11][iY + 6][TeeTreeZ] = ACanvas->Pen->Color;

		ACanvas->Brush->Style = bsClear;

		ACanvas->Pen->Color = TColor(RGB(0, 0, 0));
		ACanvas->LineWithZ(iX + 1, iY + 2, iX + 11, iY + 2, TeeTreeZ);

		ACanvas->Pen->Color = TColor(RGB(64, 64, 64));
		ACanvas->LineWithZ(iX + 1, iY + 4, iX + 9, iY + 4, TeeTreeZ);

		ACanvas->Pen->Color = TColor(RGB(128, 128, 128));
		ACanvas->LineWithZ(iX + 1, iY + 6, iX + 7, iY + 6, TeeTreeZ);

		ACanvas->Pen->Color = TColor(RGB(192, 192, 192));
		ACanvas->LineWithZ(iX + 1, iY + 13, iX + 5, iY + 13, TeeTreeZ);
	}

	void DrawScript(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(4, 121, 179));
		ACanvas->Brush->Color = TColor(RGB(221, 238, 245));

		TPoint APoints[5];
		APoints[0] = Point(iX + 3, iY + 1);
		APoints[1] = Point(iX + 9, iY + 1);
		APoints[2] = Point(iX + 13, iY + 5);
		APoints[3] = Point(iX + 13, iY + 14);
		APoints[4] = Point(iX + 3, iY + 14);
		ACanvas->PolygonWithZ(APoints, 4, TeeTreeZ);

		ACanvas->Brush->Style = bsClear;

		ACanvas->LineWithZ(iX + 9, iY + 1, iX + 9, iY + 5, TeeTreeZ);
		ACanvas->LineWithZ(iX + 9, iY + 5, iX + 13, iY + 5, TeeTreeZ);

		for (int i = 0; i < 3; i++) {
			ACanvas->LineWithZ(iX + 5, iY + 7 + i * 2, iX + 11 + 1, iY + 7 + i * 2, TeeTreeZ);
		}
	}

	void DrawScript2(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		const int iY1 = iY;

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(4, 121, 179));
		ACanvas->Brush->Color = TColor(RGB(221, 238, 245));

		DrawScriptBody(ACanvas, iX, iY, 1, 1);
	}

	void DrawSend(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		/* Drawing */

		const int iY1 = iY + 2;

		// ïèñüìî
		ACanvas->Pen->Color = TColor(RGB(174, 174, 174));
		ACanvas->Brush->Color = TColor(RGB(240, 240, 240));
		const TRect ALetterRect(iX + 0, iY1 + 1, iX + 12, iY1 + 10);
		ACanvas->RectangleWithZ(ALetterRect, TeeTreeZ);
		ACanvas->LineWithZ(iX + 0, iY1 + 1, iX + 6, iY1 + 6, TeeTreeZ);
		ACanvas->LineWithZ(iX + 12, iY1 + 1, iX + 6, iY1 + 6, TeeTreeZ);
		ACanvas->Pen->Color = TColor(RGB(200, 200, 200));
		ACanvas->LineWithZ(iX + 1, iY1 + 9, iX + 5, iY1 + 6, TeeTreeZ);
		ACanvas->LineWithZ(iX + 11, iY1 + 9, iX + 7, iY1 + 6, TeeTreeZ);

		// ñòðåëî÷êà
		TPoint APoints[7];
		APoints[0] = Point(iX + 7, iY1 + 9);
		APoints[1] = Point(iX + 12, iY1 + 9);
		APoints[2] = Point(iX + 12, iY1 + 7);
		APoints[3] = Point(iX + 15, iY1 + 10);
		APoints[4] = Point(iX + 12, iY1 + 13);
		APoints[5] = Point(iX + 12, iY1 + 11);
		APoints[6] = Point(iX + 7, iY1 + 11);
		ACanvas->Pen->Color = TColor(RGB(105, 150, 0));
		ACanvas->Brush->Color = TColor(RGB(147, 204, 39));
		ACanvas->PolygonWithZ(APoints, 6, TeeTreeZ);
	}

	void DrawIf(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Color = TColor(RGB(0, 162, 232));

		DrawDiamond(ACanvas, iX, iY, 7);
	}

	void DrawRaise(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		const int iEdge = 7;

		{
			ACanvas->Pen->Color = TColor(RGB(191, 120, 54));
			ACanvas->Brush->Color = TColor(RGB(255, 200, 0));

			for (int i = 0; i < 2; i++) {

				const int iOffset = i * (iEdge - 1);

				TRect ARect(iX + iOffset, iY + iOffset, iX + iEdge + iOffset, iY + iEdge + iOffset);

				ACanvas->RectangleWithZ(ARect, TeeTreeZ);
			}
		}

		{
			ACanvas->Pen->Color = TColor(RGB(4, 121, 179));
			ACanvas->Brush->Color = TColor(RGB(210, 234, 255));

			const int iOffset = 3;

			TRect ARect(iX + iOffset, iY + iOffset, iX + iEdge + iOffset, iY + iEdge + iOffset);

			ACanvas->RectangleWithZ(ARect, TeeTreeZ);
		}

	}

	void DrawForeach(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(94, 104, 104));
		ACanvas->Brush->Color = TColor(RGB(218, 227, 227));

		TPoint APoints[8];
		APoints[0] = Point(iX + 0, iY + 2);
		APoints[1] = Point(iX + 1, iY + 1);
		APoints[2] = Point(iX + 13, iY + 1);
		APoints[3] = Point(iX + 14, iY + 2);
		APoints[4] = Point(iX + 14, iY + 12);
		APoints[5] = Point(iX + 13, iY + 13);
		APoints[6] = Point(iX + 1, iY + 13);
		APoints[7] = Point(iX + 0, iY + 12);
		ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);

		ACanvas->Sphere(iX + 7, iY + 7, TeeTreeZ, 3);
	}

	void DrawComments(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(194, 126, 64));
		ACanvas->Brush->Color = TColor(RGB(255, 255, 86));

		TPoint APoints[8];
		APoints[0] = Point(iX + 0, iY + 2);
		APoints[1] = Point(iX + 1, iY + 1);
		APoints[2] = Point(iX + 13, iY + 1);
		APoints[3] = Point(iX + 14, iY + 2);
		APoints[4] = Point(iX + 14, iY + 11);
		APoints[5] = Point(iX + 13, iY + 12);
		APoints[6] = Point(iX + 1, iY + 12);
		APoints[7] = Point(iX + 0, iY + 11);
		ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);

		ACanvas->Pen->Color = TColor(RGB(76, 66, 0));
		ACanvas->Brush->Style = bsClear;

		for (int i = 0; i < 4; i++) {
			ACanvas->LineWithZ(iX + 2, iY + 4 + i * 2, iX + 12, iY + 4 + i * 2, TeeTreeZ);
		}
	}

	void DrawExtraContent(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(64, 64, 64));
		ACanvas->Brush->Color = clWhite;

		{
			TPoint APoints[5];
			APoints[0] = Point(iX + 2, iY + 0);
			APoints[1] = Point(iX + 14, iY + 0);
			APoints[2] = Point(iX + 14, iY + 11);
			APoints[3] = Point(iX + 10, iY + 15);
			APoints[4] = Point(iX + 2, iY + 15);
			ACanvas->PolygonWithZ(APoints, 4, TeeTreeZ);
		}

		{
			TPoint APoints[3];
			APoints[0] = Point(iX + 14, iY + 11);
			APoints[1] = Point(iX + 10, iY + 15);
			APoints[2] = Point(iX + 10, iY + 11);

			ACanvas->PolygonWithZ(APoints, 2, TeeTreeZ);
		}

		ACanvas->Brush->Style = bsClear;
		ACanvas->Font->Style = TFontStyles();
		ACanvas->Font->Color = clBlack;
		ACanvas->Font->Name = L"Arial";
		ACanvas->Font->Size = 8;
		ACanvas->TextOut3D(iX + 5, iY + 1, TeeTreeZ, L"?");
	}

	void DrawTransitionXML(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(45, 78, 1));

		{
			ACanvas->Brush->Color = TColor(RGB(129, 223, 76));

			TPoint APoints[8];
			APoints[0] = Point(iX + 2, iY + 3);
			APoints[1] = Point(iX + 3, iY + 2);
			APoints[2] = Point(iX + 12, iY + 2);
			APoints[3] = Point(iX + 13, iY + 3);
			APoints[4] = Point(iX + 13, iY + 12);
			APoints[5] = Point(iX + 12, iY + 13);
			APoints[6] = Point(iX + 3, iY + 13);
			APoints[7] = Point(iX + 2, iY + 12);
			ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);
		}

		{
			TPoint APoints[8];
			APoints[0] = Point(iX + 6, iY + 10);
			APoints[1] = Point(iX + 4, iY + 8);
			APoints[2] = Point(iX + 4, iY + 7);
			APoints[3] = Point(iX + 6, iY + 5);
			APoints[4] = Point(iX + 9, iY + 5);
			APoints[5] = Point(iX + 11, iY + 7);
			APoints[6] = Point(iX + 11, iY + 8);
			APoints[7] = Point(iX + 9, iY + 10);

			ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);

			ACanvas->Pen->Color = TColor(RGB(129, 223, 76));
			ACanvas->Brush->Color = TColor(RGB(129, 223, 76));
			ACanvas->RectangleWithZ(TRect(iX + 7, iY + 5, iX + 8, iY + 10), TeeTreeZ);
		}

	}

	void DrawDoneData(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clSilver;
		ACanvas->Brush->Color = clWhite;

		{
			TPoint APoints[8];
			APoints[0] = Point(iX + 1, iY + 3);
			APoints[1] = Point(iX + 3, iY + 1);
			APoints[2] = Point(iX + 9, iY + 1);
			APoints[3] = Point(iX + 11, iY + 3);
			APoints[4] = Point(iX + 11, iY + 11);
			APoints[5] = Point(iX + 9, iY + 13);
			APoints[6] = Point(iX + 3, iY + 13);
			APoints[7] = Point(iX + 1, iY + 11);
			ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);
		}

		{
			TPoint APoints[6];
			APoints[0] = Point(iX + 1, iY + 3);
			APoints[1] = Point(iX + 3, iY + 1);
			APoints[2] = Point(iX + 9, iY + 1);
			APoints[3] = Point(iX + 11, iY + 3);
			APoints[4] = Point(iX + 9, iY + 5);
			APoints[5] = Point(iX + 3, iY + 5);
			ACanvas->PolygonWithZ(APoints, 5, TeeTreeZ);
		}

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Color = clWhite;
		ACanvas->EllipseWithZ(iX + 8, iY + 8, iX + 14, iY + 14, TeeTreeZ);

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Color = clBlack;
		ACanvas->EllipseWithZ(iX + 10, iY + 10, iX + 12, iY + 12, TeeTreeZ);
	}

	void DrawContent(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clDkGray;
		ACanvas->Brush->Color = TColor(RGB(221, 238, 245));

		DrawScriptBody(ACanvas, iX, iY, 1, 1);

		{
			ACanvas->Pen->Color = TColor(RGB(58, 107, 156));
			ACanvas->Brush->Color = TColor(RGB(50, 147, 223));
			ACanvas->EllipseWithZ(iX + 4, iY + 4, iX + 10, iY + 10, TeeTreeZ);
		}
	}

	void DrawParam(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Color = TColor(RGB(240, 240, 240));

		TPoint APoints[8];
		APoints[0] = Point(iX + 0, iY + 3);
		APoints[1] = Point(iX + 2, iY + 1);
		APoints[2] = Point(iX + 13, iY + 1);
		APoints[3] = Point(iX + 15, iY + 3);
		APoints[4] = Point(iX + 15, iY + 11);
		APoints[5] = Point(iX + 13, iY + 13);
		APoints[6] = Point(iX + 3, iY + 13);
		APoints[7] = Point(iX + 0, iY + 11);
		ACanvas->PolygonWithZ(APoints, 7, TeeTreeZ);

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Style = bsClear;

		for (int i = 0; i < 3; i++) {
			ACanvas->LineWithZ(iX + 3, iY + 5 + i * 2, iX + 12, iY + 5 + i * 2, TeeTreeZ);
		}
	}

	void DrawInvoke(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		DrawComputer(ACanvas, iX + 4, iY + 0);

		DrawComputer(ACanvas, iX + 0, iY + 4);
	}

	void DrawCancel(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clRed;
		ACanvas->Brush->Color = clWhite;
		ACanvas->Rectangle(iX + 2, iY + 2, iX + 13, iY + 11, TeeTreeZ);

		ACanvas->Pen->Color = TColor(RGB(134, 98, 4));
		ACanvas->Brush->Color = TColor(RGB(249, 242, 180));

		TPoint APoints[11];
		APoints[0] = Point(iX + 0, iY + 10);
		APoints[1] = Point(iX + 4, iY + 6);
		APoints[2] = Point(iX + 5, iY + 7);
		APoints[3] = Point(iX + 5, iY + 8);
		APoints[4] = Point(iX + 8, iY + 8);
		APoints[5] = Point(iX + 9, iY + 9);
		APoints[6] = Point(iX + 9, iY + 11);
		APoints[7] = Point(iX + 8, iY + 12);
		APoints[8] = Point(iX + 5, iY + 12);
		APoints[9] = Point(iX + 5, iY + 13);
		APoints[10] = Point(iX + 4, iY + 14);
		ACanvas->PolygonWithZ(APoints, 10, TeeTreeZ);
	}

	void DrawFinalize(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(64, 64, 64));
		ACanvas->Brush->Color = TColor(RGB(245, 245, 245));

		DrawScriptBody(ACanvas, iX, iY, 1, 1);

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Color = clWhite;
		const TPoint APtCenter = Point(iX + 4, iY + 5);
		DrawCircle(ACanvas, APtCenter.x, APtCenter.y, 4);

		ACanvas->Pen->Color = clBlack;
		ACanvas->Brush->Color = clBlack;
		DrawCircle(ACanvas, APtCenter.x, APtCenter.y, 2);

	}

	void DrawTrigger(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = TColor(RGB(66, 103, 163));
		ACanvas->Brush->Color = TColor(RGB(164, 194, 247));

		DrawRectangleCut(ACanvas, TRect(iX + 0, iY + 5, iX + 9, iY + 9), 1);

		ACanvas->Brush->Color = TColor(RGB(227, 231, 242));
		DrawCircle(ACanvas, iX + 11, iY + 7, 4);
	}

	void DrawWatch(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clNavy;
		ACanvas->Brush->Color = TColor(RGB(77, 207, 224));

		DrawDiamond(ACanvas, iX + 0, iY + 0, 7);

		ACanvas->Brush->Color = TColor(RGB(212, 225, 244));
		DrawCircle(ACanvas, iX + 7, iY + 7, 3);
	}

	void DrawVirtualFolder(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY) {

		ResetPenBrush(ACanvas);

		ACanvas->Pen->Color = clGray;
		ACanvas->Brush->Color = clWhite;

		DrawFolderBig(ACanvas, iX + 0, iY + 1);

		ACanvas->Pen->Color = TColor(RGB(84, 84, 95));
		ACanvas->Brush->Color = TColor(RGB(197, 197, 224));

		DrawFolderSmall(ACanvas, iX + 4, iY + 6);
	}
}

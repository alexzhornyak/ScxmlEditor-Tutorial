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

#ifndef VectorDrawUtilsH
#define VectorDrawUtilsH
//---------------------------------------------------------------------------

#include <TeCanvas.hpp>

namespace Vectordrawutils {

	void ResetPenBrush(Tecanvas::TCanvas3D* ACanvas);

	void DrawDiamond(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY, const int iLength);

	void DrawScriptBody(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY, const int iTopMargin = 0, const int iBottomMargin = 0);

	void DrawComputer(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawRectangleCut(Tecanvas::TCanvas3D*ACanvas, const TRect &ARect, const int iEdge);

	void DrawCircle(Tecanvas::TCanvas3D*ACanvas, const int iCenterX, const int iCenterY, const int iRadius);

	void DrawFolderBig(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawFolderSmall(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	/* Специализированные, со сбросом Canvas Pen, Brush  */

	void DrawRoundRectangleWithZ(Tecanvas::TCanvas3D* ACanvas, TRect ARect, const int iRX, const int iRY, const int iZ);

	void DrawArcWithZ(Tecanvas::TCanvas3D* ACanvas, TRect ARect, const double dStartAngle, const double dEndAngle, const int iZ);

	void DrawDatamodel(Tecanvas::TCanvas3D* ACanvas, const int iX, const int iY);

	void DrawData(Tecanvas::TCanvas3D* ACanvas, const int iX, const int iY);

	void DrawAssign(Tecanvas::TCanvas3D* ACanvas, const int iX, const int iY);

	void DrawLog(Tecanvas::TCanvas3D* ACanvas, const int iX, const int iY);

	void DrawScript(Tecanvas::TCanvas3D* ACanvas, const int iX, const int iY);

	void DrawScript2(Tecanvas::TCanvas3D* ACanvas, const int iX, const int iY);

	void DrawSend(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawIf(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawRaise(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawForeach(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawComments(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawExtraContent(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawTransitionXML(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawDoneData(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawContent(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawParam(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawInvoke(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawCancel(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawFinalize(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawTrigger(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawWatch(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);

	void DrawVirtualFolder(Tecanvas::TCanvas3D*ACanvas, const int iX, const int iY);
}

#endif

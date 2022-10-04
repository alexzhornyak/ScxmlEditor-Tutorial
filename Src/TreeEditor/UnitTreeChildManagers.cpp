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

// header
#include "UnitTreeChildManagers.h"

// local
#include "UnitScxmlBaseShape.h"
#include "UnitSVGExporter.h"
// ---------------------------------------------------------------------------

#pragma package(smart_init)

// ---------------------------------------------------------------------------
__fastcall TTreeExplorerAlignRightEx::TTreeExplorerAlignRightEx(void) {
	ArrowToAngle = 180;
}

// ---------------------------------------------------------------------------
// Returns true if parent node has an automatic connection to node.
bool TTreeExplorerAlignRightEx::AutoConnection(Teetree::TTreeNodeShape* ANode, Teetree::TTreeNodeShape* AParent) {
	TTreeConnection *tmp = AParent->Connections->ToShape(ANode);
	return tmp != NULL && (tmp->Style == csAuto);
}

// ---------------------------------------------------------------------------
Types::TPoint __fastcall TTreeExplorerAlignRightEx::CalcXYCross(Teetree::TTreeNodeShape* ANode, Teetree::TTreeNodeShape* AParent) {
	TPoint PtResult;

	assert(ANode != NULL);

	const int iChartBoundsTop = ANode->Tree ? ANode->Tree->ChartBounds.Top : 0;
	const int iChartBoundsLeft = ANode->Tree ? ANode->Tree->ChartBounds.Left : 0;

	PtResult.y = ((ANode->Y0 + ANode->Y1) / 2) + iChartBoundsTop; // YCenter

	if (AParent) {
		if (ANode->AutoPosition->Left || AutoConnection(ANode, AParent)) // optimized.
		{
			PtResult.x = 4 + 1 + AParent->X1 + iChartBoundsLeft + 2 * TeePictureHorizMargin;
			return PtResult;
		}

	}
	else if (ANode->AutoPosition->Left && ANode->Tree) {
		int t = ANode->Tree->Roots->IndexOf(ANode);
		if ((t > 0) && ANode->Tree->Roots->Items[t - 1]->AutoPosition->Left) // is not the first root
		{
			PtResult.x = CalcXYCross(ANode->Tree->Roots->Items[t - 1], NULL).x; // return previous root cross-box
			return PtResult;
		}
	}

	int tmp = 0;

	if (ANode->Tree->CrossBox->Visible)
		tmp = ANode->Tree->CrossBox->Size / 2;

	PtResult.x = ANode->AdjustedRectangle.Right + CrossMargin + tmp;
	return PtResult;
}

// ---------------------------------------------------------------------------
int __fastcall TTreeExplorerAlignRightEx::XPosition(Teetree::TTreeNodeShape* ANode) {
	int result = 0;
	const int iPictureWidth = ANode->GetPicture() ? ANode->GetPicture()->Width : 0;
	bool bShowImages = ANode->Tree ? ANode->Tree->ShowImages : true;
	if (ANode->Parent) // child node
	{
		// indent X from parent
		const int iParentPictureWidth = ANode->Parent->GetPicture() ? ANode->Parent->GetPicture()->Width : 0;
		if (bShowImages && (iParentPictureWidth == 0) && (iPictureWidth > 0))
			result = ANode->Parent->X1 - HorizMargin - iPictureWidth - TeePictureHorizMargin;
		else
			result = ANode->Parent->X1 - HorizMargin;

		result -= ANode->Width;
	}
	else {
		if (ANode->Tree) {
			const int tmp = ANode->Tree->Roots->IndexOf(ANode);
			if ((tmp == 0) || (!ANode->Tree->Roots->Items[tmp - 1]->AutoPosition->Left)) // first tree root
			{
				result = ANode->Tree->ClientWidth - 2 - ANode->Width;

				if (bShowImages && (iPictureWidth > 0))
					result -= iPictureWidth + TeePictureHorizMargin;
				if (AnyRootShouldDrawCross(ANode->Tree))
					result -= ANode->Tree->CrossBox->Size + CrossMargin + 2;
			}
			else
				result = ANode->Tree->Roots->Items[tmp - 1]->X0; // same X as previous root
		}
	}
	return result;
}

// ---------------------------------------------------------------------------
// --------------------------- TScxmlAlignChildEx ----------------------------
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
__fastcall TScxmlAlignChildEx::TScxmlAlignChildEx(void) {

}

// ---------------------------------------------------------------------------
Types::TPoint __fastcall TScxmlAlignChildEx::CalcXYCross(TTreeNodeShape* ANode, TTreeNodeShape* AParent) {

	TPoint AXYCross = TTreeExplorerAlignChild::CalcXYCross(ANode, AParent);

	TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(ANode);
	TVisualScxmlBaseShape *AVisualParent = dynamic_cast<TVisualScxmlBaseShape*>(ANode->Parent);
	if (AChildShape && AVisualParent) {

		const int iXDiff = ANode->AdjustedRectangle.Width() - ANode->Width;

		switch(AVisualParent->ChildrenAlignX) {
		case scaxRight:
			AXYCross.x += AVisualParent->Width - HorizMargin - iXDiff - TeePictureHorizMargin * 2;
			break;
		case scaxCenter:
			AXYCross.x += AVisualParent->Width / 2 - HorizMargin - iXDiff - TeePictureHorizMargin * 2;
			break;
		}

		AXYCross.x += AVisualParent->ChildrenAlignXOffset;
	}

	return AXYCross;
}

// ---------------------------------------------------------------------------
int __fastcall TScxmlAlignChildEx::XPosition(TTreeNodeShape* ANode) {
	TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(ANode);
	TVisualScxmlBaseShape *AVisualParent = dynamic_cast<TVisualScxmlBaseShape*>(ANode->Parent);
	if (AChildShape && AVisualParent) {

		int iXPos = 0;

		switch(AVisualParent->ChildrenAlignX) {
		case scaxRight:
			iXPos = AVisualParent->X1;
			break;
		case scaxCenter:
			iXPos = AVisualParent->XCenter();
			break;
		case scaxLeft:
		default:
			iXPos = TTreeExplorerAlignChild::XPosition(ANode);
			break;
		}

		iXPos += AVisualParent->ChildrenAlignXOffset;

		return iXPos;
	}

	return TTreeExplorerAlignChild::XPosition(ANode);
}

// ---------------------------------------------------------------------------
int __fastcall TScxmlAlignChildEx::YPosition(TTreeNodeShape* ANode, int ABrotherIndex) {

	TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(ANode);
	TVisualScxmlBaseShape *AVisualParent = dynamic_cast<TVisualScxmlBaseShape*>(ANode->Parent);
	if (AChildShape && AVisualParent && ABrotherIndex == 0 && AVisualParent->ChildrenAlignY == scayClusterTop) {
		return AVisualParent->CalculateChildrenTop(AVisualParent->Y1) + VertMargin;
	}
	else
		return TTreeExplorerAlignChild::YPosition(ANode, ABrotherIndex);
}

// ---------------------------------------------------------------------------
int __fastcall TScxmlAlignChildEx::GetStartConnectionTopOffset(TTreeNodeShape* ANodeFrom) {
	TVisualScxmlBaseShape *AVisualFrom = dynamic_cast<TVisualScxmlBaseShape*>(ANodeFrom);
	if (AVisualFrom && AVisualFrom->IsCluster() && AVisualFrom->ChildrenAlignY == scayClusterTop) {
		return AVisualFrom->GetClusterHeight();
	}
	return 0;
}

// ---------------------------------------------------------------------------
bool __fastcall TScxmlAlignChildEx::DrawConnection(TTreeConnection* AConnection) {
	if (AConnection && AConnection->Tree && AConnection->Tree->Canvas && AConnection->FromShape) {
		TTreeSVGCanvasEx * ATreeSVGCanvasEx = dynamic_cast<TTreeSVGCanvasEx*>(AConnection->Tree->Canvas);
		if (ATreeSVGCanvasEx) {
			TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(AConnection->FromShape);
			if (AScxmlBaseShape && AScxmlBaseShape->ExcludeFromSave) {
				return true;
			}
		}
	}

	return Teetree::TTreeExplorerAlignChild::DrawConnection(AConnection);
}

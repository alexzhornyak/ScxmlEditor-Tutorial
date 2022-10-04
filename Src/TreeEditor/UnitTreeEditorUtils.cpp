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

#include "UnitTreeEditorUtils.h"

#include <math.h>
#include "Log4cpp_VCL.hpp"
#include "TreeEdNew.hpp"
#include "UnitScxmlBaseShape.h"
#include "TreeNodeListAccess.h"
#include "UnitStateMachineConnection.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Editorutils {

	// Function to calculate distance
	double DistanceBetweenPoints(double x1, double y1, double x2, double y2) {
		// Calculating distance
		return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
	}

	bool LiangBarsky(double edgeLeft, double edgeRight, double edgeBottom, double edgeTop,
		// Define the x/y clipping values for the border.
		double x0src, double y0src, double x1src, double y1src, // Define the start and end points of the line.
		double &x0clip, double &y0clip, double &x1clip, double &y1clip) // The output values, so declare these outside.
	{

		double t0 = 0.0;
		double t1 = 1.0;
		double xdelta = x1src - x0src;
		double ydelta = y1src - y0src;
		double p, q, r;

		for (int edge = 0; edge < 4; edge++) { // Traverse through left, right, bottom, top edges.
			if (edge == 0) {
				p = -xdelta;
				q = -(edgeLeft - x0src);
			}
			if (edge == 1) {
				p = xdelta;
				q = (edgeRight - x0src);
			}
			if (edge == 2) {
				p = -ydelta;
				q = -(edgeBottom - y0src);
			}
			if (edge == 3) {
				p = ydelta;
				q = (edgeTop - y0src);
			}

			if (p == 0)
				r = 0;
			else
				r = q / p;

			if (p == 0 && q < 0)
				return false; // Don't draw line at all. (parallel line outside)

			if (p < 0) {
				if (r > t1)
					return false; // Don't draw line at all.
				else if (r > t0)
					t0 = r; // Line is clipped!
			}
			else if (p > 0) {
				if (r < t0)
					return false; // Don't draw line at all.
				else if (r < t1)
					t1 = r; // Line is clipped!
			}
		}

		x0clip = x0src + t0 * xdelta;
		y0clip = y0src + t0 * ydelta;
		x1clip = x0src + t1 * xdelta;
		y1clip = y0src + t1 * ydelta;

		if (x0clip == x1clip && y0clip == y1clip) {
			return false; // zero point
		}

		return true; // (clipped) line is drawn
	}

	// ---------------------------------------------------------------------------
	void IncrementRect(TRect &ARect, const int iVal) {
		ARect.Left -= iVal;
		ARect.Top -= iVal;
		ARect.Right += iVal;
		ARect.Bottom += iVal;
	}

	// ---------------------------------------------------------------------------
	TRect IncrementRectCopy(const TRect &ARect, const int iVal) {
		TRect AOut = ARect;
		IncrementRect(AOut, iVal);
		return AOut;
	}

	// ---------------------------------------------------------------------------
	bool PtInRectEx(const TRect &ARect, const TPoint &APoint) {
		return APoint.x >= ARect.Left && APoint.x <= ARect.Right && APoint.y >= ARect.Top && APoint.y <= ARect.Bottom;
	}

	// ---------------------------------------------------------------------------
	bool RectInsideRect(const TRect &AParent, const TRect &AChild) {
		return PtInRectEx(AParent, TPoint(AChild.Left, AChild.Top)) && PtInRectEx(AParent, TPoint(AChild.Right, AChild.Bottom));
	}

	// ---------------------------------------------------------------------------
	bool IsPointInOutsideOffsetRect(const TPoint &APoint, const TRect &ARect, const int iOffset) {
		const TRect UpRect(ARect.Left - iOffset, ARect.Top - iOffset, ARect.Right + iOffset, ARect.Top);
		const TRect DownRect(ARect.Left + iOffset, ARect.Bottom, ARect.Right + iOffset, ARect.Bottom + iOffset);
		const TRect LeftRect(ARect.Left - iOffset, ARect.Top, ARect.Left, ARect.Bottom);
		const TRect RightRect(ARect.Right, ARect.Top, ARect.Right + iOffset, ARect.Bottom);

		return PtInRectEx(UpRect, APoint) || PtInRectEx(DownRect, APoint) || PtInRectEx(LeftRect, APoint) || PtInRectEx(RightRect, APoint);
	}

	bool AreEqualPoints(const TConnectionPointArray &ASource, const TConnectionPointArray &ATarget) {
		if (ASource.Length == ATarget.Length) {
			for (int i = 0; i < ASource.Length; i++) {
				if (ASource[i].X != ATarget[i].X)
					return false;
				if (ASource[i].Y != ATarget[i].Y)
					return false;
				if (ASource[i].XStyle != ATarget[i].XStyle)
					return false;
				if (ASource[i].YStyle != ATarget[i].YStyle)
					return false;
				if (ASource[i].XValue != ATarget[i].XValue)
					return false;
				if (ASource[i].YValue != ATarget[i].YValue)
					return false;
			}
			return true;
		}
		return false;
	}

	// ---------------------------------------------------------------------------
	void RememberPoints(TTreeConnection *AConnection, TConnectionPointArray &AConnSelectedPoints) {
		WARNING_NULL_PTR(AConnection);

		AConnSelectedPoints.Length = AConnection->Points->Count();
		for (int i = 0; i < AConnSelectedPoints.Length; i++) {
			AConnSelectedPoints[i].XStyle = AConnection->Points->Item[i].XStyle;
			AConnSelectedPoints[i].YStyle = AConnection->Points->Item[i].YStyle;
			AConnSelectedPoints[i].XValue = AConnection->Points->Item[i].XValue;
			AConnSelectedPoints[i].YValue = AConnection->Points->Item[i].YValue;
			AConnSelectedPoints[i].X = AConnection->Points->Item[i].X;
			AConnSelectedPoints[i].Y = AConnection->Points->Item[i].Y;
		}
	}

	void RestorePoints(TTreeConnection * AConnection, const TConnectionPointArray & AConnSelectedPoints) {
		WARNING_NULL_PTR(AConnection);

		AConnection->Points->Clear();

		for (int i = 0; i < AConnSelectedPoints.Length; i++) {
			AConnection->Points->Add(AConnSelectedPoints[i].XStyle, AConnSelectedPoints[i].XValue, AConnSelectedPoints[i].YStyle,
				AConnSelectedPoints[i].YValue);
		}
	}

	// ---------------------------------------------------------------------------
	void CalculatePointsPosition(TTreeConnection * AConnection) {
		if (AConnection) {
			for (int i = 0; i < AConnection->Points->Count(); i++) {
				AConnection->Points->CalculatePosition(i);
			}
		}
	}

	// ---------------------------------------------------------------------------
	void ClipLastConnectionPointToShapeBounds(TTreeConnection *AConnection) {
		if (!AConnection)
			return;

		if (!AConnection->ToShape)
			return;

		const int iPointsCount = AConnection->Points->Count();
		if (iPointsCount < 2)
			return;

		const int iPointIndex = iPointsCount - 1;

		double X0clip, X1clip, Y0clip, Y1clip;
		if (Editorutils::LiangBarsky(AConnection->ToShape->X0, AConnection->ToShape->X1, -AConnection->ToShape->Y1,
				-AConnection->ToShape->Y0, AConnection->Points->Item[iPointIndex - 1].X, -AConnection->Points->Item[iPointIndex - 1].Y,
				AConnection->Points->Item[iPointIndex].X, -AConnection->Points->Item[iPointIndex].Y, X0clip, Y0clip, X1clip, Y1clip)) {

			X0clip = (X0clip - AConnection->ToShape->X0) / (AConnection->ToShape->X1 - AConnection->ToShape->X0) * 100.0f;
			Y0clip = (-Y0clip - AConnection->ToShape->Y0) / (AConnection->ToShape->Y1 - AConnection->ToShape->Y0) * 100.0f;

			AConnection->Points->ChangeXStyle(iPointIndex, cpsToPercent);
			AConnection->Points->ChangeYStyle(iPointIndex, cpsToPercent);

			AConnection->Points->Item[iPointIndex].XValue = RoundTo(fabs(X0clip), 0);
			AConnection->Points->Item[iPointIndex].YValue = RoundTo(fabs(Y0clip), 0);
		}
	}

	// ---------------------------------------------------------------------------
	void ClipFirstConnectionPointToShapeBounds(TTreeConnection *AConnection) {
		if (!AConnection)
			return;

		if (!AConnection->FromShape)
			return;

		const int iPointsCount = AConnection->Points->Count();
		if (iPointsCount < 2)
			return;

		const int iPointIndex = 0;

		double X0clip, X1clip, Y0clip, Y1clip;
		if (Editorutils::LiangBarsky(AConnection->FromShape->X0, AConnection->FromShape->X1, -AConnection->FromShape->Y1,
				-AConnection->FromShape->Y0, AConnection->Points->Item[iPointIndex + 1].X, -AConnection->Points->Item[iPointIndex + 1].Y,
				AConnection->Points->Item[iPointIndex].X, -AConnection->Points->Item[iPointIndex].Y, X0clip, Y0clip, X1clip, Y1clip)) {

			X0clip = (X0clip - AConnection->FromShape->X0) / (AConnection->FromShape->X1 - AConnection->FromShape->X0) * 100.0f;
			Y0clip = (-Y0clip - AConnection->FromShape->Y0) / (AConnection->FromShape->Y1 - AConnection->FromShape->Y0) * 100.0f;

			AConnection->Points->ChangeXStyle(iPointIndex, cpsFromPercent);
			AConnection->Points->ChangeYStyle(iPointIndex, cpsFromPercent);

			AConnection->Points->Item[iPointIndex].XValue = RoundTo(fabs(X0clip), 0);
			AConnection->Points->Item[iPointIndex].YValue = RoundTo(fabs(Y0clip), 0);
		}
	}

	// ---------------------------------------------------------------------------
	void EnsurePointInShapeRect(TTreeNodeShape *AShape, TPoint &APoint) {
		if (AShape) {
			if (APoint.x < AShape->X0) {
				APoint.x = AShape->X0;
			}
			else if (APoint.x > AShape->X1) {
				APoint.x = AShape->X1;
			}

			if (APoint.y < AShape->Y0) {
				APoint.y = AShape->Y0;
			}
			else if (APoint.y > AShape->Y1) {
				APoint.y = AShape->Y1;
			}
		}
	}

	// ---------------------------------------------------------------------------
	void AdjustSidesConnection(TTreeConnection *AConnection) {
		WARNING_NULL_PTR(AConnection);

		bool bAdjustable = false;
		if (IsPublishedProp(AConnection, AdjustableSides)) {
			bAdjustable = GetPropValue(AConnection, AdjustableSides, false);
		}

		TTreeNodeShape *AFromShape = AConnection->FromShape;
		TTreeNodeShape *AToShape = AConnection->ToShape;
		if (AFromShape && AToShape && AFromShape->InheritsFrom(__classid(TVisualScxmlBaseShape)) && AToShape->InheritsFrom
			(__classid(TVisualScxmlBaseShape)) && bAdjustable) {

			// необходимо просчитать позиции точек, так как при cpsFromRel, cpsFromPercent и т.д. - эти величины меняются
			CalculatePointsPosition(AConnection);

			const bool bAreBrothers = true /*AFromShape->Level() == AToShape->Level()*/; // если быстродействие упадет, то раскомментарить
			const bool bAreDifferent = AFromShape != AToShape;

			const bool bAreNotNested = RectInsideRect(AFromShape->Bounds(), AToShape->Bounds()) == false;

			if ((AConnection->Style == csSides || AConnection->Style == csInvertedSides) && bAreBrothers && bAreDifferent && bAreNotNested)
			{

				for (int k = 1; k < AConnection->Points->Count(); k++) {
					TPoint AFixed1(AConnection->Points->Item[k - 1].X, AConnection->Points->Item[k - 1].Y);
					TPoint AFixed2(AConnection->Points->Item[k].X, AConnection->Points->Item[k].Y);

					TRect AFixedPointRect(std::min(AFixed1.x, AFixed2.x), std::min(AFixed1.y, AFixed2.y), std::max(AFixed1.x, AFixed2.x),
						std::max(AFixed1.y, AFixed2.y));

					TIsTypes IsTypes;

					struct TClipRectangle {
						double x0clip, y0clip, x1clip, y1clip;
					};

					TClipRectangle ClipArray[isMAXSIZE];
					memset(&ClipArray, 0, sizeof(TClipRectangle));

					switch(AConnection->Style) {
					case csSides:
						if (LiangBarsky(AFromShape->Bounds().Left, AFromShape->Bounds().Right, -AFromShape->Bounds().Bottom,
								-AFromShape->Bounds().Top, AFixed1.x, -AFixed1.y, AFixed1.x, -AFixed2.y, ClipArray[isFromPrev].x0clip,
								ClipArray[isFromPrev].y0clip, ClipArray[isFromPrev].x1clip, ClipArray[isFromPrev].y1clip)) {

							// LOG_DEBUG("1)LiangBarsky=FROM");

							IsTypes = IsTypes << isFromPrev;
						}

						if (LiangBarsky(AFromShape->Bounds().Left, AFromShape->Bounds().Right, -AFromShape->Bounds().Bottom,
								-AFromShape->Bounds().Top, AFixed1.x, -AFixed2.y, AFixed2.x, -AFixed2.y, ClipArray[isFromCur].x0clip,
								ClipArray[isFromCur].y0clip, ClipArray[isFromCur].x1clip, ClipArray[isFromCur].y1clip)) {

							IsTypes = IsTypes << isFromCur;

							// LOG_DEBUG("2)LiangBarsky=FROM");
						}

						if (LiangBarsky(AToShape->Bounds().Left, AToShape->Bounds().Right, -AToShape->Bounds().Bottom,
								-AToShape->Bounds().Top, AFixed1.x, -AFixed1.y, AFixed1.x, -AFixed2.y, ClipArray[isToPrev].x0clip,
								ClipArray[isToPrev].y0clip, ClipArray[isToPrev].x1clip, ClipArray[isToPrev].y1clip)) {

							IsTypes = IsTypes << isToPrev;

							// LOG_DEBUG("1)LiangBarsky=TO");
						}

						if (LiangBarsky(AToShape->Bounds().Left, AToShape->Bounds().Right, -AToShape->Bounds().Bottom,
								-AToShape->Bounds().Top, AFixed1.x, -AFixed2.y, AFixed2.x, -AFixed2.y, ClipArray[isToCur].x0clip,
								ClipArray[isToCur].y0clip, ClipArray[isToCur].x1clip, ClipArray[isToCur].y1clip)) {

							IsTypes = IsTypes << isToCur;

							// LOG_DEBUG("2)LiangBarsky=TO");
						}
						break;
					case csInvertedSides:
						if (LiangBarsky(AFromShape->Bounds().Left, AFromShape->Bounds().Right, -AFromShape->Bounds().Bottom,
								-AFromShape->Bounds().Top, AFixed1.x, -AFixed1.y, AFixed2.x, -AFixed1.y, ClipArray[isFromPrev].x0clip,
								ClipArray[isFromPrev].y0clip, ClipArray[isFromPrev].x1clip, ClipArray[isFromPrev].y1clip)) {

							// LOG_DEBUG("1)LiangBarsky=FROM");

							IsTypes = IsTypes << isFromPrev;
						}

						if (LiangBarsky(AFromShape->Bounds().Left, AFromShape->Bounds().Right, -AFromShape->Bounds().Bottom,
								-AFromShape->Bounds().Top, AFixed2.x, -AFixed1.y, AFixed2.x, -AFixed2.y, ClipArray[isFromCur].x0clip,
								ClipArray[isFromCur].y0clip, ClipArray[isFromCur].x1clip, ClipArray[isFromCur].y1clip)) {

							IsTypes = IsTypes << isFromCur;

							// LOG_DEBUG("2)LiangBarsky=FROM");
						}

						if (LiangBarsky(AToShape->Bounds().Left, AToShape->Bounds().Right, -AToShape->Bounds().Bottom,
								-AToShape->Bounds().Top, AFixed1.x, -AFixed1.y, AFixed2.x, -AFixed1.y, ClipArray[isToPrev].x0clip,
								ClipArray[isToPrev].y0clip, ClipArray[isToPrev].x1clip, ClipArray[isToPrev].y1clip)) {

							IsTypes = IsTypes << isToPrev;

							// LOG_DEBUG("1)LiangBarsky=TO");
						}

						if (LiangBarsky(AToShape->Bounds().Left, AToShape->Bounds().Right, -AToShape->Bounds().Bottom,
								-AToShape->Bounds().Top, AFixed2.x, -AFixed1.y, AFixed2.x, -AFixed2.y, ClipArray[isToCur].x0clip,
								ClipArray[isToCur].y0clip, ClipArray[isToCur].x1clip, ClipArray[isToCur].y1clip)) {

							IsTypes = IsTypes << isToCur;

							// LOG_DEBUG("2)LiangBarsky=TO");
						}
						break;
					}

					/* АНАЛИЗИРУЕМ ПОЛУЧЕННЫЕ ПЕРЕСЕЧЕНИЯ */

					if (IsTypes.Contains(isFromPrev)) {
						switch(AConnection->Style) {
						case csSides:
							AConnection->Points->Move(k - 1, 0, -ClipArray[isFromPrev].y1clip + ClipArray[isFromPrev].y0clip);
							break;
						case csInvertedSides:
							AConnection->Points->Move(k - 1, ClipArray[isFromPrev].x1clip - ClipArray[isFromPrev].x0clip, 0);
							break;
						}

					}
					if (IsTypes.Contains(isFromCur)) {
						switch(AConnection->Style) {
						case csSides:
							AConnection->Points->Move(k - 1, ClipArray[isFromCur].x1clip - ClipArray[isFromCur].x0clip, 0);
							break;
						case csInvertedSides:
							AConnection->Points->Move(k - 1, 0, -ClipArray[isFromCur].y1clip + ClipArray[isFromCur].y0clip);
							break;
						}
					}
					if (IsTypes.Contains(isToPrev)) {
						switch(AConnection->Style) {
						case csSides:
							AConnection->Points->Move(k, 0, ClipArray[isToPrev].y1clip - ClipArray[isToPrev].y0clip);
							break;
						case csInvertedSides:
							AConnection->Points->Move(k, -ClipArray[isToPrev].x1clip + ClipArray[isToPrev].x0clip, 0);
							break;
						}
					}
					if (IsTypes.Contains(isToCur)) {

						switch(AConnection->Style) {
						case csSides:
							AConnection->Points->Move(k, -ClipArray[isToCur].x1clip + ClipArray[isToCur].x0clip, 0);
							break;
						case csInvertedSides:
							AConnection->Points->Move(k, 0, ClipArray[isToCur].y1clip - ClipArray[isToCur].y0clip);
							break;
						}
					}

					// считаем позиции, иначе X,Y содержат неправильные значения
					AConnection->Points->CalculatePosition(k - 1);
					AConnection->Points->CalculatePosition(k);

					// Для очень глубокой отладки!
					//LOG_DEBUG("Point 1[%d,%d], Point 2 [%d,%d]", AConnection->Points->Item[k - 1].X, AConnection->Points->Item[k - 1].Y,
					//AConnection->Points->Item[k].X, AConnection->Points->Item[k].Y);

					const bool bPointsEqual = AConnection->Points->Item[k - 1].X == AConnection->Points->Item[k].X //
					&& AConnection->Points->Item[k - 1].Y == AConnection->Points->Item[k].Y;

					if (!bPointsEqual) {
						bool bStyleChanged = false;

						// анализируем, удачно ли расположились точки после переназначения
						bool bIsOnEdgeFromX = true;
						bool bIsOnEdgeFromY = true;
						bool bIsOnEdgeToX = true;
						bool bIsOnEdgeToY = true;

						if (AConnection->Points->Count() == 2) {
							bIsOnEdgeFromX = AConnection->Points->Item[k].X == AFromShape->X0 || AConnection->Points->Item[k]
								.X == AFromShape->X1;
							bIsOnEdgeFromY = AConnection->Points->Item[k].Y == AFromShape->Y0 || AConnection->Points->Item[k]
								.Y == AFromShape->Y1;

							bIsOnEdgeToX = AConnection->Points->Item[k].X == AToShape->X0 || AConnection->Points->Item[k].X == AToShape->X1;
							bIsOnEdgeToY = AConnection->Points->Item[k].Y == AToShape->Y0 || AConnection->Points->Item[k].Y == AToShape->Y1;
						}

						switch(AConnection->Style) {
						case csSides:
							if (AConnection->Points->Item[k].X == AConnection->Points->Item[k - 1].X &&
								(k == AConnection->Points->Count() - 1)) {
								LOG_DEBUG("Not correct [csSides]!");
								if (AConnection->Points->Count() == 2) {
									AConnection->Style = csInvertedSides;
									AConnection->Draw();
									bStyleChanged = true;
								}
								else {
									if (AConnection->Points->Count() == 3) {
										TConnectionPointArray APointArray;
										Editorutils::RememberPoints(AConnection, APointArray);

										AConnection->Style = csInvertedSides;
										CalculatePointsPosition(AConnection);
										bStyleChanged = true;

										AConnection->Points->Item[0].X = APointArray[0].X;
										AConnection->Points->Item[0].Y = APointArray[0].Y;

										AConnection->Points->Item[1].X = APointArray[2].X;
										AConnection->Points->Item[1].Y = APointArray[2].Y;

										AConnection->Points->Insert(1, APointArray[0].X, APointArray[1].Y);
										AConnection->Points->ChangeXStyle(1, cpsToRel);
										AConnection->Points->ChangeYStyle(1, cpsToRel);
										AConnection->Draw();
									}
								}
							}
							break;
						case csInvertedSides:
							if (AConnection->Points->Item[k].Y == AConnection->Points->Item[k - 1].Y &&
								(k == AConnection->Points->Count() - 1)) {
								LOG_DEBUG("Not correct [csInvertedSides]!");
								if (AConnection->Points->Count() == 2) {
									AConnection->Style = csSides;
									AConnection->Draw();
									bStyleChanged = true;
								}
								else {
									if (AConnection->Points->Count() == 3) {
										TConnectionPointArray APointArray;
										Editorutils::RememberPoints(AConnection, APointArray);

										AConnection->Style = csSides;
										CalculatePointsPosition(AConnection);
										bStyleChanged = true;

										AConnection->Points->Item[0].X = APointArray[0].X;
										AConnection->Points->Item[0].Y = APointArray[0].Y;

										AConnection->Points->Item[1].X = APointArray[2].X;
										AConnection->Points->Item[1].Y = APointArray[2].Y;

										AConnection->Points->Insert(1, APointArray[1].X, APointArray[0].Y);
										AConnection->Points->ChangeXStyle(1, cpsToRel);
										AConnection->Points->ChangeYStyle(1, cpsToRel);
										AConnection->Draw();
									}
								}
							}
							break;
						}

						if (bStyleChanged) {
							LOG_DEBUG("Style was changed!");
							AdjustSidesStyle(AConnection);

							// ВЫХОДИМ ИЗ ОТСЧЕТА ТОЧЕК!!!
							break;
						}
					}

					// попробуем проверить точки вне диапазона
					if (AConnection->Points->Count() >= 2) {

						AConnection->Points->ChangeXStyle(0, cpsFromRel);
						AConnection->Points->ChangeYStyle(0, cpsFromRel);

						AConnection->Points->ChangeXStyle(AConnection->Points->Count() - 1, cpsToRel);
						AConnection->Points->ChangeYStyle(AConnection->Points->Count() - 1, cpsToRel);

						CalculatePointsPosition(AConnection);

						TPoint ANew1(AConnection->Points->Item[0].X, AConnection->Points->Item[0].Y);
						TPoint ANew2(AConnection->Points->Item[AConnection->Points->Count() - 1].X,
							AConnection->Points->Item[AConnection->Points->Count() - 1].Y);
						// для очень глубокой отладки
						// LOG_DEBUG("New 1[%d,%d], New 2 [%d,%d]", ANew1.x, ANew1.y, ANew2.x, ANew2.y);

						// проверим, что точки лежат на плоскости
						if (!PtInRectEx(AFromShape->Bounds(), ANew1)) {
							// для очень глубокой отладки
							//LOG_DEBUG("IS OUTSIDE [FROM]!");
						}
						// проверим, что точки лежат на плоскости
						if (!PtInRectEx(AToShape->Bounds(), ANew2)) {
							// для очень глубокой отладки
							//LOG_DEBUG("IS OUTSIDE [TO]!");
						}

						bool bDrawElse = false;
						if (AConnection->Points->Item[0].XValue < 0) {
							AConnection->Points->Item[0].XValue = 0;
							bDrawElse = true;
						}
						if (AConnection->Points->Item[0].XValue > AFromShape->Width) {
							AConnection->Points->Item[0].XValue = AFromShape->Width;
							bDrawElse = true;
						}

						if (AConnection->Points->Item[AConnection->Points->Count() - 1].XValue < 0) {
							AConnection->Points->Item[AConnection->Points->Count() - 1].XValue = 0;
							bDrawElse = true;
						}
						if (AConnection->Points->Item[AConnection->Points->Count() - 1].XValue > AToShape->Width) {
							AConnection->Points->Item[AConnection->Points->Count() - 1].XValue = AToShape->Width;
							bDrawElse = true;
						}

						if (AConnection->Points->Item[0].YValue < 0) {
							AConnection->Points->Item[0].YValue = 0;
							bDrawElse = true;
						}
						if (AConnection->Points->Item[0].YValue > AFromShape->Height) {
							AConnection->Points->Item[0].YValue = AFromShape->Height;
							bDrawElse = true;
						}

						if (AConnection->Points->Item[AConnection->Points->Count() - 1].YValue < 0) {
							AConnection->Points->Item[AConnection->Points->Count() - 1].YValue = 0;
							bDrawElse = true;
						}
						if (AConnection->Points->Item[AConnection->Points->Count() - 1].YValue > AToShape->Height) {
							AConnection->Points->Item[AConnection->Points->Count() - 1].YValue = AToShape->Height;
							bDrawElse = true;
						}

						if (bDrawElse) {
							AConnection->Tree->Invalidate();
						}

					}
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	void AdjustSidesStyle(TTreeConnection *AConnection) {
		WARNING_NULL_PTR(AConnection);

		// убираем все смещения
		AConnection->Text->HorizOffset = 0;
		AConnection->Text->VertOffset = 0;

		AConnection->HorizTextAlign = htaCenter;

		switch(AConnection->Style) {
		case csAuto:
		case csLine:
			AConnection->VertTextAlign = vtaCenter;
			break;
		case csInvertedSides: {
				const TRect ABounds = AConnection->GetBounds();
				const int iWidth = ABounds.Width();
				const int iHeight = ABounds.Height();
				const bool bNeedCenter = /*AConnection->Points->Count() >= 2 &&*/ iWidth < iHeight;
				AConnection->VertTextAlign = bNeedCenter ? vtaCenter : vtaTop;
			}break;
		case csSides:
			AConnection->VertTextAlign = vtaBottom;
			break;
		}
	}

	// ---------------------------------------------------------------------------
	void CollectInConnections(TTreeNodeShape *AShape, TConnectionFromToMap &AConnections) {
		if (AShape) {
			for (int i = 0; i < AShape->Parents->Count; i++) {
				TTreeNodeShape *AParent = AShape->Parents->Items[i];
				for (int k = 0; k < AParent->Connections->Count; k++) {
					TTreeConnection *AConnection = AParent->Connections->Items[k];
					if (AConnection->ToShape == AShape) {
						TConnectionPointArray APoints;
						RememberPoints(AConnection, APoints);
						AConnections[AConnection] = boost::make_tuple(AConnection->FromShape, AConnection->ToShape, APoints, k);
					}
				}
			}
		}

	}

	// ---------------------------------------------------------------------------
	void CollectInOutConnections(TTreeNodeShape *AShape, TConnectionFromToMap &AConnections) {
		if (AShape) {
			for (int i = 0; i < AShape->Connections->Count; i++) {
				TTreeConnection *AConnection = AShape->Connections->Items[i];
				TConnectionPointArray APoints;
				RememberPoints(AConnection, APoints);
				AConnections[AConnection] = boost::make_tuple(AConnection->FromShape, AConnection->ToShape, APoints, i);
			}
		}

		CollectInConnections(AShape, AConnections);
	}

	// ---------------------------------------------------------------------------
	void CollectInOutNamedConnections(TTreeNodeShape *AShape, TConnectionNameFromToMap &ANamedConnections,
		TMetaClass *AConnectionInheritsType) {
		if (AShape) {
			TConnectionFromToMap AConnections;
			CollectInOutConnections(AShape, AConnections);
			for (TConnectionFromToMap::iterator it = AConnections.begin(); it != AConnections.end(); ++it) {
				if (it->first && it->first->InheritsFrom(AConnectionInheritsType) && !it->first->Name.IsEmpty()) {
					TTreeNodeShape *AFromShape = boost::get<0>(it->second);
					TTreeNodeShape *AToShape = boost::get<1>(it->second);
					ANamedConnections[it->first->Name] = boost::make_tuple( //
						AFromShape ? AFromShape->Name : UnicodeString(L""), //
						AToShape ? AToShape->Name : UnicodeString(L""), //
						boost::get<2>(it->second));
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	void RevertInOutNamedConnections(TCustomTree *AInheritedTree, TCustomTree *AThisTree,
		const TConnectionNameFromToMap &ANamedConnections) {

		for (TConnectionNameFromToMap::const_iterator it = ANamedConnections.begin(); it != ANamedConnections.end(); ++it) {
			if (TStateMachineConnection * AOriginConnection = FindConnectionByName<TStateMachineConnection>(AInheritedTree, it->first)) {

				if (!AOriginConnection->FromShape || !AOriginConnection->ToShape) {
					WLOG_ERROR(L"INHERITANCE> [%s] FromShape==NULL and ToShape==NULL !", it->first.c_str());
					continue;
				}

				TScxmlBaseShape *AFromShape = FindShapeByName<TScxmlBaseShape>(AThisTree, AOriginConnection->FromShape->Name);
				if (!AFromShape) {
					WLOG_ERROR(L"INHERITANCE> [%s] FromShape:[%s] is not found!", it->first.c_str(),
						AOriginConnection->FromShape->Name.c_str());
					continue;
				}

				TScxmlBaseShape *AToShape = FindShapeByName<TScxmlBaseShape>(AThisTree, AOriginConnection->ToShape->Name);
				if (!AToShape) {
					WLOG_ERROR(L"INHERITANCE> [%s] ToShape:[%s] is not found!", it->first.c_str(),
						AOriginConnection->ToShape->Name.c_str());
					continue;
				}

				TStateMachineConnection * ATargetConnection = FindConnectionByName<TStateMachineConnection>(AThisTree, it->first);
				if (!ATargetConnection) {
					WLOG_WARNING(L"INHERITANCE> [%s] Origin exists but inherited not found!", it->first.c_str());

					ATargetConnection = new TStateMachineConnection(AThisTree->Owner);
					ATargetConnection->Tree = AThisTree;
				}

				// если создали или было найдено
				if (ATargetConnection) {
					ATargetConnection->Assign(AOriginConnection);

					ATargetConnection->FromShape = AFromShape;
					ATargetConnection->ToShape = AToShape;

					ATargetConnection->Name = AOriginConnection->Name;

					// bug fix: какого-то хрена не назначаются точки при множественном назначении
					ATargetConnection->Style = AOriginConnection->Style;

					TConnectionPointArray AConnectionPoints;
					RememberPoints(AOriginConnection, AConnectionPoints);
					RestorePoints(ATargetConnection, AConnectionPoints);

					ATargetConnection->Locked = true;
					ATargetConnection->InheritanceMismatch = false;

					WLOG_DEBUG(L"INHERITANCE> [%s] Restored connection", it->first.c_str());
				}
			}
			else {
				WLOG_WARNING(L"INHERITANCE> [%s] Can not find origin connection!", it->first.c_str());
			}
		}
	}

	// ---------------------------------------------------------------------------
	void FixUpConnections(TConnectionFromToMap &AConnections) {
		std::multimap<int, TTreeConnection*>AMultiConnections;

		for (TConnectionFromToMap::iterator it = AConnections.begin(); it != AConnections.end(); ++it) {
			it->first->FromShape = boost::get<0>(it->second);
			it->first->ToShape = boost::get<1>(it->second);
			// для родных связей это не нужно делать
			if (!it->first->InheritsFrom(__classid(TTreeConnection))) {
				RestorePoints(it->first, boost::get<2>(it->second));
			}
			AMultiConnections.insert(std::make_pair(boost::get<3>(it->second), it->first));
		}

		for (std::multimap<int, TTreeConnection*>::iterator it = AMultiConnections.begin(); it != AMultiConnections.end(); ++it) {
#if 0
			WLOG_DEBUG(L"Connection index:[%d] wasIndex:[%d]", GetConnectionIndex(it->second), it->first);
#endif
			SetConnectionIndex(it->second, it->first);
		}
	}

	// ---------------------------------------------------------------------------
	void ClearFromToInConnections(TConnectionFromToMap &AConnections) {
		for (TConnectionFromToMap::iterator it = AConnections.begin(); it != AConnections.end(); ++it) {
			it->first->FromShape = NULL;
			it->first->ToShape = NULL;
		}
	}

	// ---------------------------------------------------------------------------
	int __fastcall GetConnectionIndex(TTreeConnection *AConnection) {
		if (AConnection && AConnection->FromShape) {
			return AConnection->FromShape->Connections->IndexOf(AConnection);
		}
		return -1;
	}

	// ---------------------------------------------------------------------------
	bool __fastcall SetConnectionIndex(TTreeConnection *AConnection, int val) {
		if (AConnection->FromShape && AConnection->Tree) {
			const int iIndex = AConnection->FromShape->Connections->IndexOf(AConnection);

			if (val != iIndex && val >= 0 && val < AConnection->FromShape->Connections->Count) {

				TTreeConnection *AConnectionNext = AConnection->FromShape->Connections->Items[val];
				// меняем глобально в дереве
				{
					const int iIndex1 = AConnection->Tree->Connections->IndexOf(AConnection);
					const int iIndex2 = AConnection->Tree->Connections->IndexOf(AConnectionNext);

					TConnectionListAccess *AListAccess = reinterpret_cast<TConnectionListAccess*>(AConnection->Tree->Connections);
					AListAccess->Move(iIndex1, iIndex2);
				}
				// теперь в списке у состояния
				{
					const int iIndex1 = AConnection->FromShape->Connections->IndexOf(AConnection);
					const int iIndex2 = AConnection->FromShape->Connections->IndexOf(AConnectionNext);

					TConnectionListAccess *AListAccess = reinterpret_cast<TConnectionListAccess*>(AConnection->FromShape->Connections);
					AListAccess->Move(iIndex1, iIndex2);
				}

				return true;
			}
		}

		return false;
	}

	// ---------------------------------------------------------------------------
	void SetShapeParentEx(TTreeNodeShape *AShape, TTreeNodeShape *AParent) {
		TConnectionFromToMap AConnections;

		CollectInOutConnections(AShape, AConnections);

		ClearFromToInConnections(AConnections);

		AShape->Parent = AParent;
#if 0
		WLOG_DEBUG(L"Shape[%s] Parent[%s]", AShape->SimpleText.c_str(), AParent->SimpleText.c_str());
#endif

		FixUpConnections(AConnections);

	}

	void DumpConnections(TTreeNodeShape *AShape) {
		for (int i = 0; i < AShape->Connections->Count; i++) {
			WLOG_DEBUG(L"Out: [%d] [%s] [%s]", i, AShape->Connections->Items[i]->Name.c_str(),
				AShape->Connections->Items[i]->SimpleText.c_str());
		}
		for (int i = 0; i < AShape->Parents->Count; i++) {
			TTreeNodeShape *AParent = AShape->Parents->Items[i];
			for (int k = 0; k < AParent->Connections->Count; k++) {
				if (AParent->Connections->Items[k]->ToShape == AShape) {
					WLOG_DEBUG(L"In: [%d] [%s] [%s]", i, AParent->Connections->Items[k]->Name.c_str(),
						AParent->Connections->Items[k]->SimpleText.c_str());
				}
			}
		}
	}

	void DumpAllConnections(TCustomTree *ATree) {
		for (int i = 0; i < ATree->Connections->Count; i++) {
			WLOG_DEBUG(L"Connection: [%d] [%s] [%s]", i, ATree->Connections->Items[i]->Name.c_str(),
				ATree->Connections->Items[i]->SimpleText.c_str());
		}
	}

	void DumpConnectionPoints(TTreeConnection *AConnection) {
		std::auto_ptr<TStringList>AOutListPtr(new TStringList());
		AOutListPtr->Add(AConnection->SimpleText.IsEmpty() ? AConnection->Name : AConnection->SimpleText);
		for (int i = 0; i < AConnection->Points->Count(); i++) {
			AOutListPtr->Add(UnicodeString().sprintf(L"X:[%d] Y:[%d]", AConnection->Points->Item[i].X, AConnection->Points->Item[i].Y));
		}
		WLOG_DEBUG(L"%s", AOutListPtr->Text.c_str());
	}

	// для внутреннего пользования так как предполагает, что будет индексировать дерево с нуля
	void FixUpShapeChildrenTreeIndexes(TCustomTree*ATree, TNodeShapeList * AChildren, int &iLastIndex) {

		TNodeListAccess *ATreeList = reinterpret_cast<TNodeListAccess*>(ATree->Shapes);

		for (int i = 0; i < AChildren->Count; i++) {
			// текущий индекс
			const int iCurIndex = ATreeList->IndexOf(AChildren->Items[i]);

			iLastIndex++;

			if (iCurIndex != iLastIndex) {

				ATreeList->Move(iCurIndex, iLastIndex);
#if 0 // операция очень тяжелая, поэтому строго для отладки
				WLOG_DEBUG(L"Shape [MOVED] [%s][%s] OldIndex=%d NewIndex=%d Check=%d", AChildren->Items[i]->Name.c_str(),
					AChildren->Items[i]->SimpleText.c_str(), iCurIndex, iLastIndex, ATreeList->IndexOf(AChildren->Items[i]));
#endif
			}
			else {
#if 0 // операция очень тяжелая, поэтому строго для отладки
				WLOG_DEBUG(L"Shape [SAME] [%s][%s] OldIndex=%d NewIndex=%d Check=%d", AChildren->Items[i]->Name.c_str(),
					AChildren->Items[i]->SimpleText.c_str(), iCurIndex, iLastIndex, ATreeList->IndexOf(AChildren->Items[i]));
#endif
			}

			FixUpShapeChildrenTreeIndexes(ATree, AChildren->Items[i]->Children, iLastIndex);
		}
	}

	void FixUpShapeListIndexes(TCustomTree*ATree) {
		int iLastIndex = -1;
		FixUpShapeChildrenTreeIndexes(ATree, ATree->Roots, iLastIndex);
	}

	bool ExpandAllParents(TTreeNodeShape *AShape) {
		bool bRes = false;
		TTreeNodeShape *AParent = AShape->Parent;
		while (AParent) {
			if (!AParent->Expanded) {
				bRes = true;
			}
			AParent->Expanded = true;
			AParent = AParent->Parent;
		}

		return bRes;
	}

	void ExpandAndMakeVisibleAllParents(TTreeNodeShape *AShape) {
		TTreeNodeShape *AParent = AShape->Parent;
		while (AParent) {
			AParent->Expanded = true;
			AParent->Visible = true;

			AParent = AParent->Parent;
		}
	}

	void ExpandAll(TTreeNodeShape *AShape) {
		AShape->Expanded = true;

		for (int i = 0; i < AShape->Children->Count; i++) {
			ExpandAll(AShape->Children->Items[i]);
		}
	}

	void CollapseAll(TTreeNodeShape *AShape) {
		AShape->Expanded = false;

		for (int i = 0; i < AShape->Children->Count; i++) {
			CollapseAll(AShape->Children->Items[i]);
		}
	}

	bool IsAnyChildShapeCollapsed(TTreeNodeShape *AShape, bool bRecursive) {
		TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(AShape);
		if (AChildScxmlBaseShape) {
			if (!AChildScxmlBaseShape->Expanded && AShape->Children->Count) {
				return true;
			}
		}

		if (bRecursive) {
			for (int i = 0; i < AShape->Children->Count; i++) {
				if (IsAnyChildShapeCollapsed(AShape->Children->Items[i], bRecursive)) {
					return true;
				}
			}
		}

		return false;
	}

	bool IsAnyChildShapeExpanded(TTreeNodeShape *AShape, bool bRecursive) {
		TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(AShape);
		if (AChildScxmlBaseShape) {
			if (AChildScxmlBaseShape->Expanded && AShape->Children->Count) {
				return true;
			}
		}

		if (bRecursive) {
			for (int i = 0; i < AShape->Children->Count; i++) {
				if (IsAnyChildShapeExpanded(AShape->Children->Items[i], bRecursive)) {
					return true;
				}
			}
		}

		return false;
	}

	bool IsAnyChildShapeCollapsed(TNodeShapeList *AShapeList, bool bRecursive) {
		for (int i = 0; i < AShapeList->Count; i++) {
			if (IsAnyChildShapeCollapsed(AShapeList->Items[i], bRecursive)) {
				return true;
			}
		}
		return false;
	}

	bool IsAnyChildShapeExpanded(TNodeShapeList *AShapeList, bool bRecursive) {
		for (int i = 0; i < AShapeList->Count; i++) {
			if (IsAnyChildShapeExpanded(AShapeList->Items[i], bRecursive)) {
				return true;
			}
		}
		return false;
	}

	bool IsAnyShapeHasChildren(TNodeShapeList *AShapeList) {
		for (int i = 0; i < AShapeList->Count; i++) {
			if (AShapeList->Items[i]->Children->Count) {
				return true;
			}
		}
		return false;
	}

	int IndexOfShapeByGuid(TCustomTree *ATree, GUID *guid) {
		for (int i = 0; i < ATree->Shapes->Count; i++) {
			TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(ATree->Shapes->Items[i]);
			if (AScxmlBaseShape && AScxmlBaseShape->Guid == guid) {
				return i;
			}
		}
		return -1;
	}

	int IndexOfConnectionByGuid(TCustomTree *ATree, GUID *guid) {
		for (int i = 0; i < ATree->Connections->Count; i++) {
			TStateMachineConnection * AConnection = dynamic_cast<TStateMachineConnection*>(ATree->Connections->Items[i]);
			if (AConnection && AConnection->Guid == guid) {
				return i;
			}
		}
		return -1;
	}

	int IndexOfShapeByGuid(TCustomTree *ATree, const UnicodeString &guid) {
		for (int i = 0; i < ATree->Shapes->Count; i++) {
			TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(ATree->Shapes->Items[i]);
			if (AScxmlBaseShape && SameText(AScxmlBaseShape->GuidStr, guid)) {
				return i;
			}
		}
		return -1;
	}

	int IndexOfConnectionByGuid(TCustomTree *ATree, const UnicodeString &guid) {
		for (int i = 0; i < ATree->Connections->Count; i++) {
			TStateMachineConnection * AConnection = dynamic_cast<TStateMachineConnection*>(ATree->Connections->Items[i]);
			if (AConnection && SameText(AConnection->GuidStr, guid)) {
				return i;
			}
		}
		return -1;
	}

	TTreeNodeShape *FindShapeByStateID(TCustomTree *ATree, const UnicodeString &sStateID) {
		for (int i = 0; i < ATree->Shapes->Count; i++) {
			TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(ATree->Shapes->Items[i]);
			if (AScxmlBaseShape && AScxmlBaseShape->MustSimpleTextBeUnique() && AScxmlBaseShape->SimpleText == sStateID) {
				return AScxmlBaseShape;
			}
		}
		return NULL;
	}

	const bool IsIOConnectionType(TTreeConnection *AConnection, const TIOConnectionType AIOType,
		const TDirectionConnectionType ADirectionType) {
		if (AConnection) {
			const TRect ABounds = AConnection->GetBounds();

			TTreeNodeShape *ACheckShape = ADirectionType == cdtFrom ? AConnection->FromShape : AConnection->ToShape;
			if (ACheckShape) {

				switch(AIOType) {
				case ccftLeft:
					// соединение выходит слева
					if (abs(ACheckShape->X0 - ABounds.Right) < 3)
						return true;
					break;
				case ccftRight: // соединение выходит справа
					if (abs(ACheckShape->X1 - ABounds.Left) < 3)
						return true;
					break;
				case ccftTop:
					// соединение выходит сверху
					if (abs(ACheckShape->Y0 - ABounds.Bottom) < 3)
						return true;
					break;
				case ccftBottom:
					// соединение выходит снизу
					if (abs(ACheckShape->Y1 - ABounds.Top) < 3)
						return true;
					break;
				}
			}
		}
		return false;
	}

	const TIOConnectionType GetIOConnectionType(TTreeConnection *AConnection, const TDirectionConnectionType ADirectionType) {
		if (AConnection) {
			switch(AConnection->Style) {
			case csAuto:
			case csLine:
			case csCurve:
				if (IsIOConnectionType(AConnection, ccftLeft, ADirectionType))
					return ccftLeft;
				if (IsIOConnectionType(AConnection, ccftRight, ADirectionType))
					return ccftRight;
				if (IsIOConnectionType(AConnection, ccftTop, ADirectionType))
					return ccftTop;
				if (IsIOConnectionType(AConnection, ccftBottom, ADirectionType))
					return ccftBottom;
				break;
			case csInvertedSides:
				if (ADirectionType == cdtFrom) {
					if (IsIOConnectionType(AConnection, ccftLeft, ADirectionType))
						return ccftLeft;
					if (IsIOConnectionType(AConnection, ccftRight, ADirectionType))
						return ccftRight;
					if (IsIOConnectionType(AConnection, ccftTop, ADirectionType))
						return ccftTop;
					if (IsIOConnectionType(AConnection, ccftBottom, ADirectionType))
						return ccftBottom;
				}
				else {
					if (IsIOConnectionType(AConnection, ccftTop, ADirectionType))
						return ccftTop;
					if (IsIOConnectionType(AConnection, ccftBottom, ADirectionType))
						return ccftBottom;
					if (IsIOConnectionType(AConnection, ccftLeft, ADirectionType))
						return ccftLeft;
					if (IsIOConnectionType(AConnection, ccftRight, ADirectionType))
						return ccftRight;
				}
				break;
			case csSides:
				if (ADirectionType == cdtFrom) {
					if (IsIOConnectionType(AConnection, ccftTop, ADirectionType))
						return ccftTop;
					if (IsIOConnectionType(AConnection, ccftBottom, ADirectionType))
						return ccftBottom;
					if (IsIOConnectionType(AConnection, ccftLeft, ADirectionType))
						return ccftLeft;
					if (IsIOConnectionType(AConnection, ccftRight, ADirectionType))
						return ccftRight;
				}
				else {
					if (IsIOConnectionType(AConnection, ccftLeft, ADirectionType))
						return ccftLeft;
					if (IsIOConnectionType(AConnection, ccftRight, ADirectionType))
						return ccftRight;
					if (IsIOConnectionType(AConnection, ccftTop, ADirectionType))
						return ccftTop;
					if (IsIOConnectionType(AConnection, ccftBottom, ADirectionType))
						return ccftBottom;
				}
				break;
			}
		}
		return ccftUNKNOWN;
	}

	void ProtectTreeConnectionsToBeInSamePoints(TCustomTree *ATree) {
		if (ATree) {
			for (int i = 0; i < ATree->Connections->Count; i++) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(ATree->Connections->Items[i]);
				if (AStateMachineConnection) {
					ProtectConnectionToBeInSamePoints(AStateMachineConnection);
				}
			}
		}
	}

	void ProtectConnectionToBeInSamePoints(TTreeConnection *AConnection) {
		if (AConnection && AConnection->FromShape && AConnection->ToShape) {

			CalculatePointsPosition(AConnection);

			TTreeNodeShape *AFromShape = AConnection->FromShape;
			TTreeNodeShape *AToShape = AConnection->ToShape;

			const int iPtCount = AConnection->Points->Count();
			if (iPtCount >= 2) {
				const TIOConnectionType AFromIOType = GetIOConnectionType2(AConnection, cdtFrom);
				const TIOConnectionType AToIOType = GetIOConnectionType2(AConnection, cdtTo);

				if (AFromIOType == TIOConnectionType::ccftUNKNOWN || AToIOType == TIOConnectionType::ccftUNKNOWN)
					return;

				while (IsConnectionInSamePoints(AConnection)) {

					const int iFirstX = AConnection->Points->Item[0].X;
					const int iFirstY = AConnection->Points->Item[0].Y;

					const int iLastX = AConnection->Points->Item[iPtCount - 1].X;
					const int iLastY = AConnection->Points->Item[iPtCount - 1].Y;

					const int iPtPercentFirstX = RoundTo(double(iFirstX - AFromShape->X0) * 100.0f / (double)AFromShape->Width, 0);
					const int iPtPercentFirstY = RoundTo(double(iFirstY - AFromShape->Y0) * 100.0f / (double)AFromShape->Height, 0);

					const int iPtPercentLastX = RoundTo(double(iLastX - AToShape->X0) * 100.0f / (double)AToShape->Width, 0);
					const int iPtPercentLastY = RoundTo(double(iLastY - AToShape->Y0) * 100.0f / (double)AToShape->Height, 0);

					// смещение пикселей в процентах
					const int iOffsetFromPercentX = RoundTo(15.0f * 100.0f / (double)AFromShape->Width, 0);
					const int iOffsetFromPercentY = RoundTo(15.0f * 100.0f / (double)AFromShape->Height, 0);

					const int iOffsetToPercentX = RoundTo(15.0f * 100.0f / (double)AToShape->Width, 0);
					const int iOffsetToPercentY = RoundTo(15.0f * 100.0f / (double)AToShape->Height, 0);

					/* восстанавливаем точки так, как они были относительно входов-выходов */
					AConnection->Points->ChangeXStyle(0, cpsFromPercent);
					AConnection->Points->ChangeYStyle(0, cpsFromPercent);
					AConnection->Points->ChangeXStyle(iPtCount - 1, cpsToPercent);
					AConnection->Points->ChangeYStyle(iPtCount - 1, cpsToPercent);

					CalculatePointsPosition(AConnection);

					int iFromX = 0;
					int iFromY = 0;

					switch(AFromIOType) {
					case ccftLeft:
					case ccftRight:
						AConnection->Points->Item[0].XValue = iPtPercentFirstX;
						AConnection->Points->Item[0].YValue = iPtPercentFirstY + iOffsetFromPercentY;

						iFromY = iOffsetFromPercentY;
						break;
					case ccftTop:
					case ccftBottom:
						AConnection->Points->Item[0].XValue = iPtPercentFirstX + iOffsetFromPercentX;
						AConnection->Points->Item[0].YValue = iPtPercentFirstY;

						iFromX = iOffsetFromPercentX;
						break;
					default:
						AConnection->Points->Item[0].XValue = iPtPercentFirstX + iOffsetFromPercentX;
						AConnection->Points->Item[0].YValue = iPtPercentFirstY + iOffsetFromPercentY;

						iFromX = iOffsetFromPercentX;
						iFromY = iOffsetFromPercentY;
					}

					int iToX = 0;
					int iToY = 0;

					switch(AToIOType) {
					case ccftLeft:
					case ccftRight:
						AConnection->Points->Item[iPtCount - 1].XValue = iPtPercentLastX;
						AConnection->Points->Item[iPtCount - 1].YValue = iPtPercentLastY + iOffsetToPercentY;
						iToY = iOffsetToPercentY;
						break;
					case ccftTop:
					case ccftBottom:
						AConnection->Points->Item[iPtCount - 1].XValue = iPtPercentLastX + iOffsetToPercentX;
						AConnection->Points->Item[iPtCount - 1].YValue = iPtPercentLastY;
						iToX = iOffsetToPercentX;
						break;
					default:
						AConnection->Points->Item[iPtCount - 1].XValue = iPtPercentLastX + iOffsetToPercentX;
						AConnection->Points->Item[iPtCount - 1].YValue = iPtPercentLastY + iOffsetToPercentY;

						iToX = iOffsetToPercentX;
						iToY = iOffsetToPercentY;
					}

					// средние точки тоже сдвигаем, если они связаны с From или To
					for (int i = 1; i < AConnection->Points->Count() - 1; i++) {
						switch(AConnection->Points->Item[i].XStyle) {
						case cpsFromPercent:
							AConnection->Points->Item[i].XValue += iFromX;
							break;
						case cpsToPercent:
							AConnection->Points->Item[i].XValue += iToX;
							break;
						}

						switch(AConnection->Points->Item[i].YStyle) {
						case cpsFromPercent:
							AConnection->Points->Item[i].YValue += iFromY;
							break;
						case cpsToPercent:
							AConnection->Points->Item[i].YValue += iToY;
							break;
						}
					}
				}
			}

		}
	}

	bool IsConnectionInSamePoints(TTreeConnection *AConnection) {
		if (AConnection && AConnection->FromShape) {

			TTreeNodeShape *ASourceFromShape = AConnection->FromShape;

			const int iSourcePtCount = AConnection->Points->Count();
			if (iSourcePtCount >= 2) {

				Editorutils::CalculatePointsPosition(AConnection);

				const TPoint ASourceFirstPoint(AConnection->Points->Item[0].X, AConnection->Points->Item[0].Y);
				const TPoint ASourceLastPoint(AConnection->Points->Item[iSourcePtCount - 1].X,
					AConnection->Points->Item[iSourcePtCount - 1].Y);

				TConnectionFromToMap AConnections;
				CollectInOutConnections(ASourceFromShape, AConnections);

				for (TConnectionFromToMap::iterator it = AConnections.begin(); it != AConnections.end(); ++it) {
					TTreeConnection *ACheckConnection = it->first;

					if (ACheckConnection != AConnection) {

						const int iTargetPtCount = ACheckConnection->Points->Count();
						if (iTargetPtCount >= 2) {

							Editorutils::CalculatePointsPosition(ACheckConnection);

							const TPoint ATargetFirstPoint(ACheckConnection->Points->Item[0].X, ACheckConnection->Points->Item[0].Y);
							const TPoint ATargetLastPoint(ACheckConnection->Points->Item[iTargetPtCount - 1].X,
								ACheckConnection->Points->Item[iTargetPtCount - 1].Y);

							/* WLOG_DEBUG(L"Source first(%d,%d) last(%d,%d). Target first(%d,%d) last(%d,%d)", //
							ASourceFirstPoint.x, ASourceFirstPoint.y, ASourceLastPoint.x, ASourceLastPoint.y, //
							ATargetFirstPoint.x, ATargetFirstPoint.y, ATargetLastPoint.x, ATargetLastPoint.y //
							); */

							const bool bSameDirectionEquality = //
							(abs(ASourceFirstPoint.x - ATargetFirstPoint.x) < 3) && //
							(abs(ASourceLastPoint.x - ATargetLastPoint.x) < 3) && //
							(abs(ASourceFirstPoint.y - ATargetFirstPoint.y) < 3) && //
							(abs(ASourceLastPoint.y - ATargetLastPoint.y) < 3);

							const bool bAntiDirectionEquality = //
							(abs(ASourceFirstPoint.x - ATargetLastPoint.x) < 3) && //
							(abs(ASourceLastPoint.x - ATargetFirstPoint.x) < 3) && //
							(abs(ASourceFirstPoint.y - ATargetLastPoint.y) < 3) && //
							(abs(ASourceLastPoint.y - ATargetFirstPoint.y) < 3);

							if (bSameDirectionEquality || bAntiDirectionEquality) {
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}

	void GetActualConnectionPoints(TTreeConnection *AConnection, std::vector<std::pair<TPoint, int/*Index*/ > > &AVecPoints) {
		AVecPoints.clear();

		const int tmpNum = AConnection->Points->Count();

		if (tmpNum) {
			int tmpX = AConnection->Points->Item[0].X;
			int tmpY = AConnection->Points->Item[0].Y;

			AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[0].X, AConnection->Points->Item[0].Y), 0));

			for (int i = 1; i < AConnection->Points->Count(); i++) {
				switch(AConnection->Style) {
				case csSides: {
						const double dDistance = DistanceBetweenPoints(tmpX, tmpY, tmpX, AConnection->Points->Item[i].Y);
						if (dDistance <= 1.0f) {
							// WLOG_DEBUG(L"Too small distance point=%d dist=%f", i, dDistance);
							AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[i].X, AConnection->Points->Item[i].Y), i));
						}
						else {
							AVecPoints.push_back(std::make_pair(TPoint(tmpX, AConnection->Points->Item[i].Y), i));
							if (tmpX - AConnection->Points->Item[i].X) {
								AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[i].X, AConnection->Points->Item[i].Y),
										i));
							}
						}
					}break;
				case csInvertedSides: {
						const double dDistance = DistanceBetweenPoints(tmpX, tmpY, AConnection->Points->Item[i].X, tmpY);
						if (dDistance <= 1.0f) {
							// WLOG_DEBUG(L"Too small distance point=%d dist=%f", i, dDistance);
							AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[i].X, AConnection->Points->Item[i].Y), i));
						}
						else {
							AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[i].X, tmpY), i));
							if (tmpY - AConnection->Points->Item[i].Y) {
								AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[i].X, AConnection->Points->Item[i].Y),
										i));
							}
						}
					}break;
				default:
					AVecPoints.push_back(std::make_pair(TPoint(AConnection->Points->Item[i].X, AConnection->Points->Item[i].Y), i));
				}

				tmpX = AConnection->Points->Item[i].X;
				tmpY = AConnection->Points->Item[i].Y;
			}
		}
	}

	const TIOConnectionType GetIOConnectionType2(TTreeConnection *AConnection, const TDirectionConnectionType ADirectionType) {
		if (AConnection) {

			CalculatePointsPosition(AConnection);

			std::vector<std::pair<TPoint, int> >AVecPoints;
			GetActualConnectionPoints(AConnection, AVecPoints);

			switch(ADirectionType) {
			case cdtFrom:
				if (AConnection->FromShape) {
					for (std::size_t i = 0; i < AVecPoints.size(); i++) {
						if (!PtInRectEx(AConnection->FromShape->Bounds(), AVecPoints[i].first)) {

							const TPoint ptPrev = i > 0 ? AVecPoints[i - 1].first : TPoint(AConnection->FromShape->XCenter(),
								AConnection->FromShape->YCenter());

							if (AVecPoints[i].first.x < AConnection->FromShape->X0) {
								if (ptPrev.y >= AConnection->FromShape->Y0 && ptPrev.y <= AConnection->FromShape->Y1) {
									return ccftLeft;
								}
							}
							if (AVecPoints[i].first.x > AConnection->FromShape->X1) {
								if (ptPrev.y >= AConnection->FromShape->Y0 && ptPrev.y <= AConnection->FromShape->Y1) {
									return ccftRight;
								}
							}
							if (AVecPoints[i].first.y < AConnection->FromShape->Y0) {
								if (ptPrev.x >= AConnection->FromShape->X0 && ptPrev.x <= AConnection->FromShape->X1) {
									return ccftTop;
								}
							}
							if (AVecPoints[i].first.y > AConnection->FromShape->Y1) {
								if (ptPrev.x >= AConnection->FromShape->X0 && ptPrev.x <= AConnection->FromShape->X1) {
									return ccftBottom;
								}
							}

							break;

						}
					}
				}
				break;
			case cdtTo:
				if (AConnection->ToShape) {
					for (int i = static_cast<int>(AVecPoints.size()) - 1; i >= 0; i--) {
						if (!PtInRectEx(AConnection->ToShape->Bounds(), AVecPoints[i].first)) {

							const TPoint ptPrev = (i < static_cast<int>(AVecPoints.size()) - 1) ? AVecPoints[i + 1].first : TPoint
								(AConnection->ToShape->XCenter(), AConnection->ToShape->YCenter());

							if (AVecPoints[i].first.x < AConnection->ToShape->X0) {
								if (ptPrev.y >= AConnection->ToShape->Y0 && ptPrev.y <= AConnection->ToShape->Y1) {
									return ccftLeft;
								}
							}
							if (AVecPoints[i].first.x > AConnection->ToShape->X1) {
								if (ptPrev.y >= AConnection->ToShape->Y0 && ptPrev.y <= AConnection->ToShape->Y1) {
									return ccftRight;
								}
							}
							if (AVecPoints[i].first.y < AConnection->ToShape->Y0) {
								if (ptPrev.x >= AConnection->ToShape->X0 && ptPrev.x <= AConnection->ToShape->X1) {
									return ccftTop;
								}
							}
							if (AVecPoints[i].first.y > AConnection->ToShape->Y1) {
								if (ptPrev.x >= AConnection->ToShape->X0 && ptPrev.x <= AConnection->ToShape->X1) {
									return ccftBottom;
								}
							}

							break;
						}
					}
				}
				break;
			}
		}
		return ccftUNKNOWN;
	}

	// ---------------------------------------------------------------------------
	bool __fastcall AreAllShapesOfInheritedClass(TNodeShapeList *AShapes, TMetaClass *AClass) {
		for (int i = 0; i < AShapes->Count; i++) {
			if (AShapes->Items[i]->InheritsFrom(AClass) == false) {
				return false;
			}
		}
		return AShapes->Count > 0;
	}

	// ---------------------------------------------------------------------------
	bool __fastcall AreAllShapesBrothers(TNodeShapeList *AShapes) {
		TTreeNodeShape *AParent = NULL;
		for (int i = 0; i < AShapes->Count; i++) {
			if (i == 0) {
				AParent = AShapes->Items[i]->Parent;
			}
			else {
				if (AParent != AShapes->Items[i]->Parent) {
					return false;
				}
			}
		}
		return AShapes->Count != 0;
	}

	// ---------------------------------------------------------------------------
	bool IsSelfConnection(TTreeConnection *AConnection) {
		return AConnection && (AConnection->FromShape == AConnection->ToShape) && (AConnection->FromShape != NULL);
	}

	// ---------------------------------------------------------------------------
	TPoint GetRemotePoint(TPoint AFrom, const double dDistance, const double HeadingDeg) {
		const double dPartX = dDistance * Sin(DegToRad(HeadingDeg));
		const double dPartY = dDistance * Cos(DegToRad(HeadingDeg));

		const int iX2 = RoundTo(dPartX, 0);
		const int iY2 = RoundTo(dPartY, 0);

		AFrom.x += iX2;
		AFrom.y += iY2;
		return AFrom;
	}

	// ---------------------------------------------------------------------------
	TPoint GetNestedParentTopLeft(TControl *AControl, TWinControl *ANestedParent) {
		if (!AControl)
			throw Exception(L"GetNestedParentTopLeft> AControl==NULL!");

		int iLeft = AControl->Left;
		int iTop = AControl->Top;

		TControl *AParent = AControl->Parent;
		while (AParent && AParent != ANestedParent) {
			iLeft += AParent->Left;
			iTop += AParent->Top;

			AParent = AParent->Parent;
		}

		return TPoint(iLeft, iTop);
	}

	// ---------------------------------------------------------------------------
	TRect GetRectangle2DPosition(TCustomTree *ATree, const TRect &ARect) {
		if (ATree) {
			int iLeft = ARect.Left;
			int iTop = ARect.Top;
			int iRight = ARect.Right;
			int iBottom = ARect.Bottom;
			ATree->Canvas->Calculate2DPosition(iLeft, iTop, TeeTreeZ);
			ATree->Canvas->Calculate2DPosition(iRight, iBottom, TeeTreeZ);
			return TRect(iLeft, iTop, iRight, iBottom);
		}
		return TRect(0, 0, 0, 0);
	}

	// ---------------------------------------------------------------------------
	void SetChartPointInView(TCustomTree *ATree, const int iChartX, const int iChartY, const TChartPointViewType AViewType) {
		if (ATree) {
			TPoint P = ATree->Canvas->Calculate3DPosition(iChartX, iChartY, TeeTreeZ);

			switch(AViewType) {
			case cpvtCenter: {
					const int iTmpX = ATree->ChartXCenter;
					const int iTmpY = ATree->ChartYCenter;

					P.x = P.x - iTmpX;
					P.y = P.y - iTmpY;
				}break;
			}

			ATree->View3DOptions->HorizOffset = ATree->View3DOptions->HorizOffset - P.x;
			ATree->View3DOptions->VertOffset = ATree->View3DOptions->VertOffset - P.y;
		}
	}

	// ---------------------------------------------------------------------------
	void SetChartPointInView(TCustomTree *ATree, const TPoint &AChartPoint, const TChartPointViewType AViewType) {
		SetChartPointInView(ATree, AChartPoint.x, AChartPoint.y, AViewType);
	}

	// ---------------------------------------------------------------------------
	bool HasVisualShapeScxmlChildren(TTreeNodeShape *AShape) {
		if (dynamic_cast<TVisualScxmlBaseShape*>(AShape)) {
			for (int i = 0; i < AShape->Children->Count; i++) {
				TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(AShape->Children->Items[i]);
				if (AChildScxmlBaseShape) {
					return true;
				}
			}
		}
		return false;
	}

	// ---------------------------------------------------------------------------
	bool IsTreeNodeShapeCluster(TTreeNodeShape *AShape) {
		if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape)) {
			if (AVisualScxmlBaseShape->IsCluster()) {
				return true;
			}
		}
		return false;
	}

	// ---------------------------------------------------------------------------
	bool IsTreeNodeShapeClusterOrHasScxmlChildren(TTreeNodeShape *AShape) {
		return IsTreeNodeShapeCluster(AShape) || HasVisualShapeScxmlChildren(AShape);
	}

	void CalculateSquareConnectionByPoints(const std::vector<TPoint> &AVecPoints, //
		const TRect &AIncrementedFrom, const TRect &AIncrementedTo, const TRect &AFrom, const TRect &ATo, //
		TPrecalcedSquareConnection &APrecalced) {

		for (std::size_t i = 0; i < AVecPoints.size(); i++) {
			if (!PtInRect(AIncrementedFrom, AVecPoints[i]) && !PtInRect(AIncrementedTo, AVecPoints[i])) {
				const TPoint APtSide = AVecPoints[i];

#ifdef VISUALIZED_DEBUG_CANVAS
				VISUALIZED_DEBUG_CANVAS->Pen->Width = 1;
				VISUALIZED_DEBUG_CANVAS->Pen->Color = clTeal;
				VISUALIZED_DEBUG_CANVAS->Ellipse(APtSide.x - 3, APtSide.y - 3, APtSide.x + 3, APtSide.y + 3);
#endif

				int iNext = i + 1;
				if (iNext == static_cast<int>(AVecPoints.size())) {
					iNext = 0;
				}
				int iPrev = static_cast<int>(i) - 1;
				if (iPrev == -1) {
					iPrev = AVecPoints.size() - 1;
				}

				int iDistance = 0;
				bool bInverted = false;
				TPoint APtFrom = Point(0, 0);
				TPoint APtTo = Point(0, 0);

				{
					double X0clip, Y0clip, X1clip, Y1clip;
					if (LiangBarsky(AFrom.left, AFrom.right, -AFrom.bottom, -AFrom.top, APtSide.x, -APtSide.y, AVecPoints[iNext].x,
							-AVecPoints[iNext].y, X0clip, Y0clip, X1clip, Y1clip)) {

						APtFrom = Point(X0clip, -Y0clip);

						iDistance = DistanceBetweenPoints(APtSide.x, APtSide.y, APtFrom.x, APtFrom.y);
#ifdef VISUALIZED_DEBUG_CANVAS
						VISUALIZED_DEBUG_CANVAS->Pen->Color = clRed;
						VISUALIZED_DEBUG_CANVAS->Pen->Width = 3;
						VISUALIZED_DEBUG_CANVAS->MoveTo(APtSide.x, APtSide.y);
						VISUALIZED_DEBUG_CANVAS->LineTo(APtFrom.x, APtFrom.y);
#endif
					}
					else if (LiangBarsky(AFrom.left, AFrom.right, -AFrom.bottom, -AFrom.top, APtSide.x, -APtSide.y, AVecPoints[iPrev].x,
							-AVecPoints[iPrev].y, X0clip, Y0clip, X1clip, Y1clip)) {

						APtFrom = Point(X0clip, -Y0clip);

						iDistance = DistanceBetweenPoints(APtSide.x, APtSide.y, APtFrom.x, APtFrom.y);
#ifdef VISUALIZED_DEBUG_CANVAS
						VISUALIZED_DEBUG_CANVAS->Pen->Color = clRed;
						VISUALIZED_DEBUG_CANVAS->Pen->Width = 3;
						VISUALIZED_DEBUG_CANVAS->MoveTo(APtSide.x, APtSide.y);
						VISUALIZED_DEBUG_CANVAS->LineTo(X0clip, -Y0clip);
#endif
					}
				}

				{
					double X0clip, Y0clip, X1clip, Y1clip;
					if (LiangBarsky(ATo.left, ATo.right, -ATo.bottom, -ATo.top, APtSide.x, -APtSide.y, AVecPoints[iNext].x,
							-AVecPoints[iNext].y, X0clip, Y0clip, X1clip, Y1clip)) {

						APtTo = Point(X0clip, -Y0clip);

						iDistance += DistanceBetweenPoints(APtSide.x, APtSide.y, APtTo.x, APtTo.y);

						bInverted = APtSide.x == APtTo.x;
#ifdef VISUALIZED_DEBUG_CANVAS
						VISUALIZED_DEBUG_CANVAS->Pen->Color = clGreen;
						VISUALIZED_DEBUG_CANVAS->Pen->Width = 3;
						VISUALIZED_DEBUG_CANVAS->MoveTo(APtSide.x, APtSide.y);
						VISUALIZED_DEBUG_CANVAS->LineTo(APtTo.x, APtTo.y);
						VISUALIZED_DEBUG_CANVAS->TextOut(APtTo.x, APtTo.y, bInverted ? "Inv" : "Sid");
#endif
					}
					else if (LiangBarsky(ATo.left, ATo.right, -ATo.bottom, -ATo.top, APtSide.x, -APtSide.y, AVecPoints[iPrev].x,
							-AVecPoints[iPrev].y, X0clip, Y0clip, X1clip, Y1clip)) {

						APtTo = Point(X0clip, -Y0clip);

						iDistance += DistanceBetweenPoints(APtSide.x, APtSide.y, APtTo.x, APtTo.y);

						bInverted = APtSide.x == APtTo.x;
#ifdef VISUALIZED_DEBUG_CANVAS
						VISUALIZED_DEBUG_CANVAS->Pen->Color = clGreen;
						VISUALIZED_DEBUG_CANVAS->Pen->Width = 3;
						VISUALIZED_DEBUG_CANVAS->MoveTo(APtSide.x, APtSide.y);
						VISUALIZED_DEBUG_CANVAS->LineTo(APtTo.x, APtTo.y);
						VISUALIZED_DEBUG_CANVAS->TextOut(APtTo.x, APtTo.y, bInverted ? "Inv" : "Sid");
#endif
					}
				}

				if (iDistance > APrecalced.iDistance) {
					APrecalced.iDistance = iDistance;
					APrecalced.iPoint = i;
					APrecalced.bInverted = bInverted;

					APrecalced.APtFrom = APtFrom;
					APrecalced.APtTo = APtTo;
					APrecalced.APtSide = APtSide;
				}
			}
		}
	}

	void CalculateSquareConnection(const TRect &AFrom, const TRect &ATo, const unsigned int iMargin,
		TPrecalcedSquareConnection &APrecalced) {

		const int i_MARGIN = iMargin;

		const TRect AIncrementedFrom = IncrementRectCopy(AFrom, i_MARGIN);
		const TRect AIncrementedTo = IncrementRectCopy(ATo, i_MARGIN);

		if (!RectangleInRectangle(AIncrementedFrom, ATo) && !RectangleInRectangle(AIncrementedTo, AFrom)) {
			const TRect ACombinedRect = CombineRects(AFrom, ATo);
			//const TRect AInside = IncrementRectCopy(ACombinedRect, -i_MARGIN);

			int iMinWidth = std::min(AFrom.Width() / 2, ATo.Width() / 2);
			int iMinHeight = std::min(AFrom.Height() / 2, ATo.Height() / 2);

			TRect AInside(ACombinedRect);
			AInside.left += iMinWidth;
			AInside.right -= iMinWidth;
			AInside.top += iMinHeight;
			AInside.bottom -= iMinHeight;

#ifdef VISUALIZED_DEBUG_CANVAS
			VISUALIZED_DEBUG_CANVAS->Brush->Style = bsClear;
			int iFromCenterX, iFromCenterY, iToCenterX, iToCenterY;
			RectCenter(AFrom, iFromCenterX, iFromCenterY);
			RectCenter(ATo, iToCenterX, iToCenterY);

			VISUALIZED_DEBUG_CANVAS->TextOut(iFromCenterX, iFromCenterY, "From");
			VISUALIZED_DEBUG_CANVAS->TextOut(iToCenterX, iToCenterY, "To");

			VISUALIZED_DEBUG_CANVAS->Pen->Width = 1;
			VISUALIZED_DEBUG_CANVAS->Pen->Color = clBlack;
			VISUALIZED_DEBUG_CANVAS->Rectangle(ACombinedRect);

			VISUALIZED_DEBUG_CANVAS->Pen->Color = clBlue;
			VISUALIZED_DEBUG_CANVAS->Rectangle(AInside);
#endif

			std::vector<TPoint>AVecPoints;
			AVecPoints.push_back(Point(AInside.Left, AInside.Top));
			AVecPoints.push_back(Point(AInside.Right, AInside.Top));
			AVecPoints.push_back(Point(AInside.Right, AInside.Bottom));
			AVecPoints.push_back(Point(AInside.Left, AInside.Bottom));

			CalculateSquareConnectionByPoints(AVecPoints, AIncrementedFrom, AIncrementedTo, AFrom, ATo, APrecalced);

			if (APrecalced.iPoint == -1) {
				AVecPoints.clear();

				const TRect ACombinedIncrementRect = IncrementRectCopy(ACombinedRect, 0);
				APrecalced = TPrecalcedSquareConnection();

				AVecPoints.push_back(Point(ACombinedIncrementRect.Left, ACombinedIncrementRect.Top));
				AVecPoints.push_back(Point(ACombinedIncrementRect.Right, ACombinedIncrementRect.Top));
				AVecPoints.push_back(Point(ACombinedIncrementRect.Right, ACombinedIncrementRect.Bottom));
				AVecPoints.push_back(Point(ACombinedIncrementRect.Left, ACombinedIncrementRect.Bottom));

				CalculateSquareConnectionByPoints(AVecPoints, AIncrementedFrom, AIncrementedTo, AFrom, ATo, APrecalced);
			}
		}
	}

	TRect CombineRects(const TRect &ARect1, const TRect &ARect2) {
		return TRect(std::min(ARect1.left, ARect2.left), std::min(ARect1.top, ARect2.top), std::max(ARect1.right, ARect2.right),
			std::max(ARect1.bottom, ARect2.bottom));
	}

	void SetShapeTextAlign(TTreeNodeShape *AShape, const TDirectionType AType) {
		if (AShape && AShape->Tree) {
			AShape->Text->HorizOffset = 0;
			AShape->Text->VertOffset = 0;

			const TRect AShapeRect = AShape->Bounds();

			const int iShapeWidth = AShapeRect.Width();
			const int iShapeHeight = AShapeRect.Height();

			const int iWidth = AShape->Tree->Canvas->TextWidth(AShape->SimpleText.Trim());

			switch(AType) {
			case dtLeft: {
					int iCrossOffset = 0;

					if (AShape->Tree->CrossBox->Visible && AShape->ShouldDrawCross()) {
						const TPoint APt = AShape->CalcXYCross(AShape);
						iCrossOffset = AShapeRect.left - APt.x;
					}

					AShape->Text->HorizOffset = -iWidth - iShapeWidth / 2 - iCrossOffset;
					AShape->Text->HorizAlign = htaLeft;
				}break;
			case dtTop:
				AShape->Text->VertOffset = -iShapeHeight;
				AShape->Text->HorizAlign = htaCenter;
				break;
			case dtRight:
				AShape->Text->HorizOffset = iWidth + iShapeWidth / 2;
				AShape->Text->HorizAlign = htaRight;
				break;
			case dtBottom: {
					int iOffset = 0;

					TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
					if (AVisualScxmlBaseShape) {
						iOffset = AVisualScxmlBaseShape->MaxHeightExpandedChildsEx();
					}
					else {
						iOffset = AShape->MaxHeightExpandedChilds();
					}
					iOffset -= AShape->Y0;

					AShape->Text->VertOffset = iOffset;
					AShape->Text->HorizAlign = htaCenter;
				}break;
			}
		}
	}

	TTreeNodeShape *GetNextSibling(TTreeNodeShape *ANode) {
		if (ANode && ANode->Parent) {
			const int iIndex = ANode->Parent->Children->IndexOf(ANode);
			if (iIndex >= 0 && iIndex < ANode->Parent->Children->Count - 2) {
				return ANode->Parent->Children->Items[iIndex + 1];
			}

		}

		return NULL;
	}

	// ---------------------------------------------------------------------------
	bool IsParentSelected(TTreeNodeShape *ANode) {
		TTreeNodeShape *tmpNode;
		bool bRes = false;
		tmpNode = ANode->Parent;
		while (tmpNode) {
			bRes = tmpNode->Selected;
			if (bRes)
				break;
			tmpNode = tmpNode->Parent;
		}
		return bRes;
	}

	// ---------------------------------------------------------------------------
	bool IsNodeOrParentSelected(TTreeNodeShape *ANode) {
		if (ANode) {
			return ANode->Selected || IsParentSelected(ANode);
		}
		return false;
	}

	// ---------------------------------------------------------------------------
	bool IsConnectionOrParentsSelected(TTreeConnection *AConnection) {
		if (AConnection->Tree) {
			if (AConnection->Tree->Connections->Selected == AConnection) {
				return true;
			}

			if (AConnection->FromShape && AConnection->ToShape) {
				return IsNodeOrParentSelected(AConnection->FromShape) || IsNodeOrParentSelected(AConnection->ToShape);
			}
		}
		return false;
	}
}

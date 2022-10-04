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

#ifndef UnitTreeEditorUtilsH
#define UnitTreeEditorUtilsH
//---------------------------------------------------------------------------

#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>
#include <TeeTree.hpp>

namespace Editorutils {

#define WARNING_NULL_PTR_RET(X,RET)  	if (!X) {	LOG_WARNING("%s> %s is NULL!!!", __FUNCTION__,#X);	return RET; }
#define WARNING_NULL_PTR(X)  			WARNING_NULL_PTR_RET(X, ; )

	typedef enum {
		isFromPrev, isFromCur, isToPrev, isToCur, isMAXSIZE
	}TIsType;
	typedef Set<TIsType, isFromPrev, isMAXSIZE>TIsTypes;

	typedef enum {
		ccftLeft, ccftTop, ccftRight, ccftBottom, ccftUNKNOWN
	}TIOConnectionType;

    typedef enum {
		dtLeft, dtTop, dtRight, dtBottom, dtMAXSIZE
	} TDirectionType;

	static UnicodeString DirectionTypeToString(const TDirectionType AType) {
		switch (AType) {
		case dtLeft: return L"Left";
		case dtTop: return L"Top";
		case dtRight: return L"Right";
		case dtBottom: return L"Bottom";
		}

		return L"UNKNOWN";
	}

	typedef enum {
		cdtFrom, cdtTo
	}TDirectionConnectionType;

	template <typename T>
	class UniversalUpdateLock {
		T *FInstance;
	public:
		UniversalUpdateLock(T *AInstance):FInstance(AInstance) { if (FInstance) FInstance->BeginUpdate(); }
		~UniversalUpdateLock() { if (FInstance) FInstance->EndUpdate(); }
	};

	typedef UniversalUpdateLock<TCustomTree> TreeUpdateLock;

	// ---------------------------------------------------------------------------
	bool LiangBarsky(double edgeLeft, double edgeRight, double edgeBottom, double edgeTop, // Define the x/y clipping values for the border.
		double x0src, double y0src, double x1src, double y1src, // Define the start and end points of the line.
		double&x0clip, double&y0clip, double&x1clip, double&y1clip); // The output values, so declare these outside.

	// ---------------------------------------------------------------------------

	double DistanceBetweenPoints(double x1, double y1, double x2, double y2);

	void IncrementRect(TRect &ARect, const int iVal);

	TRect IncrementRectCopy(const TRect &ARect, const int iVal);

	bool PtInRectEx(const TRect & ARect, const TPoint & APoint);

	bool RectInsideRect(const TRect & AParent, const TRect & AChild);

	bool IsPointInOutsideOffsetRect(const TPoint & APoint, const TRect & ARect, const int iOffset);

	bool AreEqualPoints(const TConnectionPointArray &ASource, const TConnectionPointArray &ATarget);

	void RememberPoints(TTreeConnection * AConnection, TConnectionPointArray & AConnSelectedPoints);

	void RestorePoints(TTreeConnection * AConnection, const TConnectionPointArray & AConnSelectedPoints);

	void CalculatePointsPosition(TTreeConnection * AConnection);

	void ClipFirstConnectionPointToShapeBounds(TTreeConnection *AConnection);

	void ClipLastConnectionPointToShapeBounds(TTreeConnection *AConnection);

	void EnsurePointInShapeRect(TTreeNodeShape *AShape, TPoint &APoint);

	void AdjustSidesConnection(TTreeConnection * AConnection);

	void AdjustSidesStyle(TTreeConnection * AConnection);

	typedef std::map<TTreeConnection*,
		boost::tuple<TTreeNodeShape* /*AFrom*/, TTreeNodeShape* /*ATo*/, TConnectionPointArray /*Points*/, int /* TransitionIndex */> > TConnectionFromToMap;

    void CollectInConnections(TTreeNodeShape *AShape, TConnectionFromToMap &AConnections);

	void CollectInOutConnections(TTreeNodeShape *AShape, TConnectionFromToMap &AConnections);

	typedef std::map<UnicodeString, boost::tuple<UnicodeString /*AFrom*/, UnicodeString /*ATo*/,
				TConnectionPointArray /*Points*/ > > TConnectionNameFromToMap;

	void CollectInOutNamedConnections(TTreeNodeShape *AShape, TConnectionNameFromToMap &ANamedConnections, TMetaClass *AConnectionInheritsType);

	void RevertInOutNamedConnections(TCustomTree *AInheritedTree, TCustomTree *AThisTree, const TConnectionNameFromToMap &ANamedConnections);

	void FixUpConnections(TConnectionFromToMap &AConnections);

	void ClearFromToInConnections(TConnectionFromToMap &AConnections);

	// при установке родителя штатным методом могут теряться соединения, поэтому предварительно запоминаем и потом восстанавливаем
	void SetShapeParentEx(TTreeNodeShape * AShape, TTreeNodeShape * AParent);

	int __fastcall GetConnectionIndex(TTreeConnection *AConnection);

	bool __fastcall SetConnectionIndex(TTreeConnection *AConnection, int val);

	// индексы должны соответствовать строго иерархии
	void FixUpShapeListIndexes(TCustomTree*ATree);

	// возвращает true, если хоть один из родителей не был развернут
	bool ExpandAllParents(TTreeNodeShape *AShape);

	void ExpandAndMakeVisibleAllParents(TTreeNodeShape *AShape);

	void CollapseAll(TTreeNodeShape *AShape);

	void ExpandAll(TTreeNodeShape *AShape);

	bool IsAnyChildShapeCollapsed(TTreeNodeShape *AShape, bool bRecursive);

	bool IsAnyChildShapeExpanded(TTreeNodeShape *AShape, bool bRecursive);

	bool IsAnyChildShapeCollapsed(TNodeShapeList *AShapeList, bool bRecursive);

	bool IsAnyChildShapeExpanded(TNodeShapeList *AShapeList, bool bRecursive);

	bool IsAnyShapeHasChildren(TNodeShapeList *AShapeList);

	int IndexOfShapeByGuid(TCustomTree *ATree, GUID *guid);

	int IndexOfConnectionByGuid(TCustomTree *ATree, GUID *guid);

	int IndexOfShapeByGuid(TCustomTree *ATree, const UnicodeString &guid);

	int IndexOfConnectionByGuid(TCustomTree *ATree, const UnicodeString &guid);

	TTreeNodeShape *FindShapeByStateID(TCustomTree *ATree, const UnicodeString &sStateID);

	TTreeNodeShape *GetNextSibling(TTreeNodeShape *ANode);

	template<typename T>
	T * FindShapeByName(TCustomTree *ATree, const UnicodeString &sName) {
		for (int i = 0; i < ATree->Shapes->Count; i++) {
			if (sName == ATree->Shapes->Items[i]->Name) {
				return dynamic_cast<T *>(ATree->Shapes->Items[i]);
			}
		}
		return NULL;
	}

	template<typename T>
	T * FindConnectionByName(TCustomTree *ATree, const UnicodeString &sName) {
		for (int i = 0; i < ATree->Connections->Count; i++) {
			if (sName == ATree->Connections->Items[i]->Name) {
				return dynamic_cast<T *>(ATree->Connections->Items[i]);
			}
		}
		return NULL;
	}

	template<typename T>
	T * FindShapeByType(TTreeNodeShape *AShape, const int iStartIndex = 0, const int iMaxIndex = -1) {
		if (AShape) {
			int iCount = AShape->Count();
			if ((iMaxIndex > -1) && (iMaxIndex < iCount)) {
				iCount = iMaxIndex;
			}

			for (int i = iStartIndex; i < iCount; i++) {
				T *AFindShape = dynamic_cast<T* >(AShape->Children->Items[i]);
				if (AFindShape) {
					return AFindShape;
				}
			}
		}
		return NULL;
	}

    void GetActualConnectionPoints(TTreeConnection *AConnection, std::vector<std::pair<TPoint, int/*Index*/ > > &AVecPoints);

	const TIOConnectionType GetIOConnectionType(TTreeConnection *AConnection, const TDirectionConnectionType ADirectionType);

	const TIOConnectionType GetIOConnectionType2(TTreeConnection *AConnection, const TDirectionConnectionType ADirectionType);

	void ProtectConnectionToBeInSamePoints(TTreeConnection *AConnection);

	void ProtectTreeConnectionsToBeInSamePoints(TCustomTree *ATree);

	// находится ли ещё с кем-нибудь это соединение в одних и тех же точках
	bool IsConnectionInSamePoints(TTreeConnection *AConnection);

	bool __fastcall AreAllShapesOfInheritedClass(TNodeShapeList *AShapes, TMetaClass *AClass);

	bool __fastcall AreAllShapesBrothers(TNodeShapeList *AShapes);

	bool IsSelfConnection(TTreeConnection *AConnection);

	bool IsParentSelected(TTreeNodeShape *ANode);

	bool IsNodeOrParentSelected(TTreeNodeShape *ANode);

	bool IsConnectionOrParentsSelected(TTreeConnection *AConnection);

	bool HasVisualShapeScxmlChildren(TTreeNodeShape *AShape);

	bool IsTreeNodeShapeCluster(TTreeNodeShape *AShape);

	bool IsTreeNodeShapeClusterOrHasScxmlChildren(TTreeNodeShape *AShape);

	TPoint GetRemotePoint(TPoint AFrom, const double dDistance, const double HeadingDeg);

    TPoint GetNestedParentTopLeft(TControl *AControl, TWinControl *ANestedParent);

	/* Отладочные функции */
	void DumpConnections(TTreeNodeShape * AShape);

	void DumpAllConnections(TCustomTree * ATree);

	void DumpConnectionPoints(TTreeConnection *AConnection);

	typedef enum {
		cpvtZeroXY, cpvtCenter
	}TChartPointViewType;

	void SetChartPointInView(TCustomTree *ATree, const int iChartX, const int iChartY, const TChartPointViewType AViewType);

	void SetChartPointInView(TCustomTree *ATree, const TPoint &AChartPoint, const TChartPointViewType AViewType);

	TRect GetRectangle2DPosition(TCustomTree *ATree, const TRect &ARect);

	struct TPrecalcedSquareConnection {

	TPrecalcedSquareConnection() {
			iPoint = -1;
			iDistance = 0;
			bInverted = false;
			APtFrom = Point(0, 0);
			APtTo = Point(0, 0);
			APtSide = Point(0, 0);
		}

		int iPoint;
		int iDistance;
		bool bInverted;
		TPoint APtFrom;
		TPoint APtTo;
		TPoint APtSide;

	};

	TRect CombineRects(const TRect &ARect1, const TRect &ARect2);

	void CalculateSquareConnection(const TRect &AFrom, const TRect &ATo, const unsigned int iMargin, TPrecalcedSquareConnection &APrecalced);

	void SetShapeTextAlign(TTreeNodeShape *AShape, const TDirectionType AType);
}

#endif

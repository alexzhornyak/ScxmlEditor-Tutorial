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

#include "UnitGraphViz.h"

#include <memory>
#include <vector>
#include <StrUtils.hpp>
#include <IOUtils.hpp>

#include "Log4cpp_VCL.hpp"
#include "ScxmlShapes.hpp"
#include "UTF8NoBomEncoding.hpp"
#include "UnitStateMachineConnection.h"
#include "UnitHTMLUtils.h"
#include "UnitTreeEditorUtils.h"
#include "UnitSettings.h"
#include "UnitSpawnAndRedirect.h"
#include "UnitDialogWait.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Graphviz {

	UnicodeString MakeClusterName(TTreeNodeShape *AShape) {
		return "cluster_" + AShape->Name;
	}

	UnicodeString MakeChildrenName(TTreeNodeShape *AShape) {
		return AShape->Name + "_children";
	}

	TVisualScxmlBaseShape * FindNonClusterNode(TTreeNodeShape *AShape, UnicodeString &sNodeName) {
		if (AShape) {
			if (Editorutils::IsTreeNodeShapeClusterOrHasScxmlChildren(AShape)) {
				for (int i = 0; i < AShape->Children->Count; i++) {
					if (TVisualScxmlBaseShape * AVisualShape = FindNonClusterNode(AShape->Children->Items[i], sNodeName)) {
						return AVisualShape;
					}
				}
			}
			else {
				if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape)) {
					sNodeName = AShape->Name;
					return AVisualScxmlBaseShape;
				}
				else if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(AShape)) {
					TVisualScxmlBaseShape * AVisualParent = AChildScxmlBaseShape->VisualParent;
					if (AVisualParent) {
						sNodeName = MakeChildrenName(AVisualParent);
						return AVisualParent;
					}
				}

			}
		}
		return NULL;
	}

	void IterateChildScxmlNodes(TTreeNodeShape * AShape, TStringList *AOutput, const UnicodeString &sTab) {
		if (AShape) {
			for (int i = 0; i < AShape->Children->Count; i++) {
				if (TChildScxmlBaseShape * AChildScxmlBaseShape = dynamic_cast<TChildScxmlBaseShape*>(AShape->Children->Items[i])) {

					AOutput->Add(sTab + Htmlutils::StrXHtmlEncodeEx(AChildScxmlBaseShape->SimpleText) + "<br align=\"left\"/>");

					IterateChildScxmlNodes(AShape->Children->Items[i], AOutput, sTab + "    ");
				}
			}
		}
	}

	UnicodeString GetHTMLConnectionText(TStateMachineConnection * AStateMachineConnection) {
		std::auto_ptr<TStringList>ACaptionListPtr(new TStringList());
		for (int k = 0; k < AStateMachineConnection->TextCount; k++) {
			ACaptionListPtr->Add(Htmlutils::StrToHTMLColorStr(AStateMachineConnection->Text->Strings[k], clTeal, k == 0) + L"<br/>");
		}
		return ACaptionListPtr->Text;
	}

	void AddStringDummy(const UnicodeString &sText, TStrings *AOutput) {
		AOutput->Add(Htmlutils::StrXHtmlEncodeEx(sText) + L"<br/>");
	}

	void ParseTreeNode(TTreeNodeShape *AShape, TStrings *AOutput, const UnicodeString &sTab) {
		TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
		if (AVisualShape) {

			const bool bIsCluster = Editorutils::IsTreeNodeShapeClusterOrHasScxmlChildren(AShape);

			std::auto_ptr<TStringList>ACaptionListPtr(new TStringList());

			for (int i = AVisualShape->Connections->Count - 1; i >= 0; i--) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
				(AVisualShape->Connections->Items[i]);
				if (AStateMachineConnection) {
					if (AStateMachineConnection->IsSelfConnection) {
						for (int k = 0; k < AStateMachineConnection->TextCount; k++) {
							ACaptionListPtr->Add(Htmlutils::StrToHTMLColorStr( //
									(k == 0 ? L"<---- " : L" ") + //
									AStateMachineConnection->Text->Strings[k] //
									+ (k == 0 ? L" ---->" : L" ") //
									, clMaroon, k == 0) + //
								L"<br/>" + UnicodeString(k == AStateMachineConnection->TextCount - 1 ? L"<br/>" : L""));
						}
					}
				}
			}

			ACaptionListPtr->Add(Htmlutils::StrXHtmlEncodeEx(AShape->SimpleText));

			std::auto_ptr<TStringList>APseudoChildrenListPtr(new TStringList());

			if (bIsCluster) {

				IterateChildScxmlNodes(AShape, APseudoChildrenListPtr.get(), L"    ");

				AOutput->Add(sTab + "subgraph " + MakeClusterName(AShape) + " {");
				AOutput->Add(sTab + "\t" "label=<" //
					+ ACaptionListPtr->Text + ">;");
			}
			else {
				AOutput->Add(sTab + AShape->Name + " [shape=rectangle label=<" //
					+ ACaptionListPtr->Text + ">];");
			}

			for (int i = 0; i < AShape->Children->Count; i++) {
				ParseTreeNode(AShape->Children->Items[i], AOutput, sTab + "\t");
			}

			if (APseudoChildrenListPtr->Count) {
				AOutput->Add(sTab + MakeChildrenName(AShape) + " [shape=rectangle fontcolor=blue label=<\n" +
					APseudoChildrenListPtr->Text + "\n>];");
			}

			if (bIsCluster) {
				AOutput->Add(sTab + "}");
				AOutput->Add("");
			}
		}
	}

	int GetVisualChildrenCount(TTreeNodeShape *AShape) {
		int iCount = 0;
		TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
		if (AVisualShape) {
			for (int i = 0; i < AVisualShape->Children->Count; i++) {
				TVisualScxmlBaseShape * AVisualChildShape = dynamic_cast<TVisualScxmlBaseShape*>(AVisualShape->Children->Items[i]);
				if (AVisualChildShape) {
					iCount++;
				}
			}
		}
		return iCount;
	}

	int GetSelfConnectionCount(TTreeNodeShape *AShape) {
		int iCount = 0;
		TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
		if (AShape) {
			for (int i = 0; i < AShape->Connections->Count; i++) {
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(AShape->Connections->Items[i]);
				if (AStateMachineConnection && AStateMachineConnection->IsSelfConnection) {
					iCount++;
				}
			}
		}
		return iCount;
	}

	void ExportToDot(TCustomTree *ATree, TStringList *AOutputList) {
		if (!ATree)
			throw Exception("'ATree' is NULL!");

		if (!AOutputList)
			throw Exception("'AOutputList' is NULL!");

		AOutputList->Add("digraph {");
		AOutputList->Add("\t" "compound=true;");
		AOutputList->Add("\t" "rankdir=TB;");

		switch(SettingsData->AutoLayoutSplines) {
		case alsPolyline:
			AOutputList->Add(L"\t" "splines=polyline;");
			break;
		default:
			AOutputList->Add(L"\t" "splines=ortho;");
			// при XLabels сдвигаются расстояния, и происходят наложения, поэтому увеличиваем
			AOutputList->Add(L"\t" "ranksep=1.2;");
		}

		/* по умолчанию размер шрифта 14, это дает небольшие отступы для фигур */
		AOutputList->Add("graph [ fontname=Arial ];");
		AOutputList->Add("node [ fontname=Arial ];");
		/* для соединений уменьшаем размер, так как запаса пустого пространства хватает */
		AOutputList->Add("edge [ fontname=Arial, fontsize=10 ];");

		for (int i = 0; i < ATree->Roots->Count; i++) {
			ParseTreeNode(ATree->Roots->Items[i], AOutputList, "\t");
		}

		for (int i = 0; i < ATree->Connections->Count; i++) {
			TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(ATree->Connections->Items[i]);
			if (AStateMachineConnection && !AStateMachineConnection->IsSelfConnection) {

				UnicodeString sFromName, sToName;
				TVisualScxmlBaseShape *AVisualFrom = FindNonClusterNode(AStateMachineConnection->FromShape, sFromName);
				TVisualScxmlBaseShape *AVisualTo = FindNonClusterNode(AStateMachineConnection->ToShape, sToName);
				if (AVisualFrom && AVisualTo) {

					UnicodeString sEdge = "\t" + sFromName + "->" + sToName;

					UnicodeString sExtra = "tooltip=\"" + AStateMachineConnection->Name + "\",color=\"teal\"";

					const bool bIsFromCluster = Editorutils::IsTreeNodeShapeClusterOrHasScxmlChildren(AStateMachineConnection->FromShape);
					const bool bIsToCluster = Editorutils::IsTreeNodeShapeClusterOrHasScxmlChildren(AStateMachineConnection->ToShape);

					if (bIsFromCluster) {
						sExtra = sExtra + ",ltail=" + AnsiQuotedStr(MakeClusterName(AStateMachineConnection->FromShape), L'"');
					}
					if (bIsToCluster) {
						sExtra = sExtra + ",lhead=" + AnsiQuotedStr(MakeClusterName(AStateMachineConnection->ToShape), L'"');
					}

					switch(SettingsData->AutoLayoutSplines) {
					case alsOrtho: {
							const bool bIsFromTrueCluster = Editorutils::IsTreeNodeShapeCluster(AStateMachineConnection->FromShape);
							const bool bIsToTrueCluster = Editorutils::IsTreeNodeShapeCluster(AStateMachineConnection->ToShape);
							if (bIsFromTrueCluster || bIsToTrueCluster) {
								int iMinlen = 2;

								if (bIsFromTrueCluster && bIsToTrueCluster && AStateMachineConnection->FromShape->Parent ==
									AStateMachineConnection->ToShape->Parent) {

									iMinlen = 4;
								}

								sExtra = sExtra + ",minlen=" + UnicodeString(iMinlen);
							}
						}break;
					}

					if (!AStateMachineConnection->Text->Text.IsEmpty()) {
						switch(SettingsData->AutoLayoutSplines) {
						case alsOrtho:
							sExtra = sExtra + ",xlabel=<" + GetHTMLConnectionText(AStateMachineConnection) + ">";
							break;
						case alsPolyline:
						default:
							sExtra = sExtra + ",label=<" + GetHTMLConnectionText(AStateMachineConnection) + ">";
							break;
						}
					}

					if (!sExtra.IsEmpty()) {
						sEdge = sEdge + "[" + sExtra + "]";
					}

					AOutputList->Add(sEdge);
				}
				else {
					WLOG_ERROR(L"Can not build edge for [%s]:[%s]", AStateMachineConnection->Name.c_str(),
						AStateMachineConnection->Caption.c_str());
				}
			}
		}

		AOutputList->Add("}");
#if 0 // для глубокой отладки
		WLOG_DEBUG(L"%s", AOutputList->Text.c_str());
#endif
	}

	void ExportToDot(TCustomTree *ATree, const UnicodeString &sFileName) {
		// сохраняем Dot в текстовый файл
		std::auto_ptr<TStringList>AOutListPtr(new TStringList());
		ExportToDot(ATree, AOutListPtr.get());

		// конвертируем в UTF-8 без BOM, иначе парсер неправильно принимает значения
		std::auto_ptr<TUTF8EncodingNoBOM>AUTF8EncodingPtr(new TUTF8EncodingNoBOM());
		AOutListPtr->SaveToFile(sFileName, AUTF8EncodingPtr.get());
	}

	typedef std::vector<TFloatPoint>TVecFloatPoints;

	void GetPointsFromString(const UnicodeString &sText, TVecFloatPoints &AVecPoints) {
		std::auto_ptr<TStringList>APointsListPtr(new TStringList());
		APointsListPtr->Delimiter = L' ';
		APointsListPtr->StrictDelimiter = true;
		APointsListPtr->DelimitedText = sText;
		for (int i = 0; i < APointsListPtr->Count; i++) {
			std::auto_ptr<TStringList>APairListPtr(new TStringList());
			APairListPtr->CommaText = APointsListPtr->Strings[i];

			if (APairListPtr->Count != 2)
				throw Exception("Expect 2 points but got " + UnicodeString(APairListPtr->Count));

			TFloatPoint APoint;
			APoint.X = APairListPtr->Strings[0].ToDouble();
			APoint.Y = APairListPtr->Strings[1].ToDouble();

			AVecPoints.push_back(APoint);
		}
	}

	// #define _OVERLAP_DEBUG_
	/* Для углубленной отладки пересечений используем ассоциированные прямоугольники */
	/* делаем через Define, чтобы оптимизировать время */
#ifdef _OVERLAP_DEBUG_
	typedef std::pair<TRect, UnicodeString>TURect;
#else
	typedef TRect TURect;
#endif

	bool IsOverlappedWithVecRect(const TURect &ARect, const std::vector<TURect> &AVecRect) {
		for (std::vector<TURect>::const_iterator it = AVecRect.begin(); it != AVecRect.end(); ++it) {
			TRect AIntersect;
#ifdef _OVERLAP_DEBUG_
			if (IntersectRect(AIntersect, it->first, ARect.first)) {

				WLOG_DEBUG(L"[%s](%d,%d,%d,%d) is overlapped with [%s](%d,%d,%d,%d)", //
					ARect.second.c_str(), ARect.first.left, ARect.first.top, ARect.first.right, ARect.first.bottom, //
					it->second.c_str(), it->first.left, it->first.top, it->first.right, it->first.bottom);

				return true;
			}
#else
			if (IntersectRect(AIntersect, *it, ARect))
				return true;
#endif
		}
		return false;
	}

	void ImportFromSvgDot(TCustomTree *ATree, const UnicodeString &sFileName) {
		if (!FileExists(sFileName))
			throw Exception("File <" + sFileName + "> is not found!");

		// используем LMD, так как у него менее геморная работа с namespace
		_di_ILMDXmlDocument ADoc = LMDLoadXmlDocument(sFileName);
		if (!ADoc)
			throw Exception("Can not load SVG document!");

		if (!ADoc->DocumentElement)
			throw Exception("SVG document element is EMPTY!");

		const int iBoundsHeight = StringReplace(ADoc->DocumentElement->GetAttr("height"), "pt", "", TReplaceFlags()).ToInt();

		std::map<TStateMachineConnection*, TVecFloatPoints>AMapConnectionPoints;

		std::vector<TURect>AVecConnectionText;

		Editorutils::TreeUpdateLock ALock(ATree);

		for (int i = 0; i < ADoc->DocumentElement->ChildNodes->Count; i++) {
			_di_ILMDXmlNode ANode = ADoc->DocumentElement->ChildNodes->Item[i];
			if (ANode->NodeName == "g" && ANode->GetAttr("class", "") == "graph") {

				for (int k = 0; k < ANode->ChildNodes->Count; k++) {

					_di_ILMDXmlNode AElement = ANode->ChildNodes->Item[k];

					const UnicodeString sClass = AElement->GetAttr("class", "");

					if (sClass == "cluster" || sClass == "node") {
						// работать только через 'points', так как 'bb' может и не быть
						TVecFloatPoints AVecBoundPoints, AVecLabelPoints;

						UnicodeString sObjectName = "";
						try {
							for (int n = 0; n < AElement->ChildNodes->Count; n++) {
								_di_ILMDXmlNode ASubElement = AElement->ChildNodes->Item[n];
								if (ASubElement->NodeName == "title") {
									sObjectName = ASubElement->Text;
								}
								else if (ASubElement->NodeName == "polygon") {
									GetPointsFromString(ASubElement->GetAttr("points"), AVecBoundPoints);
								}
								else if (ASubElement->NodeName == "text") {
									TFloatPoint ALabelPoint;
									ALabelPoint.X = ASubElement->GetFloatAttr("x");
									ALabelPoint.Y = ASubElement->GetFloatAttr("y");

									AVecLabelPoints.push_back(ALabelPoint);
								}

							}

							if (!sObjectName.IsEmpty()) {

								bool bIsChild = false;

								if (sObjectName.Pos("cluster_") == 1) {
									sObjectName = StringReplace(sObjectName, "cluster_", "", TReplaceFlags());
								}
								else if (sObjectName.Pos("_children")) {
									bIsChild = true;
									sObjectName = StringReplace(sObjectName, "_children", "", TReplaceFlags());
								}

								TVisualScxmlBaseShape *AVisualShape = Editorutils::FindShapeByName<TVisualScxmlBaseShape>(ATree,
									sObjectName);
								if (!AVisualShape)
									throw Exception("Can not find shape on chart!");

								if (AVecBoundPoints.size() != 5)
									throw Exception("Expect 5 points but got [" + UnicodeString(AVecBoundPoints.size()) + "]");

								TPoint APoints[4];
								for (std::size_t n = 0; n < AVecBoundPoints.size(); n++) {
									APoints[n].x = RoundTo(AVecBoundPoints[n].X, 0);
									APoints[n].y = iBoundsHeight + RoundTo(AVecBoundPoints[n].Y, 0);
								}
								TRect ARect = RectFromPolygon(APoints, 3, 4);

								if (bIsChild) {
									const int iChildrenOffsetX = ARect.left - AVisualShape->X0;

									if (iChildrenOffsetX > 20) {
										AVisualShape->ChildrenAlignXOffset = iChildrenOffsetX;
									}

									TRect ARectText(ARect);
									ARectText.top = std::min(ARect.Top, ARect.Bottom);
									ARectText.bottom = std::max(ARect.Top, ARect.Bottom);

									/*
									---------------
									|   Shape1    |
									---------------
									--> |children| <--

									Размер прямоугольника с дочерними фигурами в реальности меньше слева и справа
									 */

									const int iCutRight = ARectText.Width() - RoundTo((double)ARectText.Width() * 0.85f, 0);
									const int iCutLeft = 25;
									ARectText.left += iCutLeft;
									const int iNewWidth = ARectText.Width() - iCutRight;
									if (iNewWidth > 10) {
										ARectText.right = ARectText.left + iNewWidth;
#ifdef _OVERLAP_DEBUG_
										AVecConnectionText.push_back(std::make_pair(ARectText, "children_" + AVisualShape->SimpleText));
#else
										AVecConnectionText.push_back(ARectText);
#endif
									}

								}
								else {
									AVisualShape->ChildrenAlignX = scaxLeft;
									AVisualShape->ChildrenAlignY = scayClusterTop;
									AVisualShape->ChildrenAlignXOffset = 0;

									AVisualShape->X0 = ARect.Left;

									/* по оси Y делаем вертикально отражение (от высоты фигуры отнимаем Y) */
									/* Y0 определять до Y1, иначе потом растягивает */
									// это говорит о том, что у нас есть Self-Connection
									if (AVecLabelPoints.size() > 1) {
										const int iLabelIndex = AVecLabelPoints.size() - 1;
										AVisualShape->Y0 = iBoundsHeight + RoundTo(AVecLabelPoints[iLabelIndex].Y, 0) - 14;
									}
									else {
										AVisualShape->Y0 = std::min(ARect.Top, ARect.Bottom);
									}

									switch(AVisualShape->StateChildType) {
									case sctInitial:
									case sctFinal:
									case sctHistory: {
											AVisualShape->X1 = AVisualShape->X0 + 25;
											AVisualShape->Y1 = AVisualShape->Y0 + 25;
										}break;
									default: {
											AVisualShape->X1 = ARect.Right;

											AVisualShape->Y1 = std::max(ARect.Top, ARect.Bottom);

											// экспериментально подобранная высота, относительно высоты фигуры,
											// которую строит Graphviz при одной строке в тексте фигуры
											const int i_MINIMUM_SHAPE_HEIGHT = 37;

											if (!AVisualShape->IsCluster() && Editorutils::HasVisualShapeScxmlChildren(AVisualShape)) {
												AVisualShape->Y1 = AVisualShape->Y0 + i_MINIMUM_SHAPE_HEIGHT;
												// поправка на значек expand|collapse
												AVisualShape->X1 = AVisualShape->X1 - 10;
											}
											else {
												if (AVisualShape->Y1 - AVisualShape->Y0 < i_MINIMUM_SHAPE_HEIGHT) {
													AVisualShape->Y1 = AVisualShape->Y0 + i_MINIMUM_SHAPE_HEIGHT;
												}
											}

										}break;
									}

									Editorutils::ExpandAll(AVisualShape);

									AVisualShape->ArrangeDefaultSelfConnections();

									const TRect ARectText(AVisualShape->X0, AVisualShape->Y0, //
										AVisualShape->X1, AVisualShape->IsCluster() ? //
										(AVisualShape->Y0 + AVisualShape->GetClusterHeight()) : AVisualShape->Y1);
#ifdef _OVERLAP_DEBUG_
									AVecConnectionText.push_back(std::make_pair(ARectText, AVisualShape->SimpleText));
#else
									AVecConnectionText.push_back(ARectText);
#endif

								}
							}
						}
						catch(Exception * E) {
							throw Exception(UnicodeString().sprintf(L"Object:[%d] Name:[%s] Reason:[%s]", k, sObjectName.c_str(),
									E->Message.c_str()));
						}

					}
					else if (sClass == "edge") {
						try {
							ILMDXmlElement * ASubEdge = AElement->FindElement("g", L"", Variant());
							if (!ASubEdge)
								throw Exception("Can not find <g> element!");

							ILMDXmlElement * ABodyA = ASubEdge->FindElement("a", L"", Variant());
							if (!ABodyA)
								throw Exception("Can not find <a> element!");

							ILMDXmlElement * APath = ABodyA->FindElement("path", L"", Variant());
							if (!APath)
								throw Exception("Can not find <path> element!");

							const UnicodeString sConnectionName = ABodyA->GetAttr("xlink:title");

							TStateMachineConnection *AConnection = Editorutils::FindConnectionByName<TStateMachineConnection>(ATree,
								sConnectionName);
							if (!AConnection)
								throw Exception("Can not find connection:[" + sConnectionName + "] on chart!");

							// пользователь пропускает AutoLayout для этого соединения
							if (AConnection->SkipAutolayout)
								continue;

							UnicodeString sPath = APath->GetAttr("d");
							const int iIndexC = sPath.Pos("C");

							if (!iIndexC)
								throw Exception("Path require 'C' keyword! [" + sPath + "]");

							const UnicodeString sTextM = sPath.SubString(2, iIndexC - 2);
							const UnicodeString sTextC = sPath.Delete(1, iIndexC);

							TVecFloatPoints AVecBoundPoints;
							GetPointsFromString(sTextM, AVecBoundPoints);
							GetPointsFromString(sTextC, AVecBoundPoints);

							if (AVecBoundPoints.size() < 2)
								throw Exception("Require at least 2 points!");

							TFloatPoint ATextPoint;
							ATextPoint.X = 0.0f;
							ATextPoint.Y = 0.0f;
							ILMDXmlElement *AText = AElement->FindElement("text", L"", Variant());
							if (AText) {
								if (AText->AttrExists("x") && AText->AttrExists("y")) {
									ATextPoint.X = AText->GetFloatAttr("x");
									ATextPoint.Y = AText->GetFloatAttr("y");
								}
							}

							// последней точкой будет точка текста
							AVecBoundPoints.push_back(ATextPoint);

							AMapConnectionPoints.insert(std::make_pair(AConnection, AVecBoundPoints));

						}
						catch(Exception * E) {
							throw Exception(UnicodeString().sprintf(L"Edge:[%d] Name:[%s] Reason:[%s]", k,
									AElement->GetAttr("id", "").c_str(), E->Message.c_str()));
						}

					}

				}

				break;
			}
		}

		for (int i = 0; i < ATree->Connections->Count; i++) {
			TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(ATree->Connections->Items[i]);
			if (AStateMachineConnection && AStateMachineConnection->IsSelfConnection) {
				AStateMachineConnection->CalculateTextPosWithoutDraw();

				TRect ARectText(AStateMachineConnection->GetDrawTextData().Bounds);
				Editorutils::IncrementRect(ARectText, 0);

#ifdef _OVERLAP_DEBUG_
				AVecConnectionText.push_back(std::make_pair(ARectText, AStateMachineConnection->Caption));
#else
				AVecConnectionText.push_back(ARectText);
#endif

#if 0
				WLOG_DEBUG(L"Self-conn:[%s] Bounds:[%d,%d,%d,%d] Shape:[%d,%d,%d,%d]", AStateMachineConnection->Caption.c_str(),
					ARectText.left, ARectText.top, ARectText.Width(), ARectText.Height(), //
					AStateMachineConnection->FromShape->X0, AStateMachineConnection->FromShape->Y0, //
					AStateMachineConnection->FromShape->X1, AStateMachineConnection->FromShape->Y1);
#endif
			}
		}

		for (std::map<TStateMachineConnection*, TVecFloatPoints>::iterator it = AMapConnectionPoints.begin();
			it != AMapConnectionPoints.end(); ++it) {

			TStateMachineConnection *AConnection = it->first;

			// заложил получение позиции текста,
			// но дальше не использую, так как нормально не вяжется с механизмом меток TeeTree
			TFloatPoint ATextPoint;
			ATextPoint = *it->second.rbegin();
			it->second.pop_back();

			const TVecFloatPoints &AVecBoundPoints = it->second;

			AConnection->Style = csLine;
			AConnection->AdjustableSides = true;
			AConnection->Text->VertAlign = vtaCenter;
			AConnection->Text->HorizAlign = htaCenter;
			AConnection->VerticalCentering = false;
			AConnection->Text->VertOffset = 0;
			AConnection->Text->HorizOffset = 0;

			AConnection->Points->Clear();

			const std::size_t nLastIndex = AVecBoundPoints.size() - 1;

			for (std::size_t n = 0; n < AVecBoundPoints.size(); n++) {
				TPoint APt(RoundTo(AVecBoundPoints[n].X, 0), iBoundsHeight + RoundTo(AVecBoundPoints[n].Y, 0));

				// ввиду ограничений Dot, размеры фигур уменьшены,
				// поэтому необходимо корректировать первые и последние точки по Y

				if (n == 0) {

					if (!Editorutils::PtInRectEx(AConnection->FromShape->Bounds(), APt)) {
						double X0clip, X1clip, Y0clip, Y1clip;

						if (Editorutils::LiangBarsky(AConnection->FromShape->X0, AConnection->FromShape->X1, -AConnection->FromShape->Y1,
								-AConnection->FromShape->Y0, APt.x, -APt.y, AConnection->FromShape->XCenter(),
								-AConnection->FromShape->YCenter(), X0clip, Y0clip, X1clip, Y1clip)) {

							APt.y = RoundTo(-Y0clip, 0);
						}
					}

					Editorutils::EnsurePointInShapeRect(AConnection->FromShape, APt);

					APt.x = RoundTo((double)(APt.x - AConnection->FromShape->X0) / (double)
						(AConnection->FromShape->X1 - AConnection->FromShape->X0) * 100.0f, 0);
					APt.y = RoundTo((double)(APt.y - AConnection->FromShape->Y0) / (double)
						(AConnection->FromShape->Y1 - AConnection->FromShape->Y0) * 100.0f, 0);

					// добавляем именно в виде процента, так как смена типа после приводит к смещению точки
					AConnection->Points->Add(cpsFromPercent, APt.x, cpsFromPercent, APt.y);
				}
				else if (n == nLastIndex) {

					if (!Editorutils::PtInRectEx(AConnection->ToShape->Bounds(), APt)) {
						double X0clip, X1clip, Y0clip, Y1clip;

						if (Editorutils::LiangBarsky(AConnection->ToShape->X0, AConnection->ToShape->X1, -AConnection->ToShape->Y1,
								-AConnection->ToShape->Y0, APt.x, -APt.y, AConnection->ToShape->XCenter(),
								-AConnection->ToShape->YCenter(), X0clip, Y0clip, X1clip, Y1clip)) {

							APt.y = RoundTo(-Y0clip, 0);
						}
					}

					Editorutils::EnsurePointInShapeRect(AConnection->ToShape, APt);

					APt.x = RoundTo((double)(APt.x - AConnection->ToShape->X0) / (double)
						(AConnection->ToShape->X1 - AConnection->ToShape->X0) * 100.0f, 0);
					APt.y = RoundTo((double)(APt.y - AConnection->ToShape->Y0) / (double)
						(AConnection->ToShape->Y1 - AConnection->ToShape->Y0) * 100.0f, 0);

					// добавляем именно в виде процента, так как смена типа после приводит к смещению точки
					AConnection->Points->Add(cpsToPercent, APt.x, cpsToPercent, APt.y);
				}
				else {
					AConnection->Points->Add(cpsFixed, APt.x, cpsFixed, APt.y);

					// меняем тип привязки, чтобы точка перемещалась вместе
					AConnection->Points->ChangeXStyle(n, n <= AVecBoundPoints.size() / 2 ? cpsFromRel : cpsToRel);
					AConnection->Points->ChangeYStyle(n, n <= AVecBoundPoints.size() / 2 ? cpsFromRel : cpsToRel);
				}

			}

			if (SettingsData->AutoLayoutUseNativeLabelPos) {
				/* Оригинальное расположение меток Graphviz */
				AConnection->CalculateTextPosWithoutDraw();
				const TDrawTextData ADrawTextData = AConnection->GetDrawTextData();
				int iBoundsCenterX, iBoundsCenterY;
				RectCenter(ADrawTextData.Bounds, iBoundsCenterX, iBoundsCenterY);
				const TPoint ATextPos = Point(ADrawTextData.Bounds.Left, ADrawTextData.Bounds.Top);

				AConnection->Text->VertOffset = (iBoundsHeight + ATextPoint.Y) - iBoundsCenterY;
				AConnection->Text->HorizOffset = ATextPoint.X + 10 - iBoundsCenterX; // поправка на 10 из-за разности шрифтов
			}

			// этот метод просчитывает границы соединения и координаты текстовой метки
			AConnection->CalculateTextPosWithoutDraw();

			const TRect AConnectionBounds = AConnection->GetBounds();
			// при соединении кластеров иногда создаются очень маленькие соединения,
			// тогда используем свой алгоритм
			if (AConnectionBounds.Width() < 25 && AConnectionBounds.Height() < 25) {
				WLOG_WARNING(L"Connection:[%s] has small bounds(%d,%d,%d,%d)", AConnection->Caption.c_str(), AConnectionBounds.left,
					AConnectionBounds.top, AConnectionBounds.right, AConnectionBounds.bottom);

				AConnection->Text->VertOffset = 0;
				AConnection->Text->HorizOffset = 0;

				TTreeNodeShape *AFromShape = AConnection->FromShape;
				TTreeNodeShape *AToShape = AConnection->ToShape;
				if (AFromShape && AToShape) { // по-нормальному должно всегда выполняться
					Editorutils::TPrecalcedSquareConnection APrecalced;
					Editorutils::CalculateSquareConnection(AFromShape->Bounds(), AToShape->Bounds(), 25, APrecalced);
					if (APrecalced.iPoint != -1) {
						AConnection->AdjustableSides = false;
						AConnection->Style = APrecalced.bInverted ? csInvertedSides : csSides;
						AConnection->Points->Clear();

						const int iPtPercentFirstX = RoundTo((double)(APrecalced.APtFrom.x - AFromShape->X0) * 100.0f / (double)
							AFromShape->Width, 0);
						const int iPtPercentFirstY = RoundTo((double)(APrecalced.APtFrom.y - AFromShape->Y0) * 100.0f / (double)
							AFromShape->Height, 0);

						const int iPtPercentLastX = RoundTo((double)(APrecalced.APtTo.x - AToShape->X0) * 100.0f / (double)AToShape->Width,
							0);
						const int iPtPercentLastY = RoundTo((double)(APrecalced.APtTo.y - AToShape->Y0) * 100.0f / (double)
							AToShape->Height, 0);

						const int iFirst = AConnection->Points->Add(cpsFromPercent, iPtPercentFirstX, cpsFromPercent, iPtPercentFirstY);
						const int iLast = AConnection->Points->Add(cpsToPercent, iPtPercentLastX, cpsToPercent, iPtPercentLastY);

						AConnection->AdjustableSides = true;

						Editorutils::AdjustSidesStyle(AConnection);
					}
					else {
						// если у фигур равные размеры, тогда соединяем через верх или сторону
						WLOG_WARNING(L"Can not create direct square connection:[%s]!", AConnection->Caption.c_str());

						const int iTopDiff = AConnection->FromShape->YCenter() - AConnection->ToShape->YCenter();
						const int iLeftDiff = AConnection->FromShape->XCenter() - AConnection->ToShape->XCenter();

						if (abs(iTopDiff) > abs(iLeftDiff)) {
							AConnection->SetConnectionOverStyle(cotOverLeft);
						}
						else {
							AConnection->SetConnectionOverStyle(cotOverTop);
						}
					}

					/* финальные действия */
					Editorutils::ProtectConnectionToBeInSamePoints(AConnection);
					AConnection->CalculateTextPosWithoutDraw();
				}
				else {
					WLOG_WARNING(L"Connection:[%s] has incomplete From|To shapes!", AConnection->Caption.c_str());
				}

			}

			/* проверка на пересечение меток друг с другом и атомарными фигурами */
			if (SettingsData->AutoLayoutCheckOverlappedLabel && AConnection->TextCount) {
				const TDrawTextData ADrawTextData = AConnection->GetDrawTextData();

				TRect ATextRect = ADrawTextData.Bounds;
				if (!IsRectEmpty(ATextRect)) {

					const int i_INCREMENT = 1;

					Editorutils::IncrementRect(ATextRect, i_INCREMENT);

					TRect ARects[8] = {
						ATextRect, ATextRect, ATextRect, ATextRect, ATextRect, ATextRect, ATextRect, ATextRect
					};

					int iDirection = 0;

					const int i_CHECK_INCREMENT = 1;
#ifdef _OVERLAP_DEBUG_
					while (IsOverlappedWithVecRect(std::make_pair(ATextRect, AConnection->Caption), AVecConnectionText))
#else
						while (IsOverlappedWithVecRect(ATextRect, AVecConnectionText))
#endif

						{
							switch(iDirection) {
							case 0: {
									ARects[iDirection].left -= i_CHECK_INCREMENT;
									ARects[iDirection].right -= i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 1: {
									ARects[iDirection].left += i_CHECK_INCREMENT;
									ARects[iDirection].right += i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 2: {
									ARects[iDirection].top -= i_CHECK_INCREMENT;
									ARects[iDirection].bottom -= i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 3: {
									ARects[iDirection].top += i_CHECK_INCREMENT;
									ARects[iDirection].bottom += i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 4: {
									ARects[iDirection].top += i_CHECK_INCREMENT;
									ARects[iDirection].bottom += i_CHECK_INCREMENT;
									ARects[iDirection].left += i_CHECK_INCREMENT;
									ARects[iDirection].right += i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 5: {
									ARects[iDirection].top -= i_CHECK_INCREMENT;
									ARects[iDirection].bottom -= i_CHECK_INCREMENT;
									ARects[iDirection].left -= i_CHECK_INCREMENT;
									ARects[iDirection].right -= i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 6: {
									ARects[iDirection].top += i_CHECK_INCREMENT;
									ARects[iDirection].bottom += i_CHECK_INCREMENT;
									ARects[iDirection].left -= i_CHECK_INCREMENT;
									ARects[iDirection].right -= i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							case 7: {
									ARects[iDirection].top -= i_CHECK_INCREMENT;
									ARects[iDirection].bottom -= i_CHECK_INCREMENT;
									ARects[iDirection].left += i_CHECK_INCREMENT;
									ARects[iDirection].right += i_CHECK_INCREMENT;

									ATextRect = ARects[iDirection];
								}break;
							}

							iDirection++;
							if (iDirection == 8)
								iDirection = 0;
						}

					const int iTextOffsetX = (ATextRect.left - i_INCREMENT) - ADrawTextData.Bounds.left;
					const int iTextOffsetY = (ATextRect.top - i_INCREMENT) - ADrawTextData.Bounds.top;
					AConnection->Text->HorizOffset = AConnection->Text->HorizOffset + iTextOffsetX;
					AConnection->Text->VertOffset = AConnection->Text->VertOffset + iTextOffsetY;
#if 0
					WLOG_DEBUG(
						L"Connection:[%s] Text polygon: L=%d T=%d R=%d B=%d Graphiv poly: X=%.f Y=%.f CalcOffsetX:%d CalcOffsetY:%d ResultOffsetX:%d ResultOffsetY:%d",
						//
						AConnection->Caption.c_str(), ATextRect.left, ATextRect.top, ATextRect.right, ATextRect.bottom, ATextPoint.X,
						ATextPoint.Y, iTextOffsetX, iTextOffsetY, AConnection->Text->HorizOffset, AConnection->Text->VertOffset);
#endif

#ifdef _OVERLAP_DEBUG_
					AVecConnectionText.push_back(std::make_pair(ATextRect, AConnection->Caption));
#else
					AVecConnectionText.push_back(ATextRect);
#endif

				}
				else {
					WLOG_WARNING(L"Connection:[%s] has empty text rect!", AConnection->Caption.c_str());
				}
			}

		} // end work with connection
	}

	UnicodeString GetGraphvizPath(void) {
		return TPath::Combine(SettingsData->AppPath, L"Graphviz");
	}

	void ArrangeTreeLayout(TCustomTree *ATree) {
		if (!ATree)
			throw Exception(L"Tree is NULL!");

		SettingsData->ProfilerReset(__FUNCTION__);

		const UnicodeString sGraphVizPath = GetGraphvizPath();

		if (!DirectoryExists(sGraphVizPath))
			throw Exception(L"Graphviz is not installed! Path not found:[" + sGraphVizPath + L"]!");

		const UnicodeString sGraphVisTempFile = TPath::Combine(sGraphVizPath, L"_temp_");
		const UnicodeString sDotFile = sGraphVisTempFile + L".gv";
		const UnicodeString sSvgFile = sGraphVisTempFile + L".svg";

		try {
			// проверяем, чтобы не оставались хвосты с прошлого раза
			DeleteFile(sDotFile);
			DeleteFile(sSvgFile);

			std::auto_ptr<TDialogWait>ADialogWaitPtr(new TDialogWait(NULL));
			ADialogWaitPtr->ShowDialog(L"Exporting to dot ...");

			ExportToDot(ATree, sDotFile);

			ADialogWaitPtr->ShowDialog(L"Building graphs ...");

#ifdef _DEBUG // для глубокой отладки
			{

				const UnicodeString sCMD = TPath::Combine(sGraphVizPath, L"dot.exe")
					+ L" -Tpng \"" + sDotFile + L"\" -o \"" + sGraphVisTempFile + L".png\"";

				TSpawnAndRedirect ASpawner(sCMD, sGraphVizPath);
				ASpawner.ConvertOutputEncoding = coeOEM866;
				ASpawner.StartProcessInfinite();
			}

#endif
			try {
				const UnicodeString sCMD = TPath::Combine(sGraphVizPath, L"dot.exe")
					+ L" -Tsvg \"" + sDotFile + L"\" -o \"" + sSvgFile + L"\"";

				TSpawnAndRedirect ASpawner(sCMD, sGraphVizPath);
				ASpawner.ConvertOutputEncoding = coeOEM866;
				ASpawner.StartProcessInfinite();
			}
			catch(Exception * E) {
				throw Exception(E->Message + L" Check that 'Graphviz\\Dot.exe' is present and 'MSVC 2015 runtime' is installed!");
			}

			ADialogWaitPtr->ShowDialog(L"Applying layout ...");
			ImportFromSvgDot(ATree, sSvgFile);
		}
		__finally {
#ifndef _DEBUG // при отладке не удаляем
			DeleteFile(sDotFile);
			DeleteFile(sSvgFile);
#endif

			SettingsData->ProfileOperation(__FUNCTION__, L"");
		}
	}

	void ConvertDotToPng(const UnicodeString &sFileDot, const UnicodeString &sFilePng) {

		const UnicodeString sGraphVizPath = GetGraphvizPath();

		const UnicodeString sCMD = TPath::Combine(sGraphVizPath, L"dot.exe") + L" -Tpng \"" + sFileDot + L"\" -o \"" + sFilePng + L"\"";

		TSpawnAndRedirect ASpawner(sCMD, sGraphVizPath);
		ASpawner.ConvertOutputEncoding = coeOEM866;
		ASpawner.StartProcessInfinite();
	}
}

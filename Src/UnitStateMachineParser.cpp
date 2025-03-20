/** *********************************************************************************
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
 ************************************************************************************** */

#include <vcl.h>
#pragma hdrstop

#include <memory>
#include <vector>

#include <boost/regex.hpp>
#include <boost/tuple/tuple.hpp>

#include <XMLDoc.hpp>
#include <IOUtils.hpp>
#include <EncdDecd.hpp>
#include <AnsiStrings.hpp>
#include <Character.hpp>
#include <LMDXML.hpp>

#include "UnitStateMachineParser.h"
#include "UnitStateXInclude.h"
#include "Log4cpp_VCL.hpp"
#include "UnitStateMachineConnection.h"
#include "ScxmlShapes.hpp"
#include "UnitSettings.h"
#include "UnitTreeEx.h"
#include "UnitTreeEditorUtils.h"
#include "UnitTreeEditorStateMachine.h"
#include "UnitUniqueUtils.h"
#include "UnitStateMachineUnit.h"
#include "UnitTreeLoader.h"
#include "UnitGraphViz.h"

// ---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma link "soaprtl.lib"

#define ATTR_INHERITED_MODULE	L"inheritedModule"

#define LITERAL_QT_EDITORINFO	L"qt:editorinfo"

namespace Statemachine {

	typedef enum {
		sifDefault, sifOldGui, sifQt
	}TScxmlImportFormat;

	typedef std::vector<TPoint>TVecPoint;

	typedef boost::tuple<TTreeNodeShape* /* 0 - source */ , //
	UnicodeString /* 1 - event */ , //
	UnicodeString /* 2 - target */ , //
	UnicodeString /* 3 - cond */ , //
	UnicodeString /* 4 - body */ , //
	TVecPoint /* 5 */ , //
	TScxmlTransitionType /* 6 */ , //
	UnicodeString /* 7 - extra params */ , //
	TScxmlImportFormat /* 8 - scxml import type */ >TTransitionTuple;

	typedef std::vector<TTransitionTuple>TTransitionVec;
	typedef enum {
		tttSource, tttEvent, tttTarget, tttCond, tttBody, tttPoints, tttTransitionType, tttExtraParams, tttImportFormat
	}TTransitionTupleType;

	static const UnicodeString __VISUAL_STATE_NAMES[] = {
		L"state", L"parallel", L"final"
	};
	static const std::set<UnicodeString>VISUAL_STATE_NAMES(__VISUAL_STATE_NAMES,
		__VISUAL_STATE_NAMES + ARRAYSIZE(__VISUAL_STATE_NAMES));

	typedef boost::tuple<UnicodeString, TVecPoint, TScxmlImportFormat>TTransitionBodyTuple;

	UnicodeString LastError = L"";

	static std::map<TVisualScxmlBaseShape*, TPoint>g_MapQtParentPoints;

	bool MatchesNodesRect(const UnicodeString &sText, TRect &ARect) {
		boost::wregex e(
			L"^\\s*node-size-and-position x=([-+]?[0-9]+)[\\.,]*[0-9]* y=([-+]?[0-9]+)[\\.,]*[0-9]* w=([-+]?[0-9]+)[\\.,]*[0-9]* h=([-+]?[0-9]+)[\\.,]*[0-9]*\\s*$");
		boost::wsmatch what;

		std::wstring wstr = sText.c_str();
		if (boost::regex_match(wstr, what, e)) {
			if (what.size() == 5) {
				ARect.Left = StrToInt(std::wstring(what[1]).c_str());
				ARect.Top = StrToInt(std::wstring(what[2]).c_str());
				ARect.Right = ARect.Left + StrToInt(std::wstring(what[3]).c_str());
				ARect.Bottom = ARect.Top + StrToInt(std::wstring(what[4]).c_str());
				return true;
			}
		}
		return false;
	}

	bool MatchesEdgePath(const UnicodeString &sText, TVecPoint &AVecPoint) {
		boost::wregex e(L"^\\s*edge-path \\[([^\\[\\]]*)\\]\\s{2}(.*)\\s*$");
		boost::wsmatch what;

		std::wstring wstr = sText.c_str();
		if (boost::regex_match(wstr, what, e)) {
			if (what.size() == 3) {

				UnicodeString sName = std::wstring(what[1]).c_str();

				std::wstring xStr = what[2];
				boost::wregex xRegEx(L"x=(-*\\w*) y=(-*\\w*)");

				std::wstring::const_iterator start, end;
				start = xStr.begin();
				end = xStr.end();
				boost::match_results<std::wstring::const_iterator>what;
				boost::match_flag_type flags = boost::match_default;
				while (regex_search(start, end, what, xRegEx, flags)) {

					if (what.size() == 3) {
						AVecPoint.push_back(TPoint(StrToInt(std::wstring(what[1]).c_str()),
								StrToInt(std::wstring(what[2]).c_str())));
					}
					// update search position:
					start = what[0].second;
				}

				return true;
			}
		}
		return false;
	}

	// ---------------------------------------------------------------------------
	UnicodeString LMDNodeTypeToText(ShortInt iNodeType) {
		switch(iNodeType) {
		case 0x0:
			return "LMD_NODE_INVALID";
		case 0x1:
			return "LMD_NODE_ELEMENT";
		case 0x2:
			return "LMD_NODE_ATTRIBUTE";
		case 0x3:
			return "LMD_NODE_TEXT";
		case 0x4:
			return "LMD_NODE_CDATA_SECTION";
		case 0x5:
			return "LMD_NODE_ENTITY_REFERENCE";
		case 0x6:
			return "LMD_NODE_ENTITY";
		case 0x7:
			return "LMD_NODE_PROCESSING_INSTRUCTION";
		case 0x8:
			return "LMD_NODE_COMMENT";
		case 0x9:
			return "LMD_NODE_DOCUMENT";
		case 0xa:
			return "LMD_NODE_DOCUMENT_TYPE";
		case 0xb:
			return "LMD_NODE_DOCUMENT_FRAGMENT";
		case 0xc:
			return "LMD_NODE_NOTATION";
		default:
			return "";
		}

	}

	TTransitionBodyTuple XMLNodeChildrenToString(ILMDXmlNode * AParentNode) {

		TVecPoint AVecPoint;
		TScxmlImportFormat AScxmlImportFormat = sifDefault;

		_di_ILMDXmlDocument ADoc = LMDCreateXmlDocument();
		for (int k = 0; k < AParentNode->ChildNodes->Count; k++) {
			if (AParentNode->ChildNodes->Item[k]->NodeName == LITERAL_QT_EDITORINFO) {

				int iMoveX = 0;
				int iMoveY = 0;

				/* MovePoint */ {
					std::auto_ptr<TStringList>AStringListPtr(new TStringList());
					AStringListPtr->StrictDelimiter = true;
					AStringListPtr->Delimiter = L';';
					AStringListPtr->DelimitedText = AParentNode->ChildNodes->Item[k]->GetAttr("movePoint", "");
					if (AStringListPtr->Count == 2) {
						iMoveX = RoundTo(StrToFloatDef(AStringListPtr->Strings[0], 0.0), 0);
						iMoveY = RoundTo(StrToFloatDef(AStringListPtr->Strings[1], 0.0), 0);
					}
				}

				/* Source */ {
					std::auto_ptr<TStringList>AStringListPtr(new TStringList());
					AStringListPtr->StrictDelimiter = true;
					AStringListPtr->Delimiter = L';';
					AStringListPtr->DelimitedText = AParentNode->ChildNodes->Item[k]->GetAttr("startTargetFactors",
						"-1;-1");
					for (int n = 0; n < AStringListPtr->Count; n += 2) {
						const int iX = RoundTo(StrToFloatDef(AStringListPtr->Strings[n], 0.0), 0);
						const int iY = RoundTo(StrToFloatDef(AStringListPtr->Strings[n + 1], 0.0), 0);
						AVecPoint.push_back(Point(iX, iY));
					}
				}

				/* Middle */ {
					std::auto_ptr<TStringList>AStringListPtr(new TStringList());
					AStringListPtr->StrictDelimiter = true;
					AStringListPtr->Delimiter = L';';
					AStringListPtr->DelimitedText = AParentNode->ChildNodes->Item[k]->GetAttr("localGeometry", "");

					if (AStringListPtr->Count % 2 == 0) {
						for (int n = 0; n < AStringListPtr->Count; n += 2) {
							const int iX = RoundTo(StrToFloatDef(AStringListPtr->Strings[n], 0.0), 0);
							const int iY = RoundTo(StrToFloatDef(AStringListPtr->Strings[n + 1], 0.0), 0);
							AVecPoint.push_back(Point(iX, iY));
						}
					}

				}

				/* Target */ {
					std::auto_ptr<TStringList>AStringListPtr(new TStringList());
					AStringListPtr->StrictDelimiter = true;
					AStringListPtr->Delimiter = L';';
					AStringListPtr->DelimitedText = AParentNode->ChildNodes->Item[k]->GetAttr("endTargetFactors",
						"-1;-1");
					for (int n = 0; n < AStringListPtr->Count; n += 2) {
						const int iX = RoundTo(StrToFloatDef(AStringListPtr->Strings[n], 0.0), 0);
						const int iY = RoundTo(StrToFloatDef(AStringListPtr->Strings[n + 1], 0.0), 0);
						AVecPoint.push_back(Point(iX, iY));
					}
				}

				AScxmlImportFormat = sifQt;

				if (SettingsData->ParserDoNotLoadQtEditorInfo)
					continue;
			}

			ADoc->AppendChild(AParentNode->ChildNodes->Item[k]);
		}
		return boost::make_tuple(ADoc->XML, AVecPoint, AScxmlImportFormat);
	}

	void DebugInfo(const UnicodeString &sName, int iType, const UnicodeString &sXML) {
		UnicodeString sText = sXML.Trim();
		if (sText.Length() > 40) {
			sText.SetLength(40).Trim();
		}
		std::auto_ptr<TStringList>ADebugList(new TStringList());
		ADebugList->Text = sText;
		ADebugList->StrictDelimiter = true;
		ADebugList->Delimiter = L'|';

		WLOG_DEBUG(L"Name=[%s] Type [%s] [%s]", sName.c_str(), LMDNodeTypeToText(iType).c_str(),
			ADebugList->DelimitedText.Trim().c_str());
	}

	void IterateStateNodes(TCustomTree * ATree, ILMDXmlNode * AParentNode, int iIndexXML, TScxmlBaseShape * AShape,
		TTransitionVec & AVecTransition, bool bInsideTransition, TScxmlImportFormat &AScxmlImportFormat) {
#ifdef _DEBUG
		DebugInfo(AParentNode->NodeName, AParentNode->NodeType, AParentNode->XML);
#endif
		if (AParentNode->NodeType == LMD_NODE_COMMENT) {
			if (AShape != 0) {
				TRect AShapeRect;
				TVecPoint AVecPoint;

				// комментарий, который содержит мета-информацию
				bool bMetaInfoComment = false;
				if (MatchesNodesRect(AParentNode->Text, AShapeRect)) {
					// попался случай, когда были дальше записаны какие-то мусорные значения,
					// поэтому реагируем только на первый комментарий!
					if (iIndexXML == 0) {
						// применимо только к визуальным фигурам
						TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
						if (AVisualShape) {
							if (AShape->Parent) {
								AShapeRect.right = AShapeRect.right + AShape->Parent->X0;
								AShapeRect.left = AShapeRect.left + AShape->Parent->X0;
								AShapeRect.bottom = AShapeRect.bottom + AShape->Parent->Y0;
								AShapeRect.top = AShapeRect.top + AShape->Parent->Y0;
#if 0
								WLOG_INFO(L"Old Gui> Shape:[%s] left=%d,top=%d,width=%d,height=%d", //
									AShape->SimpleText.c_str(), //
									AShapeRect.left, AShapeRect.top, AShapeRect.Width(), AShapeRect.Height());
#endif
							}

							AShape->X0 = AShapeRect.left;
							AShape->Y0 = AShapeRect.top;
							AShape->X1 = AShapeRect.right;
							AShape->Y1 = AShapeRect.bottom;

							switch(AShape->StateChildType) {
							case sctFinal:
							case sctHistory:
							case sctInitial: {
									AVisualShape->SetDefaultSize();
								}break;
							default:
								break;
							}
						}

						bMetaInfoComment = true;
						AScxmlImportFormat = sifOldGui;
					}
				}
				else if (MatchesEdgePath(AParentNode->Text, AVecPoint)) {
					AVecTransition.back().get<tttPoints>() = AVecPoint;
					AVecTransition.back().get<tttImportFormat>() = sifOldGui;

					bMetaInfoComment = true;
					AScxmlImportFormat = sifOldGui;
				}

				if ((!bMetaInfoComment || SettingsData->SaveCommentsAnywayOnLoad) && !bInsideTransition) {
					TScxmlBaseShape *AParentShape = AShape;
					AShape = AParentShape->AddStateChild(sctComment, false);
					if (!AShape)
						throw Exception("Type <" + AParentNode->NodeName + "> is not allowed to be child of <" +
						AParentShape->XMLName + ">");

					AShape->Comments->Text = AParentNode->Text;
				}
			}
		}
		else if (AParentNode->NodeType == LMD_NODE_TEXT) {
			if (AShape && IsPublishedProp(AShape, "XMLText")) {
				AShape->XMLText->Text = AParentNode->Text;
				AShape->XMLText->Type = TXMLTextType::xttText;
			}
		}
		else if (AParentNode->NodeType == LMD_NODE_CDATA_SECTION) {
			if (AShape && IsPublishedProp(AShape, "XMLText")) {
				AShape->XMLText->Text = AParentNode->Text;
				AShape->XMLText->Type = TXMLTextType::xttCDATA;
			}
		}
		else if (AParentNode->NodeType == LMD_NODE_ELEMENT) {

			if (AShape && IsPublishedProp(AShape, "XMLText")) {

				// !!! непонятный случай, скорей всего такого не может быть, для удаления в будущем
				if (SettingsData->ParserDoNotLoadQtEditorInfo && AParentNode->NodeName == LITERAL_QT_EDITORINFO) {
					return;
				}

				AShape->XMLText->Text = AShape->XMLText->Text + AParentNode->XML;
				AShape->XMLText->Type = AParentNode->NodeName == L"scxml" ?
					TXMLTextType::xttSCXML : TXMLTextType::xttXML;
				AShape->XMLText->SyntaxScheme = L"XML";
				return; // значит добавляем данные в узлы типа <content> или <script>
			}

			bool bAdded = false;
			TScxmlBaseShape *AParentShape = AShape;
			bool bIsTransition = false;
			bool bExtraContent = false;
			// у этого класса не будет родителя, поэтому создаем вручную
			if (AParentNode->NodeName == "scxml") {
				TScxmlShape *AScxmlShape = new TScxmlShape(ATree->Owner);
				AScxmlShape->AutoPosition->Left = false;
				AScxmlShape->AutoPosition->Top = false;
				AScxmlShape->SetDefaultSize();
				AShape = AScxmlShape;
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(ATree->Owner, __classid(TScxmlShape));
				AShape->Name = AUniqueName.second;
				AShape->SimpleText = AUniqueName.first;
				AShape->Tree = ATree;
				bAdded = true;
				AScxmlShape->X0 = 8;
				AScxmlShape->Y0 = 8;

				/* bugfix: 'https://github.com/alexzhornyak/ScxmlEditor-Tutorial/issues/43' */
				if (AParentNode->OwnerDocument) {

					/* проверяем, что это scxml верхнего уровня */
					if (AParentNode->ParentNode && AParentNode->OwnerDocument && //
						(AParentNode->ParentNode->AsElement() == AParentNode->OwnerDocument->AsElement())) {

						/* по умолчанию создастся ScxmlShape1, но если у нас есть название юнита,
						то логичнее дать название на основе его
						 */
						if (AScxmlShape->StateMachineEditorUnit) {
							UnicodeString sUnitFileNameOnly = TPath::GetFileNameWithoutExtension
								(AScxmlShape->StateMachineEditorUnit->FileName);

							// проверим, что это внешний файл, который не был создан по умолчанию
							if (!sUnitFileNameOnly.IsEmpty() && //
								sUnitFileNameOnly.Pos(TStateMachineEditorUnit::DefaultUnitBaseName) != 1 && //
								sUnitFileNameOnly.Pos(TStateMachineEditorUnit::DefaultVirtualUnitBaseName) != 1) {

								// make CamelCase
								sUnitFileNameOnly[1] = ToUpper(sUnitFileNameOnly[1]);

								const bool bHasScxml = sUnitFileNameOnly.LowerCase().Pos(L"scxml");

								AShape->SimpleText = (bHasScxml ? L"" : L"Scxml") + sUnitFileNameOnly;
							}

						}

						for (int i = 0; i < AParentNode->OwnerDocument->ChildNodes->Count; i++) {
							/* перебираем все комменты сверху DocumentElement */
							if (AParentNode->OwnerDocument->ChildNodes->Item[i]->NodeType == LMD_NODE_ELEMENT) {
								if (AParentNode->OwnerDocument->ChildNodes->Item[i]->AsElement()
									== AParentNode->AsElement()) {
									break;
								}
							}
							else if (AParentNode->OwnerDocument->ChildNodes->Item[i]->NodeType == LMD_NODE_COMMENT) {
								IterateStateNodes(ATree, AParentNode->OwnerDocument->ChildNodes->Item[i], i,
									AScxmlShape, AVecTransition, false, AScxmlImportFormat);
							}
						}
					}
				}

			}
			else if (AParentNode->NodeName == "transition") {
				UnicodeString sEvent = L"";
				UnicodeString sTarget = L"";
				UnicodeString sCondition = L"";
				UnicodeString sType = L"";

				std::auto_ptr<TStringList>AExtraParamsPtr(new TStringList());
				for (int i = 0; i < AParentNode->AttrCount; i++) {
					if (AParentNode->AttrNames[i] == L"event") {
						sEvent = AParentNode->AttrValues[i];
					}
					else if (AParentNode->AttrNames[i] == L"target") {
						sTarget = AParentNode->AttrValues[i];
					}
					else if (AParentNode->AttrNames[i] == L"cond") {
						sCondition = AParentNode->AttrValues[i];
					}
					else if (AParentNode->AttrNames[i] == L"type") {
						sType = AParentNode->AttrValues[i];
					}
					else {
						AExtraParamsPtr->Values[AParentNode->AttrNames[i]] = AParentNode->AttrValues[i];
					}
				}

				const TTransitionBodyTuple ABodyTuple = XMLNodeChildrenToString(AParentNode);
				if (boost::get<2>(ABodyTuple) != sifDefault) {
					AScxmlImportFormat = boost::get<2>(ABodyTuple);
				}

				AVecTransition.push_back(boost::make_tuple(AShape, sEvent, sTarget, sCondition, //
						boost::get<0>(ABodyTuple), boost::get<1>(ABodyTuple),
						sType == L"internal" ? TScxmlTransitionType::internal : TScxmlTransitionType::external,
						AExtraParamsPtr->Text, AScxmlImportFormat));

				bIsTransition = true;
			}
			else {
				if (AParentShape) {

					const TStateChildType AStateType = TScxmlBaseShape::XMLNodeNameToType(AParentNode->NodeName);
					if (AStateType != sctMAXSIZE) {
						AShape = AParentShape->AddStateChild(AStateType, false);
						if (!AShape)
							throw Exception
								("Type <" + AParentNode->NodeName + "> is not allowed to be child of <" +
							AParentShape->XMLName + ">");
						bAdded = true;
					}
					else {
						LOG_WARNING("%s> Unknown state type <%s>", __FUNCTION__,
							AnsiString(AParentNode->NodeName).c_str());
					}

				}
				else {
					LOG_WARNING("%s> Can not convert <%s> to <%s>", __FUNCTION__,
						AnsiString(AParentNode->NodeName).c_str(),
						AnsiString(__classid(TScxmlBaseShape)->ClassName()).c_str());
				}
			}

			// если была добавлена новая нода
			if (bAdded) {

				AShape->AttributesToProps(AParentNode->AsElement());

				if (AShape->ClassType() == __classid(TInitialShape)) {
					// у этой ноды не будет положения в scxml редакторе
					TInitialShape *AInitialShape = dynamic_cast<TInitialShape*>(AShape);
					if (AInitialShape && AInitialShape->Parent) {
						AInitialShape->X0 = AInitialShape->Parent->X0;
						AInitialShape->Y0 = AInitialShape->Parent->Y0;
					}
				}
			}
			else {
				if (!bIsTransition) {

					TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>(AShape);
					if (AVisualShape) {

						if (AParentNode->NodeName == LITERAL_QT_EDITORINFO) {

							std::auto_ptr<TStringList>AStringListPtr(new TStringList());
							AStringListPtr->StrictDelimiter = true;
							AStringListPtr->Delimiter = L';';
							AStringListPtr->DelimitedText = AParentNode->GetAttr("geometry", "");

							if (AStringListPtr->Count % 2 == 0) {
								std::vector<TPoint>AVecPoints;
								int iParentX = 0;
								int iParentY = 0;

								if (AShape->Parent && !AShape->Parent->InheritsFrom(__classid(TScxmlShape))) {
									iParentX = AShape->Parent->XCenter();
									iParentY = AShape->Parent->YCenter();

									TVisualScxmlBaseShape*AVisualParent = dynamic_cast<TVisualScxmlBaseShape*>
										(AShape->Parent);
									if (AVisualParent) {
										std::map<TVisualScxmlBaseShape*,
										TPoint>::iterator it = g_MapQtParentPoints.find(AVisualParent);
										if (it != g_MapQtParentPoints.end()) {
										iParentX -= it->second.x;
										iParentY -= it->second.y;
										}
									}
								}

								for (int n = 0; n < AStringListPtr->Count; n += 2) {
									const int iX = RoundTo(StrToFloatDef(AStringListPtr->Strings[n], 0.0), 0);
									const int iY = RoundTo(StrToFloatDef(AStringListPtr->Strings[n + 1], 0.0), 0);
									AVecPoints.push_back(Point(iX, iY));
								}

								if (AVecPoints.size() == 3) {
									const int iRectCenterX = AVecPoints[1].x + AVecPoints[2].x / 2;
									const int iRectCenterY = AVecPoints[1].y + AVecPoints[2].y / 2;
#if 0
									WLOG_INFO(L"Shape:[%s] rect center x=%d, y=%d", AShape->SimpleText.c_str(),
										iRectCenterX, iRectCenterY);
#endif

									g_MapQtParentPoints.insert
										(std::make_pair(dynamic_cast<TVisualScxmlBaseShape*>(AShape),
										Point(iRectCenterX, iRectCenterY)));

									AShape->X0 = iParentX + AVecPoints[0].x + AVecPoints[1].x;
									AShape->Y0 = iParentY + AVecPoints[0].y + AVecPoints[1].y;
									AShape->X1 = iParentX + AVecPoints[0].x + AVecPoints[1].x + AVecPoints[2].x;
									AShape->Y1 = iParentY + AVecPoints[0].y + AVecPoints[1].y + AVecPoints[2].y;

									switch(AShape->StateChildType) {
									case sctFinal:
									case sctHistory:
									case sctInitial: {
										AVisualShape->SetDefaultSize();
										}break;
									default:
										break;
									}
								}
							}

							AScxmlImportFormat = sifQt;

							if (SettingsData->ParserDoNotLoadQtEditorInfo)
								return;
						}
					}

					WLOG_WARNING(L"Unknown node type <%s> with parent <%s> <%s>", AParentNode->NodeName.c_str(),
						AShape->Name.c_str(), AShape->SimpleText.c_str());

					TScxmlBaseShape *AParentShape = AShape;
					AShape = AParentShape->AddStateChild(sctExtraContent, false);
					if (!AShape)
						throw Exception("Type <" + AParentNode->NodeName + "> is not allowed to be child of <" +
						AParentShape->XMLName + ">");

					AShape->ExtraContent->Text = AParentNode->XML;
					bExtraContent = true;
				}
			}

			for (int i = 0; i < AParentNode->ChildNodes->Count; i++) {
				if (bExtraContent)
					continue;
				// проанализируем только комменты для поддержки гуев, если это ноды внутри соединения
				if (!bIsTransition || (AParentNode->ChildNodes->Item[i]->NodeType == LMD_NODE_COMMENT && bIsTransition)
					) {
					IterateStateNodes(ATree, AParentNode->ChildNodes->Item[i], i, AShape, AVecTransition,
						bIsTransition, AScxmlImportFormat);
				}
			}
		}
	}

	void CreatePoints_Qt(TTreeConnection *AConnection, TTreeNodeShape *AShape, TTreeNodeShape *ASourceNode,
		TTreeNodeShape *ATargetNode, const TVecPoint &AVecPoint) {

		if (AVecPoint.size() < 2) {
			WLOG_WARNING(L"CreatePoints_Qt> Requires at least 2 points but got:[%d]", (int)AVecPoint.size());
			return;
		}

		const int iSourceCenterX = ASourceNode->XCenter();
		const int iSourceCenterY = ASourceNode->YCenter();

		const int iTargetCenterX = ATargetNode->XCenter();
		const int iTargetCenterY = ATargetNode->YCenter();

		AConnection->Points->Clear();

		int iStartX = iSourceCenterX;
		int iStartY = iSourceCenterY;

		for (std::size_t i = 0; i < AVecPoint.size(); i++) {
			if (i == 0) {
				const int iX = AVecPoint[i].x == -1 ? 50 : AVecPoint[i].x;
				const int iY = AVecPoint[i].y == -1 ? 50 : AVecPoint[i].y;

				AConnection->Points->Add(cpsFromPercent, iX, cpsFromPercent, iY);

				AConnection->Points->CalculatePosition(i);

				iStartX = AConnection->Points->Item[0].X;
				iStartY = AConnection->Points->Item[0].Y;
			}
			else if (i == AVecPoint.size() - 1) {
				const int iX = AVecPoint[i].x == -1 ? 50 : AVecPoint[i].x;
				const int iY = AVecPoint[i].y == -1 ? 50 : AVecPoint[i].y;

				AConnection->Points->Add(cpsToPercent, iX, cpsToPercent, iY);
			}
			else {
				AConnection->Points->Add(cpsFixed, iStartX + AVecPoint[i].x, cpsFixed, iStartY + AVecPoint[i].y);
			}
		}

		Editorutils::CalculatePointsPosition(AConnection);
		Editorutils::ClipFirstConnectionPointToShapeBounds(AConnection);
		Editorutils::ClipLastConnectionPointToShapeBounds(AConnection);
	}

	void CreatePoints_OldGui(TTreeConnection *AConnection, TTreeNodeShape *AShape, TTreeNodeShape *ASourceNode,
		TTreeNodeShape *ATargetNode, const TVecPoint &AVecPoint) {

		const int iSourceCenterX = ASourceNode->XCenter();
		const int iSourceCenterY = ASourceNode->YCenter();

		const int iTargetCenterX = ATargetNode->XCenter();
		const int iTargetCenterY = ATargetNode->YCenter();

		for (std::size_t i = 0; i < AVecPoint.size(); i++) {
			const int iX = AShape->X0 + AVecPoint[i].x;
			const int iY = AShape->Y0 + AVecPoint[i].y;

			if (i == 0) {
				double X0clip, X1clip, Y0clip, Y1clip;
				if (Editorutils::LiangBarsky(ASourceNode->X0, ASourceNode->X1, -ASourceNode->Y1, -ASourceNode->Y0, iX,
						-iY, iSourceCenterX, -iSourceCenterY, X0clip, Y0clip, X1clip, Y1clip)) {

					const int iIndex = 0;
					if (iIndex < AConnection->Points->Count()) {
						X0clip = (X0clip - ASourceNode->X0) / (ASourceNode->X1 - ASourceNode->X0) * 100.0f;
						Y0clip = (-Y0clip - ASourceNode->Y0) / (ASourceNode->Y1 - ASourceNode->Y0) * 100.0f;

						AConnection->Points->ChangeXStyle(iIndex, cpsFromPercent);
						AConnection->Points->ChangeYStyle(iIndex, cpsFromPercent);

						AConnection->Points->Item[iIndex].XValue = abs(X0clip);
						AConnection->Points->Item[iIndex].YValue = abs(Y0clip);
					}
				}
			}
			if (i == AVecPoint.size() - 1) {
				double X0clip, X1clip, Y0clip, Y1clip;
				if (Editorutils::LiangBarsky(ATargetNode->X0, ATargetNode->X1, -ATargetNode->Y1, -ATargetNode->Y0, iX,
						-iY, iTargetCenterX, -iTargetCenterY, X0clip, Y0clip, X1clip, Y1clip)) {

					const int iIndex = AConnection->Points->Count() - 1;
					if (iIndex >= 0) {
						X0clip = (X0clip - ATargetNode->X0) / (ATargetNode->X1 - ATargetNode->X0) * 100.0f;
						Y0clip = (-Y0clip - ATargetNode->Y0) / (ATargetNode->Y1 - ATargetNode->Y0) * 100.0f;

						AConnection->Points->ChangeXStyle(iIndex, cpsToPercent);
						AConnection->Points->ChangeYStyle(iIndex, cpsToPercent);

						AConnection->Points->Item[iIndex].XValue = abs(X0clip);
						AConnection->Points->Item[iIndex].YValue = abs(Y0clip);
					}

				}
			}

			AConnection->Points->Insert(i + 1, iX, iY);
		}
	}

	void CreateTransition(const TTransitionTuple &ATransition, TCustomTree *ATree, const UnicodeString &sTarget) {
		TTreeNodeShape *ASourceNode = ATransition.get<tttSource>();
		const UnicodeString &sEvent = ATransition.get<tttEvent>();
		const UnicodeString &sCond = ATransition.get<tttCond>();
		const UnicodeString &sXML = ATransition.get<tttBody>();
		const TVecPoint &AVecPoint = ATransition.get<tttPoints>();
		const TScxmlTransitionType AType = ATransition.get<tttTransitionType>();
		const UnicodeString &sExtraParams = ATransition.get<tttExtraParams>();
		const TScxmlImportFormat AScxmlImportFormat = ATransition.get<tttImportFormat>();

		/* DEBUG (UnicodeString().sprintf(L"Transition with params[%s;%s;%s;%s]", sSource.c_str(), sEvent.c_str(),
		sTarget.c_str(), sCond.c_str())); */

		const UnicodeString sSource = ASourceNode != NULL ? ASourceNode->SimpleText.c_str() : L"";
		TTreeNodeShape *ATargetNode = sTarget.IsEmpty() ? ASourceNode : ATree->Items->Find(sTarget, false);
		if (!ASourceNode || !ATargetNode)
			throw Exception
				(UnicodeString().sprintf(L"Can not create transition from:[%s] to:[%s] event:[%s] cond:[%s]", //
				sSource.c_str(), sTarget.c_str(), sEvent.c_str(), sCond.c_str()));

		// приходится создавать отдельно из-за невозможности штатными средствами создавать множественные соединения
		TStateMachineConnection* AConnection = new TStateMachineConnection(ATree->Owner);
		const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(ATree->Owner, __classid(TStateMachineConnection));
		AConnection->Name = AUniqueName.second;
		AConnection->FromShape = ASourceNode;
		AConnection->ToShape = ATargetNode;
		AConnection->Tree = ATree;

		/* SCXML свойства */
		AConnection->IsTargetStored = !sTarget.IsEmpty();
		AConnection->Type = AType;
		AConnection->ExtraParams = sExtraParams;
		AConnection->XML = sXML;
		AConnection->Event = sEvent;
		AConnection->Condition = sCond;

		/* визуальные настройки */
		AConnection->Style = csLine;
		AConnection->Text->Angle = 90;
		AConnection->ArrowTo->Size = 10;

		// если были указаны точки в конфиге
		if (AVecPoint.size()) {

			// если соединение замыкается на себя
			if (ASourceNode == ATargetNode) {

				switch(AScxmlImportFormat) {
				case sifQt:
					AConnection->MakeSelfConnection();
					break;
				case sifOldGui:
				case sifDefault: {
						AConnection->Border->Style = psDot;
						AConnection->Border->SmallDots = false;

						const int iHalfWidth = ASourceNode->Width / 2;
						const int iHalfHeight = ASourceNode->Height / 2;

						// внизу
						if (AVecPoint[0].x >= 0 && (ASourceNode->Y0 + AVecPoint[0].y) >= ASourceNode->Y1) {
							AConnection->Points->Clear();

							AConnection->Points->Add(TPoint(ASourceNode->X0 + iHalfWidth, ASourceNode->Y1));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + AVecPoint[0].x,
									ASourceNode->Y0 + AVecPoint[0].y));
							AConnection->Points->Add(TPoint(ASourceNode->X1 - AVecPoint[0].x,
									ASourceNode->Y0 + AVecPoint[0].y));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + iHalfWidth, ASourceNode->Y1));
						}
						// вверху
						else if (AVecPoint[0].x >= 0 && AVecPoint[0].y < 0) {

							AConnection->Points->Clear();

							AConnection->Points->Add(TPoint(ASourceNode->X0 + iHalfWidth, ASourceNode->Y0));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + AVecPoint[0].x,
									ASourceNode->Y0 + AVecPoint[0].y));
							AConnection->Points->Add(TPoint(ASourceNode->X1 - AVecPoint[0].x,
									ASourceNode->Y0 + AVecPoint[0].y));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + iHalfWidth, ASourceNode->Y0));
						}
						// слева
						else if (AVecPoint[0].x < 0 && AVecPoint[0].y >= 0) {
							AConnection->Points->Clear();

							AConnection->Points->Add(TPoint(ASourceNode->X0, ASourceNode->Y0 + iHalfHeight));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + AVecPoint[0].x,
									ASourceNode->Y0 + AVecPoint[0].y));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + AVecPoint[0].x,
									ASourceNode->Y1 - AVecPoint[0].y));
							AConnection->Points->Add(TPoint(ASourceNode->X0, ASourceNode->Y0 + iHalfHeight));
						}
						// справа
						else {

							AConnection->Points->Clear();

							AConnection->Points->Add(TPoint(ASourceNode->X1, ASourceNode->Y0 + iHalfHeight));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + AVecPoint[0].x,
									ASourceNode->Y0 + AVecPoint[0].y / 2));
							AConnection->Points->Add(TPoint(ASourceNode->X0 + AVecPoint[0].x,
									ASourceNode->Y1 - AVecPoint[0].y / 2));
							AConnection->Points->Add(TPoint(ASourceNode->X1, ASourceNode->Y0 + iHalfHeight));
						}

						// меняем тип привязки, чтобы точка перемещалась вместе
						for (int i = 0; i < AConnection->Points->Count(); i++) {
							AConnection->Points->ChangeXStyle(i, cpsFromRel);
							AConnection->Points->ChangeYStyle(i, cpsFromRel);
						}
					}break;
				}
			}
			else {
				TTreeNodeShape *AShape = ATargetNode;
				if (ATargetNode->Parent) {
					AShape = ATargetNode->Parent;
				}

				if (AConnection->Points->Count() != 2)
					throw Exception("Connection must create at least 2 points!");

				switch(AScxmlImportFormat) {
				case sifQt:
					CreatePoints_Qt(AConnection, AShape, ASourceNode, ATargetNode, AVecPoint);
					break;
				case sifOldGui:
				case sifDefault:
					CreatePoints_OldGui(AConnection, AShape, ASourceNode, ATargetNode, AVecPoint);
					break;
				}

				const int iPtCount = AConnection->Points->Count();
				const int iMiddle = iPtCount / 2;
				if (iPtCount > 2) {
					const int iMiddle = iPtCount / 2;
					for (int i = iPtCount - 2; i >= iMiddle; i--) {
						AConnection->Points->ChangeXStyle(i, cpsNext);
						AConnection->Points->ChangeYStyle(i, cpsNext);

						AConnection->Points->CalculatePosition(i + 1);

						const double dX = ((double)AConnection->Points->Item[i].X - AConnection->Points->Item[i + 1].X);
						const double dY = ((double)AConnection->Points->Item[i].Y - AConnection->Points->Item[i + 1].Y);

						AConnection->Points->Item[i].XValue = dX;
						AConnection->Points->Item[i].YValue = dY;
					}

					for (int i = 1; i < iMiddle; i++) {
						AConnection->Points->ChangeXStyle(i, cpsPrevious);
						AConnection->Points->ChangeYStyle(i, cpsPrevious);

						AConnection->Points->CalculatePosition(i - 1);

						const double dX = ((double)AConnection->Points->Item[i].X - AConnection->Points->Item[i - 1].X);
						const double dY = ((double)AConnection->Points->Item[i].Y - AConnection->Points->Item[i - 1].Y);

						AConnection->Points->Item[i].XValue = dX;
						AConnection->Points->Item[i].YValue = dY;
					}
				}

			}
		}
		else {
			// если соединение замыкается на себя
			if (ASourceNode == ATargetNode) {
				AConnection->MakeSelfConnection();
			}
			else {
				switch(AScxmlImportFormat) {
				case sifOldGui: {
						if (AConnection->Points->Count() != 2)
							throw Exception("Connection must create at least 2 points!");

						const int iSourceCenterX = ASourceNode->XCenter();
						const int iSourceCenterY = ASourceNode->YCenter();

						const int iTargetCenterX = ATargetNode->XCenter();
						const int iTargetCenterY = ATargetNode->YCenter();

						// нулевая точка
						{
							const int iIndex = 0;

							double X0clip, X1clip, Y0clip, Y1clip;
							if (Editorutils::LiangBarsky(ASourceNode->X0, ASourceNode->X1, -ASourceNode->Y1,
									-ASourceNode->Y0, iTargetCenterX, -iTargetCenterY, iSourceCenterX,
									-iSourceCenterY, X0clip, Y0clip, X1clip, Y1clip)) {

								X0clip = (X0clip - ASourceNode->X0) / (ASourceNode->X1 - ASourceNode->X0) * 100.0f;
								Y0clip = (-Y0clip - ASourceNode->Y0) / (ASourceNode->Y1 - ASourceNode->Y0) * 100.0f;

								AConnection->Points->ChangeXStyle(0, cpsFromPercent);
								AConnection->Points->ChangeYStyle(0, cpsFromPercent);

								AConnection->Points->Item[iIndex].XValue = abs(X0clip);
								AConnection->Points->Item[iIndex].YValue = abs(Y0clip);
							}
						}

						// крайняя точка
						{
							const int iIndex = 1;

							double X0clip, X1clip, Y0clip, Y1clip;
							if (Editorutils::LiangBarsky(ATargetNode->X0, ATargetNode->X1, -ATargetNode->Y1,
									-ATargetNode->Y0, iSourceCenterX, -iSourceCenterY, iTargetCenterX,
									-iTargetCenterY, X0clip, Y0clip, X1clip, Y1clip)) {

								X0clip = (X0clip - ATargetNode->X0) / (ATargetNode->X1 - ATargetNode->X0) * 100.0f;
								Y0clip = (-Y0clip - ATargetNode->Y0) / (ATargetNode->Y1 - ATargetNode->Y0) * 100.0f;

								AConnection->Points->ChangeXStyle(1, cpsToPercent);
								AConnection->Points->ChangeYStyle(1, cpsToPercent);

								AConnection->Points->Item[iIndex].XValue = abs(X0clip);
								AConnection->Points->Item[iIndex].YValue = abs(Y0clip);
							}
						}
					}break;
				case sifQt: {
						// не используем от центра фигуры к центру, так как:
						// 1) фигуры могут перекрывать друг-друга
						// 2) могут теряться связи от родителя к дочерней
					}break;
				}
			}

		}

	}

	void ParseScxmlGuiFormat(TCustomTree *ATree, ILMDXmlNode * ADocumentElement) {
		if (!ADocumentElement)
			throw Exception("XML Document is empty!");

		ATree->Clear();

		// будет определяться по метаинформации
		TScxmlImportFormat AScxmlImportFormat = sifDefault;

		{
			// класс, который в конце закончит обновление
			const Editorutils::TreeUpdateLock ATreeLock(ATree);

			TTransitionVec AVecTransition;

			// чистим точки соответствия для Qt Import
			g_MapQtParentPoints.clear();

			_di_ILMDXmlElement ARootElement = ADocumentElement->AsElement();

			IterateStateNodes(ATree, ARootElement, NULL, 0, AVecTransition, false, AScxmlImportFormat);

			TScxmlShape *AScxmlShape = NULL;

			for (int i = 0; i < ATree->Shapes->Count; i++) {

				TVisualScxmlBaseShape *AShape = dynamic_cast<TVisualScxmlBaseShape*>(ATree->Shapes->Items[i]);
				if (AShape) {
					AShape->Expanded = true;

					switch(AShape->StateChildType) {
					case sctFinal: {
							if (AShape->HasChildren()) {
								Editorutils::SetShapeTextAlign(AShape, Editorutils::dtRight);
							}
						}break;
					}
				}
			}

			for (std::size_t i = 0; i < AVecTransition.size(); i++) {
				// оказалось, что по стандарту может быть несколько target'ов через пробел
				std::auto_ptr<TStringList>ATransitionsList(new TStringList());
				ATransitionsList->StrictDelimiter = true;
				ATransitionsList->Delimiter = L' ';
				ATransitionsList->DelimitedText = AVecTransition[i].get<tttTarget>();

				// может быть случай, когда target не указан, тогда мы просто передаем пустое значение и это будет означать "замыкание на себя"
				if (!ATransitionsList->Count) {
					CreateTransition(AVecTransition[i], ATree, L"");
				}
				else {
					for (int k = 0; k < ATransitionsList->Count; k++) {
						if (!ATransitionsList->Strings[k].IsEmpty()) {
							CreateTransition(AVecTransition[i], ATree, ATransitionsList->Strings[k]);
						}
					}
				}
			}
		}

		switch(AScxmlImportFormat) {
		case sifOldGui: {
				// не проверяем размер
			}break;
		case sifQt: {
				if (ATree->Shapes->Count) {
					const Editorutils::TreeUpdateLock ATreeLock(ATree);

					ATree->Draw(); // посчитать границы

					const int iMinX = ATree->TotalBounds.left;
					const int iMinY = ATree->TotalBounds.top;

					TScxmlShape * AScxmlShape = dynamic_cast<TScxmlShape*>(ATree->Shapes->Items[0]);
					if (AScxmlShape) {

						// проверка смещения меньше нуля
						for (int i = 0; i < AScxmlShape->Children->Count; i++) {
							TVisualScxmlBaseShape * AVisualShape = dynamic_cast<TVisualScxmlBaseShape*>
								(AScxmlShape->Children->Items[i]);
							if (AVisualShape) {
								AVisualShape->MoveRelative(iMinX < 0 ? (iMinX*-1 + AScxmlShape->X0 + 50) : 0,
									iMinY < 0 ? (iMinY*-1 + AScxmlShape->Y0 + 50) : 0, true);
							}
						}

						ATree->Draw(); // посчитать границы

						if (AScxmlShape->X1 < ATree->TotalBounds.right + 20) {
							AScxmlShape->X1 = ATree->TotalBounds.right + 20;
							ATree->Page->Width = AScxmlShape->X1 + 20; // bugfix: правая граница была меньше
						}

						if (AScxmlShape->Y1 < ATree->TotalBounds.bottom + 20) {
							AScxmlShape->Y1 = ATree->TotalBounds.bottom + 20;
							ATree->Page->Height = AScxmlShape->Y1 + 20;
						}
					}

				}

			}break;
		case sifDefault:
		default: {
				try {
					// пробуем отстроить фигуры с помощью Graphviz
					Graphviz::ArrangeTreeLayout(ATree);
				}
				catch(Exception * E) {

					// проверим границы старым методом
					if (SettingsData->CheckPositionOnLoadScxml) {
						const Editorutils::TreeUpdateLock ATreeLock(ATree);
						CheckVisualScxmlStateBounds(ATree);
						Editorutils::ProtectTreeConnectionsToBeInSamePoints(ATree);
					}

					throw E;
				}
			}break;
		}

		ATree->Draw();
	}

	/* INHERITANCE */
	void SetInheritance(TScxmlBaseShape *AScxmlBaseShape, bool bSet, bool bWithConnections /* =true */ ) {
		if (AScxmlBaseShape) {

			// необходимо сменить имя, иначе фигура даже при снятом Locked будет удаляться
			if (!bSet) {
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(AScxmlBaseShape->Owner,
					AScxmlBaseShape->ClassType());
				AScxmlBaseShape->Name = AUniqueName.second;
			}

			AScxmlBaseShape->Locked = bSet;
			AScxmlBaseShape->InheritanceMismatch = false;

			if (bWithConnections) {
				for (int i = 0; i < AScxmlBaseShape->Connections->Count; i++) {
					TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
						(AScxmlBaseShape->Connections->Items[i]);
					if (AStateMachineConnection) {

						// необходимо сменить имя, иначе фигура даже при снятом Locked будет удаляться
						if (!bSet) {
							const Unique::TUniquePair AUniqueName = Unique::GetUniqueName
								(AStateMachineConnection->Owner, AStateMachineConnection->ClassType());
							AStateMachineConnection->Name = AUniqueName.second;
						}

						AStateMachineConnection->Locked = bSet;
						AStateMachineConnection->InheritanceMismatch = false;
					}
				}
			}

			for (int i = 0; i < AScxmlBaseShape->Children->Count; i++) {
				SetInheritance(dynamic_cast<TScxmlBaseShape*>(AScxmlBaseShape->Children->Items[i]), bSet);
			}
		}
	}

	void SetInheritance(TStateMachineConnection *AConnection, bool bSet) /* overload */ {
		if (AConnection) {

			// необходимо сменить имя, иначе фигура даже при снятом Locked будет удаляться
			if (!bSet) {
				const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(AConnection->Owner,
					AConnection->ClassType());
				AConnection->Name = AUniqueName.second;
			}

			AConnection->Locked = bSet;
			AConnection->InheritanceMismatch = false;
		}
	}

	void CheckAndFixInheritedParent(TComponent *ABaseOwner, TScxmlBaseShape *AScxmlBaseShape,
		TScxmlBaseShape * AOriginShape) {
		if (AScxmlBaseShape && AOriginShape && AOriginShape->Parent && ABaseOwner) {
			if (!AOriginShape->Parent->Name.IsEmpty() && (!AScxmlBaseShape->Parent ||
					(AOriginShape->Parent->Name != AScxmlBaseShape->Parent->Name))) {

				const UnicodeString sShapeParentName = AScxmlBaseShape->Parent ?
					AScxmlBaseShape->Parent->Name : UnicodeString(L"NULL");

				LastError = UnicodeString().sprintf(L"INHERITANCE> Shape:[%s] Repair parent from:[%s] to:[%s]",
					AScxmlBaseShape->SimpleText.c_str(), sShapeParentName.c_str(), AOriginShape->Parent->Name.c_str());
				WLOG_WARNING(L"%s", LastError.c_str());

				// 1) ищем у хозяина, чтобы проверить, что правильно назначен владелец
				TScxmlBaseShape *ANewParent = dynamic_cast<TScxmlBaseShape*>
					(ABaseOwner->FindComponent(AOriginShape->Parent->Name));
				if (ANewParent) {
					Editorutils::SetShapeParentEx(AScxmlBaseShape, ANewParent);
				}
				else {
					LastError = UnicodeString().sprintf(L"CRITICAL! Can not find or cast new parent[%s] in Owner[%s]",
						AOriginShape->Parent->Name.c_str(), ABaseOwner->Name.c_str());
					WLOG_ERROR(L"%s", LastError.c_str());
				}
			}
		}
	}

	class TAncestorTreeReader : public TReader {
	private:
		std::set<TComponent*>FNotFoundComponents;

		std::set<TComponent*>FInheritedComponents;

		TComponent *FTreeOwner;

		bool FParsingErrors;

	protected:

		virtual bool __fastcall Error(const System::UnicodeString Message) {
			bool bRes = TReader::Error(Message);
			if (!bRes) {
				LastError = Message;
				WLOG_ERROR(L"%s", Message.c_str());
				bRes = true; /* даем возможность загрузить в любом случае */
				FParsingErrors = true;
			}
			return bRes;
		}

		// именно таким методом
		void __fastcall OnReaderAncestorNotFound(TReader* Reader, const System::UnicodeString ComponentName,
			TPersistentClass ComponentClass, TComponent* &Component) {
			if (FTreeOwner) {
#if 0 // deep debug
				WLOG_DEBUG(L"AncestorNotFound [%s][%s]", ComponentName.c_str(), ComponentClass->ClassName().c_str());
#endif
				Component = FTreeOwner->FindComponent(ComponentName);

				// финальная часть, где будет вызываться 'return'
				TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(Component);
				if (AScxmlBaseShape) {
					AScxmlBaseShape->Locked = true;
					FInheritedComponents.insert(AScxmlBaseShape);
					return;
				}
				TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>(Component);
				if (AStateMachineConnection) {
					AStateMachineConnection->Locked = true;
					FInheritedComponents.insert(AStateMachineConnection);
					return;
				}

				// если дошли сюда, значит объект удален в базовом классе
				if (!Component) {
					// это означает, что объекты удалены в базовом классе
					Application->CreateForm(ComponentClass, &Component);
					FTreeOwner->InsertComponent(Component);
					FNotFoundComponents.insert(Component);
					LastError = UnicodeString().sprintf(L"INHERITANCE> Component:[%s] was deleted in base module!",
						ComponentName.c_str());
					WLOG_WARNING(L"%s", LastError.c_str());
				}
			}
		}

		void ClearNotFoundComponents() {
			while (!FNotFoundComponents.empty()) {
				std::set<TComponent*>::iterator it = FNotFoundComponents.begin();
				delete(*it);
				FNotFoundComponents.erase(it);
			}
		}

	public:

		__fastcall TAncestorTreeReader(TStream* Stream, int BufSize, TComponent *ATreeOwner) : TReader(Stream,
			BufSize), FTreeOwner(ATreeOwner) {
			FParsingErrors = false;
			this->OnAncestorNotFound = OnReaderAncestorNotFound;
		}

		virtual __fastcall ~TAncestorTreeReader() {
			ClearNotFoundComponents();
		}

		void CheckInheritedComponents(TComponent *AOwnerAncestor) {
			if (AOwnerAncestor) {
				for (std::set<TComponent*>::iterator it = FInheritedComponents.begin();
					it != FInheritedComponents.end(); ++it) {
#if 0
					WLOG_DEBUG(L"Start:[%s]", (*it)->Name.c_str());
#endif

					TComponent *AComponentAncestor = AOwnerAncestor->FindComponent((*it)->Name);
					if (AComponentAncestor) {
						if (TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(*it)) {
							CheckAndFixInheritedParent(FTreeOwner, AScxmlBaseShape,
								dynamic_cast<TScxmlBaseShape*>(AComponentAncestor));

							// именно в этом месте
							Unique::CompareError ACompareError;
							const bool bEqual = Unique::CompareInheritedObjects(AComponentAncestor, AScxmlBaseShape,
								AScxmlBaseShape->InheritanceResolver->SkippedProps, ACompareError);
							AScxmlBaseShape->InheritanceMismatch = !bEqual;
						}
						else if (TStateMachineConnection * AConnection = dynamic_cast<TStateMachineConnection*>(*it)) {
							Unique::CompareError ACompareError;
							const bool bEqual = Unique::CompareInheritedObjects(AComponentAncestor, AConnection,
								AConnection->InheritanceResolver->SkippedProps, ACompareError);
							AConnection->InheritanceMismatch = !bEqual;
						}
					}
					else {
						if (TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(*it)) {
							AScxmlBaseShape->Locked = false;
							AScxmlBaseShape->InheritanceMismatch = false;

						}
						else if (TStateMachineConnection * AConnection = dynamic_cast<TStateMachineConnection*>(*it)) {
							AConnection->Locked = false;
							AConnection->InheritanceMismatch = false;
						}
					}
#if 0
					WLOG_DEBUG(L"End:[%s]", (*it)->Name.c_str());
#endif
				}
			}
		}

		__property bool HasParsingErrors = {
			read = FParsingErrors
		};
	};

	// ---------------------------------------------------------------------------
	// -------------------------- LoadTreeFromScxml ------------------------------
	// ---------------------------------------------------------------------------

	void SyntaxXMLCheck(const UnicodeString &sScxmlData) {
		SettingsData->ProfilerReset(__FUNCTION__);

		// здесь удобнее проверка
		std::auto_ptr<TXMLDocument>AAVCLXMLDocPtr(new TXMLDocument(Application));
		AAVCLXMLDocPtr->XML->Text = sScxmlData;
		AAVCLXMLDocPtr->Active = true;

		SettingsData->ProfileOperationReset(__FUNCTION__, "Syntax check");
	}

	void GetGuiAppendix(const UnicodeString &sFileName, TStream *AOutGuiStream) {
		_di_ILMDXmlDocument ADoc = LMDLoadXmlDocument(sFileName);
		if (!ADoc)
			throw Exception("GetGuiAppendix> Can not load [" + sFileName + "] SCXML document!");

		if (ADoc->ChildNodes->Count > 1 && ADoc->ChildNodes->Item[ADoc->ChildNodes->Count - 1]
			->NodeType == LMD_NODE_COMMENT) {
			std::auto_ptr<TBytesStream>ABytesStreamPtr
				(new TBytesStream(DecodeBase64(ADoc->ChildNodes->Item[ADoc->ChildNodes->Count - 1]->Text)));

			ABytesStreamPtr->Position = 0;
			ABytesStreamPtr->SaveToStream(AOutGuiStream);
		}
	}

	void DoLoadTreeFromScxmlData(TCustomTree *&ATree, const UnicodeString &sScxmlData, TCustomTree *ATreeAncestor) {

		// сброс всех ошибок
		LastError = L"";

		SettingsData->ProfilerReset(__FUNCTION__);

		// используем LMD, так как у него менее геморная работа с namespace
		_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML(sScxmlData);
		if (!ADoc)
			throw Exception("2) Can not load SCXML document!");

		SettingsData->ProfileOperationReset(__FUNCTION__, "Load XML");

		if (ADoc->ChildNodes->Count > 1 && ADoc->ChildNodes->Item[ADoc->ChildNodes->Count - 1]
			->NodeType == LMD_NODE_COMMENT) {

			try {
				std::auto_ptr<TBytesStream>ABytesStreamPtr
					(new TBytesStream(DecodeBase64(ADoc->ChildNodes->Item[ADoc->ChildNodes->Count - 1]->Text)));

				ABytesStreamPtr->Position = 0;

				if (ATreeAncestor) {

					/* ЗЛОБНЕЙШИЙ БАГ, БЕЗ КОТОРОГО НЕВОЗМОЖНО ЗАГРУЗИТЬ ДЕРЕВО, ЕСЛИ РОДИТЕЛЬ НЕ БЫЛ ОТКРЫТ ДО ЭТОГО !!! */
					TWinControl * AWinControl = dynamic_cast<TWinControl*>(ATree->Owner);
					if (AWinControl) {
						// блокируем перерисовку, чтобы не было моргания
						SendMessage(AWinControl->Handle, WM_SETREDRAW, 0, 0);
						AWinControl->DisableAlign();
						try {
							TWinControl *AOldParent = AWinControl->Parent;
							AWinControl->Parent = NULL;
							AWinControl->Parent = AOldParent;
						}
						__finally {
							AWinControl->EnableAlign();
							SendMessage(AWinControl->Handle, WM_SETREDRAW, 1, 0);
						}
					}

					bool bWasDesign = ATree->ComponentState.Contains(csDesigning);

					std::auto_ptr<TMemoryStream>ACloneStreamPtr(new TMemoryStream());
					ATreeAncestor->Cursor = crDefault;
					ATreeAncestor->OriginalCursor = crDefault;
					SaveTreeToStream(ATreeAncestor, ACloneStreamPtr.get());

					ACloneStreamPtr->Position = 0;

					LoadTreeFromStreamEx(ATree, ACloneStreamPtr.get());

					// мы должны все пометить, как Locked, так как может добавиться что-то новое
					for (int i = 0; i < ATree->Shapes->Count; i++) {
						TScxmlBaseShape * AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(ATree->Shapes->Items[i]);
						if (AScxmlBaseShape) {
							AScxmlBaseShape->Locked = true;
							AScxmlBaseShape->InheritanceMismatch = false;
						}
					}

					for (int i = 0; i < ATree->Connections->Count; i++) {
						TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
							(ATree->Connections->Items[i]);
						if (AStateMachineConnection) {
							AStateMachineConnection->Locked = true;
							AStateMachineConnection->InheritanceMismatch = false;
						}
					}

					// должен быть в 'namespace', так как в деструкторе удаление не найденных
					{
						std::auto_ptr<TAncestorTreeReader>AReaderPtr(new TAncestorTreeReader(ABytesStreamPtr.get(),
								4096, ATree->Owner));
						// !!! Внимание, Owner может меняться в процессе
						AReaderPtr->Owner = ATree->Owner;

						if (!bWasDesign) {
							ATree->SetDesignTime(true);
						}

						try {
							TCustomTree *ATmpTree = dynamic_cast<TCustomTree*>(AReaderPtr->ReadRootComponent(ATree));
							if (ATmpTree) {
								ATree = ATmpTree;
							}
						}
						__finally {
							ATree->SetDesignTime(bWasDesign);
						}

						// проходимся по старым, которые были записаны до этого
						AReaderPtr->CheckInheritedComponents(ATreeAncestor->Owner);

						if (AReaderPtr->HasParsingErrors) {
							CheckTreeVersion(ATree);
						}
					}

					for (int i = ATree->Connections->Count - 1; i >= 0; i--) {
						TTreeConnection *AConnection = ATree->Connections->Items[i];
						if (!AConnection->FromShape || !AConnection->ToShape) {
							// ошибка, которую позже нужно обработать
							LastError = UnicodeString().sprintf(L"Broken connection:[%s] text:[%s]",
								AConnection->Name.c_str(), AConnection->SimpleText.c_str());
							WLOG_ERROR(L"%s", LastError.c_str());
							delete AConnection;
						}
						else {
							TStateMachineConnection * AStateMachineConnection = dynamic_cast<TStateMachineConnection*>
								(AConnection);
							if (AStateMachineConnection && AStateMachineConnection->Locked) {

								TStateMachineConnection * ASourceConnection =
									Editorutils::FindConnectionByName<TStateMachineConnection>(ATreeAncestor,
									AStateMachineConnection->Name);

								if (ASourceConnection) {

									// восстанавливаем точки, если не в игноре
									if (!AStateMachineConnection->AreInheritedPointsIgnored) {
										TConnectionPointArray ASourcePoints;
										Editorutils::RememberPoints(ASourceConnection, ASourcePoints);

										TConnectionPointArray ATargetPoints;
										Editorutils::RememberPoints(AStateMachineConnection, ATargetPoints);

										if (!Editorutils::AreEqualPoints(ASourcePoints, ATargetPoints)) {
										Editorutils::RestorePoints(AStateMachineConnection, ASourcePoints);
										WLOG_DEBUG(L"INHERITANCE> Restored points=%s",
										AStateMachineConnection->Name.c_str());
										}
									}

								}
								else {
									LastError = UnicodeString().sprintf(L"Connection:[%s] ancestor not found",
										AConnection->Name.c_str());
								}
							}
						}
					}

					for (int i = 0; i < ATree->Shapes->Count; i++) {
						// смещение родителя для всех визуальных фигур
						if (TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
							(ATree->Shapes->Items[i])) {
							if (AVisualScxmlBaseShape->Parent && !AVisualScxmlBaseShape->Locked &&
								!AVisualScxmlBaseShape->ParentOffsetStored->IsEmpty()) {
								const int iOffsetX = AVisualScxmlBaseShape->ParentOffsetStored->X -
									AVisualScxmlBaseShape->ParentOffsetX;
								const int iOffsetY = AVisualScxmlBaseShape->ParentOffsetStored->Y -
									AVisualScxmlBaseShape->ParentOffsetY;
								if (iOffsetX || iOffsetY) {
									AVisualScxmlBaseShape->MoveRelative(iOffsetX, iOffsetY, false);
								}
							}
						}
					}

					TScxmlBaseShape::ValidateSimpleText(ATree);
				}
				else {
					// стандартная загрузка
					LoadTreeFromStreamEx(ATree, ABytesStreamPtr.get());
				}

				/* STREAM SUCCESSFULLY LOADED */
				return;
			}
			catch(EStreamError * E) {
				// если не смогли распарсить как гуи, тогда попробуем как текст ниже
				WLOG_ERROR(L"Can not load GUI stream: %s", E->Message.c_str());
			}
		}

		if (ADoc->DocumentElement && ADoc->DocumentElement->AsElement()) {
			ParseScxmlGuiFormat(ATree, ADoc->DocumentElement->AsElement());

			SettingsData->ProfileOperationReset(__FUNCTION__, "ParseScxmlGuiFormat");
		}
		else
			throw Exception("XML parse error");
	}

	void LoadTreeFromScxmlData(TCustomTree *&ATree, const UnicodeString &sScxmlData, TCustomTree *ATreeAncestor) {
		SyntaxXMLCheck(sScxmlData);

		DoLoadTreeFromScxmlData(ATree, sScxmlData, ATreeAncestor);
	}

	void LoadTreeFromScxmlData(TCustomTree *&ATree, const UnicodeString &sScxmlData) {
		LoadTreeFromScxmlData(ATree, sScxmlData, NULL);
	}

	void LoadTreeFromEditor(TStateMachineEditor *ATreeEditor, const UnicodeString &sFileName) {
		if (!FileExists(sFileName))
			throw Exception("File <" + sFileName + "> does not exist!");

		WLOG_INFO(L"Start loading <%s>...", sFileName.c_str());
		std::auto_ptr<TStringStream>AStringStream(new TStringStream(L"", TEncoding::UTF8, false));
		AStringStream->LoadFromFile(sFileName);
		AStringStream->Position = 0;

		WLOG_INFO(L"Start SCXML parsing <%s>...", sFileName.c_str());

		SettingsData->ProfilerReset(__FUNCTION__);

		// здесь удобнее проверка
		std::auto_ptr<TXMLDocument>AAVCLXMLDocPtr(new TXMLDocument(Application));
		AAVCLXMLDocPtr->LoadFromStream(AStringStream.get(), TXMLEncodingType::xetUTF_8);
		AAVCLXMLDocPtr->Active = true;

		SettingsData->ProfileOperationReset(__FUNCTION__, "Syntax check");

		UnicodeString sInherited = L"";
		if (AAVCLXMLDocPtr->DocumentElement && AAVCLXMLDocPtr->DocumentElement->HasAttribute(ATTR_INHERITED_MODULE)) {
			sInherited = AAVCLXMLDocPtr->DocumentElement->GetAttribute(ATTR_INHERITED_MODULE);
		}

		if (!sInherited.IsEmpty()) {
			if (!ATreeEditor->StateMachineEditorUnit)
				throw Exception("File <" + sFileName + "> is inherited from <" + sInherited +
				"> and require <StateMachineEditorUnit>");

			ATreeEditor->StateMachineEditorUnit->Inherited = sInherited;

			TStateMachineEditor *AInheritedEditor = FindStateMachineEditorByFileName
				(ATreeEditor->StateMachineEditorUnit->Inherited);
			// если редактор не в проекте, то попробуем его добавить к проекту
			if (!AInheritedEditor) {
				TStateMachineEditorUnit *AInheritedUnit = GlobalFindAndAddExistingUnit
					(ATreeEditor->StateMachineEditorUnit->Parent, ATreeEditor->StateMachineEditorUnit->Inherited,
					true, true);
				if (!AInheritedUnit->Opened) {
					AInheritedUnit->Open(true);
				}
				if (AInheritedUnit->StateMachineDockPanel) {
					AInheritedEditor = AInheritedUnit->StateMachineDockPanel->StateMachineEditor;
				}
			}
			// если уже не смогли добавить, тогда глобальная ошибка загрузки
			if (!AInheritedEditor)
				throw Exception("Can not find inherited editor [" + sInherited + "]!");

			DoLoadTreeFromScxmlData(ATreeEditor->TheTree, AAVCLXMLDocPtr->XML->Text, AInheritedEditor->TheTree);
		}
		else {
			DoLoadTreeFromScxmlData(ATreeEditor->TheTree, AAVCLXMLDocPtr->XML->Text, (TCustomTree*)NULL);
		}
	}

	void IterateInvisibleStateNodes(ILMDXmlNode * AParentNode, TScxmlBaseShape * AShape) {
#if 0 // для глубокой отладки !
		DebugInfo(AParentNode->NodeName, AParentNode->NodeType, AParentNode->XML);
#endif

		if (AParentNode->NodeType == LMD_NODE_COMMENT) {
			if (AShape != 0) {
				TScxmlBaseShape *AParentShape = AShape;
				AShape = AParentShape->AddStateChild(sctComment, false);
				if (!AShape)
					throw Exception("Type <" + AParentNode->NodeName + "> is not allowed to be child of <" +
					AParentShape->XMLName + ">");

				AShape->Comments->Text = AParentNode->Text;
			}
		}
		else if (AParentNode->NodeType == LMD_NODE_TEXT) {
			if (AShape && IsPublishedProp(AShape, "XMLText")) {
				AShape->XMLText->Text = AParentNode->Text;
				AShape->XMLText->Type = TXMLTextType::xttText;
			}
		}
		else if (AParentNode->NodeType == LMD_NODE_CDATA_SECTION) {
			if (AShape && IsPublishedProp(AShape, "XMLText")) {
				AShape->XMLText->Text = AParentNode->Text;
				AShape->XMLText->Type = TXMLTextType::xttCDATA;
			}
		}
		else if (AParentNode->NodeType == LMD_NODE_ELEMENT) {

			if (AShape && IsPublishedProp(AShape, "XMLText")) {
				AShape->XMLText->Text = AShape->XMLText->Text + AParentNode->XML;
				AShape->XMLText->Type = AParentNode->NodeName == L"scxml" ?
					TXMLTextType::xttSCXML : TXMLTextType::xttXML;
				AShape->XMLText->SyntaxScheme = L"XML";
				return; // значит добавляем данные в узлы типа <content> или <script>
			}

			bool bAdded = false;
			TTreeNodeShape *AParentShape = AShape;

			bool bExtraContent = false;

			TScxmlBaseShape *AScxmlBaseShape = dynamic_cast<TScxmlBaseShape*>(AParentShape);
			if (AScxmlBaseShape) {

				const TStateChildType AStateType = TScxmlBaseShape::XMLNodeNameToType(AParentNode->NodeName);
				if (AStateType != sctMAXSIZE) {
					AShape = AScxmlBaseShape->AddStateChild(AStateType, false);
					if (!AShape)
						throw Exception("Type <" + AParentNode->NodeName + "> is not allowed to be child of <" +
						AScxmlBaseShape->XMLName + ">");
					bAdded = true;
				}
				else {
					LOG_WARNING("%s> Unknown state type <%s>", __FUNCTION__, AnsiString(AParentNode->NodeName).c_str());
				}
			}
			else {
				LOG_WARNING("%s> Can not convert <%s> to <%s>", __FUNCTION__,
					AnsiString(AParentNode->NodeName).c_str(),
					AnsiString(__classid(TScxmlBaseShape)->ClassName()).c_str());
			}

			// если была добавлена новая нода
			if (bAdded) {

				AShape->AttributesToProps(AParentNode->AsElement());

			}
			else {
				WLOG_WARNING(L"Unknown node type <%s> on parent <%s> <%s>", AParentNode->NodeName.c_str(),
					AShape->Name.c_str(), AShape->SimpleText.c_str());

				TScxmlBaseShape *AParentShape = AShape;
				AShape = AParentShape->AddStateChild(sctExtraContent, false);
				if (!AShape)
					throw Exception("Type <" + AParentNode->NodeName + "> is not allowed to be child of <" +
					AParentShape->XMLName + ">");

				AShape->ExtraContent->Text = AParentNode->XML;
				bExtraContent = true;
			}

			for (int i = 0; i < AParentNode->ChildNodes->Count; i++) {
				if (bExtraContent)
					continue;

				IterateInvisibleStateNodes(AParentNode->ChildNodes->Item[i], AShape);

			}
		}
	}

	// ---------------------------------------------------------------------------
	void XMLStringToInvisibleShapeChildren(TScxmlBaseShape *AShape, const UnicodeString &sXMLString) {
		const Editorutils::TreeUpdateLock ATreeLock(AShape->Tree);

		_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML(sXMLString);
		if (!ADoc)
			throw Exception("Can not load SCXML document!");

		if (ADoc) {
			// чистим всё, что может относиться к содержанию внутри XML
			for (int i = AShape->Count() - 1; i >= 0; i--) {
				TChildScxmlBaseShape *AChildShape = dynamic_cast<TChildScxmlBaseShape*>(AShape->Children->Items[i]);
				if (AChildShape) {
					delete AChildShape;
				}
			}

			AShape->Comments->Text = L"";
			AShape->ExtraContent->Text = L"";

			// теперь добавляем рекурсивно дочерние
			for (int i = 0; i < ADoc->ChildNodes->Count; i++) {
				IterateInvisibleStateNodes(ADoc->ChildNodes->Item[i], AShape);
			}
		}
		else
			throw Exception("XML parse error");
	}

	// ---------------------------------------------------------------------------
	void LoadTreeFromScxml(TCustomTree *&ATree, const UnicodeString &sFileName, TCustomTree *ATreeAncestor) {
		if (!FileExists(sFileName))
			throw Exception("File <" + sFileName + "> does not exist!");

		WLOG_INFO(L"Start loading <%s>...", sFileName.c_str());
		std::auto_ptr<TStringList>AStringList(new TStringList());
		AStringList->LoadFromFile(sFileName, TEncoding::UTF8);

		WLOG_INFO(L"Start SCXML parsing <%s>...", sFileName.c_str());
		LoadTreeFromScxmlData(ATree, AStringList->Text, ATreeAncestor);
	}

	// ---------------------------------------------------------------------------
	void LoadTreeFromScxml(TCustomTree *&ATree, const UnicodeString &sFileName) {
		LoadTreeFromScxml(ATree, sFileName, NULL);
	}

	// ---------------------------------------------------------------------------
	void ForeachTreeNodeToXmlNode(TScxmlBaseShape *ABaseShape, ILMDXmlElement *ANode, const bool bAppendFirstNode,
		const TIterateSaveTypes ASaveTypes,
		const TVisualMetaInformationTypes AMetaTypes = TVisualMetaInformationTypes()) {
		if (ABaseShape && ANode) {
#if 0	// для очень детальной отладки
			WLOG_DEBUG(L"Shape [%s][%s]", ABaseShape->Name.c_str(), ABaseShape->SimpleText.c_str());
#endif

			if (ABaseShape->ExcludeFromSave) {
				WLOG_WARNING(L"Shape [%s][%s] is excluded from save", ABaseShape->Name.c_str(),
					ABaseShape->SimpleText.c_str());
				return;
			}

			// особый случай, протокольная нода
			std::set<TMetaClass*>AIgnoredCustomShapes;
			AIgnoredCustomShapes.insert(__classid(TProtocolBindingShape));
			AIgnoredCustomShapes.insert(__classid(TProtocolWeakBindingShape));
			AIgnoredCustomShapes.insert(__classid(TVirtualFolderShape));
			AIgnoredCustomShapes.insert(__classid(TTriggerShape));
			AIgnoredCustomShapes.insert(__classid(TWatchShape));

			if (AIgnoredCustomShapes.find(ABaseShape->ClassType()) != AIgnoredCustomShapes.end()) {
				return;
			}
			// особый случай, пользовательская инфа
			if (ABaseShape->ClassType() == __classid(TExtraContentShape)) {
				if (ASaveTypes.Contains(istExtraContent) || ASaveTypes.Contains(istMAXSIZE)) {
					// какие-то пользовательские штуки
					if (!ABaseShape->ExtraContent->Text.IsEmpty()) {
						_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML(ABaseShape->ExtraContent->Text);
						if (ADoc && ADoc->DocumentElement && ADoc->DocumentElement->AsElement()) {
							ANode->AppendChild((ILMDXmlElement*)ADoc->DocumentElement->AsElement());
						}
						else
							WLOG_ERROR(L"ExtraContent in shape [%s][%s] XML format is invalid[%s]",
							ABaseShape->Name.c_str(), ABaseShape->SimpleText.c_str(),
							ABaseShape->ExtraContent->Text.c_str());
					}
				}
				return;
			}

			// особый случай, комментарии
			if (ABaseShape->ClassType() == __classid(TCommentShape)) {
				if (ASaveTypes.Contains(istComments) || ASaveTypes.Contains(istMAXSIZE)) {
					if (!ABaseShape->Comments->Text.IsEmpty()) {
						ANode->AppendComment(ABaseShape->Comments->Text);
					}
				}
				return;
			}

			ILMDXmlElement *AStateNode = ANode;

			/* Особый случай: вставка другого SCXML */
			if (TVirtualShape * AVirtualShape = dynamic_cast<TVirtualShape*>(ABaseShape)) {

				if (ASaveTypes.Contains(istVirtual) || ASaveTypes.Contains(istMAXSIZE)) {

					ILMDXmlElement *AVirtualElement = NULL;

					try {
						std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));

						TStateMachineEditorUnit *AUnit = AVirtualShape->SrcUnit;
						if (AUnit && AUnit->StateMachineDockPanel) {
							Statemachine::SaveTreeToScxml(AUnit->StateMachineDockPanel->StateMachineEditor->TheTree,
								AStreamPtr.get(), false, true, ASaveTypes, AMetaTypes);
						}
						else {
							if (AVirtualShape->Src.IsEmpty())
								throw Exception(L"'Src' is not defined!");

							const UnicodeString sInvokePath = AVirtualShape->FullPath;
							if (!FileExists(sInvokePath))
								throw Exception("File [" + sInvokePath + "] is not found!");

							AStreamPtr->LoadFromFile(sInvokePath);
						}

						UnicodeString sCorrected = L"";

						if (!AVirtualShape->Alias.IsEmpty()) {
							sCorrected = AVirtualShape->GetAliasedText(AStreamPtr->DataString);
							if (!AVirtualShape->AliasParams.IsEmpty()) {
								sCorrected = AVirtualShape->GetAliasedParamsText(sCorrected);
							}
						}

						AStreamPtr->Position = 0;
						_di_ILMDXmlDocument ADoc = LMDLoadXmlDocumentFromXML
							(sCorrected.IsEmpty() ? AStreamPtr->DataString : sCorrected);
						if (!ADoc || !ADoc->DocumentElement)
							throw Exception(L"Can not load SCXML document! Path: " + AVirtualShape->Src);

						// пока что разрешаем только 1 фигуру
						for (int k = 0; k < ADoc->DocumentElement->ChildNodes->Count; k++) {
							if (ADoc->DocumentElement->ChildNodes->Item[k]->NodeType == LMD_NODE_ELEMENT) {

								const UnicodeString sNodeName = ADoc->DocumentElement->ChildNodes->Item[k]->NodeName;
								if (VISUAL_STATE_NAMES.find(sNodeName) == VISUAL_STATE_NAMES.end()) {

									WLOG_WARNING(L"VIRTUAL> Submachine:[%s] element:[%s] is ignored for save!",
										ABaseShape->SimpleText.c_str(), sNodeName.c_str());

									continue;
								}

								const UnicodeString sVirtualID = ADoc->DocumentElement->ChildNodes->Item[k]->GetAttr
									(L"id", L"");
								if (sVirtualID != AVirtualShape->SimpleText)
									throw Exception
										(L"Submachine root ID:[" + sVirtualID +
									"] mismatches to Parent:[" + AVirtualShape->SimpleText + "]");

								AVirtualElement = ADoc->DocumentElement->ChildNodes->Item[k]->AsElement();

								ANode->AppendChild(AVirtualElement);

								AStateNode = ANode->ChildNodes->Item[ANode->ChildNodes->Count - 1]->AsElement();

								AVirtualShape->ExtraParamsToAttributes(AStateNode);
								break;
							}
						}

						if (!AVirtualElement)
							throw Exception("Submachine ROOT is not found!");
					}
					catch(Exception * E) {
						WLOG_ERROR(L"VIRTUAL> Submachine:[%s] save failed. Reason: %s", ABaseShape->SimpleText.c_str(),
							E->Message.c_str());
					}

					if (SettingsData->ValidateChildren) {
						if (AVirtualElement) {

							std::set<UnicodeString>AElements;

							for (int k = 0; k < AVirtualElement->ChildNodes->Count; k++) {
								if (AVirtualElement->ChildNodes->Item[k]->NodeType == LMD_NODE_ELEMENT) {
									AElements.insert(AVirtualElement->ChildNodes->Item[k]->NodeName);
								}
							}

							for (int k = 0; k < AVirtualShape->Children->Count; k++) {
								TScxmlBaseShape * AVirtualChildShape = dynamic_cast<TScxmlBaseShape*>
									(AVirtualShape->Children->Items[k]);
								if
									(AVirtualChildShape && AVirtualChildShape->OccursOnce &&
									!AVirtualChildShape->ExcludeFromSave) {
									if (AElements.find(AVirtualChildShape->XMLName) != AElements.end()) {
										throw EScxmlBaseShapeException(AVirtualChildShape, L"Duplicated in virtual",
										L"<" + AVirtualChildShape->XMLName +
										"> must occur only 1 time but is duplicated in virtual <" +
										AVirtualShape->Src + ">");
									}
								}
							}
						}
					}
				} else {
					/* WARNING: this is non-standard inclusion just to save visual layout */
					AStateNode = ANode->AppendElement(L"state");
					AStateNode->SetAttr(L"id", AVirtualShape->SimpleText);
					AVirtualShape->ExtraParamsToAttributes(AStateNode);
					AStateNode->AppendComment(L"ScxmlEditor:virtual_src=" + AVirtualShape->Src);
				}
			}
			else {

				if (TScxmlBaseShape * AScxmlParentShape = dynamic_cast<TScxmlBaseShape*>(ABaseShape->Parent)) {
					if (SettingsData->ValidateChildren) {
						AScxmlParentShape->ValidateChild(ABaseShape);
					}
				}

				if (bAppendFirstNode) {
					AStateNode = ANode->AppendElement(ABaseShape->XMLName);
				}

				if (ASaveTypes.Contains(istAttributes) || ASaveTypes.Contains(istMAXSIZE)) {
					ABaseShape->PropsToAttributes(AStateNode);
				}

				if (ASaveTypes.Contains(istXmlText) || ASaveTypes.Contains(istMAXSIZE)) {
					if (IsPublishedProp(ABaseShape, "XMLText")) {
						if (SettingsData->ValidateXMLText) {
							ABaseShape->ValidateXMLText();
						}

						ABaseShape->XMLText->AppendToElement(AStateNode);
					}
				}
			}

			if (ASaveTypes.Contains(istVisibleChilds) || ASaveTypes.Contains(istMAXSIZE)) {
				if (STATE_TYPES_VISUAL_SHAPES.find(ABaseShape->StateChildType) != STATE_TYPES_VISUAL_SHAPES.end()) {

					const bIsScxml = ABaseShape->StateChildType == sctScxml;

					TStateShape *AStateShape = dynamic_cast<TStateShape*>(ABaseShape);

					// OLD GUI
					if (AMetaTypes.Contains(vmiScxmlGui)) {
						int iX = ABaseShape->X0;
						int iY = ABaseShape->Y0;

						if (ABaseShape->VisualParent) {
							iX -= ABaseShape->VisualParent->X0;
							iY -= ABaseShape->VisualParent->Y0;
						}

						const UnicodeString sGuiFormat = UnicodeString().sprintf
							(L"   node-size-and-position x=%d y=%d w=%d h=%d  ", iX, iY,
							ABaseShape->Width, ABaseShape->Height);
						AStateNode->AppendComment(sGuiFormat);
					}

					// Simple
					if (AMetaTypes.Contains(vmiSimple)) {
						const UnicodeString sSimpleFormat = UnicodeString().sprintf
							(L"shape_x_y_w_h:%d;%d;%d;%d", ABaseShape->X0, ABaseShape->Y0,
							ABaseShape->Width, ABaseShape->Height);
						AStateNode->AppendComment(sSimpleFormat);
                    }

					// VSCode
					if (AMetaTypes.Contains(vmiVSCode)) {
						if (bIsScxml) {
							AStateNode->SetAttr("xmlns:viz", "http://phrogz.net/visual-scxml");
						}
						else {

							if (AStateShape && AStateShape->StateChildType != sctParallel) {

								TColor AColor = AStateShape->Color;
								if (AStateShape->IsCluster()) {
                                	AColor = TColor(RGB(128, 128, 128));
								}

								// Convert TColor to RGB
								const COLORREF rgb = ColorToRGB(AColor);

								// Extract red, green, and blue components
								const int red = GetRValue(rgb);
								const int green = GetGValue(rgb);
								const int blue = GetBValue(rgb);

								UnicodeString hexColor;
								hexColor.sprintf(L"%.2x%.2x%.2x", red, green, blue);

								AStateNode->SetAttr("viz:rgb", hexColor);
							}

							AStateNode->SetAttr("viz:xywh", UnicodeString().sprintf(L"%d %d %d %d", ABaseShape->X0,
									ABaseShape->Y0, ABaseShape->Width, ABaseShape->Height));

						}
					}

					// QT
					if (AMetaTypes.Contains(vmiQt)) {
						if (bIsScxml) {
							AStateNode->SetAttr("xmlns:qt", "http://www.qt.io/2015/02/scxml-ext");
							AStateNode->SetAttr("qt:editorversion", "4.12.0");
						}
						else {
							ILMDXmlNode *AQtNode = AStateNode->AppendElement("qt:editorinfo");

							int iSceneCenterX = ABaseShape->XCenter();
							int iSceneCenterY = ABaseShape->YCenter();

							int iDiffX = 0;
							int iDiffY = 0;

							if (ABaseShape->VisualParent && ABaseShape->VisualParent->StateChildType != sctScxml) {
								iDiffX = iSceneCenterX - ABaseShape->Parent->XCenter();
								iDiffY = iSceneCenterY - ABaseShape->Parent->YCenter();

								iSceneCenterX = iDiffX;
								iSceneCenterY = iDiffY;
							}

							AQtNode->SetAttr("geometry", UnicodeString().sprintf(L"%d;%d;%d;%d;%d;%d", iSceneCenterX,
									iSceneCenterY, -ABaseShape->Width / 2, -ABaseShape->Height / 2, ABaseShape->Width,
									ABaseShape->Height));

							AQtNode->SetAttr("scenegeometry", UnicodeString().sprintf(L"%d;%d;%d;%d;%d;%d",
									ABaseShape->XCenter() - iDiffX, ABaseShape->YCenter() - iDiffY,
									ABaseShape->X0 - iDiffX, ABaseShape->Y0 - iDiffY, ABaseShape->Width,
									ABaseShape->Height));
						}
					}

				}
			}

			/* дочерние элементы */
			if (ASaveTypes.Contains(istInvisibleChilds) || ASaveTypes.Contains(istMAXSIZE)) {
				// все дочерние невидимые
				for (int i = 0; i < ABaseShape->Children->Count; i++) {
					TScxmlBaseShape *ABaseChildShape = dynamic_cast<TScxmlBaseShape*>(ABaseShape->Children->Items[i]);
					if (ABaseChildShape && //
						STATE_TYPES_VISUAL_SHAPES.find(ABaseChildShape->StateChildType) == STATE_TYPES_VISUAL_SHAPES.end
						()) {

						ForeachTreeNodeToXmlNode(ABaseChildShape, (ILMDXmlElement*)AStateNode, true, ASaveTypes,
							AMetaTypes);
					}
				}
			}

			/* соединения */
			if (ASaveTypes.Contains(istConnections) || ASaveTypes.Contains(istMAXSIZE)) {

				/* Experimental */
				using namespace Editorutils;
				TConnectionFromToMap AInConnections;
				CollectInConnections(ABaseShape, AInConnections);

				std::map<TVisualScxmlBaseShape* /* From */ , TStateMachineConnection*>AEmptyConnections;

				for (TConnectionFromToMap::iterator it = AInConnections.begin(); it != AInConnections.end(); ++it) {
					TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>(it->first);
					/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
					if (AConnection && !AConnection->ExcludeFromSave) {
						switch(AConnection->SWITCH) {
						case tstNONE: {
								if (!AConnection->IsSelfConnection) {
									if (AConnection->IsContentEmpty) {
										AEmptyConnections.insert(std::make_pair(AConnection->VisualFrom, AConnection));
									}
								}
							}break;
						default:
							AConnection->AppendTransitionSwitchElement(AStateNode, AConnection->VisualFrom->SimpleText, AMetaTypes);
							break;
						}

					}
				}

				// transitions
				for (int i = 0; i < ABaseShape->Connections->Count; i++) {
					TStateMachineConnection *AConnection = dynamic_cast<TStateMachineConnection*>
						(ABaseShape->Connections->Items[i]);
					/* 'ExcludeFromSave==false' гарантирует VisualFrom и VisualTo */
					if (AConnection && !AConnection->ExcludeFromSave) {
						if (!AConnection->IsSelfConnection) {
							if (AConnection->IsContentEmpty) {
								std::map<TVisualScxmlBaseShape*,
								TStateMachineConnection*>::iterator it = AEmptyConnections.find(AConnection->VisualTo);
								if (it != AEmptyConnections.end()) {
									throw EStateMachineConnectionException(AConnection, L"Circular deadlock", //
										UnicodeString().sprintf(L"Circular deadlock detected between [%s] and [%s]",
										AConnection->Name.c_str(), it->second->Name.c_str()));
								}
							}
						}

						AConnection->AppendTransitionElement(AStateNode, AMetaTypes);
					}
				}
			}

			/* визуальные элементы */
			if (ASaveTypes.Contains(istVisibleChilds) || ASaveTypes.Contains(istMAXSIZE)) {
				// теперь все дочерние видимые
				for (int i = 0; i < ABaseShape->Children->Count; i++) {
					TScxmlBaseShape *ABaseChildShape = dynamic_cast<TScxmlBaseShape*>(ABaseShape->Children->Items[i]);
					if (ABaseChildShape && STATE_TYPES_VISUAL_SHAPES.find(ABaseChildShape->StateChildType)
						!= STATE_TYPES_VISUAL_SHAPES.end()) {
						ForeachTreeNodeToXmlNode(ABaseChildShape, (ILMDXmlElement*)AStateNode, true, ASaveTypes,
							AMetaTypes);
					}
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	UnicodeString InvisibleShapeChildrenToXMLString(TScxmlBaseShape *AShape) {
		if (AShape) {
			_di_ILMDXmlDocument AMainDoc = LMDCreateXmlDocument(L"root", L"1.0", L"UTF-8");
			ForeachTreeNodeToXmlNode(AShape, AMainDoc->DocumentElement, false,
				TIterateSaveTypes() << istComments << istExtraContent << istInvisibleChilds << istXmlText <<
				istAttributes);
			std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
			for (int i = 0; i < AMainDoc->DocumentElement->ChildNodes->Count; i++) {
				AStreamPtr->WriteString(AMainDoc->DocumentElement->ChildNodes->Item[i]->XML);
			}
			return AStreamPtr->DataString;
		}
		return L"";
	}

	// ---------------------------------------------------------------------------
	void IterateFlatNodes(ILMDXmlNode * AParentNode, const UnicodeString &sPath) {
		if (AParentNode && (AParentNode->NodeType == LMD_NODE_ELEMENT || AParentNode->NodeType == LMD_NODE_DOCUMENT)) {
			if (AParentNode->NodeName == "invoke" && AParentNode->AttrExists("src") && !AParentNode->GetBoolAttr
				("disableflatbuild", false)) {
				const UnicodeString sInvokeFile = TPath::Combine(sPath, AParentNode->GetAttr("src"));
				const UnicodeString sExt = ExtractFileExt(sInvokeFile);
				// подгружаем только XML и SCXML
				if (SameText(sExt, ".xml") || SameText(sExt, ".scxml")) {
					if (!FileExists(sInvokeFile))
						throw Exception("File <" + sInvokeFile + "> is not found!");

					std::auto_ptr<TStringList>AStringList(new TStringList());
					AStringList->LoadFromFile(sInvokeFile, TEncoding::UTF8);

					_di_ILMDXmlDocument AFlatDoc = LMDCreateXmlDocument(L"scxml", L"1.0", L"UTF-8");
					AFlatDoc->LoadXML(AStringList->Text);

					if (!AFlatDoc && !AFlatDoc->DocumentElement)
						throw Exception("Can not load XML <" + sInvokeFile + ">!");

					IterateFlatNodes(AFlatDoc->DocumentElement, ExtractFilePath(sInvokeFile));

					ILMDXmlNode *AContentNode = AParentNode->AppendElement("content");
					AContentNode->AppendChild((ILMDXmlElement*)AFlatDoc->DocumentElement->AsElement());

					AParentNode->RemoveAttr("src");
				}
				else
					throw Exception("Extension [" + sExt + "] is not supported for flat invokation!");
			}
			// возможность загрузить в тело scxml файл, указанный в 'src'
			else if ((AParentNode->NodeName == "script" || AParentNode->NodeName == "data") && AParentNode->AttrExists
				("src") && !AParentNode->GetBoolAttr("disableflatbuild", false)) {
				const UnicodeString sFile = TPath::Combine(sPath, AParentNode->GetAttr("src"));
				if (!FileExists(sFile))
					throw Exception("File <" + sFile + "> is not found!");

				std::auto_ptr<TStringList>AStringList(new TStringList());
				AStringList->LoadFromFile(sFile);

				AParentNode->RemoveAttr("src");
				AParentNode->Text = AStringList->Text;
			}
			else {
				int i = 0;
				while (i < AParentNode->ChildNodes->Count) {
					if (AParentNode->ChildNodes->Item[i]->NodeType == LMD_NODE_COMMENT) {
						AParentNode->RemoveChild(AParentNode->ChildNodes->Item[i]);
					}
					else {
						if (AParentNode->NodeType == LMD_NODE_ELEMENT || AParentNode->NodeType == LMD_NODE_DOCUMENT) {
							IterateFlatNodes(AParentNode->ChildNodes->Item[i], sPath);
						}
						i++;
					}
				}
			}
		}
	}

	// ---------------------------------------------------------------------------

	// ---------------------------------------------------------------------------

	typedef std::map<ILMDXmlNode*, std::set<UnicodeString> >TMapUniqueStates;

	void ValidateUniqueStateIDs(ILMDXmlNode * AParentNode, TMapUniqueStates &AStateIDs, ILMDXmlNode * AScxmlNode) {
		if (AParentNode && (AParentNode->NodeType == LMD_NODE_ELEMENT || AParentNode->NodeType == LMD_NODE_DOCUMENT)) {
			// SCXML могут быть вложенными, поэтому определяем текущий,
			// как только встретится новый элемент, переключаем родителя на него,
			// и рекурсивно переходим, а потом возвращаемся обратно
			if (AParentNode->NodeName == L"scxml") {
				AStateIDs.insert(std::make_pair(AParentNode, std::set<UnicodeString>()));
				AScxmlNode = AParentNode;
			}
			else if (VISUAL_STATE_NAMES.find(AParentNode->NodeName) != VISUAL_STATE_NAMES.end()) {
				TMapUniqueStates::iterator it = AStateIDs.find(AScxmlNode);
				if (it == AStateIDs.end()) {
					throw Exception("Can not find SCXML parent for [" + AParentNode->NodeName + "]!");
				}

				const UnicodeString sId = AParentNode->GetAttr(L"id", "");

				// пустые ID допускаются
				if (!sId.IsEmpty()) {
					if (it->second.find(sId) != it->second.end()) {
						const UnicodeString sScxmlName = it->first->GetAttr(L"name", L"");
						throw EScxmlDuplicateStateIDException(UnicodeString().sprintf
							(L"Element:[%s] has duplicated ID:[%s] Scxml:[%s]", AParentNode->NodeName.c_str(),
								sId.c_str(), sScxmlName.c_str()), sScxmlName, sId);
					}

					it->second.insert(sId);
				}

			}

			for (int i = 0; i < AParentNode->ChildNodes->Count; i++) {
				ValidateUniqueStateIDs(AParentNode->ChildNodes->Item[i], AStateIDs, AScxmlNode);
			}
		}
	}

	typedef std::set<UnicodeString>TUniqueIDs;

	void ValidateInitialIDs(ILMDXmlNode * AParentNode, TUniqueIDs &AParentIDs, ILMDXmlNode * AScxmlNode) {
		if (AParentNode && (AParentNode->NodeType == LMD_NODE_ELEMENT || AParentNode->NodeType == LMD_NODE_DOCUMENT)) {

			TUniqueIDs AChildrenIDs;

			bool bIsScxml = false;
			bool bIsState = false;

			UnicodeString sId = "";

			// SCXML могут быть вложенными, поэтому определяем текущий,
			// как только встретится новый элемент, переключаем родителя на него,
			// и рекурсивно переходим, а потом возвращаемся обратно
			if (AParentNode->NodeName == L"scxml") {
				bIsScxml = true;
				AScxmlNode = AParentNode;
				sId = AParentNode->GetAttr(L"name", "");
			}
			else if (VISUAL_STATE_NAMES.find(AParentNode->NodeName) != VISUAL_STATE_NAMES.end()) {
				bIsState = true;
				sId = AParentNode->GetAttr(L"id", "");
				if (!sId.IsEmpty()) {
					AParentIDs.insert(sId);
				}
			}

			for (int i = 0; i < AParentNode->ChildNodes->Count; i++) {
				ValidateInitialIDs(AParentNode->ChildNodes->Item[i], AChildrenIDs, AScxmlNode);
			}

			if (bIsScxml || bIsState) {
				std::auto_ptr<TStringList>AUniqueIDs(new TStringList);
				AUniqueIDs->Delimiter = L' ';
				AUniqueIDs->StrictDelimiter = true;
				AUniqueIDs->DelimitedText = AParentNode->GetAttr(L"initial", "");
				for (int i = 0; i < AUniqueIDs->Count; i++) {
					const UnicodeString sInitialID = AUniqueIDs->Strings[i];
					if (AChildrenIDs.find(sInitialID) == AChildrenIDs.end()) {
						const UnicodeString sScxmlName = AScxmlNode ? UnicodeString(AScxmlNode->GetAttr(L"name", L""))
							: UnicodeString("");
						throw EScxmlDuplicateStateIDException(UnicodeString().sprintf
							(L"Element:[%s]:[%s] initial ID:[%s] not found Scxml:[%s]", AParentNode->NodeName.c_str(),
								sId.c_str(), sInitialID.c_str(), sScxmlName.c_str()), sScxmlName, sId);
					}
				}

			}

			if (!bIsScxml) {
				if (!AChildrenIDs.empty()) {
					AParentIDs.insert(AChildrenIDs.begin(), AChildrenIDs.end());
				}
			}
		}
	}

	typedef std::multimap<UnicodeString, UnicodeString>TUnicodeDictionary;

	void GetInPredicateValues(const UnicodeString &sText, const std::set<UnicodeString> &AStateIDs,
		const bool bIsNullDatamodel) {
		if (sText.IsEmpty())
			return;

		std::wstring xStr = sText.c_str();
		boost::wregex xRegEx(L"(^|;?[ ]+|\\r?\\n)In\\s*[(]([^()]*)[)]");

		std::wstring::const_iterator start, end;
		start = xStr.begin();
		end = xStr.end();
		boost::match_results<std::wstring::const_iterator>what;
		boost::match_flag_type flags = boost::match_default;
		while (regex_search(start, end, what, xRegEx, flags)) {

			if (what.size() == 3) {
				const UnicodeString sWasExpr(std::wstring(what[2]).c_str());
				UnicodeString sInPredExpr = sWasExpr.Trim();

				if (!bIsNullDatamodel) {
					UnicodeString sOutExpr = L"";

					const int iLength = sInPredExpr.Length();
					if (iLength > 3) {
						if (sInPredExpr[1] == sInPredExpr[iLength] &&
							(sInPredExpr[1] == L'\'' || sInPredExpr[1] == L'"')) {
							sOutExpr = sInPredExpr.SubString(2, iLength - 2);
						}
					}

					sInPredExpr = sOutExpr;
				}

				if (!sInPredExpr.IsEmpty() && AStateIDs.find(sInPredExpr) == AStateIDs.end())
					throw Exception(UnicodeString().sprintf(L"Predicate 'In(%s)' error! ID:[%s] is missing!",
						sWasExpr.c_str(), sInPredExpr.c_str()));
			}

			// update search position:
			start = what[0].second;
		}
	}

	void ValidateInPredicate(ILMDXmlNode * AParentNode, const TMapUniqueStates &AStateIDs, ILMDXmlNode * AScxmlNode) {
		if (AParentNode && (AParentNode->NodeType == LMD_NODE_ELEMENT || AParentNode->NodeType == LMD_NODE_DOCUMENT)) {
			const UnicodeString sNodeName = AParentNode->NodeName;

			// SCXML могут быть вложенными, поэтому определяем текущий,
			// как только встретится новый элемент, переключаем родителя на него,
			// и рекурсивно переходим, а потом возвращаемся обратно
			if (sNodeName == L"scxml") {
				AScxmlNode = AParentNode;
			}
			else {

				TMapUniqueStates::const_iterator itScxml = AStateIDs.find(AScxmlNode);
				if (itScxml == AStateIDs.end()) {
					throw Exception("Can not find SCXML parent for [" + sNodeName + "]!");
				}

				TUnicodeDictionary AMapExpr;
				AMapExpr.insert(std::make_pair("transition", "cond"));
				AMapExpr.insert(std::make_pair("elseif", "cond"));
				AMapExpr.insert(std::make_pair("if", "cond"));
				AMapExpr.insert(std::make_pair("log", "expr"));
				AMapExpr.insert(std::make_pair("data", "expr"));
				AMapExpr.insert(std::make_pair("assign", "expr"));
				AMapExpr.insert(std::make_pair("content", "expr"));
				AMapExpr.insert(std::make_pair("param", "expr"));
				AMapExpr.insert(std::make_pair("send", "eventexpr"));
				AMapExpr.insert(std::make_pair("send", "targetexpr"));
				AMapExpr.insert(std::make_pair("send", "typeexpr"));
				AMapExpr.insert(std::make_pair("send", "delayexpr"));
				AMapExpr.insert(std::make_pair("cancel", "sendidexpr"));
				AMapExpr.insert(std::make_pair("invoke", "typeexpr"));
				AMapExpr.insert(std::make_pair("invoke", "srcexpr"));

				const UnicodeString sScxmlDataModel = AScxmlNode->GetAttr(L"datamodel", L"");
				const bool bIsNull = sScxmlDataModel.IsEmpty() || SameText(sScxmlDataModel, "null");

				try {
					std::pair<TUnicodeDictionary::iterator,
					TUnicodeDictionary::iterator>it_range = AMapExpr.equal_range(sNodeName);
					for (TUnicodeDictionary::iterator it = it_range.first; it != it_range.second; ++it) {
						if (AParentNode->AttrExists(it->second)) {
							GetInPredicateValues(AParentNode->GetAttr(it->second, ""), itScxml->second, bIsNull);
						}
					}

					std::set<UnicodeString>AContentSet;
					AContentSet.insert("content");
					AContentSet.insert("script");
					AContentSet.insert("data");

					if (AContentSet.find(sNodeName) != AContentSet.end()) {
						GetInPredicateValues(AParentNode->Text, itScxml->second, bIsNull);
					}
				}
				catch(Exception * E) {
					const UnicodeString sScxmlName = AScxmlNode->GetAttr(L"name", L"");

					WLOG_WARNING(L"Full context:[%s]", AParentNode->XML.Trim().c_str());
					throw Exception(UnicodeString().sprintf(L"Scxml:[%s] Node:[%s] Msg: %s", sScxmlName.c_str(),
							sNodeName.c_str(), E->Message.c_str()));
				}
			}

			for (int i = 0; i < AParentNode->ChildNodes->Count; i++) {
				ValidateInPredicate(AParentNode->ChildNodes->Item[i], AStateIDs, AScxmlNode);
			}
		}
	}

	// ---------------------------------------------------------------------------
	void DumpUniqueStates(const TMapUniqueStates &AUniqueStates) {
		for (TMapUniqueStates::const_iterator itScxml = AUniqueStates.begin(); itScxml != AUniqueStates.end();
			++itScxml) {
			for (std::set<UnicodeString>::const_iterator itState = itScxml->second.begin();
				itState != itScxml->second.end(); ++itState) {
				WLOG_DEBUG(L"Scxml:[0x%08X][%s] node id:[%s]", reinterpret_cast<int>(itScxml->first),
					itScxml->first->GetAttr(L"name", L"").c_str(), (*itState).c_str());
			}
		}
	}

	// ---------------------------------------------------------------------------
	void SaveTreeToScxml(TCustomTree *ATree, Classes::TStream *AStream, bool bAppendGui /* =true */ ,
		bool bSkipVersionEncoding /* =false */ ,
		const TIterateSaveTypes ASaveTypes /* = TIterateSaveTypes() << istMAXSIZE */ ,
		const TVisualMetaInformationTypes AMetaTypes /* = TVisualMetaInformationTypes() */ ) {

		/* не делать Try-Catch, приводим к единому поведению */

		if (!ATree)
			throw Exception(__FUNCTION__ "> ATree==NULL");

		TScxmlShape *AScxmlShape = dynamic_cast<TScxmlShape*>(GetSafeTreeListFirst(ATree->Items));
		if (AScxmlShape) {

			_di_ILMDXmlDocument AMainDoc = LMDCreateXmlDocument(L"scxml", L"1.0", L"UTF-8");

			ForeachTreeNodeToXmlNode(AScxmlShape, AMainDoc->DocumentElement, false, ASaveTypes, AMetaTypes);

			if (AMainDoc->DocumentElement) {

				TMapUniqueStates AUniqueStates;
				ValidateUniqueStateIDs(AMainDoc->DocumentElement, AUniqueStates, NULL);

				TUniqueIDs AUniqueIDs;
				ValidateInitialIDs(AMainDoc->DocumentElement, AUniqueIDs, NULL);

#if 0 // для глубокой отладки
				DumpUniqueStates(AUniqueStates);
#endif

				// проверка предиката 'In()'
				if (SettingsData->ValidateInPredicate) {
					if (AScxmlShape->StateMachineEditorUnit && !AScxmlShape->StateMachineEditorUnit->IsVirtual) {
						ValidateInPredicate(AMainDoc->DocumentElement, AUniqueStates, NULL);
					}
				}

				if (bAppendGui) {
					std::auto_ptr<TMemoryStream>AMemStreamPtr(new TMemoryStream());
					ATree->Cursor = crDefault; // fix bug: может записаться не дефолтный курсор
					ATree->OriginalCursor = crDefault;
					SaveTreeToStream(ATree, AMemStreamPtr.get());

					AMainDoc->AppendComment(LMDBinToBase64(AMemStreamPtr->Memory, AMemStreamPtr->Size, 75));
				}
				if (!bSkipVersionEncoding) {
					AMainDoc->Save(AStream);
				}
				else {
					std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
					AStreamPtr->WriteString(AMainDoc->DocumentElement->XML);
					AStreamPtr->SaveToStream(AStream);
				}
			}
		}
		else {
			LOG_WARNING("Can not cast tree root to <TScxmlShape>");
		}
	}

	// ---------------------------------------------------------------------------
	void SaveTreeToScxml(TCustomTree *ATree, const UnicodeString &sFileName, bool bAppendGui /* =true */ ,
		bool bSkipVersionEncoding /* =false */ ,
		const TIterateSaveTypes ASaveTypes /* = TIterateSaveTypes() << istMAXSIZE */ ,
		const TVisualMetaInformationTypes AMetaTypes /* = TVisualMetaInformationTypes() */ ) {

		/* не делать Try-Catch, приводим к единому поведению */
		std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));

		SaveTreeToScxml(ATree, AStreamPtr.get(), bAppendGui, bSkipVersionEncoding, ASaveTypes, AMetaTypes);

		AStreamPtr->SaveToFile(sFileName);
	}

	// ---------------------------------------------------------------------------
	void SaveScxmlAsFlat(const UnicodeString &sFileName, const UnicodeString &sFlatFileName) {
		try {
			const UnicodeString sInvokeFile = sFileName;
			if (!FileExists(sInvokeFile))
				throw Exception("File <" + sInvokeFile + "> is not found!");

			std::auto_ptr<TStringList>AStringList(new TStringList());
			AStringList->LoadFromFile(sInvokeFile, TEncoding::UTF8);

			_di_ILMDXmlDocument AFlatDoc = LMDCreateXmlDocument(L"scxml", L"1.0", L"UTF-8");

			AFlatDoc->LoadXML(AStringList->Text);

			if (!AFlatDoc)
				throw Exception("Can not load XML <" + sInvokeFile + ">!");

			IterateFlatNodes(AFlatDoc, ExtractFilePath(sInvokeFile));

			AFlatDoc->Save(sFlatFileName);

			WLOG_INFO(L"Successfully saved <%s>!", sFlatFileName.c_str());
		}
		catch(Exception * E) {
			LOG_ERROR(LOG_ERROR_MSG);
		}
	}

	// ---------------------------------------------------------------------------
	UnicodeString GetRawScxml(TCustomTree *ATree, bool bSkipComments) {
		const Statemachine::TIterateSaveTypes ATypes = bSkipComments == false ? Statemachine::MaxPossibleTypes() :
			(Statemachine::MaxPossibleTypes() >> Statemachine::istComments);

		// только формируем текст
		std::auto_ptr<TStringStream>AStreamPtr(new TStringStream(L"", TEncoding::UTF8, false));
		Statemachine::SaveTreeToScxml(ATree, AStreamPtr.get(), false, true, ATypes);

		return AStreamPtr->DataString;
	}

	// ---------------------------------------------------------------------------
	void SaveInheritedToScxml(TCustomTree *AEditorRoot, TCustomTree *AEditorAncestor, TStream *AStream,
		const UnicodeString &sInherited, const TVisualMetaInformationTypes AMetaTypes /* = TVisualMetaInformationTypes() */) {

		if (!AEditorRoot)
			throw Exception(__FUNCTION__ "> AEditorRoot==NULL");

		if (!AEditorAncestor)
			throw Exception(__FUNCTION__ "> AEditorAncestor==NULL");

		TScxmlShape *AScxmlShape = dynamic_cast<TScxmlShape*>(GetSafeTreeListFirst(AEditorRoot->Items));
		if (AScxmlShape) {

			for (int i = 0; i < AEditorRoot->Shapes->Count; i++) {
				TVisualScxmlBaseShape * AVisualScxmlBaseShape = dynamic_cast<TVisualScxmlBaseShape*>
					(AEditorRoot->Shapes->Items[i]);
				if (AVisualScxmlBaseShape && AVisualScxmlBaseShape->Parent && !AVisualScxmlBaseShape->Locked) {
					AVisualScxmlBaseShape->ParentOffsetStored->X = AVisualScxmlBaseShape->ParentOffsetX;
					AVisualScxmlBaseShape->ParentOffsetStored->Y = AVisualScxmlBaseShape->ParentOffsetY;
				}
			}

			_di_ILMDXmlDocument AMainDoc = LMDCreateXmlDocument(L"scxml", L"1.0", L"UTF-8");
			const TIterateSaveTypes ASaveTypes = TIterateSaveTypes() << istMAXSIZE;
			ForeachTreeNodeToXmlNode(AScxmlShape, AMainDoc->DocumentElement, false, ASaveTypes, AMetaTypes);

			if (AMainDoc->DocumentElement) {

				TMapUniqueStates AUniqueStates;
				ValidateUniqueStateIDs(AMainDoc->DocumentElement, AUniqueStates, NULL);

#if 0 // для глубокой отладки
				DumpUniqueStates(AUniqueStates);
#endif

				// More precise check for initials
				TUniqueIDs AUniqueIDs;
				ValidateInitialIDs(AMainDoc->DocumentElement, AUniqueIDs, NULL);

				// проверка предиката 'In()'
				if (SettingsData->ValidateInPredicate) {
					if (AScxmlShape->StateMachineEditorUnit && !AScxmlShape->StateMachineEditorUnit->IsVirtual) {
						ValidateInPredicate(AMainDoc->DocumentElement, AUniqueStates, NULL);
					}
				}

				if (!sInherited.IsEmpty()) {
					AMainDoc->DocumentElement->SetAttr(ATTR_INHERITED_MODULE, sInherited);
				}

				std::auto_ptr<TMemoryStream>AMemStreamPtr(new TMemoryStream());

				{
					AEditorAncestor->Cursor = crDefault;
					std::auto_ptr<TWriter>AWriterPtr(new TWriter(AMemStreamPtr.get(), 4096));
					AWriterPtr->WriteDescendent(AEditorRoot, AEditorAncestor);
				}

				AMemStreamPtr->Position = 0;

				AMainDoc->AppendComment(LMDBinToBase64(AMemStreamPtr->Memory, AMemStreamPtr->Size, 75));

				AMainDoc->Save(AStream);
			}
		}
		else {
			LOG_WARNING("Can not cast tree root to <TScxmlShape>");
		}
	}
}

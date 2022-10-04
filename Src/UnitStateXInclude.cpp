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

#include <set>
#include <memory>

#include <XMLDoc.hpp>
#include <IOUtils.hpp>

#include "UnitStateXInclude.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Statemachine {

	void IterateCombineNodes(_di_IXMLNode ANode, std::set<UnicodeString>&AStateIdSet, const UnicodeString sPath = L"",
		UnicodeString sResolveID = L"");

	//---------------------------------------------------------------------------
	void CheckDuplicatedId(_di_IXMLNode ANode, std::set<UnicodeString> &AStateIdSet) {
		if (ANode) {
			if (ANode->NodeName == "state" && ANode->HasAttribute("id")) {
				const UnicodeString sStateId = ANode->GetAttribute("id");
				if (AStateIdSet.find(sStateId) != AStateIdSet.end())
					throw Exception(L"Duplicated state with ID <" + sStateId + L"> is present!!!");
				else
					AStateIdSet.insert(sStateId);
			}
		}
	}

	typedef enum {
		icSRC, icXInclude
	}TIncludeType;

	void InsertIncludeSCXML(_di_IXMLNode ANode, const UnicodeString &sRelativePath, TIncludeType AIncludeType,
		std::set<UnicodeString> &AStateIdSet, const UnicodeString sPath = L"", UnicodeString sResolveID = L"") {

		const UnicodeString sInclude = TPath::Combine(sPath, sRelativePath);
		if (FileExists(sInclude)) {
			std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(Application));
			AXMLDocument->LoadFromFile(sInclude);

			_di_IXMLNode RootNode = ANode->ParentNode;
			assert(RootNode != NULL);

			// у родительской ноды получаем идентификатор
			if ((AIncludeType == icXInclude ? RootNode : ANode)->HasAttribute("id")) {
				const UnicodeString sParentId = (AIncludeType == icXInclude ? RootNode : ANode)->GetAttribute("id");
				sResolveID = sResolveID.IsEmpty() ? sParentId : sResolveID + "." + sParentId;
			}

			UnicodeString sPositionAndSize = "";
			_di_IXMLNode ACommentNode = ANode->ChildNodes->First();
			if (ACommentNode && ACommentNode->NodeType == ntComment) {
				sPositionAndSize = ACommentNode->Text;
			}
			_di_IXMLNode StateNode = 0;
			if (AIncludeType == icXInclude) {
				const int iRealIndex = RootNode->ChildNodes->IndexOf(ANode);

				const int iIndex = RootNode->ChildNodes->Remove(ANode);
				StateNode = RootNode->AddChild("state", iIndex);
			}
			else {
				StateNode = ANode->AddChild("state");
			}

			if (!AXMLDocument->DocumentElement->HasAttribute("name"))
				throw Exception("Need attribute <name>! " + ANode->XML);

			const UnicodeString sRootNodeName = sResolveID + "." + AXMLDocument->DocumentElement->GetAttribute("name");

			StateNode->SetAttribute("id", sRootNodeName);
			StateNode->SetAttribute("extern_filename", sRelativePath);

			CheckDuplicatedId(StateNode, AStateIdSet);

			if (AXMLDocument->DocumentElement->HasAttribute("initial")) {
				const UnicodeString sInitial = sResolveID + "." + AXMLDocument->DocumentElement->GetAttribute("initial");
				StateNode->SetAttribute("initial", sInitial);
			}
			if (!sPositionAndSize.IsEmpty()) {
				StateNode->SetAttribute("position_and_size", sPositionAndSize);
			}

			for (int i = 0; i < AXMLDocument->DocumentElement->ChildNodes->Count; i++) {
				int iNewIndex = StateNode->ChildNodes->Add(AXMLDocument->DocumentElement->ChildNodes->Nodes[i]);
				const UnicodeString sNewPath = TPath::GetDirectoryName(sInclude);
				IterateCombineNodes(StateNode->ChildNodes->Nodes[iNewIndex], AStateIdSet, sNewPath, sResolveID);
			}
		}
		else
			throw Exception(L"Include file <" + sInclude + L"> is not found!!!");
	}

	//---------------------------------------------------------------------------
	void IterateCombineNodes(_di_IXMLNode ANode, std::set<UnicodeString> &AStateIdSet, const UnicodeString sPath/* = L""*/,
		UnicodeString sResolveID/* = L""*/) {
		if (ANode) {
			if (ANode->NodeName == "xi:include" && ANode->HasAttribute("href")) {
				InsertIncludeSCXML(ANode, ANode->GetAttribute("href"), icXInclude, AStateIdSet, sPath, sResolveID);
			}
			else {
				if (!sResolveID.IsEmpty()) {

					if (ANode->HasAttribute("id")) {
						const UnicodeString sNewId = sResolveID + "." + ANode->GetAttribute("id");
						ANode->SetAttribute("id", sNewId);
					}
					if (ANode->HasAttribute("target")) {
						const UnicodeString sNewId = sResolveID + "." + ANode->GetAttribute("target");
						ANode->SetAttribute("target", sNewId);
					}
					if (ANode->HasAttribute("initial")) {
						const UnicodeString sNewId = sResolveID + "." + ANode->GetAttribute("initial");
						ANode->SetAttribute("initial", sNewId);
					}
				}

				CheckDuplicatedId(ANode, AStateIdSet);
				for (int i = 0; i < ANode->ChildNodes->Count; i++) {
					IterateCombineNodes(ANode->ChildNodes->Nodes[i], AStateIdSet, sPath, sResolveID);
				}

				if (ANode->NodeName == "state" || ANode->NodeName == "initial" || ANode->NodeName == "parallel" || ANode->NodeName ==
					"final") {
					if (ANode->HasAttribute("src")) {
						InsertIncludeSCXML(ANode, ANode->GetAttribute("src"), icSRC, AStateIdSet, sPath, sResolveID);
					}
				}
			}
		}
	}

	//---------------------------------------------------------------------------
	void CombineSCXML(UnicodeString sSourceName, TStream *AOutStream) {
		if (!TPath::IsPathRooted(sSourceName)) {
			sSourceName = TPath::Combine(GetCurrentDir(), sSourceName);
		}
		if (!FileExists(sSourceName))
			throw Exception(L"Source file <" + sSourceName + L"> is not found!!!");

		std::set<UnicodeString>AStateIdSet;

		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(Application));
		AXMLDocument->LoadFromFile(sSourceName);

		IterateCombineNodes(AXMLDocument->DocumentElement, AStateIdSet, TPath::GetDirectoryName(sSourceName));

		AXMLDocument->SaveToStream(AOutStream);
	}

}

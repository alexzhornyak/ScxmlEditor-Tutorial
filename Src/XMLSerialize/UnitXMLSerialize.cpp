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

#include <memory>

#include "UnitXMLSerializeLoad.h"
#include "UnitXMLSerializeSave.h"

#include "UnitXMLSerialize.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Xmlserialize {

	//---------------------------------------------------------------------------
	void LoadComponentFromXMLDocument(TXMLDocument *AXMLDocument, TComponent *AComponent, ON_ERROR_GET_FUNC OnErrorGet/* = 0*/) {
		if (AXMLDocument->DocumentElement) {
			_di_IXMLNode ARootObjectNode = AXMLDocument->DocumentElement->ChildNodes->FindNode(XML_NODE_OBJECT_NAME);
			ComponentFromXML(ARootObjectNode, AComponent, Application, OnErrorGet);
		}
	}

	//---------------------------------------------------------------------------
	void LoadComponentFromStreamXML(TComponent *AComponent, TStream *AStream,
		Xmlintf::TXMLEncodingType EncodingType/* = (Xmlintf::TXMLEncodingType)(0x0)*/, ON_ERROR_GET_FUNC OnErrorGet/* = 0*/) {

		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(Application));
		AXMLDocument->LoadFromStream(AStream);
		LoadComponentFromXMLDocument(AXMLDocument.get(), AComponent, OnErrorGet);
	}

	//---------------------------------------------------------------------------
	void LoadComponentFromXML(TComponent *AComponent, const UnicodeString &sFileName, ON_ERROR_GET_FUNC OnErrorGet/* = 0*/) {
		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(Application));
		AXMLDocument->LoadFromFile(sFileName);
		LoadComponentFromXMLDocument(AXMLDocument.get(), AComponent, OnErrorGet);
	}

	//---------------------------------------------------------------------------
	void SaveComponentToXMLDocument(TXMLDocument *AXMLDocument, TComponent *AComponent, ON_ERROR_GET_FUNC OnErrorGet) {

		_di_IXMLNode ACommentNode = AXMLDocument->CreateNode("This document is generated automatically! Please, do not edit!", ntComment);

		AXMLDocument->ChildNodes->Add(ACommentNode);

		_di_IXMLNode ARootNode = AXMLDocument->AddChild("root");

		_di_IXMLNode AOwnerNode = XMLSerializeComponent(AComponent, ARootNode);

		for (int i = 0; i < AComponent->ComponentCount; i++) {
			_di_IXMLNode AComponentChildNode = XMLSerializeComponent(AComponent->Components[i], AOwnerNode);
			XMLSerializeObject(AComponent->Components[i], AComponentChildNode);
		}

		XMLSerializeObject(AComponent, AOwnerNode);
	}

	//---------------------------------------------------------------------------
	void SaveComponentToStreamXML(TComponent * AComponent, const UnicodeString & sFileName, TStream * AStream,
		ON_ERROR_GET_FUNC OnErrorGet/* = 0*/) {

		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(Application));
		AXMLDocument->Active = true;
		SaveComponentToXMLDocument(AXMLDocument.get(), AComponent, OnErrorGet);

		AXMLDocument->SaveToStream(AStream);
	}

	//---------------------------------------------------------------------------
	void SaveComponentToXML(TComponent *AComponent, const UnicodeString &sFileName, const UnicodeString & sEncoding,
		ON_ERROR_GET_FUNC OnErrorGet/* = 0*/) {

		std::auto_ptr<TXMLDocument>AXMLDocument(new TXMLDocument(Application));
		AXMLDocument->Active = true;
		SaveComponentToXMLDocument(AXMLDocument.get(), AComponent, OnErrorGet);

		AXMLDocument->XML->Text = FormatXMLData(AXMLDocument->XML->Text);
		AXMLDocument->Active = true;
		AXMLDocument->Encoding = sEncoding;
		AXMLDocument->SaveToFile(sFileName);
	}

};

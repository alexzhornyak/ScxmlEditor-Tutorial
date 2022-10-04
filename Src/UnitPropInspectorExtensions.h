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

#ifndef UnitPropInspectorExtensionsH
#define UnitPropInspectorExtensionsH
//---------------------------------------------------------------------------

#include <LMDInsPropEditors.hpp>
#include <LMDInsPropInsp.hpp>
#include <LMDDsgPropInsp.hpp>

namespace Propinspext {

	typedef bool __fastcall(__closure * TSettingsFilterPropEvent)(const UnicodeString & sPropName);
	#define ON_FILTER_PROP_EVENT	 	"OnFilterPropEvent"

	typedef void __fastcall(__closure * TSettingsOnGetPropEditorClass)(TPersistent *AInstance, Lmdinspropinsp::ILMDProperty *APropInfo,Lmdinspropinsp::TLMDPropEditorClass &AEditorClass);
	#define ON_GET_PROP_EDITOR_CLASS	"OnGetPropEditorClass"

	typedef void __fastcall(__closure * TSettingsOnGetCaptionColor)(TObject *Sender, Lmdinspropinsp::TLMDPropertyInspectorItem *AItem, TColor &AColor);
	#define ON_GET_PROP_CAPTION_COLOR	"OnGetPropCaptionColor"

	typedef UnicodeString __fastcall(__closure * TSettingsOnGetHTMLPropertyInfo)(const UnicodeString &sPropName);
	#define ON_GET_HTML_PROP_INFO		"OnGetHTMLPropertyInfo"

	/* by default: 'true' */
	void __fastcall IsPropIncluded(TObject *AInstance, const UnicodeString &sPropName, bool &AIncludeProp);

	void __fastcall PropSettingsInspectorFilterProp(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, bool AIsSubProp, bool &AIncludeProp);
	void __fastcall PropSettingsInspectorGetEditorClass(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass);
	void __fastcall PropSettingsInspectorGetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor);
	UnicodeString __fastcall PropSettingsInspectorGetHTMLInfo(TObject *Sender, TObject *AInstance, const UnicodeString &sPropName);

	bool IsWritablePropertyOfPropEditor(TLMDPropEditor *AEditor);

	class TSafeEnumPropEditor : public Lmdinspropeditors::TLMDEnumPropEditor {
	protected:
		virtual void __fastcall SetValue(const Lmdtypes::TLMDString Value);
	};

	class TSafeSetElemPropEditor : public TLMDSetElemPropEditor {
	protected:
		virtual void __fastcall SetValue(const Lmdtypes::TLMDString Value);
	public:
		__fastcall TSafeSetElemPropEditor(Lmdinspropinsp::TLMDPropEditor* AParent, int AElement):TLMDSetElemPropEditor(AParent,AElement) {}
	};

	class TSafeSetPropEditor : public Lmdinspropeditors::TLMDSetPropEditor {
	protected:

		virtual void __fastcall SetValue(const Lmdtypes::TLMDString Value);

		virtual void __fastcall GetSubProps(Classes::TList* AResult);
	};

};

namespace Lmddsgpropinsp {

	class TSettingsPropertyInspector: public TLMDPropertyInspector {
	public:
		__fastcall TSettingsPropertyInspector(TComponent *AOwner):TLMDPropertyInspector(AOwner) {
			this->OnFilterProp = PropSettingsInspectorFilterProp;
			this->OnGetCaptionColor = PropSettingsInspectorGetCaptionColor;
			this->OnGetEditorClass = PropSettingsInspectorGetEditorClass;
		}

		void __fastcall PropSettingsInspectorFilterProp(TObject *Sender,
			TPersistent *AInstance, ILMDProperty *APropInfo, bool AIsSubProp, bool &AIncludeProp) {
			Propinspext::PropSettingsInspectorFilterProp(Sender, AInstance, APropInfo, AIsSubProp, AIncludeProp);
		}
		void __fastcall PropSettingsInspectorGetEditorClass(TObject *Sender,
			TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass) {
			Propinspext::PropSettingsInspectorGetEditorClass(Sender, AInstance, APropInfo, AEditorClass);
		}

		void __fastcall PropSettingsInspectorGetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor) {
			Propinspext::PropSettingsInspectorGetCaptionColor(Sender, AItem, AColor);
		}
	};

};

#endif

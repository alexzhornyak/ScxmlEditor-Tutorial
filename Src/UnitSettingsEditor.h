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

#ifndef UnitSettingsEditorH
#define UnitSettingsEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDDsgObjects.hpp"
#include "LMDDsgPropInsp.hpp"
#include "LMDInsPropInsp.hpp"
#include "LMDInsPropPage.hpp"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class PACKAGE TFormSettingsEditor : public TForm
{
__published:	// IDE-managed Components
	TLMDDesignObjects *LMDDesignObjects1;
	TPanel *Panel5;
	TLabel *LabelCaption;
	TSpeedButton *BtnArrangeType;
	TSpeedButton *SpBtnExpand;
	TSpeedButton *SpBtnCollapse;
	TLMDPropertyInspector *PropSettingsInspector;
	TPanel *Panel1;
	TButton *BtnCancel;
	TButton *BtnOk;
	TImageList *ImageList1;
	TButton *BtnOpen;
	TButton *BtnSave;
	TActionList *ActionList1;
	TFileOpen *FileOpen1;
	TFileSaveAs *FileSaveAs1;
	TButton *BtnImport;
	TFileOpen *FileImport;
	TAction *ActionResetToDefault;
	TButton *BtnResetToDefault;
	TPopupMenu *PopupMenu1;
	TMenuItem *Resetselectedtodefault1;
	TAction *ActionResetSelectedToDefault;
	TMenuItem *N1;
	void __fastcall SpBtnCollapseClick(TObject *Sender);
	void __fastcall SpBtnExpandClick(TObject *Sender);
	void __fastcall BtnArrangeTypeClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall PropSettingsInspectorChange(TObject *Sender);
	void __fastcall BtnOkClick(TObject *Sender);
	void __fastcall FileOpen1Accept(TObject *Sender);
	void __fastcall FileSaveAs1Accept(TObject *Sender);
	void __fastcall FileImportAccept(TObject *Sender);
	void __fastcall ActionResetToDefaultExecute(TObject *Sender);
	void __fastcall ActionResetSelectedToDefaultExecute(TObject *Sender);
	void __fastcall PropSettingsInspectorFilterProp(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, bool AIsSubProp, bool &AIncludeProp);
	void __fastcall PropSettingsInspectorGetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor);
	void __fastcall PropSettingsInspectorGetEditorClass(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass);

private:	// User declarations

	TPersistent *FPersistent;

    bool m_b_modified;

public:		// User declarations

	__fastcall TFormSettingsEditor(TComponent* Owner, TPersistent *APersistent);
	virtual __fastcall ~TFormSettingsEditor();

    void __fastcall SelectItemOrRestoreState(const UnicodeString &sItemName, TLMDPropPageStateObject *APropStateObject);

};
//---------------------------------------------------------------------------
#endif

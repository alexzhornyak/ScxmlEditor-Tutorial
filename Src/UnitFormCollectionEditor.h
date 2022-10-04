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

#ifndef UnitFormCollectionEditorH
#define UnitFormCollectionEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDDsgObjects.hpp"
#include "LMDDsgPropInsp.hpp"
#include "LMDInsPropInsp.hpp"
#include "LMDInsPropPage.hpp"
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "LMDInsPropEditors.hpp"
#include <ImgList.hpp>
#include <ActnList.hpp>
#include <Menus.hpp>
#include <StdActns.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class PACKAGE TFormCollectionEditor : public TForm
{
__published:	// IDE-managed Components
	TLMDDesignObjects *LMDDesignObjects1;
	TPanel *Panel1;
	TButton *BtnCancel;
	TButton *BtnOk;
	TSplitter *Splitter1;
	TButton *BtnAdd;
	TButton *BtnDelete;
	TImageList *ImageList1;
	TSplitter *Splitter2;
	TPanel *Panel2;
	TListBox *ListBox1;
	TSplitter *Splitter3;
	TLMDPropertyInspector *PropInspectorCollection;
	TSplitter *Splitter4;
	TButton *BtnUp;
	TButton *BtnDown;
	TSplitter *Splitter5;
	TButton *BtnCopy;
	TButton *BtnPaste;
	TActionList *ActionList1;
	TAction *ActionAdd;
	TAction *ActionDelete;
	TAction *ActionUp;
	TAction *ActionDown;
	TAction *ActionCopy;
	TAction *ActionPaste;
	TPopupMenu *PopupMenu1;
	TMenuItem *ActionAdd1;
	TMenuItem *ActionDelete1;
	TMenuItem *N1;
	TMenuItem *ActionUp1;
	TMenuItem *ActionDown1;
	TMenuItem *ActionCopy1;
	TMenuItem *ActionPaste1;
	TSplitter *Splitter6;
	TFileOpen *FileOpen1;
	TFileSaveAs *FileSaveAs1;
	TMenuItem *N3;
	TMenuItem *Open1;
	TMenuItem *SaveAs1;
	TButton *BtnLoad;
	TButton *BtnSave;
	TMenuItem *MenuPresets;
	TMenuItem *N4;
	TMenuItem *N2;
	TPanel *Panel3;
	TLMDPropertyInspector *PropertyInspector1;
	TPanel *Panel5;
	TSpeedButton *BtnArrangeType;
	TSpeedButton *SpBtnExpand;
	TSpeedButton *SpBtnCollapse;
	TSpeedButton *SpBtnAutoExpand;
	TAction *ActionReplaceValues;
	TMenuItem *N5;
	TMenuItem *ActionReplaceValues1;
	TReplaceDialog *ReplaceDialog1;
	TLMDDesignObjects *DesignObjectsCollection;
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall PropertyInspector1Change(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall PropInspectorCollectionChange(TObject *Sender);
	void __fastcall BtnOkClick(TObject *Sender);
	void __fastcall ListBox1DrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall ActionAddExecute(TObject *Sender);
	void __fastcall ActionDeleteExecute(TObject *Sender);
	void __fastcall ActionUpExecute(TObject *Sender);
	void __fastcall ActionDownExecute(TObject *Sender);
	void __fastcall ActionCopyExecute(TObject *Sender);
	void __fastcall ActionPasteExecute(TObject *Sender);
	void __fastcall FileSaveAs1Accept(TObject *Sender);
	void __fastcall FileOpen1Accept(TObject *Sender);
	void __fastcall FileOpen1BeforeExecute(TObject *Sender);
	void __fastcall FileSaveAs1BeforeExecute(TObject *Sender);
	void __fastcall PopupMenu1Popup(TObject *Sender);
	void __fastcall SpBtnCollapseClick(TObject *Sender);
	void __fastcall SpBtnExpandClick(TObject *Sender);
	void __fastcall BtnArrangeTypeClick(TObject *Sender);
	void __fastcall ActionReplaceValuesExecute(TObject *Sender);
	void __fastcall PropertyInspector1FilterProp(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, bool AIsSubProp, bool &AIncludeProp);
	void __fastcall PropertyInspector1GetCaptionColor(TObject *Sender, TLMDPropertyInspectorItem *AItem, TColor &AColor);
	void __fastcall PropertyInspector1GetEditorClass(TObject *Sender, TPersistent *AInstance, ILMDProperty *APropInfo, TLMDPropEditorClass &AEditorClass);




private:	// User declarations

    void __fastcall UpdateDisplayName();

	bool m_b_modified;

	// нельз€ перегружать, так как используетс€ в конструкторе
	// также эта форма модальна€, поэтому дл€ потомков просто в конструкторе примен€йте €зыковые свойства
	void OnLanguageChanged();

    TLMDPropPageStateObject *FPropStageObject;

    UnicodeString FFormCaption;

protected:

	TOwnedCollection *FSourceCollection;
    TOwnedCollection *FOwnedCollection;

	// эту функцию надо будет переопределить, если понадоб€тс€ специальные настройки коллекции
	virtual void __fastcall CloneSourceCollection();

	virtual void __fastcall OnPresetItemClick(TObject *ASender);

	virtual void __fastcall SetModified(bool bVal);

	bool IsActiveItemWritableString(void);

    virtual void __fastcall OnStringPropReplace(TObject *ASender);

public:		// User declarations
	__fastcall TFormCollectionEditor(TComponent* Owner, TOwnedCollection *ASourceCollection);
	virtual __fastcall ~TFormCollectionEditor();

	__property TOwnedCollection *OwnedCollection = {read=FOwnedCollection};

	void __fastcall FillListBox();

	void __fastcall MarkModified() { SetModified(true); }
};

class TOwnedCollectionPropEditor: public TLMDClassPropEditor {
protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);

	// должна создатьс€ форма с редактором
	virtual TFormCollectionEditor *__fastcall DoCreateCollectionEditor(TOwnedCollection *ACollection);

public:
	virtual void __fastcall Edit(void);
};

//---------------------------------------------------------------------------
#endif

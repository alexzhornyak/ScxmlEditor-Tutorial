//---------------------------------------------------------------------------

#ifndef UnitLuaEditorH
#define UnitLuaEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDSedView.hpp"
#include <ExtCtrls.hpp>
#include "LMDSedDocument.hpp"
#include <ActnList.hpp>
#include <StdActns.hpp>
#include <ImgList.hpp>
#include "LMDInsPropEditors.hpp"
#include <memory>
//---------------------------------------------------------------------------

class TFrameSyntaxEditor;

class PACKAGE TFormLuaEditor : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TButton *BtnCancel;
	TButton *BtnOk;
	TActionList *ActionList1;
	TFileOpen *FileOpen1;
	TFileSaveAs *FileSaveAs1;
	TButton *BtnOpen;
	TButton *BtnSave;
	TImageList *ImageList1;
	TLMDEditDocument *EditDocumentLua;
	void __fastcall FileOpen1Accept(TObject *Sender);
	void __fastcall FileSaveAs1Accept(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall BtnOkClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	TFrameSyntaxEditor *FSyntaxEditor;

    std::auto_ptr<TStringList> FCompleteTable;

	void __fastcall OnFillCompletionListEvent(System::TObject* Sender, Classes::TStrings* Items, const Types::TPoint &CursorPoint,
					TLMDEditFillCompletionCause Cause, const Lmdtypes::TLMDString LeftPart, Types::TPoint &PosForCombo, bool &Handled);
	void __fastcall EditViewCompletionReplace(System::TObject* Sender, Classes::TStrings* Items, int ItemIndex,
					const Types::TPoint &CursorPoint, TLMDEditReplaceCompletionCause Cause, bool &Handled);

public:		// User declarations
	__fastcall TFormLuaEditor(TComponent* Owner);

	virtual void OnLanguageChanged();
};

// возможность редактирования строк только у одного объекта
class PACKAGE TLuaStringsEditorEx : public TLMDClassPropEditor {
private:

protected:
	virtual TLMDPropAttrs __fastcall GetAttrs();
	virtual Lmdtypes::TLMDString __fastcall GetValue(void);
public:
	virtual void __fastcall Edit(void);
};


//---------------------------------------------------------------------------
#endif

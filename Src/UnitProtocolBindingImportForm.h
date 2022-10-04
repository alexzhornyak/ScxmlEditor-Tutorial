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

#ifndef UnitProtocolBindingImportFormH
#define UnitProtocolBindingImportFormH
// ---------------------------------------------------------------------------

// без этого будет ругаться на TTreeConnection->DrawTextA
#undef DrawText // exclude Winapi DrawText

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "LMDSedDocument.hpp"
#include "LMDSedView.hpp"
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include "TeeProcs.hpp"
#include "TeeTree.hpp"
#include <Menus.hpp>
#include "XMLCalibrator.h"
#include "XMLCanserviceControls.h"
#include "XMLModel.h"
#include "CommonConsts.h"

class TBindingImportingShape : public TTreeNodeShape {
	UnicodeString 		FAviaName;
	Integer 			FSignal;
	UnicodeString 		FType;
	UnicodeString 		FParam;
	UnicodeString 		FChannel;
	Integer 			FOffset;
	double 				FMin;
	double 				FMax;
	Integer 			FLength;
	Integer 			FHwMin;
	Integer 			FHwMax;
	Integer 			FSbit;
	UnicodeString 		FCalibration;

	Integer 			Fnumber;
	UnicodeString 		Fdiscription;
public:
	__fastcall TBindingImportingShape(TComponent* Owner);

	virtual void __fastcall Assign(Classes::TPersistent* Source);

	void __fastcall AssignFromCalibratorElement(Calibrator::IXMLcontrolType* element);
	void __fastcall AssignFromCanserviceElement(Canservice::IXMLitemType *element);
	void __fastcall AssignFromModelElement(Model::IXMLitemType *element);

__published:
  // TSCanService config
  __property UnicodeString AviaName = { read=FAviaName, write=FAviaName };
  __property Integer Signal = { read=FSignal, write=FSignal, default=0 };
  __property UnicodeString Type = { read=FType, write=FType };
  __property UnicodeString Param = { read=FParam, write=FParam };
  __property UnicodeString Channel = {read=FChannel, write=FChannel };
  __property Integer Offset = { read=FOffset, write=FOffset, default=0 };
  __property double Min = { read=FMin, write=FMin, default=0 };
  __property double Max = { read=FMax, write=FMax, default=0 };
  __property Integer Length = { read=FLength, write=FLength };
  __property Integer HwMin = { read=FHwMin, write=FHwMin, default=0 };
  __property Integer HwMax = { read=FHwMax, write=FHwMax, default=0 };
  __property Integer Sbit = { read=FSbit, write=FSbit };
  __property UnicodeString Calibration = { read=FCalibration, write=FCalibration };
  // FCalibrator
  __property Integer number = { read=Fnumber, write=Fnumber, default=0 };
  __property UnicodeString discription = { read=Fdiscription, write=Fdiscription };


};

// ---------------------------------------------------------------------------
class TBindingsImporter : public TForm {
__published: // IDE-managed Components
	TPanel *Panel1;
	TButton *Button2;
	TButton *Button1;
	TTabControl *TabControl1;
	TLMDEditView *EditView;
	TLMDEditDocument *EditDocCalibrator;
	TButton *BtnValidateCanservice;
	TSplitter *Splitter1;
	TImageList *StateImages;
	TPopupMenu *PopupMenu1;
	TMenuItem *MenuCheck;
	TMenuItem *MenuUncheck;
	TLMDEditDocument *EditDocCanservice;
	TPanel *Panel2;
	TTree *Tree1;
	TEdit *Edit1;
	TMenuItem *MenuCheckAll;
	TMenuItem *Uncheckall1;
	TButton *BtnValidateModel;
	TLMDEditDocument *EditDocModel;
	TCheckBox *CheckIgnoreCanSuffix;
	TCheckBox *CheckInput;
	TCheckBox *CheckOutput;

	void __fastcall BtnValidateCanserviceClick(TObject *Sender);
	void __fastcall MenuCheckClick(TObject *Sender);
	void __fastcall MenuUncheckClick(TObject *Sender);
	void __fastcall TabControl1Change(TObject *Sender);
	void __fastcall Edit1Change(TObject *Sender);
	void __fastcall MenuCheckAllClick(TObject *Sender);
	void __fastcall Uncheckall1Click(TObject *Sender);
	void __fastcall BtnValidateModelClick(TObject *Sender);

private: // User declarations
	Calibrator::_di_IXMLdevConfigType 	FDevConfig;
	Canservice::_di_IXMLConfigType 		FCanserviceConfig;
	Model::_di_IXMLConfigType			FModelConfig;

	TCustomTree *FTreeBase;

	Canservice::IXMLitemType *FindCanserviceControl(const UnicodeString &sName);

	// если присутствует и калибратора конфиг, и кансервиса
	void ValidateBothCanFiles();

	// если только кана присутствует
	void ValidateCanservice();

protected:

	void __fastcall OnPostEditChange(TMessage &msg);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_SCXML_POST_EDIT_CHANGE, TMessage, OnPostEditChange)
	END_MESSAGE_MAP(TForm)

public: // User declarations
	__fastcall TBindingsImporter(TComponent* Owner);
	__fastcall ~TBindingsImporter();
};

// ---------------------------------------------------------------------------
extern PACKAGE TBindingsImporter *BindingsImporter;
// ---------------------------------------------------------------------------
#endif

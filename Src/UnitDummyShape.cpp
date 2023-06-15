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

#include "UnitDummyShape.h"

#include "Log4cpp_VCL.hpp"

#include "UnitStateMachineConnection.h"
#include "UnitSettings.h"
#include "KeyboardUtils.h"
#include "UnitFrameTrigger.h"
#include "VISUAL_GLOBALS.h"
#include "UnitCreateTriggers.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall TTextDummyShape::TTextDummyShape(Classes::TComponent* AOwner) : TPolygonShape(AOwner), Connection(NULL) {
	this->Text->Visible = false;

	this->AutoSize = false;
	this->Border->Visible = true;

	this->Transparent = true;

	if (SettingsData->TestingSendEventClickMode == seClick) {
		this->OnClick = OnClickSendEvent;
	}
	else {
		this->OnDblClick = OnClickSendEvent;
	}
}

//---------------------------------------------------------------------------
void __fastcall TTextDummyShape::UpdateAppearance(void) {
	if (TTextDummyShape::IsClickSendEnabled()) {
		if (this->Border->Color != clRed) {
			this->Border->Color = clRed;
		}
		if (this->Border->Style != psSolid) {
			this->Border->Style = psSolid;
		}

		const bool bIsLeftMousePressed = Keyboardutils::IsLeftMousePressed();

		const int iBorderWidth = bIsLeftMousePressed ? 1 : 2;

		if (this->Border->Width != iBorderWidth) {
			this->Border->Width = iBorderWidth;
		}

	}
	else {
		if (this->Border->Color != SettingsData->ThemeSettings->ChildNormalBorderColor) {
			this->Border->Color = SettingsData->ThemeSettings->ChildNormalBorderColor;
		}
		if (this->Border->Style != psDot) {
			this->Border->Style = psDot;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TTextDummyShape::DrawShapeCanvas(Tecanvas::TCanvas3D* ACanvas, const Types::TRect &R) {
	this->UpdateAppearance();

	TPolygonShape::DrawShapeCanvas(ACanvas, R);
}

//---------------------------------------------------------------------------
TStateMachineConnection *__fastcall TTextDummyShape::GetStateMachineConnection(void) {
	return dynamic_cast<TStateMachineConnection*>(Connection);
}

//---------------------------------------------------------------------------
void __fastcall TTextDummyShape::UpdatePosition(void) {
	if (StateMachineConnection) {
		StateMachineConnection->AssignTextPolygonToPoints(this->Points);
	}
}

//---------------------------------------------------------------------------
void __fastcall TTextDummyShape::DrawHandles(void) {
	if (IsClickSendEnabled()) {
		/* disabled in Debug mode */
	}
	else {
		TPolygonShape::DrawHandles();
	}
}

//---------------------------------------------------------------------------
void __fastcall TTextDummyShape::OnClickSendEvent(TTreeNodeShape* Sender, Controls::TMouseButton Button, Classes::TShiftState Shift, int X,
	int Y) {

	if (!TTextDummyShape::IsClickSendEnabled())
		return;

	if (Button != mbLeft)
		return;

	try {
		bool bIsSent = false;
		UnicodeString sEvent = L"";

		if (StateMachineConnection && !StateMachineConnection->Event.IsEmpty()) {

			TWinControl *ATriggersPanel = GlobalScrollBoxTriggers();

			// 1) Exact Search
			TFrameTrigger *AFrameTrigger = FindTrigger(ATriggersPanel, StateMachineConnection->ContentTrigger,
				StateMachineConnection->Params);

			if (!AFrameTrigger) {
				// 2) Search by Event name
				for (int i = 0; i < ATriggersPanel->ControlCount; i++) {
					TFrameTrigger * ACurrentFrame = dynamic_cast<TFrameTrigger*>(ATriggersPanel->Controls[i]);
					if (ACurrentFrame && ACurrentFrame->EventName == StateMachineConnection->Event) {
						switch(ACurrentFrame->OriginalContentTrigger->Appearance) {
						case ctaButton:
						case ctaCheckBox:
						case ctaEdit: {
								AFrameTrigger = ACurrentFrame;
							}break;
						}

						if (AFrameTrigger) {
							break;
						}
					}
				}
			}

			/* if we found Trigger then send event by its content */
			if (AFrameTrigger) {
				if (AFrameTrigger->SendUniversalEvent()) {
					sEvent = AFrameTrigger->EventName;
					bIsSent = true;
				}
			}
			else {
				/* Make a ContentTrigger Proxy */
				std::auto_ptr<TContentTrigger>AContentTriggerPtr(new TContentTrigger(this));
				AContentTriggerPtr->Assign(StateMachineConnection->ContentTrigger);
				if (AContentTriggerPtr->Event.IsEmpty()) {
					AContentTriggerPtr->Event = StateMachineConnection->Event;
				}
				AContentTriggerPtr->Enabled = true;

				/* Params are ignored */
				std::auto_ptr<TParamTriggerItems>ADummyParamsPtr(new TParamTriggerItems(this));

				std::auto_ptr<TFrameTrigger>AFrameTriggerPtr(new TFrameTrigger(this, AContentTriggerPtr.get(), ADummyParamsPtr.get(),
						AContentTriggerPtr->Event));
				if (AFrameTriggerPtr->SendUniversalEvent()) {
					sEvent = AContentTriggerPtr->Event;
					bIsSent = true;
				}
			}
		}

		if (!bIsSent) {
			WLOG_WARNING(L"Event:[%s] is not adjusted for sending! Check 'ContentTrigger' settings!", sEvent.c_str());
		}
	}
	catch(Exception * E) {
		WLOG_ERROR(L"TRIGGER> %s", E->Message.c_str());
	}

}

//---------------------------------------------------------------------------
bool TTextDummyShape::IsClickSendEnabled(void) {
	return SettingsData && SettingsData->IsTesterWorkingInDebugMode() && SettingsData->TestingSendEventClickMode != seNone;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TTextDummyShape *__fastcall CreateDummyShape(TComponent *AOwner, TCustomTree *ATree, TStateMachineConnection *AStateMachineConnection) {

	ATree->Selected->Clear();

	TTextDummyShape *ATextShape = new TTextDummyShape(AOwner);
	ATextShape->Connection = AStateMachineConnection;
	const Unique::TUniquePair AUniqueName = Unique::GetUniqueName(AOwner, __classid(TTextDummyShape));
	ATextShape->Name = AUniqueName.second;

	AStateMachineConnection->AssignTextPolygonToPoints(ATextShape->Points);

	ATextShape->Tree = ATree;

	ATextShape->Selected = true;

	return ATextShape;
}

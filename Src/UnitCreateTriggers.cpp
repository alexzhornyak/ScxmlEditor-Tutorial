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

#include "UnitCreateTriggers.h"

#include <memory>

#include "UnitFrameTrigger.h"
#include "Log4cpp_VCL.hpp"
#include "UnitCustomUtils.h"
#include "UnitUniqueUtils.h"
#include "UnitStateMachineConnection.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
//------------------------------ CreateTrigger -------------------------------
//---------------------------------------------------------------------------

TFrameTrigger *__fastcall FindTrigger(TWinControl *AParent, TContentTrigger *AContentTrigger, TParamTriggerItems *AParams) {
	if (!AParent || !AContentTrigger || !AParams)
		return NULL;

	if (!AContentTrigger->Event.IsEmpty() && AContentTrigger->Enabled) {

		/* проверяем, что ещё нет триггера с такими же параметрами */
		for (int i = 0; i < AParent->ControlCount; i++) {
			TFrameTrigger * ACurrentFrame = dynamic_cast<TFrameTrigger*>(AParent->Controls[i]);
			if (ACurrentFrame) {
				std::auto_ptr<TStringList>ASkippedPtr(new TStringList());

				Unique::CompareError ACompareError;
				const bool bSameContent = Unique::CompareObjects(ACurrentFrame->OriginalContentTrigger, AContentTrigger, ASkippedPtr.get(),
					L"", ACompareError, true);

				if (bSameContent) {
					// если оба набора в Disable, то нет необходимости дублировать такой же триггер
					const bool bBothParamsDisabled = !ACurrentFrame->OriginalParams->Enabled && !AParams->Enabled;
					if (bBothParamsDisabled) {
						return ACurrentFrame;
					}

					const bool bSameParams = Unique::CompareObjects(ACurrentFrame->OriginalParams, AParams, ASkippedPtr.get(), L"",
						ACompareError, true);

					// уже есть с точно такими параметрами
					if (bSameParams) {
						return ACurrentFrame;
					}
				}
			}
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
TFrameTrigger * __fastcall CreateTrigger(TWinControl *AParent, TContentTrigger *AContentTrigger, TParamTriggerItems *AParams) {
	if (!AParent || !AContentTrigger || !AParams)
		return NULL;

	if (!AContentTrigger->Event.IsEmpty() && AContentTrigger->Enabled) {

		TFrameTrigger * ACurrentFrame = FindTrigger(AParent, AContentTrigger, AParams);
		if (ACurrentFrame) {
			/* No need to create trigger with the same parameters */
			return NULL;
		}

		/* если дошли сюда, значит триггер будет уникальным */
		TFrameTrigger *AFrameTrigger = new TFrameTrigger(AParent, AContentTrigger, AParams, AContentTrigger->Event);
		AFrameTrigger->Name = TeeGetUniqueName(AParent, L"FrameTrigger");

		AFrameTrigger->AutoSize = true;
		AFrameTrigger->Align = alTop;

		AFrameTrigger->Top = Customutils::GetWinControlAlignedChildrenHeight(AParent);

		AFrameTrigger->Parent = AParent;

		return AFrameTrigger;
	}

	return NULL;
}

// ---------------------------------------------------------------------------

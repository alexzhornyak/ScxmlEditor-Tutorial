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

#include "UnitPropertyConfiguratorEx.h"

#include <log4cpp/PropertyConfiguratorImpl.hh>

#include "UnitBallonAppender.h"
#include "UnitListBoxAppender.h"
#include "UnitEditDocumentAppender.h"
#include "TrayIconErrorPriority.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace log4cpp {

	//---------------------------------------------------------------------------
	class PropertyConfiguratorImplEx : public PropertyConfiguratorImpl {
		TStrings *FVCLAppenders;

	public:
		PropertyConfiguratorImplEx(TStrings *AVCLAppenders) : PropertyConfiguratorImpl(), FVCLAppenders(AVCLAppenders) {
		}

		virtual Appender *createAppenderFromProp(const std::string& appenderName, const std::string &appenderType,
			const std::string &appenderPrefix) {
			Appender* appender = NULL;
			// and instantiate the appropriate object
			const int iIndex = FVCLAppenders->IndexOf(appenderType.c_str());
			if (appenderType == "BalloonAppender" && iIndex != -1) {
				appender = new BalloonAppender(appenderName, dynamic_cast<TTrayIconErrorPriority*>(FVCLAppenders->Objects[iIndex]));
			}
			else if (appenderType == "ListboxAppender" && iIndex != -1) {
				appender = new ListboxAppender(appenderName, dynamic_cast<TListBox*>(FVCLAppenders->Objects[iIndex]));
			}
			else if (appenderType == "EditDocumentAppender" && iIndex != -1) {
				appender = new EditDocumentAppender(appenderName, dynamic_cast<TLMDEditDocument*>(FVCLAppenders->Objects[iIndex]));
			}
			else {
				appender = PropertyConfiguratorImpl::createAppenderFromProp(appenderName, appenderType, appenderPrefix);
			}
			return appender;
		}
	};

	//---------------------------------------------------------------------------
	void PropertyConfiguratorEx::configure(const std::string& initFileName, TStrings *AVCLAppenders)throw(ConfigureFailure) {
		PropertyConfiguratorImplEx configurator(AVCLAppenders);

		configurator.doConfigure(initFileName);
	}

	//---------------------------------------------------------------------------
	void PropertyConfiguratorEx::configure(std::istream& in, TStrings *AVCLAppenders)throw(ConfigureFailure) {
		PropertyConfiguratorImplEx configurator(AVCLAppenders);

		configurator.doConfigure(in);
	}
};

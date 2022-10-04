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

#include "WildCardUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace Wildcardutils {

	// ---------------------------------------------------------------------------
	int wildcmp_w(const wchar_t *wild, const wchar_t *string) {

		const wchar_t *cp = NULL, *mp = NULL;

		while ((*string) && (*wild != L'*')) {
			if ((*wild != *string) && (*wild != L'?')) {
				return 0;
			}
			wild++;
			string++;
		}

		while (*string) {
			if (*wild == L'*') {
				if (!*++wild) {
					return 1;
				}
				mp = wild;
				cp = string + 1;
			}
			else if ((*wild == *string) || (*wild == L'?')) {
				wild++;
				string++;
			}
			else {
				wild = mp;
				string = cp++;
			}
		}

		while (*wild == L'*') {
			wild++;
		}
		return !*wild;
	}

	// ---------------------------------------------------------------------------
	bool WildCardMatch(const UnicodeString &sWildCard, const UnicodeString &sText, bool CaseSensitive/* = true*/) {
		return wildcmp_w(CaseSensitive ? sWildCard.c_str() : sWildCard.LowerCase().c_str(),
			CaseSensitive ? sText.c_str() : sText.LowerCase().c_str());
	}

}

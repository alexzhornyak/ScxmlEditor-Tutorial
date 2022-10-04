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

#ifndef PersistentRectH
#define PersistentRectH
//---------------------------------------------------------------------------

#include "LMDInsPropEditors.hpp"

//---------------------------------------------------------------------------
class TPersistentRect: public TPersistent {
private:

	 TRect FRect;

	 int __fastcall GetLeft() { return FRect.Left; }
	 void __fastcall SetLeft(int val) { FRect.Left = val; }

	 int __fastcall GetTop() { return FRect.Top; }
	 void __fastcall SetTop(int val) { FRect.Top = val; }

	 int __fastcall GetRight() { return FRect.Right; }
	 void __fastcall SetRight(int val) { FRect.Right = val; }

	 int __fastcall GetBottom() { return FRect.Bottom; }
	 void __fastcall SetBottom(int val) { FRect.Bottom = val; }

	 int __fastcall GetWidth() { return FRect.Width(); }
	 int __fastcall GetHeight() { return FRect.Height(); }

public:

	__fastcall TPersistentRect();

	void __fastcall Assign(Classes::TPersistent * Source);

	bool __fastcall IsEmpty() const { return (FRect.Left == 0) && (FRect.Top == 0) && (FRect.Bottom == 0) && (FRect.Right == 0); }

    __property TRect Rect = {read=FRect, write=FRect};

__published:
	__property int Left = {read=GetLeft, write=SetLeft, default=0};
	__property int Top = {read=GetTop, write=SetTop, default=0};
	__property int Right = {read=GetRight, write=SetRight, default=0};
	__property int Bottom = {read=GetBottom, write=SetBottom, default=0};
	__property int Width = {read=GetWidth};
	__property int Height = {read=GetHeight};
};

class TPersistentRectEditor : public TLMDClassPropEditor {
protected:

	virtual Lmdtypes::TLMDString __fastcall GetValue(void);

};

#endif

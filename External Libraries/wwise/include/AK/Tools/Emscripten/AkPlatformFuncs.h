/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#include <emscripten.h>

#define AK_MSEC_TO_NANOSEC 1000000ULL
#define AK_SEC_TO_NANOSEC  1000000000ULL

namespace AKPLATFORM
{
    // Time functions
    // ------------------------------------------------------------------

	/// Platform Independent Helper
    inline void PerformanceCounter( AkInt64 * out_piLastTime )
	{
		// emscripten_get_now returns a double representing milliseconds with fractions. Convert to nanoseconds
		*out_piLastTime = (AkInt64)(emscripten_get_now() * AK_MSEC_TO_NANOSEC);
	}

	/// Platform Independent Helper
	inline void PerformanceFrequency( AkInt64 * out_piFreq )
	{
		*out_piFreq = AK_SEC_TO_NANOSEC;
	}

	template<class destType, class srcType>
	inline size_t AkSimpleConvertString( destType* in_pdDest, const srcType* in_pSrc, size_t in_MaxSize, size_t destStrLen(const destType *),  size_t srcStrLen(const srcType *) )
	{ 
		size_t i;
		size_t lenToCopy = srcStrLen(in_pSrc);
		
		lenToCopy = (lenToCopy > in_MaxSize-1) ? in_MaxSize-1 : lenToCopy;
		for(i = 0; i < lenToCopy; i++)
		{
			in_pdDest[i] = (destType) in_pSrc[i];
		}
		in_pdDest[lenToCopy] = (destType)0;
		
		return lenToCopy;
	}

	#define CONVERT_UTF16_TO_CHAR( _astring_, _charstring_ ) \
		_charstring_ = (char*)AkAlloca( (1 + AKPLATFORM::AkUtf16StrLen((const AkUtf16*)_astring_)) * sizeof(char) ); \
		AK_UTF16_TO_CHAR( _charstring_, (const AkUtf16*)_astring_, AKPLATFORM::AkUtf16StrLen((const AkUtf16*)_astring_)+1 ) 

	#define AK_UTF8_TO_OSCHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, strlen, strlen )
    #define AK_UTF16_TO_CHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, strlen, AKPLATFORM::AkUtf16StrLen )
    #define AK_UTF16_TO_OSCHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, strlen, AKPLATFORM::AkUtf16StrLen )
    #define AK_UTF16_TO_WCHAR(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, wcslen, AKPLATFORM::AkUtf16StrLen )
    #define AK_CHAR_TO_UTF16(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, AKPLATFORM::AkUtf16StrLen, strlen )
    #define AK_OSCHAR_TO_UTF16(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, AKPLATFORM::AkUtf16StrLen, strlen )
    #define AK_WCHAR_TO_UTF16(	in_pdDest, in_pSrc, in_MaxSize )	AKPLATFORM::AkSimpleConvertString( in_pdDest, in_pSrc, in_MaxSize, AKPLATFORM::AkUtf16StrLen, wcslen )

	/// Stack allocations.
	#define AkAlloca( _size_ ) __builtin_alloca( _size_ )
}

#include <AK/Tools/POSIX/AkPlatformFuncs.h>


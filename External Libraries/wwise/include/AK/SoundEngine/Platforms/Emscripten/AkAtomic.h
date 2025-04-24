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

// AkAtomic.h

#pragma once

#include <emscripten/threading.h>
#include <unistd.h>
#include <AK/SoundEngine/Common/AkAtomicTypes.h>

#define AkThreadYield() usleep(1)

inline void AkSpinHint()
{
	//No spin hint supported on this CPU	
}

#ifdef __cplusplus
extern "C" {
#endif

#define						AK_ATOMIC_FENCE_FULL_BARRIER()												emscripten_atomic_fence();

static inline	int32_t		AkAtomicLoad32( AkAtomic32* pSrc )											{ return ( int32_t )emscripten_atomic_load_u32( ( void* )pSrc ); }
static inline	void		AkAtomicStore32( AkAtomic32* pDest, int32_t value )							{ emscripten_atomic_exchange_u32( ( void* )pDest, ( uint32_t )value ); }
static inline	int32_t		AkAtomicInc32( AkAtomic32* pValue )											{ return ( int32_t )emscripten_atomic_add_u32( ( void* )pValue, 1 ) + 1; }
static inline	int32_t		AkAtomicDec32( AkAtomic32* pValue )											{ return ( int32_t )emscripten_atomic_sub_u32( ( void* )pValue, 1 ) - 1; }
static inline	int32_t		AkAtomicExchange32( AkAtomic32* pDest, int32_t value )						{ return ( int32_t )emscripten_atomic_exchange_u32( ( void* )pDest, ( uint32_t )value ); }
static inline	int32_t		AkAtomicAdd32( AkAtomic32* pDest, int32_t value )							{ return ( int32_t )emscripten_atomic_add_u32( ( void* )pDest, ( uint32_t )value ) + value; }
static inline	int32_t		AkAtomicSub32( AkAtomic32* pDest, int32_t value )							{ return ( int32_t )emscripten_atomic_sub_u32( ( void* )pDest, ( uint32_t )value ) - value; }
static inline	int32_t		AkAtomicAnd32( AkAtomic32* pDest, int32_t value )							{ return ( int32_t )emscripten_atomic_and_u32( ( void* )pDest, ( uint32_t )value ) & value; }
static inline	int32_t		AkAtomicOr32(  AkAtomic32* pDest, int32_t value )							{ return ( int32_t )emscripten_atomic_or_u32(  ( void* )pDest, ( uint32_t )value ) | value; }
static inline	int			AkAtomicCas32( AkAtomic32* pDest, int32_t proposed, int32_t expected )		{ return emscripten_atomic_cas_u32( ( void* )pDest, ( uint32_t )expected, ( uint32_t )proposed ) == ( uint32_t )expected ? 1 : 0; }

static inline	int64_t		AkAtomicLoad64( AkAtomic64* pSrc )											{ return ( int64_t )emscripten_atomic_load_u64( ( void* )pSrc ); }
static inline	void		AkAtomicStore64( AkAtomic64* pDest, int64_t value )							{ emscripten_atomic_exchange_u64( ( void* )pDest, ( uint64_t )value); }
static inline	int64_t		AkAtomicInc64( AkAtomic64* pValue )											{ return ( int64_t )emscripten_atomic_add_u64( ( void* )pValue, 1 ) + 1; }
static inline	int64_t		AkAtomicDec64( AkAtomic64* pValue )											{ return ( int64_t )emscripten_atomic_sub_u64( ( void* )pValue, 1 ) - 1; }
static inline	int64_t		AkAtomicExchange64( AkAtomic64* pDest, int64_t value )						{ return ( int64_t )emscripten_atomic_exchange_u64( ( void* )pDest, ( uint64_t )value ); }
static inline	int64_t		AkAtomicAdd64( AkAtomic64* pDest, int64_t value )							{ return ( int64_t )emscripten_atomic_add_u64( ( void* )pDest, ( uint64_t )value ) + value; }
static inline	int64_t		AkAtomicSub64( AkAtomic64* pDest, int64_t value )							{ return ( int64_t )emscripten_atomic_sub_u64( ( void* )pDest, ( uint64_t )value ) - value; }
static inline	int64_t		AkAtomicAnd64( AkAtomic64* pDest, int64_t value )							{ return ( int64_t )emscripten_atomic_and_u64( ( void* )pDest, ( uint64_t )value ) & value; }
static inline	int64_t		AkAtomicOr64(  AkAtomic64* pDest, int64_t value )							{ return ( int64_t )emscripten_atomic_or_u64(  ( void* )pDest, ( uint64_t )value ) | value; }
static inline	int			AkAtomicCas64( AkAtomic64* pDest, int64_t proposed, int64_t expected )		{ return emscripten_atomic_cas_u64( ( void* )pDest, ( uint64_t )expected, ( uint64_t )proposed ) == ( uint64_t )expected ? 1 : 0; }

static inline	void*		AkAtomicLoadPtr( AkAtomicPtr* pSrc )										{ return ( void* )emscripten_atomic_load_u32( ( void** )pSrc ); }
static inline	void		AkAtomicStorePtr( AkAtomicPtr* pDest, void* value )							{ emscripten_atomic_exchange_u32( ( void** )pDest, ( uint32_t )value ); }
static inline	void*		AkAtomicExchangePtr( AkAtomicPtr* pDest, void* value )						{ return ( void* )emscripten_atomic_exchange_u32( ( void** )pDest, ( uint32_t )value ); }
static inline	int			AkAtomicCasPtr( AkAtomicPtr* pDest, void* proposed, void* expected )		{ return emscripten_atomic_cas_u32( ( void** )pDest, ( uint32_t )expected, ( uint32_t )proposed ) == ( uint32_t )expected ? 1 : 0; }

#ifdef __cplusplus
}
#endif

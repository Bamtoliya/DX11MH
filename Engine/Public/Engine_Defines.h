#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

#include <string>
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <functional>
using namespace std;

#include "Engine_Struct.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Macro.h"

namespace Engine
{
	const _wstring	g_strTransformTag = TEXT("Com_Transform");
}

using namespace Engine;

#pragma region FMOD
#include "fmod.h"
#include "fmod_studio.hpp"
using namespace FMOD;
#pragma endregion

#pragma region WWISE
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#ifdef _DEBUG
#include <AK/Comm/AkCommunication.h> // Debug/Profile 빌드용
#endif
#pragma endregion

#pragma region imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma endregion
#pragma region imGuizmo
#include "ImGuizmo.h"
#pragma endregion




#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif




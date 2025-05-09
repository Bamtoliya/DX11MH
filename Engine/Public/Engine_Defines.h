#pragma once

#pragma warning (disable : 4251)
//PhysX Symbol PDB warning Disable
#pragma warning (disable : 4099)

#pragma region DirectX
#include <d3d11.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include <DirectXCollision.h>
#include "Effects.h"
#include "d3dx11effect.h"
#include "d3dcompiler.h"
using namespace DirectX;
#pragma endregion

#pragma region assimp
#include "scene.h"
#include "Importer.hpp"
#include "assimp/postprocess.h"
#pragma endregion


#pragma region JSON
#include "../../External Libraries/json.hpp"
using json = nlohmann::json;
#include "../../External Libraries/simdjson.h"
#pragma endregion

#pragma region C++
#include <string>
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <filesystem>
using namespace std;
#pragma endregion

#pragma region FMOD
#include "fmod.h"
#include "fmod_studio.hpp"
using namespace FMOD;
#pragma endregion

#pragma region Engine
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Enum.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"


namespace Engine
{
	const _wstring	g_strTransformTag = TEXT("Com_Transform");
}
using namespace Engine;
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

#pragma region DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <functional>
#pragma endregion

#pragma region GameInput
#include <GameInput.h>
#define GAMEINPUT_API_VERSION 1
using namespace GameInput;
using namespace v1;
#pragma endregion

#pragma region PhysX
#include "PxPhysicsAPI.h"
using namespace physx;
#pragma endregion

//#include ""

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif




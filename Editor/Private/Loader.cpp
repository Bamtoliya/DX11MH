#include "Loader.h"

#include "GameInstance.h"

#include "Camera_Free.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Monster.h"
#include "Statue.h"
#include "Sonic.h"
//#include "player.h"
//#include "Effect.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	/* 자원로딩한다. */
	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;	

	return 0;

}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;
	m_pGameInstance->Clear_CameraManager();
	// 지연된 컨텍스트 생성
	if (FAILED(m_pDevice->CreateDeferredContext(0, &m_pDeferredContext)))
	{
		MSG_BOX("Failed to Create Deferred Context");
		return E_FAIL;
	}

	//Safe_AddRef(m_pDeferredContext);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	CoInitializeEx(nullptr, 0);	

	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	/*case LEVEL::LOADING:
		hr = Loading_For_Loading();
		break;*/
	case LEVEL::UI:
		hr = Loading_For_UI();
		break;
	case LEVEL::MAP:
		hr = Loading_For_Map();
		break;
	case LEVEL::EFFECT:
		hr = Loading_For_Logo();
		break;
	case LEVEL::ANIMATION:
		hr = Loading_For_Animation();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_Gameplay();
		break;
	case LEVEL::TEST:
		hr = Loading_For_Test();
		break;
	case LEVEL::OPEN:
		hr = Loading_For_Open();
		break;
	case LEVEL::STAGE01:
		hr = Loading_For_Stage01();
		break;
	case LEVEL::STAGE02:
		hr = Loading_For_Stage02();
		break;

	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);



	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::LOGO), SAVEPATH L"Prototypes_UI.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Loading()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::LOADING), SAVEPATH L"Prototypes_UI.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_UI()
{

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::UI), SAVEPATH L"Prototypes_UI.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	

	return S_OK;
}

HRESULT CLoader::Loading_For_Map()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::MAP), SAVEPATH L"Prototypes_Map.json", CLoader::JsonLoadCallback, this, m_pDeferredContext)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	
	return S_OK;
}

HRESULT CLoader::Loading_For_Effect()
{
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Gameplay()
{
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Animation()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::ANIMATION), SAVEPATH L"Prototypes_Animation.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Test()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::TEST), SAVEPATH L"Prototypes_Test.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::TEST), SAVEPATH L"Stage/W6D01/Prototypes_W6D01_Instance_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Open()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::OPEN), SAVEPATH L"Prototypes_Open.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::OPEN), SAVEPATH L"Stage/Open/Prototypes_Open_Collider_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::OPEN), SAVEPATH L"Stage/Open/Prototypes_Open_Instance_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Stage01()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STAGE01), SAVEPATH L"Prototypes_Stage01.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STAGE01), SAVEPATH L"Stage/W6D01/Prototypes_W6D01_Collider_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STAGE01), SAVEPATH L"Stage/W6D01/Prototypes_W6D01_Instance_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Stage02()
{
	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STAGE02), SAVEPATH L"Prototypes_Stage02.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STAGE02), SAVEPATH L"Stage/W6D02/Prototypes_W6D02_Collider_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STAGE02), SAVEPATH L"Stage/W6D02/Prototypes_W6D02_Instance_Models.json", CLoader::JsonLoadCallback, this)))
	{
		return E_FAIL;
	}


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;
	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	// 남은 커맨드 리스트 정리
	std::lock_guard<std::mutex> lock(m_QueueMutex);
	while (!m_CommandListQueue.empty())
	{
		ID3D11CommandList* pList = m_CommandListQueue.front();
		m_CommandListQueue.pop();
		Safe_Release(pList);
	}

	if (m_pDeferredContext)
		m_pDeferredContext->ClearState();

	// 지연된 컨텍스트 해제
	Safe_Release(m_pDeferredContext);

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CoUninitialize();

	DeleteCriticalSection(&m_CriticalSection);
}

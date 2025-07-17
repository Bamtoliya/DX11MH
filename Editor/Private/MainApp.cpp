#include "Levels.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "Sound_Manager.h"
#include "Tools.h"
#include "GameObjects.h"
#include "Game_Manager.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;
	EngineDesc.iNumLevels = static_cast<_uint>(LEVEL::END);
	EngineDesc.hInstance = g_hInst;

	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext, &m_pSound_Manager)))
		return E_FAIL;

	if (FAILED(Register_Class_Types()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Components()))	
		return E_FAIL;

	if (FAILED(CreateImGui()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	if (KEY_DOWN(DIK_O))
	{
		m_pGameInstance->ToggleShowImGui(L"ToolBar");
	}
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	m_pGameInstance->Begin_Draw();

	m_pGameInstance->Draw();

	m_pGameInstance->End_Draw();

	return S_OK;
}

HRESULT CMainApp::Register_Class_Types()
{
#pragma region Components
	if (FAILED(m_pGameInstance->Register_Creator_Function("CSonic_Controller", CSonic_Controller::Create))) return E_FAIL;
#pragma endregion

#pragma region Gameobjects
	if (FAILED(m_pGameInstance->Register_Creator_Function("CSonic", CSonic::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CTerrain", CTerrain::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CStatue", CStatue::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CBackGround", CBackGround::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CCamera_Free", CCamera_Free::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CCamera_Follow", CCamera_Follow::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CCamera_TPS", CCamera_TPS::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CMonster", CMonster::Create))) return E_FAIL;
#pragma endregion	

#pragma region Stages
	if (FAILED(m_pGameInstance->Register_Creator_Function("CGameObject_W6D01", CGameObject_W6D01::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CGameObject_W6D02", CGameObject_W6D02::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CGameObject_Open", CGameObject_Open::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CGameObject_W6D01FallDead_Collider", CGameObject_W6D01FallDead_Collider::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CGameObject_W6D02FallDead_Collider", CGameObject_W6D02FallDead_Collider::Create))) return E_FAIL;
#pragma endregion

#pragma region Tutorial
	if (FAILED(m_pGameInstance->Register_Creator_Function("CVirtual_Space", CVirtual_Space::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CTutorial_Wall", CTutorial_Wall::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CTutorial_Enemy", CTutorial_Enemy::Create))) return E_FAIL;
#pragma endregion

#pragma region StageObjects
	if (FAILED(m_pGameInstance->Register_Creator_Function("CGameObject_SplineRoad", CGameObject_SplineRoad::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CRing", CRing::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CRail", CRail::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CDashPanel", CDashPanel::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CSpring", CSpring::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CDashRing", CDashRing::Create))) return E_FAIL;
#pragma endregion

#pragma region Monsters
	if (FAILED(m_pGameInstance->Register_Creator_Function("CMan", CMan::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CWolf", CWolf::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CMotora", CMotora::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CDefender", CDefender::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CBommer", CBommer::Create))) return E_FAIL;
	if (FAILED(m_pGameInstance->Register_Creator_Function("CBlader", CBlader::Create))) return E_FAIL;
#pragma endregion



	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevel)
{
	if (FAILED(m_pGameInstance->Change_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevel))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Components()
{
	m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STATIC), SAVEPATH L"Prototypes_Static.json");

	m_pGameInstance->Load_Prototypes(ENUM_CLASS(LEVEL::STATIC), SAVEPATH L"Prototypes_UI.json");

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Default_Font"), ASSETPATH L"Fonts/sonic.spritefont")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Ex_Font"), ASSETPATH L"Fonts/sonicex.spritefont")))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::CreateImGui()
{
	if (FAILED(m_pGameInstance->Add_ImGui(L"ToolBar", CToolBar::Create(m_pDevice, m_pContext)))) return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ImGui(L"Inspector", CInspector::Create(m_pDevice, m_pContext)))) return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ImGui(L"Hierarchy", CHierarchy::Create(m_pDevice, m_pContext)))) return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ImGui(L"Asset Browser", CAsset_Browser::Create(m_pDevice, m_pContext)))) return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ImGui(L"Viewport2D", CViewport_2D::Create(m_pDevice, m_pContext)))) return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ImGui(L"Viewport3D", CViewport_3D::Create(m_pDevice, m_pContext)))) return E_FAIL;
	m_pGameInstance->HideAll();
	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pSound_Manager);
	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);


}

#include "GameInstance.h"

#include "Systems.h"
#include "Devices.h"
#include "Managers.h"
#include "Renderer.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppContextOut, _Out_ CSound_Manager** ppSoundOut)
{
#pragma region Devices
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDeviceOut, ppContextOut);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

#pragma endregion

#pragma region Systems
	m_pPhysics_System = CPhysics_System::Create(*ppDeviceOut, *ppContextOut);
	if (nullptr == m_pPhysics_System)
		return E_FAIL;

	m_pPicking_System = CPicking_System::Create(*ppDeviceOut, *ppContextOut);
	if (nullptr == m_pPicking_System)
		return E_FAIL;
#pragma endregion


#pragma region Managers
	if (m_bIsEditor)
	{
		m_pMyImGui_Manager = CMyImGui_Manager::Create(EngineDesc.hWnd, *ppDeviceOut, *ppContextOut);
		if (nullptr == m_pGraphic_Device)
			return E_FAIL;
	}

	m_pInput_Device = CInput_Device::Create(EngineDesc.hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;


	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL);
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	*ppSoundOut = m_pSound_Manager;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDeviceOut, *ppContextOut);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

#pragma endregion

	m_pRenderer = CRenderer::Create(*ppDeviceOut, *ppContextOut);
	if (nullptr == m_pRenderer)
		return E_FAIL;
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pInput_Device->Update(fTimeDelta);
	m_pObject_Manager->Priority_Update(fTimeDelta);
	m_pPipeLine->Update();
	m_pObject_Manager->Update(fTimeDelta);	
	m_pObject_Manager->Late_Update(fTimeDelta);
	m_pPicking_System->Update(fTimeDelta);

	if (m_pMyImGui_Manager)
		m_pMyImGui_Manager->Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
	m_pSound_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Begin_Draw()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.0f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pRenderer->Draw();

	if(m_pMyImGui_Manager)
		m_pMyImGui_Manager->Render();

	m_pLevel_Manager->Render();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	m_pGraphic_Device->Present();

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨의 자원을 삭제한다. */
	
	/* 특정 레벨의 객체을 삭제한다. */
	m_pObject_Manager->Clear(iLevelIndex);
	m_pSound_Manager->Stop_All_Event();
	/* 특정 레벨의 원형객을 삭제한다. */
	m_pPrototype_Manager->Clear(iLevelIndex);
}


_float CGameInstance::Compute_Random_Normal()
{
	return rand() / static_cast<_float>(RAND_MAX);	
}

_float CGameInstance::Compute_Random(_float fMin, _float fMax)
{
	return fMin + (fMax - fMin) * Compute_Random_Normal();	
}

HRESULT CGameInstance::Resize(_uint iWinResizeX, _uint iWinResizeY)
{
	if (m_pGraphic_Device)
		m_pGraphic_Device->Resize(iWinResizeX, iWinResizeY);

	if (m_pMyImGui_Manager)
		m_pMyImGui_Manager->Resize(iWinResizeX, iWinResizeY);
		
	return S_OK;
}

#pragma region GRAPHIC_DEVICE
ID3D11RenderTargetView* CGameInstance::Get_RTV() const
{
	return m_pGraphic_Device->Get_RTV();
}
ID3D11DepthStencilView* CGameInstance::Get_DSV() const
{
	return m_pGraphic_Device->Get_DSV();
}
ID3D11Texture2D* CGameInstance::Get_BackBufferTexture() const
{
	return m_pGraphic_Device->Get_BackBufferTexture();
}
ID3D11ShaderResourceView* CGameInstance::Get_BackBufferSRV() const
{
	return m_pGraphic_Device->Get_BackBufferSRV();
}
#pragma endregion




#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNewLevel);
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}
CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototypeType, iPrototypeLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, eFormat, vClearColor);
}
CRenderTarget* CGameInstance::Get_RenderTarget(const _wstring& strTargetTag) const
{
	return m_pTarget_Manager->Get_RenderTarget(strTargetTag);
}
HRESULT CGameInstance::Clear_RenderTarget(const _wstring& strTargetTag, _float4* pClearColor)
{
	return m_pTarget_Manager->Clear_RenderTarget(strTargetTag, pClearColor);
}
HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const std::vector<_wstring>& pTargetTags)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, pTargetTags);
}
HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}
HRESULT CGameInstance::End_MRT(ID3D11RenderTargetView* pBackBufferRTV, ID3D11DepthStencilView* pMainDSV, const D3D11_VIEWPORT& vp)
{
	return m_pTarget_Manager->End_MRT(pBackBufferRTV, pMainDSV, vp);
}
HRESULT CGameInstance::Clear_MRT(const _wstring& strMRTTag)
{
	return m_pTarget_Manager->Clear_MRT(strMRTTag);
}
ID3D11ShaderResourceView* CGameInstance::Get_SRV(const _wstring& strTargetTag) const
{
	return m_pTarget_Manager->Get_SRV(strTargetTag);
}
HRESULT CGameInstance::Copy_ToBackBuffer(const _wstring& strSourceRenderTargetTag)
{
	return m_pTarget_Manager->Copy_ToBackBuffer(strSourceRenderTargetTag);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLevelIndex, strLayerTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
	
}

CGameObject* CGameInstance::Get_GameObjectById(_uint iId)
{
	return m_pObject_Manager->Get_GameObjectById(iId);
}

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Update(strTimerTag);
}

#pragma endregion

#pragma region MYIMGUI_MANAGER

HRESULT CGameInstance::Add_ImGui(const _wstring& strKey, CMyImGui* pMyImGui)
{
	return m_pMyImGui_Manager->Add_ImGui(strKey, pMyImGui);
}

#pragma endregion

#pragma region PIPELINE

void CGameInstance::Set_Transform(D3DTS eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(D3DTS eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();
}

#pragma endregion

#pragma region Picking_System
HRESULT CGameInstance::ReadPickingTargetPixel(_int screenX, _int screenY, _float4* pPickedColor)
{
	return m_pPicking_System->ReadPickingTargetPixel(screenX, screenY, pPickedColor);
}
HRESULT CGameInstance::PickObjectByColor(_float4* pColor)
{
	return m_pPicking_System->PickObject(pColor);
}
#pragma endregion


#pragma region Input_Device

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(DIM eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(DIMM eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Key_Down(_ubyte eKeyID)
{
	return m_pInput_Device->Key_Down(eKeyID);
}

_bool CGameInstance::Key_Up(_ubyte eKeyID)
{
	return m_pInput_Device->Key_Up(eKeyID);
}

_bool CGameInstance::Key_Pressing(_ubyte eKeyID)
{
	return m_pInput_Device->Key_Pressing(eKeyID);
}

_bool CGameInstance::Mouse_Down(DIM eMouseKeyID)
{
	return m_pInput_Device->Mouse_Down(eMouseKeyID);
}

_bool CGameInstance::Mouse_Up(DIM eMouseKeyID)
{
	return m_pInput_Device->Mouse_Up(eMouseKeyID);
}

_bool CGameInstance::Mouse_Pressing(DIM eMouseKeyID)
{
	return m_pInput_Device->Mouse_Pressing(eMouseKeyID);
}

#pragma endregion

//#pragma region PICKING
//void CGameInstance::Transform_Picking_ToLocalSpace(const _float4x4& WorldMatrixInverse)
//{
//	m_pPicking->Transform_ToLocalSpace(WorldMatrixInverse);
//}
//_bool CGameInstance::Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
//{
//	return m_pPicking->Picking_InWorld(vPickedPos, vPointA, vPointB, vPointC);
//}
//_bool CGameInstance::Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
//{
//	return m_pPicking->Picking_InLocal(vPickedPos, vPointA, vPointB, vPointC);
//}
//
//#pragma endregion

void CGameInstance::Release_Engine()
{
	//Safe_Release(m_pPicking);

	

	Safe_Release(m_pMyImGui_Manager);

	Safe_Release(m_pPipeLine);

	Safe_Release(m_pTimer_Manager);

	Safe_Release(m_pRenderer);

	Safe_Release(m_pObject_Manager);

	Safe_Release(m_pPrototype_Manager);

	Safe_Release(m_pLevel_Manager);

	Safe_Release(m_pInput_Device);

	Safe_Release(m_pTarget_Manager);

	Safe_Release(m_pGraphic_Device);

	Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();
}

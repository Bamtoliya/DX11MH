#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	D3D11_VIEWPORT ViewportDesc = {};
	_uint iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_FinalScene",                // 렌더 타겟 고유 태그
		ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,  // 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 1.f, 1.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_Background Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	vector<wstring> FinalTargets = { L"RT_FinalScene" };
	if (FAILED(m_pGameInstance->Add_MRT(L"RT_FinalScene_Group", FinalTargets)))
	{
		MSG_BOX("Failed to create MRT_BackgroundOnly group");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >=	RENDERGROUP::RG_END ||
		nullptr == pRenderObject)
		return E_FAIL;

	m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{

	ID3D11DepthStencilView* pMainDSV = m_pGameInstance->Get_DSV(); // Get main DSV


	//// 1. Switch to Off-Screen Target (TargetManager handles backup)
	if (FAILED(m_pGameInstance->Begin_MRT(L"RT_FinalScene_Group", pMainDSV))) // Group name example
		return E_FAIL;

	// 2. Clear the Off-Screen Target
	_float4 tempColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_pGameInstance->Clear_RenderTarget(L"RT_FinalScene", &tempColor);
	m_pContext->ClearDepthStencilView(pMainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Copy_ToBackBuffer(L"RT_FinalScene")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT(nullptr, nullptr, {})))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_PRIORITY)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_PRIORITY)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONBLEND)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONBLEND)].clear();

	return S_OK;
}
HRESULT CRenderer::Render_Debug()
{
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_DEBUG)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONBLEND)].clear();

	return S_OK;
}
//
//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return dynamic_cast<CBlendObject*>(pSour)->Get_Depth() > dynamic_cast<CBlendObject*>(pDest)->Get_Depth();
//}

HRESULT CRenderer::Render_Blend()
{
	/*m_RenderObjects[RG_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return dynamic_cast<CBlendObject*>(pSour)->Get_Depth() > dynamic_cast<CBlendObject*>(pDest)->Get_Depth();
	});*/

	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_BLEND)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_BLEND)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_UI)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_UI)].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		return nullptr;
	}
	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);
		ObjectList.clear();
	}		
}

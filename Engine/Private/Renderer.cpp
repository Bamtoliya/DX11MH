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
#pragma region Prepare RenderTargets
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_FinalScene",               // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,     // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)     // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_FinalScene Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Skybox",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_Skybox Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_ObjectsOnly",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_FinalScene Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Picking",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_Picking Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	vector<wstring> SkyboxTargets = { L"RT_Skybox", L"RT_FinalScene" };
	if (FAILED(m_pGameInstance->Add_MRT(L"RT_Skybox_Group", SkyboxTargets)))
	{
		MSG_BOX("Failed to create RT_Skybox group");
		return E_FAIL;
	}

	vector<wstring> FinalTargets = { L"RT_ObjectsOnly", L"RT_Picking", L"RT_FinalScene" };
	if (FAILED(m_pGameInstance->Add_MRT(L"RT_FinalScene_Group", FinalTargets)))
	{
		MSG_BOX("Failed to create RT_FinalScene group");
		return E_FAIL;
	}
#pragma endregion

#pragma region Prepare Physx DebugDraw
	m_iNumMaxDebugLineVertices = 20000;
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = sizeof(VTXDEBUGDRAW) * m_iNumMaxDebugLineVertices;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, nullptr, &m_pVB_DebugLine)))
	{
		MSG_BOX("CRenderer::Initialize - Failed to create m_pVB_DebugLine");
		return E_FAIL;
	}
#pragma endregion

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
	if (FAILED(m_pGameInstance->Begin_MRT(L"RT_Skybox_Group", pMainDSV))) // Group name example
		return E_FAIL;
	_float4 tempColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_pGameInstance->Clear_RenderTarget(L"RT_Skybox", &tempColor);
	m_pGameInstance->Clear_RenderTarget(L"RT_FinalScene", &tempColor);
	m_pContext->ClearDepthStencilView(pMainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
 	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(m_pGameInstance->End_MRT(nullptr, nullptr, {})))
		return E_FAIL;

	//// 1. Switch to Off-Screen Target (TargetManager handles backup)
	if (FAILED(m_pGameInstance->Begin_MRT(L"RT_FinalScene_Group", pMainDSV))) // Group name example
		return E_FAIL;

	// 2. Clear the Off-Screen Target
	
	m_pGameInstance->Clear_RenderTarget(L"RT_ObjectsOnly", &tempColor);
	m_pGameInstance->Clear_RenderTarget(L"RT_Picking", &tempColor);
	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	//if (FAILED(Render_Lights()))
	//	return E_FAIL;

	if (FAILED(Render_BackBuffer()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	/* 블렌딩이전에 백버퍼를 완성시키낟.  */
	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;
	if (m_pGameInstance->Get_IsDebugDraw())
	{
		if (FAILED(Render_Debug_Collider()))
			return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Copy_ToBackBuffer(L"RT_FinalScene")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT(nullptr, nullptr, {})))
		return E_FAIL;

	

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	m_DebugComponent.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}

#endif

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
HRESULT CRenderer::Render_Debug_Collider()
{	
	const auto& debugLines = m_pGameInstance->Get_DebugLines();
	if (debugLines.empty())
	{
		return S_OK;
	}

	_uint iNumVerticesToRender = static_cast<_uint>(debugLines.size() * 2);
	if (iNumVerticesToRender == 0) {
		return S_OK;
	}

	if (iNumVerticesToRender > m_iNumMaxDebugLineVertices) {
		iNumVerticesToRender = m_iNumMaxDebugLineVertices;
		// MSG_BOX("CRenderer::Render_Debug_PhysX - Debug line vertex count exceeds buffer capacity. Clamping.");
	}

	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	if (FAILED(m_pContext->Map(m_pVB_DebugLine, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource)))
	{
		return E_FAIL;
	}

	VTXDEBUGDRAW* pVertices = (VTXDEBUGDRAW*)MappedSubResource.pData;
	_uint iVertexIndex = 0;
	for (const auto& line : debugLines)
	{
		if (iVertexIndex + 1 >= iNumVerticesToRender) break;
		pVertices[iVertexIndex].vPosition = line.vP0;
		pVertices[iVertexIndex].vColor = line.vColor;
		iVertexIndex++;

		pVertices[iVertexIndex].vPosition = line.vP1;
		pVertices[iVertexIndex].vColor = line.vColor;
		iVertexIndex++;
	}
	m_pContext->Unmap(m_pVB_DebugLine, 0);

	CShader* pShader = static_cast<CShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_COMPONENT, 0, TEXT("Prototype_Component_Shader_VtxDebugDraw"), nullptr, nullptr));

	if (nullptr == pShader) {
		MSG_BOX("Render_Debug_PhysX - Failed to Get Prototype_Shader_DebugLine");
		return E_FAIL;
	}
	// 프로토타입을 직접 사용하는 경우 AddRef/Release는 필요 없음. 복제해서 사용한다면 Release 필요.
	// 여기서는 직접 사용한다고 가정.

	_float4x4 matWorld, matView, matProj;
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity()); // 디버그 라인은 이미 월드 좌표
	XMStoreFloat4x4(&matView, m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	XMStoreFloat4x4(&matProj, m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	pShader->Bind_Matrix("g_WorldMatrix", &matWorld);
	pShader->Bind_Matrix("g_ViewMatrix", &matView);
	pShader->Bind_Matrix("g_ProjMatrix", &matProj);

	// 중요: 셰이더의 Begin을 호출하기 전에 입력 레이아웃 설정이 필요할 수 있음
	// CShader::Create에서 입력 레이아웃을 이미 생성했으므로, Begin에서 자동으로 바인딩됨.
	pShader->Begin(0); // 디버그 셰이더의 0번 패스 사용

	_uint iStride = sizeof(VTXDEBUGDRAW);
	_uint iOffset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVB_DebugLine, &iStride, &iOffset);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); // 라인 리스트로 설정
	m_pContext->Draw(iVertexIndex, 0);
	Safe_Release(pShader);
	return S_OK;
	/*for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_DEBUG)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONBLEND)].clear();

	return S_OK;*/
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

HRESULT CRenderer::Render_Lights()
{
	/* Shade */
	ID3D11DepthStencilView* pMainDSV = m_pGameInstance->Get_DSV(); // Get main DSV

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"), pMainDSV)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;	

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT(nullptr, nullptr, {})))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BackBuffer()
{
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONLIGHT)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONLIGHT)].clear();

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponent)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponent.clear();

	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer);
	

	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
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

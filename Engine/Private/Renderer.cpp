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

#pragma region DEBUG
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
#pragma endregion

#pragma region GameObjects
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Diffuse",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Normal",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R16G16B16A16_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(1.f, 1.f, 1.f, 1.f)			// 클리어 색상 (불투명 하양)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Depth",						// 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,          // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,         // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 1.f, 0.f, 0.f)			// 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_PickObj",					// 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,     // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R8G8B8A8_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)     // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_PickPos",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}
#pragma endregion

#pragma region Lights
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Shade",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R16G16B16A16_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Specular",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R16G16B16A16_UNORM,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(0.f, 0.f, 0.f, 0.f)      // 클리어 색상 (투명 검정 등)
	)))
	{
		MSG_BOX("Failed to create RT_PickObj Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}
#pragma endregion

#pragma region Shadow
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_Shadow",                // 렌더 타겟 고유 태그
		(_uint)g_iMaxWidth,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)g_iMaxHeight,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(1.f, 1.f, 1.f, 1.f)      // 클리어 색상 (불투명 하양 등)
	)))
	{
		MSG_BOX("Failed to create RT_Shadow Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}
#pragma endregion

#pragma region Blur
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_BlurX",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(1.f, 1.f, 1.f, 1.f)      // 클리어 색상 (불투명 하양 등)
	)))
	{
		MSG_BOX("Failed to create RT_BlurX Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}
	if (FAILED(m_pGameInstance->Add_RenderTarget(
		L"RT_BlurY",                // 렌더 타겟 고유 태그
		(_uint)ViewportDesc.Width,                      // 초기 너비 (예: 윈도우 너비)
		(_uint)ViewportDesc.Height,                      // 초기 높이 (예: 윈도우 높이)
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// 포맷 (백버퍼/ImGui 호환성 고려)
		_float4(1.f, 1.f, 1.f, 1.f)      // 클리어 색상 (불투명 하양 등)
	)))
	{
		MSG_BOX("Failed to create RT_BlurY Render Target");
		return E_FAIL; // 또는 적절한 에러 처리
	}
#pragma endregion

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer){
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("G:/My Drive/Assets/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, "VTXPOSTEX");
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f));

	m_iOriginalViewportWidth = ViewportDesc.Width;
	m_iOriginalViewportHeight = ViewportDesc.Height;

	if (FAILED(Ready_DepthStencilView(g_iMaxWidth, g_iMaxHeight)))
		return E_FAIL;

	vector<wstring> GameObjectsTargets = { L"RT_Diffuse", L"RT_Normal", L"RT_Depth", L"RT_PickObj", L"RT_PickPos" };
	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_GameObjects", GameObjectsTargets)))
	{
		MSG_BOX("Failed to create MRT_GameObjects");
		return E_FAIL;
	}
	vector<wstring> LightsTargets = { L"RT_Shade", L"RT_Specular" };
	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Lights", LightsTargets)))
	{
		MSG_BOX("Failed to create MRT_Lights");
		return E_FAIL;
	}
	vector<wstring> ShadowTargets = { L"RT_Shadow" };
	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_ShadowObjects", ShadowTargets)))
	{
		MSG_BOX("Failed to create MRT_ShadowObjects");
		return E_FAIL;
	}
	vector<wstring> FinalTargets = { L"RT_FinalScene" };
	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Final", FinalTargets)))
	{
		MSG_BOX("Failed to create MRT_FinalScene group");
		return E_FAIL;
	}
#pragma endregion

#ifdef _DEBUG
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
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 300.f, 100.0f, 200.0f, 200.0f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 300.f, 300.0f, 200.0f, 200.0f)))
	//	return E_FAIL;
#endif
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
 	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Lights()))
		return E_FAIL;;
	 
	if (FAILED(Render_BackBuffer()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Copy_ToBackBuffer(L"MRT_Final")))
		return E_FAIL;

	/* 블렌딩이전에 백버퍼를 완성시키낟.  */
	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;


	

#ifdef _DEBUG
	if (m_pGameInstance->Get_IsDebugDraw())
	{
		/*if (FAILED(Render_Debug()))
			return E_FAIL;*/
		if (FAILED(Render_Debug_Collider()))
			return E_FAIL;
	}
#endif

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
	m_pGameInstance->Begin_MRT(TEXT("MRT_Final"));
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_PRIORITY)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_PRIORITY)].clear();
	m_pGameInstance->End_MRT();
	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObjects"), m_pShadowDSV, true);

	if (FAILED(Change_ViewportDesc(g_iMaxWidth, g_iMaxHeight)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_SHADOW)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Shadow();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_SHADOW)].clear();

	m_pGameInstance->End_MRT();

	if (FAILED(Change_ViewportDesc(m_iOriginalViewportWidth, m_iOriginalViewportHeight)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"));
	for (auto& pGameObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONBLEND)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_NONBLEND)].clear();
	m_pGameInstance->End_MRT();
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
}
//
//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return dynamic_cast<CBlendObject*>(pSour)->Get_Depth() > dynamic_cast<CBlendObject*>(pDest)->Get_Depth();
//}

HRESULT CRenderer::Render_Blend()
{
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
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;	
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4Inverse(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4Inverse(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BackBuffer()
{
	m_pGameInstance->Begin_MRT(TEXT("MRT_Final"));

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Shadow"), m_pShader, "g_ShadowTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4Inverse(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4Inverse(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_Light_ViewMatrix())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_Light_ProjMatrix())))
		return E_FAIL;

	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

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

HRESULT CRenderer::Render_Blur()
{

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX"));

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Final"), m_pShader, "g_FinalTexture")))
		return E_FAIL;

	m_pShader->Begin(4);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT(nullptr, nullptr, {});

	
	/* 백버퍼에 찍는다. */
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BlurX"), m_pShader, "g_BlurXTexture")))
		return E_FAIL;

	m_pShader->Begin(5);
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Ready_DepthStencilView(_uint iWidth, _uint iHeight)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아에 렌더링을 못함. */
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;	
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;


	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::Change_ViewportDesc(_uint iWidth, _uint iHeight)
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };

	ViewportDesc.TopLeftX = 0;
	ViewportDesc.TopLeftY = 0;
	ViewportDesc.Width = (FLOAT)iWidth;
	ViewportDesc.Height = (FLOAT)iHeight;
	ViewportDesc.MinDepth = 0.f;
	ViewportDesc.MaxDepth = 1.f;	

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

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
	m_pGameInstance->Render_MRT_Debug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer);
	

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

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);
		ObjectList.clear();
	}		

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVB_DebugLine);
	Safe_Release(m_pShader);
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

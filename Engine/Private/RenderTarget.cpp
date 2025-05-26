#pragma once
#include "RenderTarget.h" // 경로 확인 필요
#include "Engine_Function.h" // Safe_Release 등
#include "GameInstance.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext { pContext }
    , m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(const _wstring& strName, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
    if (m_pDevice == nullptr)
        return E_FAIL;

    m_strName = strName;
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_eFormat = eFormat;
    m_vClearColor = vClearColor; // DirectX::XMFLOAT4로 변경하거나 _float4 유지

    // 1. 텍스처 생성
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = iWidth;
    textureDesc.Height = iHeight;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = eFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // RTV, SRV로 바인딩 가능해야 함
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture)))
    {
        MSG_BOX("Failed to Create RenderTarget Texture2D");
        return E_FAIL;
    }

    // 2. RTV 생성
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = eFormat;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &rtvDesc, &m_pRTV)))
    {
        MSG_BOX("Failed to Create RenderTarget RTV");
        return E_FAIL;
    }

    // 3. SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = eFormat;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = -1;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pSRV)))
    {
        MSG_BOX("Failed to Create RenderTarget SRV");
        return E_FAIL;
    }

    // 4. 뷰포트 설정
    ZeroMemory(&m_Viewport, sizeof(D3D11_VIEWPORT));
    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
    m_Viewport.Width = static_cast<float>(iWidth);
    m_Viewport.Height = static_cast<float>(iHeight);
    m_Viewport.MinDepth = 0.f;
    m_Viewport.MaxDepth = 1.f;

    return S_OK;
}

HRESULT CRenderTarget::Clear(_float4* pClearColor)
{
    if (nullptr == m_pContext || nullptr == m_pRTV)
        return E_FAIL;

    // 클리어 색상 결정
    float clearColor[4];
    if (pClearColor) {
        clearColor[0] = pClearColor->x;
        clearColor[1] = pClearColor->y;
        clearColor[2] = pClearColor->z;
        clearColor[3] = pClearColor->w;
    }
    else {
        clearColor[0] = m_vClearColor.x;
        clearColor[1] = m_vClearColor.y;
        clearColor[2] = m_vClearColor.z;
        clearColor[3] = m_vClearColor.w;
    }

    m_pContext->ClearRenderTargetView(m_pRTV, clearColor);

    return S_OK;
}

void CRenderTarget::Bind(ID3D11DepthStencilView* pDSV)
{
    if (nullptr == m_pContext)
        return;

    // 이 RTV와 전달받은 DSV를 바인딩
    m_pContext->OMSetRenderTargets(1, &m_pRTV, pDSV);
    // 이 렌더타겟에 맞는 뷰포트 설정
    m_pContext->RSSetViewports(1, &m_Viewport);
}

HRESULT CRenderTarget::Resize(_uint iWinResizeX, _uint iWinResizeY)
{
    if (m_pDevice == nullptr || m_pContext == nullptr)
        return E_FAIL;

    // 0 이하의 크기로 리사이즈 시도 방지
    if (iWinResizeX == 0 || iWinResizeY == 0)
    {
        // 필요하다면 경고 메시지 또는 최소 크기로 설정
        return S_FALSE; // 또는 E_INVALIDARG
    }

    // 현재 크기와 동일하면 아무것도 하지 않음
    if (m_iWidth == iWinResizeX && m_iHeight == iWinResizeY)
    {
        return S_OK;
    }

    // 1. 기존 리소스 해제
    // 중요: 만약 이 렌더타겟이 현재 파이프라인에 바인딩되어 있다면,
    // 먼저 바인딩을 해제해야 합니다. (예: OMSetRenderTargets(0, nullptr, nullptr))
    // CTarget_Manager::Resize에서 이 작업을 선행하거나, 여기서 처리할 수 있지만
    // CTarget_Manager에서 전체적으로 관리하는 것이 더 안전할 수 있습니다.
    // 여기서는 CRenderTarget 자체의 리소스만 해제한다고 가정합니다.
    Safe_Release(m_pSRV);
    Safe_Release(m_pRTV);
    Safe_Release(m_pTexture);

    // 2. 새로운 크기로 멤버 변수 업데이트
    m_iWidth = iWinResizeX;
    m_iHeight = iWinResizeY;
    // m_eFormat 와 m_vClearColor 는 유지됩니다. 필요시 변경 로직 추가 가능.

    // 3. 새로운 크기로 리소스 재생성 (Initialize 함수의 로직과 유사)
    // 텍스처 생성
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = m_iWidth;
    textureDesc.Height = m_iHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = m_eFormat; // 기존 포맷 사용
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture)))
    {
        MSG_BOX("Failed to Recreate RenderTarget Texture2D during Resize");
        // 이전 상태로 롤백하거나, 에러 처리. 여기서는 간단히 실패 반환.
        m_iWidth = 0; // 유효하지 않은 상태로 표시
        m_iHeight = 0;
        return E_FAIL;
    }

    // RTV 생성
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = m_eFormat;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &rtvDesc, &m_pRTV)))
    {
        MSG_BOX("Failed to Recreate RenderTarget RTV during Resize");
        Safe_Release(m_pTexture); // 생성된 텍스처 해제
        m_iWidth = 0;
        m_iHeight = 0;
        return E_FAIL;
    }

    // SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = m_eFormat;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pSRV)))
    {
        MSG_BOX("Failed to Recreate RenderTarget SRV during Resize");
        Safe_Release(m_pRTV); // 생성된 RTV 해제
        Safe_Release(m_pTexture); // 생성된 텍스처 해제
        m_iWidth = 0;
        m_iHeight = 0;
        return E_FAIL;
    }

    // 뷰포트 업데이트
    ZeroMemory(&m_Viewport, sizeof(D3D11_VIEWPORT));
    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
    m_Viewport.Width = static_cast<float>(m_iWidth);
    m_Viewport.Height = static_cast<float>(m_iHeight);
    m_Viewport.MinDepth = 0.f;
    m_Viewport.MaxDepth = 1.f;

    // 디버그 이름이 설정되어 있었다면, 재생성된 리소스에도 다시 설정해줄 수 있습니다.
    // Set_DebugName(...);

    return S_OK;
}

//// 디버깅용 이름 설정 함수 (선택 사항)
//#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
//#include <string> // std::string 사용 위해
//void CRenderTarget::Set_DebugName(const std::string& name)
//{
//    if (m_pTexture) m_pTexture->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), name.c_str());
//    if (m_pRTV) m_pRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), (name + "_RTV").c_str());
//    if (m_pSRV) m_pSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.length()), (name + "_SRV").c_str());
//}
//#else
//void CRenderTarget::Set_DebugName(const std::string&) {}
//#endif


CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strName, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(strName, iWidth, iHeight, eFormat, vClearColor)))
    {
        MSG_BOX("Failed to Created : CRenderTarget");
        Safe_Release(pInstance);
        return nullptr; // 실패 시 nullptr 반환
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    __super::Free();
    Safe_Release(m_pSRV);
    Safe_Release(m_pRTV);
    Safe_Release(m_pTexture);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
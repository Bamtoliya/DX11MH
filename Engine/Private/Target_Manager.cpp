#include "Target_Manager.h" // 경로 확인
#include "RenderTarget.h" // CRenderTarget 클래스 사용 시
#include "GameInstance.h"

#pragma region Constructor&Destructor
CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
    // 필요시 초기화 로직 추가
    return S_OK;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTarget_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTarget_Manager::Free()
{
    // MRT 그룹 정보 정리 (포인터만 가지고 있으므로 별도 해제 불필요)
    m_MRTs.clear();
    m_MRTTags.clear();
    m_MRTViewports.clear();

    // 렌더 타겟 객체들 해제
    for (auto& pair : m_RenderTargets) {
        Safe_Release(pair.second);
    }
    m_RenderTargets.clear();

    // 백업 포인터들 안전하게 릴리즈 (혹시 End_MRT가 호출되지 않은 경우 대비)
    for (_uint i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        Safe_Release(m_pBackupRTVs[i]);
    }
    Safe_Release(m_pBackupDSV);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
#pragma endregion

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
    if (Find_RenderTarget(strTargetTag) != nullptr) {
        MSG_BOX("Render Target already exists with this tag.");
        return E_FAIL; // 이미 존재하는 태그
    }

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, strTargetTag, iWidth, iHeight, eFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const std::vector<_wstring>& pTargetTags)
{
    if (pTargetTags.empty() || pTargetTags.size() > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) {
        MSG_BOX("Invalid number of render targets for MRT.");
        return E_FAIL;
    }

    if (m_MRTs.count(strMRTTag)) {
        MSG_BOX("MRT Group already exists with this tag.");
        return E_FAIL; // 이미 존재하는 MRT 그룹 태그
    }

    list<CRenderTarget*> rtvs;
    vector<_wstring> rtvtags;
    D3D11_VIEWPORT firstViewport = {}; // 첫 번째 타겟의 뷰포트를 그룹 뷰포트로 사용

    for (size_t i = 0; i < pTargetTags.size(); ++i) {
        CRenderTarget* pRT = Find_RenderTarget(pTargetTags[i]);
        rtvtags.push_back(pTargetTags[i]);
        if (nullptr == pRT) {
            MSG_BOX("Render target not found for MRT group.");
            return E_FAIL; // 그룹에 포함될 렌더 타겟이 존재하지 않음
        }
        rtvs.push_back(pRT);

        // 모든 렌더 타겟의 크기가 동일한지 확인 (선택적이지만 권장됨)
        const D3D11_VIEWPORT& currentVP = pRT->Get_Viewport();
        if (i == 0) {
            firstViewport = currentVP;
        }
        else {
            if (firstViewport.Width != currentVP.Width || firstViewport.Height != currentVP.Height) {
                MSG_BOX("Render targets in MRT group must have the same dimensions.");
                // return E_FAIL; // 필요시 에러 처리
            }
        }
    }

    m_MRTs.emplace(strMRTTag, rtvs);
    m_MRTTags.emplace(strMRTTag, rtvtags);
    m_MRTViewports.emplace(strMRTTag, firstViewport); // 첫 번째 뷰포트 저장

    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool isDepthClear)
{
    if (m_pContext == nullptr) return E_FAIL;

    // 이전에 바인딩된 모든 픽셀 셰이더 리소스를 해제하여 리소스 해저드를 방지합니다.
    ID3D11ShaderResourceView* pSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr, };
    m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRVs);

    auto mrtIter = m_MRTs.find(strMRTTag);
    if (mrtIter == m_MRTs.end()) {
        MSG_BOX("MRT Group not found.");
        return E_FAIL; // MRT 그룹을 찾을 수 없음
    }

    auto vpIter = m_MRTViewports.find(strMRTTag);
    if (vpIter == m_MRTViewports.end()) {
        // 이론적으로는 MRT 그룹과 함께 생성되므로 이 경우는 드물어야 함
        MSG_BOX("Viewport for MRT Group not found.");
        return E_FAIL;
    }

    // 1. 현재 상태 백업
    m_iNumViews = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; // 최대 개수만\큼 가져오기 시도
    m_pContext->OMGetRenderTargets(m_iNumViews, m_pBackupRTVs, &m_pBackupDSV);

    ID3D11DepthStencilView* targetDSV = nullptr == pDSV ? m_pBackupDSV : pDSV;
    
    if (true == isDepthClear)
		m_pContext->ClearDepthStencilView(targetDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    // 실제 바인딩된 뷰 개수 확인 (OMGetRenderTargets는 첫 번째 nullptr에서 멈춤)
    for (m_iNumViews = 0; m_iNumViews < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++m_iNumViews) {
        if (m_pBackupRTVs[m_iNumViews] == nullptr) break;
    }
    _uint numViewports = 1;
    m_pContext->RSGetViewports(&numViewports, &m_BackupViewport); // 보통 뷰포트는 1개

    // 2. MRT 바인딩
    const auto& rtvs = mrtIter->second;
    ID3D11RenderTargetView* pRenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    _uint iNumTargets = 0;
    for (auto& pRenderTarget : rtvs)
    {
        pRenderTarget->Clear();
        pRenderTargets[iNumTargets++] = pRenderTarget->Get_RTV();
    }
    m_pContext->OMSetRenderTargets(iNumTargets, pRenderTargets, targetDSV);

    // 3. 뷰포트 설정
    m_pContext->RSSetViewports(1, &(vpIter->second));

    return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11RenderTargetView* pBackBufferRTV, ID3D11DepthStencilView* pMainDSV, const D3D11_VIEWPORT& vp)
{
    if (m_pContext == nullptr) return E_FAIL;

    // 백업된 상태 복원
    m_pContext->OMSetRenderTargets(m_iNumViews, m_pBackupRTVs, m_pBackupDSV);
    m_pContext->RSSetViewports(1, &m_BackupViewport);

    // 백업 포인터 릴리즈 (OMGetRenderTargets가 참조 카운트 증가시킴)
    for (_uint i = 0; i < m_iNumViews; ++i) {
        Safe_Release(m_pBackupRTVs[i]);
        m_pBackupRTVs[i] = nullptr; // nullptr로 초기화
    }
    Safe_Release(m_pBackupDSV);
    m_pBackupDSV = nullptr; // nullptr로 초기화
    m_iNumViews = 0; // 개수 초기화

    return S_OK;
}

HRESULT CTarget_Manager::Clear_RenderTarget(const _wstring& strTargetTag, _float4* pClearColor)
{
    CRenderTarget* pRT = Find_RenderTarget(strTargetTag);
    if (nullptr == pRT) {
        MSG_BOX("Render Target not found for clearing.");
        return E_FAIL;
    }
    return pRT->Clear(pClearColor); // CRenderTarget의 Clear 호출
}

HRESULT CTarget_Manager::Clear_MRT(const _wstring& strMRTTag)
{
    auto mrtIter = m_MRTs.find(strMRTTag);
    if (mrtIter == m_MRTs.end()) {
        MSG_BOX("MRT Group not found for clearing.");
        return E_FAIL;
    }

    for (const auto& rtvPtr : mrtIter->second) {
        bool found = false;
        for (const auto& pair : m_RenderTargets) {
            if (FAILED(pair.second->Clear())) { // 기본 클리어 색상 사용
                // 에러 처리
            }
            found = true;
            break;
        }
        if (!found) {

            MSG_BOX("Could not find RenderTarget object for an RTV in MRT group during clear.");
        }
    }

    return S_OK;
}

HRESULT CTarget_Manager::Render_Debug_Collider(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

ID3D11ShaderResourceView* CTarget_Manager::Get_SRV(const _wstring& strTargetTag) const
{
    CRenderTarget* pRT = Find_RenderTarget(strTargetTag);
    return (pRT != nullptr) ? pRT->Get_SRV() : nullptr;
}

CRenderTarget* CTarget_Manager::Get_RenderTarget(const _wstring& strTargetTag) const
{
    auto iter = m_RenderTargets.find(strTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;
    return iter->second;
}

HRESULT CTarget_Manager::Copy_ToBackBuffer(const _wstring& strSourceRenderTargetTag)
{
    // 1. 대상 리소스 (백버퍼) 가져오기
    m_pContext->OMGetRenderTargets(1, m_pBackupRTVs, &m_pBackupDSV);
    ID3D11RenderTargetView* pBackBufferRTV = m_pBackupRTVs[0];
    if (!pBackBufferRTV)
    {
        MSG_BOX("Failed to get BackBuffer RTV from GameInstance.");
        return E_FAIL;
    }

    ID3D11Resource* pDestResource = nullptr;
    pBackBufferRTV->GetResource(&pDestResource); // AddRef됨
    if (!pDestResource)
    {
        MSG_BOX("Failed to get Resource from BackBuffer RTV.");
        return E_FAIL;
    }

    // 2. 원본 RenderTarget 찾기 및 리소스 가져오기
    CRenderTarget* pSourceRT = Find_RenderTarget(strSourceRenderTargetTag);
    if (!pSourceRT)
    {
        MSG_BOX("Source Render Target not found for copy.");
        Safe_Release(pDestResource);
        return E_FAIL;
    }

    ID3D11Resource* pSourceResource = nullptr;
    ID3D11Texture2D* pSourceTexture = pSourceRT->Get_Texture(); // CRenderTarget에 Get_Texture()가 있다고 가정
    if (!pSourceTexture)
    {
        MSG_BOX("Failed to get Texture from source Render Target.");
        Safe_Release(pDestResource);
        return E_FAIL;
    }
    // GetResource 대신 직접 텍스처를 가져와서 QueryInterface를 사용하거나,
    // CRenderTarget이 ID3D11Resource*를 반환하는 함수를 제공해야 할 수도 있습니다.
    // 여기서는 pSourceTexture가 ID3D11Resource로 캐스팅 가능하다고 가정합니다.
    pSourceResource = static_cast<ID3D11Resource*>(pSourceTexture);
    Safe_AddRef(pSourceResource); // 직접 캐스팅 시 수동 AddRef 필요할 수 있음 (Get_Texture가 AddRef 안 해준다면)


    // 3. 서브리소스 인덱스 계산
    D3D11_TEXTURE2D_DESC sourceDesc;
    pSourceTexture->GetDesc(&sourceDesc); // 원본 텍스처의 설명 가져오기

    UINT uiSrcMipLevels = sourceDesc.MipLevels; // 원본의 실제 밉맵 레벨 수
    // 만약 Initialize 시 MipLevels = 0 으로 설정했다면, 여기서 실제 생성된 밉 레벨 수를 가져와야 합니다.
    // D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; pSourceRT->Get_SRV()->GetDesc(&srvDesc); 와 같은 방식으로 SRV에서 유추하거나,
    // 텍스처 생성 시 실제 계산된 밉 레벨 수를 저장해두는 것이 좋습니다.
    // 여기서는 sourceDesc.MipLevels가 올바른 총 밉맵 수를 가지고 있다고 가정합니다.

    UINT uiSrcSubresource = D3D11CalcSubresource(0, 0, uiSrcMipLevels); // 원본의 밉 레벨 0
    UINT uiDstSubresource = D3D11CalcSubresource(0, 0, 1);          // 대상(백버퍼)의 밉 레벨 0

    // 4. CopySubresourceRegion 호출
    m_pContext->CopySubresourceRegion(
        pDestResource,      // 대상 리소스 (백버퍼 텍스처)
        uiDstSubresource,   // 대상 서브리소스 인덱스
        0, 0, 0,            // 대상의 복사 시작 위치 (x, y, z)
        pSourceResource,    // 원본 리소스 (RT_Final 텍스처)
        uiSrcSubresource,   // 원본 서브리소스 인덱스
        nullptr             // 원본의 복사할 영역 (nullptr은 전체 서브리소스)
    );

    // 5. 사용한 리소스 참조 해제
    Safe_Release(pSourceResource);
    Safe_Release(pDestResource);

    return S_OK;
}

HRESULT CTarget_Manager::Resize(_uint iWinResizeX, _uint iWinResizeY)
{
    for (auto& pair : m_RenderTargets)
    {
        if(FAILED(pair.second->Resize(iWinResizeX, iWinResizeY)))
            return E_FAIL;
    }

    for (auto& pair : m_MRTTags)
    {
        m_MRTs[pair.first].clear();
        list<CRenderTarget*> rtvs;
        D3D11_VIEWPORT firstViewport = {}; // 첫 번째 타겟의 뷰포트를 그룹 뷰포트로 사용
        for (size_t i = 0; i < pair.second.size(); ++i) {
            CRenderTarget* pRT = Find_RenderTarget(pair.second[i]);
            if (nullptr == pRT) {
                MSG_BOX("Render target not found for MRT group.");
                return E_FAIL; // 그룹에 포함될 렌더 타겟이 존재하지 않음
            }
            rtvs.push_back(pRT);

            // 모든 렌더 타겟의 크기가 동일한지 확인 (선택적이지만 권장됨)
            const D3D11_VIEWPORT& currentVP = pRT->Get_Viewport();
            if (i == 0) {
                firstViewport = currentVP;
            }
            else {
                if (firstViewport.Width != currentVP.Width || firstViewport.Height != currentVP.Height) {
                    MSG_BOX("Render targets in MRT group must have the same dimensions.");
                    return E_FAIL; // 필요시 에러 처리
                }
            }
        }
        m_MRTs[pair.first] = rtvs;
    }

    for (auto& pair : m_MRTViewports)
    {
        pair.second.Width = static_cast<FLOAT>(iWinResizeX);
        pair.second.Height = static_cast<FLOAT>(iWinResizeY);
    }
    return S_OK;
}

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag) const
{
    auto iter = m_RenderTargets.find(strTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;
    return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
    auto	iter = m_MRTs.find(strMRTTag);
    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}





HRESULT CTarget_Manager::Bind_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pContantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pContantName);	
}

HRESULT CTarget_Manager::Copy_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pDest)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Copy_Resource(pDest);	
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);	
}

HRESULT CTarget_Manager::Render_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

#endif
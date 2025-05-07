#include "Target_Manager.h" // 경로 확인
#include "RenderTarget.h" // CRenderTarget 클래스 사용 시

BEGIN(Engine)

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

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
    if (Find_RenderTarget(strTargetTag) != nullptr) {
        MSG_BOX("Render Target already exists with this tag.");
        return E_FAIL; // 이미 존재하는 태그
    }

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, eFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    pRenderTarget->Set_DebugName(WideToUtf8(strTargetTag.c_str())); // 디버그 이름 설정
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

    std::vector<ID3D11RenderTargetView*> rtvs;
    D3D11_VIEWPORT firstViewport = {}; // 첫 번째 타겟의 뷰포트를 그룹 뷰포트로 사용

    for (size_t i = 0; i < pTargetTags.size(); ++i) {
        CRenderTarget* pRT = Find_RenderTarget(pTargetTags[i]);
        if (nullptr == pRT) {
            MSG_BOX("Render target not found for MRT group.");
            return E_FAIL; // 그룹에 포함될 렌더 타겟이 존재하지 않음
        }
        rtvs.push_back(pRT->Get_RTV());

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
    m_MRTViewports.emplace(strMRTTag, firstViewport); // 첫 번째 뷰포트 저장

    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
    if (m_pContext == nullptr) return E_FAIL;

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
    m_iNumViews = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; // 최대 개수만큼 가져오기 시도
    m_pContext->OMGetRenderTargets(m_iNumViews, m_pBackupRTVs, &m_pBackupDSV);
    // 실제 바인딩된 뷰 개수 확인 (OMGetRenderTargets는 첫 번째 nullptr에서 멈춤)
    for (m_iNumViews = 0; m_iNumViews < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++m_iNumViews) {
        if (m_pBackupRTVs[m_iNumViews] == nullptr) break;
    }
    _uint numViewports = 1;
    m_pContext->RSGetViewports(&numViewports, &m_BackupViewport); // 보통 뷰포트는 1개

    // 2. MRT 바인딩
    const auto& rtvs = mrtIter->second;
    m_pContext->OMSetRenderTargets(static_cast<_uint>(rtvs.size()), rtvs.data(), pDSV);

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

    // MRT 그룹 내 모든 RTV 클리어 (각 RTV가 속한 CRenderTarget의 Clear 호출)
    // 주의: MRT 그룹 정보에는 RTV 포인터만 있으므로, CRenderTarget 객체를 다시 찾아야 할 수 있음
    //       또는 Add_MRT 시 CRenderTarget 포인터 목록을 저장하는 방식으로 변경 필요
    // 여기서는 m_RenderTargets 맵을 다시 검색하는 방식으로 가정
    for (const auto& rtvPtr : mrtIter->second) {
        bool found = false;
        for (const auto& pair : m_RenderTargets) {
            if (pair.second->Get_RTV() == rtvPtr) {
                if (FAILED(pair.second->Clear())) { // 기본 클리어 색상 사용
                    // 에러 처리
                }
                found = true;
                break;
            }
        }
        if (!found) {
            // RTV에 해당하는 CRenderTarget 객체를 찾지 못한 경우 에러 처리
            MSG_BOX("Could not find RenderTarget object for an RTV in MRT group during clear.");
        }
    }

    return S_OK;
}


ID3D11ShaderResourceView* CTarget_Manager::Get_SRV(const _wstring& strTargetTag) const
{
    CRenderTarget* pRT = Find_RenderTarget(strTargetTag);
    return (pRT != nullptr) ? pRT->Get_SRV() : nullptr;
}

HRESULT CTarget_Manager::Copy_ToBackBuffer(const _wstring& strSourceRenderTargetTag)
{
    // 백업된 RTV가 유효한지 확인 (보통 배열의 첫 번째 요소 사용)
    if (!m_pBackupRTVs[0])
    {
        MSG_BOX("Backup Render Target View [0] is null. Did you call Begin_MRT before?");
        return E_FAIL; // 또는 다른 적절한 에러 코드
    }

    // 1. 원본 RenderTarget 찾기
    CRenderTarget* pSourceRT = Find_RenderTarget(strSourceRenderTargetTag);
    if (!pSourceRT)
    {
        MSG_BOX("Source Render Target not found for copy.");
        return E_FAIL;
    }

    // 2. 원본 RenderTarget에서 리소스(텍스처) 가져오기
    ID3D11Resource* pSourceResource = nullptr;
    ID3D11RenderTargetView* pSourceRTV = pSourceRT->Get_RTV();
    if (!pSourceRTV)
    {
        MSG_BOX("Failed to get RTV from source Render Target.");
        return E_FAIL;
    }
    pSourceRTV->GetResource(&pSourceResource); // AddRef됨
    if (!pSourceResource)
    {
        MSG_BOX("Failed to get Resource from source RTV.");
        return E_FAIL;
    }

    // 3. 대상 리소스(백업된 RTV[0]의 텍스처) 가져오기
    ID3D11Resource* pDestResource = nullptr;
    m_pBackupRTVs[0]->GetResource(&pDestResource); // AddRef됨
    if (!pDestResource)
    {
        MSG_BOX("Failed to get Resource from backup RTV[0].");
        Safe_Release(pSourceResource); // 이전에 얻은 리소스 해제
        return E_FAIL;
    }

    // --- (선택 사항) 리소스 호환성 검사 ---
    // 복사하려면 두 리소스의 크기와 포맷이 같아야 합니다.
    // 디버깅 목적으로 검사 코드를 추가할 수 있습니다.
#ifdef _DEBUG
    {
        D3D11_RESOURCE_DIMENSION srcDim, destDim;
        pSourceResource->GetType(&srcDim);
        pDestResource->GetType(&destDim);

        if (srcDim == D3D11_RESOURCE_DIMENSION_TEXTURE2D && destDim == D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        {
            ID3D11Texture2D* pSrcTex = nullptr;
            ID3D11Texture2D* pDestTex = nullptr;
            if (SUCCEEDED(pSourceResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pSrcTex)) &&
                SUCCEEDED(pDestResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pDestTex)))
            {
                D3D11_TEXTURE2D_DESC srcDesc, destDesc;
                pSrcTex->GetDesc(&srcDesc);
                pDestTex->GetDesc(&destDesc);

                if (srcDesc.Width != destDesc.Width || srcDesc.Height != destDesc.Height || srcDesc.Format != destDesc.Format)
                {
                    OutputDebugStringA("Warning: CopyResource source and destination dimensions or format mismatch.\n");
                    // 필요시 여기서 에러 처리 (return E_FAIL)
                }
            }
            Safe_Release(pSrcTex);
            Safe_Release(pDestTex);
        }
        else
        {
            OutputDebugStringA("Warning: CopyResource source or destination is not a Texture2D.\n");
            // 다른 리소스 타입 처리 또는 에러
        }
    }
#endif // _DEBUG

    // 4. 리소스 복사 (Source -> Destination)
    // 복사할 내용(pSourceResource)을 대상(pDestResource)으로 복사합니다.
    m_pContext->CopyResource(pDestResource, pSourceResource);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_pContext->PSSetShaderResources(0, 1, nullSRV); // 사용한 슬롯을 명시적으로 NULL로 설정

    // 5. 사용한 리소스 참조 해제 (GetResource로 AddRef된 것들)
    Safe_Release(pSourceResource);
    Safe_Release(pDestResource);

    return S_OK;
}

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag) const
{
    auto iter = m_RenderTargets.find(strTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;
    return iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTarget_Manager");
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

void CTarget_Manager::Free()
{
    // MRT 그룹 정보 정리 (포인터만 가지고 있으므로 별도 해제 불필요)
    m_MRTs.clear();
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

END
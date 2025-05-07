#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CRenderTarget;
class ENGINE_DLL CTarget_Manager final : public CBase
{
private:
    CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CTarget_Manager() = default;

public:
    HRESULT Initialize();
    // 단일 렌더 타겟 생성 (기존 CRenderTarget 역할 일부 흡수 또는 CRenderTarget 사용)
    HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);

    // MRT 그룹 생성
    // pTargetTags: 이 그룹에 포함될 렌더 타겟들의 이름(미리 Add_RenderTarget으로 생성되어 있어야 함)
    HRESULT Add_MRT(const _wstring& strMRTTag, const std::vector<_wstring>& pTargetTags);

    // MRT 그룹 바인딩 (렌더링 시작 전 호출)
    HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV); // 메인 깊이 버퍼 사용

    // MRT 그룹 바인딩 해제 (백 버퍼로 복귀 시 호출)
    HRESULT End_MRT(ID3D11RenderTargetView* pBackBufferRTV, ID3D11DepthStencilView* pMainDSV, const D3D11_VIEWPORT& vp);

    // 특정 렌더 타겟 클리어
    HRESULT Clear_RenderTarget(const _wstring& strTargetTag, _float4* pClearColor = nullptr);
    // MRT 그룹의 모든 렌더 타겟 클리어
    HRESULT Clear_MRT(const _wstring& strMRTTag);

    // 특정 렌더 타겟의 SRV 가져오기
    ID3D11ShaderResourceView* Get_SRV(const _wstring& strTargetTag) const;

    // (선택 사항) 특정 렌더 타겟 객체 직접 반환
    // CRenderTarget* Get_RenderTarget(const _wstring& strTargetTag) const;

    HRESULT Copy_ToBackBuffer(const _wstring& strSourceRenderTargetTag);

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    // 생성된 렌더 타겟 정보 저장 (CRenderTarget 객체 또는 직접 리소스 관리)
    // 예시: CRenderTarget 객체 사용 시
    std::map<const _wstring, CRenderTarget*> m_RenderTargets;

    // MRT 그룹 정보 저장 (예: 그룹 이름 -> RTV 포인터 배열)
    std::map<const _wstring, std::vector<ID3D11RenderTargetView*>> m_MRTs;
    // MRT 그룹에 맞는 뷰포트 정보 저장 (첫 번째 타겟 기준 또는 별도 저장)
    std::map<const _wstring, D3D11_VIEWPORT> m_MRTViewports;

    // 백업용 포인터 (Begin_MRT 호출 시 저장)
    _uint                   m_iNumViews = 0;
    ID3D11RenderTargetView* m_pBackupRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ID3D11DepthStencilView* m_pBackupDSV = nullptr;
    D3D11_VIEWPORT          m_BackupViewport = {};

private:
    CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag) const;

public:
    static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
#pragma once
#include "Base.h"
#include <d3d11.h>
#include <DirectXMath.h> // For XMFLOAT4/_float4 if not included via Base.h/Engine_Typedef.h

BEGIN(Engine)

class ENGINE_DLL CRenderTarget final : public CBase
{
private:
    // 생성자를 private으로 변경하여 Create 함수 사용 강제
    CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderTarget() = default;

public:
    // Getter 함수들
    ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }
    ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
    const D3D11_VIEWPORT& Get_Viewport() const { return m_Viewport; }

public:
    // 초기화 함수
    HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);

    // 렌더 타겟 클리어 함수
    HRESULT Clear(_float4* pClearColor = nullptr); // 인자가 없으면 멤버 변수의 클리어 색상 사용

    // 렌더 타겟 바인딩 함수 (깊이 버퍼는 외부에서 받거나, 관리하지 않음)
    void Bind(ID3D11DepthStencilView* pDSV); // 메인 깊이 버퍼를 주로 사용

    // 특정 이름으로 디버그 객체 이름 설정 (선택 사항)
    void Set_DebugName(const std::string& name);

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    ID3D11Texture2D* m_pTexture = nullptr;
    ID3D11RenderTargetView* m_pRTV = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;

    _float4                 m_vClearColor = {};
    _uint                   m_iWidth = 0;
    _uint                   m_iHeight = 0;
    DXGI_FORMAT             m_eFormat = DXGI_FORMAT_UNKNOWN;
    D3D11_VIEWPORT          m_Viewport = {};

public:
    // 생성 함수
    static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);
    virtual void Free() override;
};

END
#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CRenderTarget;
class CVIBuffer_Rect;
class CShader;
class ENGINE_DLL CTarget_Manager final : public CBase
{
#pragma region Constructor&Destructor
private:
    CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CTarget_Manager() = default;
public:
    HRESULT Initialize();
public:
    static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
#pragma endregion


public:
    HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);

    HRESULT Add_MRT(const _wstring& strMRTTag, const std::vector<_wstring>& pTargetTags);
    HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isDepthClear = false);
    HRESULT End_MRT(ID3D11RenderTargetView* pBackBufferRTV, ID3D11DepthStencilView* pMainDSV, const D3D11_VIEWPORT& vp);
    HRESULT Clear_RenderTarget(const _wstring& strTargetTag, _float4* pClearColor = nullptr);
    HRESULT Clear_MRT(const _wstring& strMRTTag);

    ID3D11ShaderResourceView* Get_SRV(const _wstring& strTargetTag) const;
    CRenderTarget* Get_RenderTarget(const _wstring& strTargetTag) const;

    HRESULT Copy_ToBackBuffer(const _wstring& strSourceRenderTargetTag);
    HRESULT Resize(_uint iWinResizeX, _uint iWinResizeY);
    map<const _wstring, CRenderTarget*> Get_RenderTargetMap() { return m_RenderTargets; }
    HRESULT Bind_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pContantName);
    HRESULT Copy_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pDest);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT Render_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_Debug_Collider(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
private:
    map<const _wstring, CRenderTarget*> m_RenderTargets;
    map<const _wstring, list<CRenderTarget*>> m_MRTs;
    map<const _wstring, vector<_wstring>> m_MRTTags;
    map<const _wstring, D3D11_VIEWPORT> m_MRTViewports;

private:
    _uint                   m_iNumViews = 0;
    ID3D11RenderTargetView* m_pBackupRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ID3D11DepthStencilView* m_pBackupDSV = nullptr;
    D3D11_VIEWPORT          m_BackupViewport = {};

private:
    CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag) const;
    list<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);
    //vector<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);


};

NS_END
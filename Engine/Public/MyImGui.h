#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CMyImGui : public CBase
{
protected:
	CMyImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyImGui() = default;
public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Pre_Render();
	virtual HRESULT Render();
	virtual HRESULT Post_Render();
public:
	static CMyImGui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	void Free();
protected:
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
protected:
	_bool					m_bShow = { false };
};
END
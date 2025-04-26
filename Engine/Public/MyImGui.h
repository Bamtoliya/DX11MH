#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CMyImGui : public CBase
{
protected:
	CMyImGui(ID3D11Device*, ID3D11DeviceContext*);
	virtual ~CMyImGui() = default;
public:
	virtual HRESULT Initialize(HWND);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();
public:
	static CMyImGui* Create(HWND, ID3D11Device*, ID3D11DeviceContext*);
	void Free();
protected:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };
protected:
	_bool					m_bShow = { false };
};
END
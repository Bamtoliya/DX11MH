#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CMyImGui : public CBase
{
protected:
	CMyImGui();
	virtual ~CMyImGui() = default;
public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();
public:
	static CMyImGui* Create();
	void Free();
protected:
	CGameInstance*			m_pGameInstance = { nullptr };
protected:
	_bool					m_bShow = { false };
};
END
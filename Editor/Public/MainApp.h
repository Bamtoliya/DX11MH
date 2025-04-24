#pragma once
#include "Base.h"
#include "Editor_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Editor)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;
};
END
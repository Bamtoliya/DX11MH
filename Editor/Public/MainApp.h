#pragma once
#include "Base.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
class CSound_Manager;
class CMyImGui_Manager;
NS_END

NS_BEGIN(Editor)
class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
	HRESULT Render();

public:
	HRESULT Register_Class_Types();
	HRESULT Start_Level(LEVEL eStartLevel);
	HRESULT Ready_Prototype_Components();
private:
	HRESULT CreateImGui();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };
	CSound_Manager*			m_pSound_Manager = { nullptr };

public:
	static CMainApp* Create();
	virtual void Free() override;
};
NS_END
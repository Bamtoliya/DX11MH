#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CGameObject;
class CGameInstance;
class ENGINE_DLL CState abstract :  public CBase
{
protected:
	CState(CGameObject* pOwner);
	virtual ~CState() = default;
public:
	virtual HRESULT Initialize() = 0;
	virtual void Enter() = 0;
	virtual void Execute(_float fTimeDelta) = 0;
	virtual void HandleInput() = 0;
	virtual void Exit() = 0;
	virtual void Free();
	virtual HRESULT RenderImGui() { ImGui::PushID(this); ImGui::Text(m_szName); ImGui::PopID(); return S_OK; }
protected:
	CGameObject*	m_pOwner = { nullptr };
	CGameInstance*	m_pGameInstance = { nullptr };
	_char			m_szName[MAX_PATH] = {};
};
NS_END
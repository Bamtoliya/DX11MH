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
	virtual void HandleInput(_float fTimeDelta) = 0;
	virtual void Exit() = 0;
	virtual void Free();
	virtual HRESULT RenderImGui() { ImGui::PushID(this); ImGui::Text(m_szName); ImGui::PopID(); return S_OK; }
	_bool Get_IsLoop() const { return m_bLoop; }
	_float Get_BlendDuration() const { return m_fBlendDuration; }
protected:
	CGameObject*	m_pOwner = { nullptr };
	CGameInstance*	m_pGameInstance = { nullptr };
	_char			m_szName[MAX_PATH] = {};
	_bool			m_bLoop = { true };
	_float			m_fBlendDuration = { 0.5f };
};
NS_END
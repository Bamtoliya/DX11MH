#pragma once
#include "Engine_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class  CGame_Manager :  public CBase
{
private:
	CGame_Manager();
	virtual ~CGame_Manager() = default;
public:
	virtual void Free() override;
	HRESULT Initialize();
	static CGame_Manager* Create();
public:
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();
public:
	void RegisterPlayer(CGameObject* pGameObject) { m_pCurrentPlayer = pGameObject; }
	void UnregisterPlayer() { m_pCurrentPlayer = nullptr; }
	CGameObject* GetCurrentPlayer() const { return m_pCurrentPlayer; }
private:
	void Culling_Collider(_float fDistance);

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CGameObject*	m_pCurrentPlayer = { nullptr };
};

NS_END
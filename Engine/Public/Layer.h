#pragma once

#include "Base.h"

/* 객체들을 모아놓는다. */

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	CGameObject* Get_GameObjectById(_uint iId);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT RenderImGui(const wstring& strLayerTag);
	HRESULT ToggleVisible();
private:
	_wstring							m_strTagName = {};
	list<class CGameObject*>			m_GameObjects;
	_bool								m_bVisible = { true };

public:
	static CLayer* Create();
	virtual void Free();
};

NS_END
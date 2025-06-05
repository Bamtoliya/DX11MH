#pragma once

#include "Base.h"

/* 레벨별로 구분하여 사본객체를 보관한다. */
/* 리스트들로 보관한다. 리스트 == 객체들 == 내가 나누고 싶은 기준에따라 그룹짓는다. */

NS_BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	class CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, _uint iPartID, const _wstring& strComponentTag, _uint iIndex);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, simdjson::ondemand::object& jsonData);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, _uint iGameObjectID, void* pArg = nullptr);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, class CGameObject* parentGameObject, simdjson::ondemand::object& jsonData);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
public:
	void Clear(_uint iLevelIndex);
	class CGameObject* Get_GameObjectById(_uint iId);
	class CGameObject* Get_GameObjectByName(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strGameObjectName);
	class CGameObject* Get_GameObjectByName(const _wstring& strGameObjectName);
public:
	HRESULT Move_GameObjectToLayer(_uint iObjectID, _uint iTargetLevelIndex, const _wstring& strTargetLayerTag);
	HRESULT Remove_GameObjectFromLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	HRESULT Remove_GameObjectFromLayer(_uint iLevelIndex, CGameObject* pGameObject); // GameObject의 현재 레이어를 찾아 제거
public:
	HRESULT RenderImGui();
	HRESULT ToggleLayerVisible(_uint iLevelIndex, const _wstring& strLayerTag);
	HRESULT Render_ImGuiLayerList(_uint iLevelIndex, _char* szBuffer, _uint& iGameObjectID, _uint iBufferSize);
	HRESULT Save_GameObjects(json& outJson);
	HRESULT Save_GameObjects(_uint iLevelIndex, json& outJson);
private:
	static _uint						m_oID;
	_uint								m_iNumLevels = {};
	map<const _wstring, class CLayer*>* m_pLayers = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };

private:
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END
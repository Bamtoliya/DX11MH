#pragma once

#include "Base.h"
#include "Transform.h"

static _uint objID = 1;

NS_BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		_tchar		szName[MAX_PATH];
	}GAMEOBJECT_DESC;
#pragma region Constructor/Destructor
protected:
	CGameObject(ID3D11Device* m_pDevice,ID3D11DeviceContext* m_pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) = 0;
	virtual CGameObject* Clone(CGameObject* pGameObject, void* pArg = nullptr);
	virtual CGameObject* Clone(CGameObject* pGameObject, simdjson::ondemand::object& jsonData);
	virtual void Free() override;
protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	
#pragma endregion


#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render();
	virtual HRESULT Post_Render();	
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT RenderImGuiComponents();
	virtual HRESULT RenderImGuiObjects();
#pragma endregion


#pragma region Functions
public:
	_vector		Get_Position() { return m_pTransformCom->Get_State(STATE::POSITION); }
	CComponent* Get_Component(const _wstring& strComponentTag);
	_uint Get_ID() const { return m_oID; }
	void Set_ID(_uint oID) { m_oID = oID; }
	_wstring Get_Name() const { return m_szName; }
	void Set_Name(const wstring& str) { lstrcpy(m_szName, str.c_str()); }
	_bool Get_Picked() { return m_bPicked; }
	void Set_Picked(_bool bPicked) { m_bPicked = bPicked; }
	_bool Get_Visible() { return m_bVisible; }
	void Set_Visible(_bool bVisible) { m_bVisible = bVisible; for (auto& Pair : m_Childs) { Pair.second->Set_Visible(bVisible); } }
	_bool Get_Dead() { return m_bDead; }
	void Set_Dead(_bool bDead = true) { m_bDead = bDead; }
	_bool Get_Active() { return m_bActive; }
	void Set_Active(_bool bActive) { m_bActive = bActive; }
	_bool Get_IsDestroy() { return m_bToDestroy; }
	void Set_IsDestroy(_bool bDestroy = true) { m_bToDestroy = bDestroy; }
	_wstring Get_LayerTag() { return m_strLayerTag; }
	void Set_LayerTag(const _wstring& strLayerTag) { m_strLayerTag = strLayerTag; }
public:
	CGameObject* Get_Parent() { return m_pParent; };
	void Set_Parent(CGameObject* pParent) { 
		if (m_pParent)
		{
			m_pParent->Remove_ChildByName(m_szName);
		}
		m_pParent = pParent;
		if (m_pParent)
		{
			m_pParent->Add_Child(m_szName, this);
		}
			
	}
	HRESULT Add_Child(const _wstring& strChildTag, CGameObject* pChild);
	HRESULT Add_ChildById(const _wstring& strChildTag, _uint oId);
	HRESULT Remove_ChildByName(const _wstring& strChildTag) { Safe_Release(m_Childs[strChildTag]); m_Childs.erase(strChildTag); return S_OK; }
public:

	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Remove_Component(const _wstring& strComponentTag);

#pragma region Collides
public:
	virtual void OnCollisionEnter(PxActor* pOther, const PxContactPair& cp) {}
	virtual void OnCollisionOverlapping(PxActor* pOther) {}
	virtual void OnCollisionExit(PxActor* pOther) {}
#pragma endregion



#pragma endregion

#pragma region Serialize
	virtual HRESULT Serialize(json& json) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion


#pragma region Variables
protected:
	_bool									m_bPicked = { false };
	_uint									m_oID = { 0 };
	CTransform*								m_pTransformCom = { nullptr };
	CGameObject*							m_pParent = { nullptr };
	map<const _wstring, class CComponent*>	m_Components	= {};
	map<const _wstring, class CGameObject*> m_Childs		= {};
	_wstring 								m_strLayerTag = {};
	_tchar									m_szName[MAX_PATH] = {};

protected:
	list<_wstring>							m_Tags = {};
protected:
	_bool									m_bVisible = { true };	
	_bool									m_bActive = { true };
	_bool									m_bDead = { false };
	_bool									m_bToDestroy = { false };

protected:
	_bool									m_bLateInitialized = { false };
#pragma endregion

};
NS_END
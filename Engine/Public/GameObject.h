#pragma once

#include "Base.h"
#include "Transform.h"

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
	virtual CGameObject* Clone(CGameObject* pGameObject, void* pArg = nullptr);
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
	virtual HRESULT RenderImGuiComponents();
	virtual HRESULT RenderImGuiObjects();
#pragma endregion


#pragma region Functions
public:
	CComponent* Get_Component(const _wstring& strComponentTag);
	_uint Get_ID() const { return m_oID; }
	void Set_ID(_uint oID) { m_oID = oID; }
	wstring Get_Name() const { return m_szName; }
	void Set_Name(const wstring& str) { lstrcpy(m_szName, str.c_str()); }
	_bool Get_Picked() { return m_bPicked; }
	void Set_Picked(_bool bPicked) { m_bPicked = bPicked; }
	_bool Get_Visible() { return m_bVisible; }
	void Set_Visible(_bool bVisible) { m_bVisible = bVisible; }
public:
	CGameObject* Get_Parent() { return m_pParent; };
	HRESULT Add_Child(const _wstring& strChildTag, CGameObject* pChild);
public:

	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	//HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CGameObject* pOwner, void* pArg = nullptr);

#pragma endregion

#pragma region Serialize
	//Save
	HRESULT Serialize();
	//Load
	HRESULT Deserialize();
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
	_bool									m_bVisible = { true };	
#pragma endregion

};
NS_END
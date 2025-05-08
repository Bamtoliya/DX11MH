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
	virtual HRESULT RenderImGui();
#pragma endregion

#pragma region Functions
public:
	CComponent* Get_Component(const _wstring& strComponentTag);
	_uint Get_ID() const { return m_oID; }
	void Set_ID(_uint oID) { m_oID = oID; }
protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

#pragma endregion

#pragma region Variables
	_uint									m_oID = { 0 };
	_wstring								m_strName;
	CTransform*								m_pTransformCom = { nullptr };
	CGameObject*							m_pParent = { nullptr };
	_wstring								m_strTag = {};
	map<const _wstring, class CComponent*>	m_Components	= {};
	map<const _wstring, class CGameObject*> m_Childs		= {};
	_wstring 	m_strLayerTag;
	_tchar		m_szName[MAX_PATH];


	_bool									m_bRelative = { false };
	_bool									m_bAbsolute = { false };
#pragma endregion

};
NS_END
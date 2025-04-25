#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CGameObject abstract : public CBase
{

#pragma region Constructor/Destructor
protected:
	CGameObject(ID3D11Device* m_pDevice,ID3D11DeviceContext* m_pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
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
#pragma endregion

#pragma region Functions
public:
	class CComponent* Get_Component(const wstring& strComponentTag) {
		auto it = m_Components.find(strComponentTag);
		return it != m_Components.end() ? it->second : nullptr;
	}
protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
#pragma endregion

#pragma region Variables
protected:
	_wstring m_strName;

	class CTransform*			m_pTransformCom = { nullptr };
	map<const _wstring, class CComponent*>	m_Components = {};

	CGameObject* m_pParent = { nullptr };
	map<const _wstring, class CGameObject*> m_Childs = {};
	_wstring m_strLayerTag;
	
#pragma endregion;
};
END
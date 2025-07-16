#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CComponent abstract : public CBase
{
public:
	typedef struct tagComponentDesc {
	} COMPONENT_DESC;
#pragma region Constructor/Destructor
protected:
	CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Late_Initialize() { if (m_bLateInitialized) return S_OK; m_bLateInitialized = true; return S_OK; }
public:
	virtual void Priority_Update(_float fTimeDelta) { if (!m_bActive) return; }
	virtual void Update(_float fTimeDelta) { if (!m_bActive) return; }
	virtual void Late_Update(_float fTimeDelta) { if (!m_bActive) return; }
	virtual HRESULT Render() { return S_OK; }
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual CComponent* Clone(class CGameObject* pGameObject, void* pArg);
	virtual void Free();
public:
	class CGameObject* Get_OwnerGameObject() const { return m_pOwner; }
	void Set_OwnerGameObject(class CGameObject* pGameObject); 
#pragma endregion
public:
	ID3D11DeviceContext* GetInitContext() {
		return m_pDeferredContext ? m_pDeferredContext : m_pContext;
	}
public:
	_bool Get_Active() { return m_bActive; }
	void Set_Active(_bool bActive = true) { m_bActive = bActive; }
public:
	virtual HRESULT RenderImGui() { return S_OK; }
	virtual void* Dump() { return nullptr; }
public:
	virtual HRESULT Serialize(json& data);
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData);
#pragma region Variables
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11DeviceContext* m_pDeferredContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
protected:
	_tchar m_szName[MAX_PATH] = { L"" };	
	class CGameObject* m_pOwner = { nullptr };
#pragma endregion
protected:
	_bool m_bIsCloned = { false };
	_bool m_bActive = { true };
	_bool	m_bLateInitialized = { false };
public:
	const _tchar* GetName() const { return m_szName; }
	void SetName(const _tchar* szName) { wcscpy_s(m_szName, szName); }
};
NS_END
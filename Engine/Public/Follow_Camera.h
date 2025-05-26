#pragma once
#include "Camera.h"

NS_BEGIN(Engine)
class CGameObject;
class ENGINE_DLL CFollow_Camera : public CCamera
{
#pragma region Constructor/Destructor
protected:
	CFollow_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFollow_Camera(const CFollow_Camera& Prototype);
	virtual ~CFollow_Camera() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static	CFollow_Camera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CFollow_Camera* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Variables
private:
	CGameObject*	m_pTarget = { nullptr };
	_float			m_fDistance = { 100.f };
	_float			m_fSmoothFactor = { 0.1f };
#pragma endregion
#pragma region Helper
public:
	CGameObject* Get_Target() { return m_pTarget; }
	void Set_Target(CGameObject* pTarget) { if (m_pTarget) Safe_Release(m_pTarget); m_pTarget = pTarget; Safe_AddRef(m_pTarget); }
	void Clear_Target() { Safe_Release(m_pTarget); }
#pragma endregion




#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;
	virtual HRESULT Render() override;
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
};
NS_END

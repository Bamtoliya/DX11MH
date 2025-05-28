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
	_float m_fCameraYaw = { 0.f };   // Y축 기준 수평 회전
	_float m_fCameraPitch = { XM_PIDIV4 }; // X축 기준 수직 회전
	// 필요하다면 수직 각도 제한 값
	const _float m_fMinPitch = { -XM_PIDIV2 + 0.2f }; // 약 -80도
	const _float m_fMaxPitch = { XM_PIDIV2 - 0.2f };  // 약 +80도
#pragma endregion
#pragma region Helper
public:
	CGameObject* Get_Target() { return m_pTarget; }
	void Set_Target(CGameObject* pTarget) { if (m_pTarget) Safe_Release(m_pTarget); m_pTarget = pTarget; Safe_AddRef(m_pTarget); }

	// Camera Orbit Parameters
	void AddYaw(_float fDelta);
	void AddPitch(_float fDelta);
	void SetDistance(_float fDistance) { m_fDistance = fDistance; if (m_fDistance < 0.1f) m_fDistance = 0.1f; }
	_float GetDistance() const { return m_fDistance; }
	void AdjustDistance(_float fDeltaDistance); // For zooming

	// Smooth Factor
	void SetSmoothFactor(_float fFactor) { m_fSmoothFactor = fFactor; }
	_float GetSmoothFactor() const { return m_fSmoothFactor; }
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

#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
class CVIBuffer_Spline;
class CSpline_Component;
class ENGINE_DLL CGameObject_Spline : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CGameObject_Spline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_Spline(const CGameObject_Spline& Prototype);
	virtual ~CGameObject_Spline() = default;
public:
	static CGameObject_Spline* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
	virtual void Free() override;
#pragma endregion
#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData) override;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
#pragma endregion
#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Render() override;
private:
	HRESULT UpdateSplineBuffer();
#pragma endregion

public:
	virtual void OnCollisionEnter(PxActor* pOther, const PxContactPair& cp) override;
	virtual void OnCollisionOverlapping(PxActor* pOther)  override;
	virtual void OnCollisionExit(PxActor* pOther)  override;

protected:
	CShader*			m_pShaderCom = { nullptr };
	//CShader*			m_pShaderCom = { nullptr };
	CRigidBody*			m_pRigidBodyCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CVIBuffer_Spline*	m_pVIBufferCom = { nullptr };
	CSpline_Component*	m_pSplineCom = { nullptr };

	ID3D11Buffer*				m_pSplineDataBuffer = { nullptr };
	ID3D11ShaderResourceView*	m_pSplineDataSRV = { nullptr };
};
NS_END
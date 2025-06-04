#pragma once
#include "Component.h"
#include "Effects.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCollider : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;
public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif

	void Update(_fmatrix WorldMatrix);
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
private:
	COLLIDER			m_eType = { COLLIDER::END };
	class CBounding*	m_pBounding = { nullptr };
	_bool				m_isColl = { false };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
#endif
};
NS_END

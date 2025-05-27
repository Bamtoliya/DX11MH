#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CController : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CController(const CController& Prototype);
	virtual ~CController() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CController* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Variables
	_uint			m_iNumActions = {};
	vector<_byte>	m_pKeys = {};
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
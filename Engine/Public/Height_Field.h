
#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CHeight_Field : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CHeight_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeight_Field(const CHeight_Field& Prototype);
	virtual ~CHeight_Field() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CHeight_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg)  override;
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
	virtual HRESULT Render() override;
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
private:
	PxHeightField*			m_pHeightField = { nullptr };
	_float				  m_fHfHeightScale = 1.f;   // HeightField의 Y축 스케일
	_float				  m_fHfRowScale = 1.f;    // HeightField의 X축 스케일 (정점 간 간격)
	_float				  m_fHfColumnScale = 1.f; // HeightField의 Z축 스케일 (정점 간 간격)
};
NS_END

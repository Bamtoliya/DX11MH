#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLight final : public CComponent
{
public:
	typedef struct tagLightFilePath
	{
		_wstring strLightPath;
	} LIGHT_INIT;
#pragma region Constructor/Destructor
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight(const CLight& Prototype);
	virtual ~CLight() = default;
public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//	virtual CLight* Clone(const LIGHT_DESC& LightDesc);
	virtual CLight* Clone(void* pArg) override;
	virtual void	Free() override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion

#pragma region Save&Load
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object & jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion
public:
	const LIGHT_DESC* Get_LightDesc() const { return &m_tInfo; }

public:
	void Update();
public:
	HRESULT RenderImGui();
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHT_DESC	m_tInfo = {};

};

NS_END


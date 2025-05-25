#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
#pragma region Constructor/Destructor
private:
	CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cube(const CVIBuffer_Cube& Prototype);
	virtual ~CVIBuffer_Cube() = default;
public:
	static CVIBuffer_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Free() override;
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(const json& data) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion
};

NS_END
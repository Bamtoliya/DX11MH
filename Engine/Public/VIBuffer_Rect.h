#pragma once
#include "VIBuffer.h"
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
#pragma region Constructor/Destructor
protected:
	CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& Prototype);
	virtual ~CVIBuffer_Rect() = default;
public:
	static	CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual void Free();
#pragma endregion

#pragma region Save&Load
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(const json& data) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
};
NS_END
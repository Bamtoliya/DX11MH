#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
#pragma region Constructor/Destructor
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;
public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
private:
	HRESULT LoadHeightMapPixelsFromDDS(const _tchar* filePath, _uint& width, _uint& height, vector<_float>& heightData);
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	_uint			m_iNumVerticesX = {};
	_uint			m_iNumVerticesZ = {};
	_wstring		m_strHeightMapFilePath = {};
	_float			m_fHeightScale = { 1000.0f };
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object & jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion
private:

public:
	virtual HRESULT RenderImGui() override;
};

NS_END
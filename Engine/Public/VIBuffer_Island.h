#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Island final : public CVIBuffer
{
#pragma region Constructor/Destructor
private:
	CVIBuffer_Island(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Island(const CVIBuffer_Island& Prototype);
	virtual ~CVIBuffer_Island() = default;
public:
	static CVIBuffer_Island* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CVIBuffer_Island* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
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
	_uint			m_iNumMapss = {};
	_uint			m_iGridWidth = {};
	_uint			m_iGridHeight = {};
	_float			m_fHeightScale = { 1024.0f };
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion
private:

public:
	virtual HRESULT RenderImGui() override;
};

NS_END#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Island final : public CVIBuffer
{
#pragma region Constructor/Destructor
private:
	CVIBuffer_Island(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Island(const CVIBuffer_Island& Prototype);
	virtual ~CVIBuffer_Island() = default;
public:
	static CVIBuffer_Island* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CVIBuffer_Island* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
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
	_float			m_fHeightScale = { 1024.0f };
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
#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
#pragma region Constructor/Destructor
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	vector<ID3D11ShaderResourceView*>				m_SRVs;
	_uint											m_iNumTextures = {};
	_wstring										m_strTextureFilePath = {};
#pragma endregion

#pragma region Save&Load
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(const json& data) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion


public:
	ID3D11ShaderResourceView* Get_SRV(_uint index) { return m_SRVs[index]; }
	/* 쉐이더에 텍스쳐를 셋한다. */
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	virtual HRESULT RenderImGui() override;
};

NS_END
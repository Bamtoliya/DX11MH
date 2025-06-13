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
	ID3D11Texture2D* m_pTextureArrayResource = { nullptr };
	_uint											m_iNumTextures = {};
	_wstring										m_strTextureFilePath = {};
	
	// 인덱스 맵 Texture1D 및 SRV
	map<_uint, _uint>								m_originalToActualIndexMap = {}; // 원본 인덱스 -> 실제 SRV 인덱스 매핑
	ID3D11Texture1D* m_pIndexMapTexture = { nullptr };
	ID3D11ShaderResourceView* m_pIndexMapSRV = { nullptr };
#pragma endregion

#pragma region Save&Load
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object & jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion

public:
	// Texture2DArray의 경우, iTextureIndex는 보통 0이 됩니다.
	ID3D11ShaderResourceView* Get_SRV(_uint index = 0) {
		if (!m_SRVs.empty() && index == 0) return m_SRVs[0];
		return nullptr;
	}
	//ID3D11ShaderResourceView* Get_SRV(_uint index) { return m_SRVs[index]; }
	/* 쉐이더에 텍스쳐를 셋한다. */
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_ShaderResourceArray(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_IndexMap(CShader* pShader, const _char* pConstantName);
	virtual HRESULT RenderImGui() override;
};

NS_END
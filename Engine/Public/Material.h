#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
#pragma region Structs/Enum
public:
	typedef struct tagMaterialFromModel
	{
		_uint iNumTexture = {};
		_char* szTextureName = {};
	} MODEL_MAT;

	enum class TEXTURE_TYPE : _uint
	{
		DIFFUSE = 0,
		SPECULAR,
		NORMAL,
		PARAMETER,
		END
	};
#pragma endregion

#pragma region Constructor/Destructor
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;
public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, ifstream& ifs);
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
public:
	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	HRESULT Initialize(const _char* pModelFilePath, ifstream& ifs);
public:
	virtual void Free() override;
#pragma endregion
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
	HRESULT RenderImGui();

#pragma region Variables
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*>			m_SRVs[AI_TEXTURE_TYPE_MAX] = {};
#pragma endregion
};

NS_END
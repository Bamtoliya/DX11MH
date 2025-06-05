#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CTextureArray;
class CVIBuffer_Terrain;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CTerrain final : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;
public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
private:
	HRESULT Ready_Components();
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = { nullptr };
	CRigidBody*				m_pRigidBodyCom = { nullptr };
	CTexture*				m_pSplatMapTextureCom	= { nullptr }; // 스플랫맵 텍스처
	CTexture*				m_pWaterTextureCom		= { nullptr }; // 물 텍스처
	CTextureArray*			m_pBaseDiffuseCom		= { nullptr };
	CTextureArray*			m_pBaseParameterCom		= { nullptr };
	CTextureArray*			m_pDetailDiffuseCom		= { nullptr };
	CTextureArray*			m_pDetailHeightCom		= { nullptr };
	CTextureArray*			m_pDetailNormalCom		= { nullptr };
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object & jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
private:
	HRESULT Bind_ShaderResources();
public:
	virtual HRESULT Render();
#pragma endregion

	


};

NS_END
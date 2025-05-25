#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
class ENGINE_DLL CGameObject_Skybox final : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CGameObject_Skybox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_Skybox(const CGameObject_Skybox& Prototype);
	virtual ~CGameObject_Skybox() = default;
public:
	static CGameObject_Skybox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData)override;
private:
	HRESULT Ready_Components();
public:
	virtual void Free();
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
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
public:
	virtual HRESULT Render()override;
#pragma endregion
#pragma region Variables
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
private:
	_uint					m_iSkyboxID = { 0 };
#pragma endregion



	// CGameObject을(를) 통해 상속됨
	

};
NS_END
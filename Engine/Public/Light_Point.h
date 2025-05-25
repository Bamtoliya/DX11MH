#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class CLight;
class CShader;
class CTexture;
class CVIBuffer_Rect;
class ENGINE_DLL CLight_Point : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CLight_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight_Point(const CLight_Point& Prototype);
	virtual ~CLight_Point() = default;
public:
	static CLight_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLight_Point* Clone(void* pArg = nullptr) override;
	virtual CLight_Point* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData) override;
private:
	HRESULT Ready_Components();
public:
	virtual void Free() override;
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render() override;
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render() override;
#pragma endregion

#pragma region Variables
protected:
	CLight* m_pLightCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object & jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion
};

NS_END
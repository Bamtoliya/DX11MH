#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CSonic final : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CSonic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSonic(const CSonic& Prototype);
	virtual ~CSonic() = default;
public:
	static CSonic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
private:
	HRESULT Ready_Components();
public:
	virtual void Free() override;
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(const json& data) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(const simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
public:
	HRESULT Bind_ShaderResources();
	virtual HRESULT Render();
#pragma endregion

#pragma region Components
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
#pragma endregion

	
};

NS_END
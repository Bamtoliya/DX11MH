#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CMonster final : public CGameObject
{
#pragma region Constructor / Destructor
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;
public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
private:
	HRESULT Ready_Components();
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(const json& data) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(const json& data) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	HRESULT Bind_ShaderResources();
#pragma endregion

};

NS_END
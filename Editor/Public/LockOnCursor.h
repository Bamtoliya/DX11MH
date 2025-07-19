#pragma once

#include "Editor_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Editor)

class CLockOnCursor final : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CLockOnCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLockOnCursor(const CLockOnCursor& Prototype);
	virtual ~CLockOnCursor() = default;
public:
	static CLockOnCursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion

#pragma region Variables
private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
#pragma endregion
};

NS_END
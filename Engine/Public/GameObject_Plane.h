#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class ENGINE_DLL CGameObject_Plane : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CGameObject_Plane(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	CGameObject_Plane(const CGameObject_Plane& Prototype);
	virtual ~CGameObject_Plane() = default;
public:
	static CGameObject_Plane* Create(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
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

protected:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
};

NS_END
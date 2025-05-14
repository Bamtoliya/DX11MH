#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Cube;
class ENGINE_DLL CGameObject_Cube : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CGameObject_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_Cube(const CGameObject_Cube& Prototype);
	virtual ~CGameObject_Cube() = default;
public:
	static CGameObject_Cube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject_Cube* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
#pragma endregion
#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Components();
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
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
};

NS_END
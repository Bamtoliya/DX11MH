#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class CVIBuffer;
class ENGINE_DLL CGameObject_Sphere : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CGameObject_Sphere(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	CGameObject_Sphere(const CGameObject_Sphere& Prototype);
	virtual ~CGameObject_Sphere() = default;
public:
	static CGameObject_Sphere* Create(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	virtual CGameObject_Sphere* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
#pragma endregion
#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
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
	CVIBuffer* m_pVIBufferCom = { nullptr };
};

NS_END
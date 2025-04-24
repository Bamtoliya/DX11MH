#pragma once
#include "Component.h"
BEGIN(Engine)
class CSound_Manager;
class ENGINE_DLL CSound_Listener final : public CComponent
{
private:
	CSound_Listener(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager);
	CSound_Listener(const CSound_Listener& Prototype);
	virtual ~CSound_Listener() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
public:
	void Set_Listner(const class CTransform& worldTrans);
private:
	CSound_Manager*			m_pSound_Manager;
public:
	static	CSound_Listener* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager);
	CSound_Listener* Clone(void *pArg);
	void	Free();
};

END;
#include "Sound_Listener.h"
#include "Sound_Manager.h"
#include "Transform.h"

CSound_Listener::CSound_Listener(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager)
    : CComponent{ pDevice, pContext }
    , m_pSound_Manager{ pSound_Manager }
{
	Safe_AddRef(m_pSound_Manager);
}

CSound_Listener::CSound_Listener(const CSound_Listener& Prototype)
	: CComponent { Prototype }
	, m_pSound_Manager { Prototype.m_pSound_Manager }
{
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CSound_Listener::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSound_Listener::Initialize(void* pArg)
{
    return S_OK;
}

void CSound_Listener::Priority_Update(_float fTimeDelta)
{
}

void CSound_Listener::Update(_float fTimeDelta)
{
}

void CSound_Listener::Late_Update(_float fTimeDelta)
{
}

void CSound_Listener::Set_Listner(CTransform* worldTrans)
{
    m_pSound_Manager->Set_Listner(worldTrans);
}

CSound_Listener* CSound_Listener::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager)
{
    CSound_Listener* pInstance = new CSound_Listener(pDevice, pContext, pSound_Manager);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSound_Listener");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSound_Listener* CSound_Listener::Clone(void* pArg)
{
    CSound_Listener* pInstance = new CSound_Listener(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSound_Listener");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Listener::Free()
{
    __super::Free();
    Safe_Release(m_pSound_Manager);
}

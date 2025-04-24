#include "Sound_Source.h"
#include "Sound_Manager.h"

CSound_Source::CSound_Source(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager)
    : CComponent{ pDevice, pContext }
    , m_pSound_Manager { pSound_Manager }
{
    Safe_AddRef(m_pSound_Manager);
}

CSound_Source::CSound_Source(const CSound_Source& Prototype)
    : CComponent{ Prototype }
    , m_pSound_Manager { Prototype.m_pSound_Manager }
{
    Safe_AddRef(m_pSound_Manager);
}

HRESULT CSound_Source::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSound_Source::Initialize(void* pArg)
{
    return S_OK;
}

void CSound_Source::Priority_Update(_float fTimeDelta)
{
}

void CSound_Source::Update(_float fTimeDelta)
{
    // 유효하지 않은 2D event 삭제
    auto iter = m_Events2D.begin();
    while (iter != m_Events2D.end())
    {
        if (!(*iter)->IsValid())
        {
            Safe_Delete(*iter);
            iter = m_Events2D.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    // 유효하지 않는 3D event 삭제
    iter = m_Events3D.begin();
    while (iter != m_Events3D.end())
    {
        if (!(*iter)->IsValid())
        {
            Safe_Delete(*iter);
            iter = m_Events3D.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

void CSound_Source::Late_Update(_float fTimeDelta)
{
}

_float CSound_Source::Get_Global_Parameter(const string& name)
{
    return m_pSound_Manager->Get_Global_Parameter(name);
}

void CSound_Source::Set_Global_Parameter(const string& name, _float value)
{
    m_pSound_Manager->Set_Global_Parameter(name, value);
}

CSound_Event* CSound_Source::Play_Event(_wstring strEvent, void* pArg)
{
    CSound_Event* e = m_pSound_Manager->Play_Event(strEvent);
    if (e->Is3D())
    {
        if (pArg)
        {
            //CTransform* pTransform = reinterpret_cast<CTransform*>(pArg);
            m_Events3D.emplace_back(e);
            //e->Set3DAttributes(*pTransform);
        }
        else
        {
            delete e;
            e = nullptr;
        }
    }
    else
    {
        m_Events2D.emplace_back(e);
    }
    return e;
}

void CSound_Source::Stop_All_Event(bool allowFadeOut)
{
    for (auto& e : m_Events2D)
    {
        e->Stop(allowFadeOut);
    }
    for (auto& e : m_Events3D)
    {
        e->Stop(allowFadeOut);
    }

    Clear();
}

void CSound_Source::Clear()
{
    for (auto& e : m_Events2D)
    {
        Safe_Delete(e);
    }
    for (auto& e : m_Events3D)
    {
        Safe_Delete(e);
    }
    // 모든 이벤트를 컨테이너에서 삭제한다.
    m_Events2D.clear();
    m_Events3D.clear();
}

CSound_Source* CSound_Source::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager)
{
    CSound_Source* pInstance = new CSound_Source(pDevice, pContext, pSound_Manager);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSound_Source");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSound_Source* CSound_Source::Clone(void* pArg)
{
    CSound_Source* pInstance = new CSound_Source(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CSound_Source");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSound_Source::Free()
{
    __super::Free();
    for (auto& iter : m_Events2D)
    {
        Safe_Release(iter);
    }
    for (auto& iter : m_Events3D)
    {
        Safe_Release(iter);
    }
    Safe_Release(m_pSound_Manager);
}

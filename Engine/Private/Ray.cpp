#include "Ray.h"

CRay::CRay()
{
    ZeroMemory(&m_tDesc, sizeof(RAY_DESC));
}

CRay* CRay::Create()
{
    CRay* pInstance = new CRay();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CInput_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRay::Free()
{
    __super::Free();
}

HRESULT CRay::Initialize(void* pArg)
{
    return S_OK;
}

void CRay::Priority_Update(_float fTimeDelta)
{
}

void CRay::Update(_float fTimeDelta)
{
}

void CRay::Late_Update(_float fTimeDelta)
{
}

_vector CRay::Get_Point(const _float fDistance) const
{
    _vector vOrigin = XMLoadFloat4(&m_tDesc.vOrigin);
    _vector vDirection = XMLoadFloat4(&m_tDesc.vDirection);

    // return vOrigin + vDirection * fDistance;
    return XMVectorMultiplyAdd(vDirection, XMVectorReplicate(fDistance), vOrigin); // FMA (Fused Multiply-Add) 사용 가능 시 더 효율적
}

void CRay::Set_Origin(FXMVECTOR vOrigin)
{
    XMStoreFloat4(&m_tDesc.vOrigin, vOrigin);
}

void CRay::Set_Direction(FXMVECTOR vDirection)
{
    XMStoreFloat4(&m_tDesc.vDirection, XMVector4Normalize(vDirection));
}

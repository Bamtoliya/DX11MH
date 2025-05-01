#include "GameObject_Plane.h"
#include "VIBuffer.h"

CGameObject_Plane::CGameObject_Plane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CGameObject_Plane::CGameObject_Plane(const CGameObject_Plane& Prototype)
	: CGameObject { Prototype }
{
}

CGameObject_Plane* CGameObject_Plane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Plane* pInstance = new CGameObject_Plane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CGameObject_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Plane::Clone(void* pArg)
{
	CGameObject_Plane* pInstance = new CGameObject_Plane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CGameObject_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameObject_Plane::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
}

HRESULT CGameObject_Plane::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Plane::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject_Plane::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Plane::Update(_float fTimeDelta)
{
}

void CGameObject_Plane::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject_Plane::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject_Plane::Render()
{
	Pre_Render();
	Post_Render();
	return S_OK;
}

HRESULT CGameObject_Plane::Post_Render()
{
	return S_OK;
}

HRESULT CGameObject_Plane::RenderImGui()
{
	__super::RenderImGui();
	return S_OK;
}

#include "Camera.h"

CCamera::CCamera(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext)
	: CGameObject { m_pDevice, m_pContext }
{
	ZeroMemory(&m_tDesc, sizeof(CAMERA_DESC));
}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject { Prototype }
	, m_tDesc { Prototype.m_tDesc }
{

}

void CCamera::Free()
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	memcpy(&m_tDesc, pArg, sizeof(CAMERA_DESC));
	m_pTransformCom->Set_State(CTransform::POS, XMLoadFloat4(&m_tDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	//m_pTransformCom	->Set_TransformDesc(m_tDesc.);

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	

	return S_OK;
}

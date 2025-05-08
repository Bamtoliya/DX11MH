#include "Camera.h"
#include "GameInstance.h"

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
	__super::Free();
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tDesc, pArg, sizeof(CAMERA_DESC));
	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_tDesc.fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	return S_OK;
}

void CCamera::Bind_Matrices()
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixOrthographicLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
	//m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
}

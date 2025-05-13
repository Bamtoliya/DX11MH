#include "GameObject_Camera.h"
#include "GameInstance.h"

CGameObject_Camera::CGameObject_Camera(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext)
	: CGameObject { m_pDevice, m_pContext }
{
	ZeroMemory(&m_tDesc, sizeof(CAMERA_DESC));
}

CGameObject_Camera::CGameObject_Camera(const CGameObject_Camera& Prototype)
	: CGameObject { Prototype }
	, m_tDesc { Prototype.m_tDesc }
{

}

void CGameObject_Camera::Free()
{
	__super::Free();
}

HRESULT CGameObject_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Camera::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tDesc, pArg, sizeof(CAMERA_DESC));
	
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_tDesc.fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&m_tDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	return S_OK;
}

void CGameObject_Camera::Bind_Matrices()
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	if(m_tDesc.bOrthographic)
		m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixOrthographicLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
	else 
		m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
}

#include "MyCamera.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Transform.h"

#pragma region Constructor/Desctuctor
CMyCamera::CMyCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CMyCamera::CMyCamera(const CMyCamera& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CMyCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMyCamera::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMyCamera::Late_Initialize()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_tDesc.fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;

	CTransform* pTranform = reinterpret_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));

	pTranform->Set_State(STATE::POSITION, XMLoadFloat4(&m_tDesc.vEye));
	pTranform->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	return S_OK;
}

CMyCamera* CMyCamera::Create()
{
	return nullptr;
}

CMyCamera* CMyCamera::Clone(void* pArg)
{
	return nullptr;
}

void CMyCamera::Free()
{
}
#pragma endregion

#pragma region Loops
void CMyCamera::Priority_Update(_float fTimeDelta)
{
}

void CMyCamera::Update(_float fTimeDelta)
{
}

void CMyCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CMyCamera::Render()
{
	return S_OK;
}

void CMyCamera::Bind_Matrices(CTransform* pTransformCom)
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixOrthographicLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
}

HRESULT CMyCamera::RenderImGui()
{
	return S_OK;
}

#pragma endregion
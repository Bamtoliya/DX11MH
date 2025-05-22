#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

#pragma region Contructor & Destructor

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCamera::CCamera(const CCamera& Prototype)
	: CComponent{ Prototype }
	, m_tDesc{ Prototype.m_tDesc }
{

}

CCamera* CCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera* pInstance = new CCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCamera* CCamera::Clone(void* pArg)
{
	CCamera* pInstance = new CCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		CAMERA_COM_DESC* pDesc = reinterpret_cast<CAMERA_COM_DESC*>(pArg);
		memcpy(&m_tDesc, pDesc, sizeof(CAMERA_COM_DESC));
	}
	else
	{
		m_tDesc.
	}
		
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_tDesc.fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;

	CTransform* pTranform = reinterpret_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));

	pTranform->Set_State(STATE::POSITION, XMLoadFloat4(&m_tDesc.vEye));
	pTranform->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	m_Components.emplace(g_strTransformTag, m_pTransformCom);


	return S_OK;
}
#pragma endregion

#pragma region Loops

#pragma endregion

#pragma region ImGui

#pragma endregion
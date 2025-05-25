#include "GameObject_Camera.h"
#include "GameInstance.h"
#include "Camera.h"

CGameObject_Camera::CGameObject_Camera(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext)
	: CGameObject { m_pDevice, m_pContext }
{

}

CGameObject_Camera::CGameObject_Camera(const CGameObject_Camera& Prototype)
	: CGameObject { Prototype }
{

}

void CGameObject_Camera::Free()
{
	__super::Free();
	Safe_Release(m_pCameraCom);

}

HRESULT CGameObject_Camera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Camera::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	CCamera::CAMERA_COM_DESC* cameraDesc = static_cast<CCamera::CAMERA_COM_DESC*>(pDesc);

	m_pCameraCom = CCamera::Create(m_pDevice, m_pContext);
	if (nullptr == m_pCameraCom)
		return E_FAIL;

	m_pCameraCom->Set_OwnerGameObject(this);

	if (FAILED(m_pCameraCom->Initialize(cameraDesc)))
		return E_FAIL;

	m_Components.emplace(g_strCameraTag, m_pCameraCom);
	Safe_AddRef(m_pCameraCom);
	m_pCameraCom->Late_Initialize();
	return S_OK;
}

HRESULT CGameObject_Camera::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	m_pCameraCom = CCamera::Create(m_pDevice, m_pContext);
	if (nullptr == m_pCameraCom)
		return E_FAIL;

	m_pCameraCom->Set_OwnerGameObject(this);

	if (FAILED(m_pCameraCom->Initialize(nullptr)))
		return E_FAIL;

	m_Components.emplace(g_strCameraTag, m_pCameraCom);
	Safe_AddRef(m_pCameraCom);
	m_pCameraCom->Late_Initialize();
	return S_OK;
}
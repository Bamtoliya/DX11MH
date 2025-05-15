#include "Camera_Free.h"
#include "GameInstance.h"

_bool g_bCameraControl = false;

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject_Camera{ pDevice, pContext }
{

}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CGameObject_Camera{ Prototype }
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CGameObject_Camera::CAMERA_DESC			Desc{};

	Desc.vEye = _float4(0.f, 20.f, -15.f, 1.0f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.0f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fSpeedPerSec = 10.0f;
	Desc.bOrthographic = false;
	lstrcpy(Desc.szName, TEXT("Camera"));

	m_fSensor = 0.1f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	if (g_bCameraControl)
	{
		if (KEY_PRESSING(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta);
		}
		if (KEY_PRESSING(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta);
		}
		if (KEY_PRESSING(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (KEY_PRESSING(DIK_S))
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}

		_long			MouseMove = {};

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * m_fSensor);
		}

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), MouseMove * fTimeDelta * m_fSensor);
		}
	}
	__super::Bind_Matrices();
}

void CCamera_Free::Update(_float fTimeDelta)
{

	if (KEY_DOWN(DIK_TAB))
	{
		m_bChanged = true;
		g_bCameraControl = !g_bCameraControl;
	}
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
	if (g_bCameraControl && m_bChanged)
	{
		m_bChanged == false;
		ShowCursor(FALSE);
	}
	else if (!g_bCameraControl && m_bChanged)
	{
		m_bChanged == false;
		ShowCursor(TRUE);
	}
}

HRESULT CCamera_Free::Render()
{
	
	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();


}
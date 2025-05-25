#include "Camera_Free.h"
#include "GameInstance.h"
#include "GameObject.h"

_bool g_bCameraControl = false;

#pragma region Constructor/Destructor
CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject_Camera{ pDevice, pContext }
{

}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CGameObject_Camera{ Prototype }
{

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

CGameObject* CCamera_Free::Clone(simdjson::ondemand::object& jsonData)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CGameObject_Camera::CAMERA_DESC Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 1.f;
	lstrcpy(Desc.szName, TEXT("Camera"));
	m_fSensor = 0.1f;
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(simdjson::ondemand::object& jsonData)
{
	CGameObject_Camera::CAMERA_DESC Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 1.f;
	lstrcpy(Desc.szName, TEXT("Camera"));
	m_fSensor = 0.1f;
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if(FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region Loops
void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	if (g_bCameraControl)
	{
		if (KEY_PRESSING(DIK_F))
		{
			Focus_OnPickedObject();
		}
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

		//줌인 아웃
		/*if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::WHEEL))
		{
			if (MouseMove > 0)
			{
				m_pCameraCom->ZoomIn(fTimeDelta * m_fSensor);
			}
			else if(MouseMove < 0)
			{
				m_pCameraCom->ZoomOut(fTimeDelta * m_fSensor);
			}

		}*/
	}

	m_pCameraCom->Priority_Update(fTimeDelta);
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
		m_bChanged = false;
	}
	else if (!g_bCameraControl && m_bChanged)
	{
		m_bChanged = false;
	}
}

HRESULT CCamera_Free::Render()
{

	return S_OK;
}
#pragma endregion

void CCamera_Free::Focus_OnPickedObject()
{
	CGameObject* pGameObject = m_pGameInstance->Get_PickedObject();
	if (pGameObject)
	{
		CTransform* pTargetTransform = static_cast<CTransform*>(pGameObject->Get_Component(g_strTransformTag));
		if (pTargetTransform)
		{
			// 현재 객체의 Transform 컴포넌트 (m_pTransformCom)
			if (m_pTransformCom)
			{
				// 1. 타겟 객체의 위치와 현재 객체의 위치 가져오기
				_vector vTargetPosition = pTargetTransform->Get_State(STATE::POSITION);
				_vector vMyCurrentPosition = m_pTransformCom->Get_State(STATE::POSITION); // 현재 위치도 필요

				// 2. 현재 객체에서 타겟을 향하는 방향 벡터 계산
				_vector vDirectionFromMeToTarget = vTargetPosition - vMyCurrentPosition;

				// 3. 방향 벡터 정규화 (이동 방향 결정)
				//    타겟으로부터 '멀어지는' 방향은 (나의 위치 - 타겟 위치) 또는 -(타겟 위치 - 나의 위치) 입니다.
				//    즉, vDirectionFromMeToTarget 에 -1을 곱한 방향입니다.
				_vector vDirectionToMove = XMVectorScale(vDirectionFromMeToTarget, -1.f);
				_vector vNormalizedDirectionToMove = XMVector3Normalize(vDirectionToMove);

				// 4. 떨어질 거리 설정
				_float fDistance = 5.f; // 여기에 원하는 거리 값을 입력하세요. 예: 10.f

				// 5. 새로운 위치 계산: 타겟의 위치에서 (정규화된 멀어지는 방향 벡터 * 거리) 만큼 이동
				_vector vNewPosition = XMVectorMultiplyAdd(vNormalizedDirectionToMove, XMVectorReplicate(fDistance), vTargetPosition);
				// vNewPosition = vTargetPosition + (vNormalizedDirectionToMove * fDistance); 와 동일

				// 6. 계산된 위치로 현재 객체 이동
				m_pTransformCom->Set_State(STATE::POSITION, vNewPosition);

				// 7. (선택 사항) 이동 후 타겟을 다시 바라보도록 설정
				m_pTransformCom->LookAt(vTargetPosition);
			}
		}
	}
}


#pragma region Save&Load

HRESULT CCamera_Free::Serialize(json& outJson)
{
	json data;
	__super::Serialize(data);
	outJson["CCamera_Free"] = data;
	outJson["CCamera_Free"]["Name"] = WideToUtf8(m_szName);
	outJson["CCamera_Free"]["PrototypeTag"] = "Prototype_GameObject_Camera_Free";
	return S_OK;
}

HRESULT CCamera_Free::Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

HRESULT CCamera_Free::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CCamera_Free";
	return S_OK;
}

HRESULT CCamera_Free::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion
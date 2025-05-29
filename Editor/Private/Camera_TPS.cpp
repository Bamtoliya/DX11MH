#include "Camera_TPS.h"
#include "GameInstance.h"
#include "GameObject.h"

#pragma region Constructor/Destructor
CCamera_TPS::CCamera_TPS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject_Camera{ pDevice, pContext }

{

}

CCamera_TPS::CCamera_TPS(const CCamera_TPS& Prototype)
	: CGameObject_Camera{ Prototype }
{

}

CCamera_TPS* CCamera_TPS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_TPS* pInstance = new CCamera_TPS(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_TPS::Clone(void* pArg)
{
	CCamera_TPS* pInstance = new CCamera_TPS(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_TPS");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_TPS::Clone(simdjson::ondemand::object& jsonData)
{
	CCamera_TPS* pInstance = new CCamera_TPS(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CCamera_TPS");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CCamera_TPS::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_TPS::Initialize(void* pArg)
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

HRESULT CCamera_TPS::Initialize(simdjson::ondemand::object& jsonData)
{
	m_fSensor = 0.1f;
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

void CCamera_TPS::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region Loops
void CCamera_TPS::Priority_Update(_float fTimeDelta)
{
	_long			MouseMove = {};

	/*if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * m_fSensor);
	}

	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM::Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), MouseMove * fTimeDelta * m_fSensor);
	}*/

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

	m_pCameraCom->Priority_Update(fTimeDelta);
}

void CCamera_TPS::Update(_float fTimeDelta)
{
	if (nullptr == m_pFollowTarget)
		return;

	CTransform* pTargetTranform = static_cast<CTransform*>(m_pFollowTarget->Get_Component(g_strTransformTag));

	if (nullptr == pTargetTranform)
		return;
	Follow(pTargetTranform->Get_State(STATE::POSITION), fTimeDelta);
}

void CCamera_TPS::Late_Update(_float fTimeDelta)
{

}

HRESULT CCamera_TPS::Render()
{

	return S_OK;
}
void CCamera_TPS::Follow(_vector vTargetPosition, _float fTimeDelta)
{
	// 카메라 기본 거리 및 높이 설정
	const _float fDistance = 7.0f;      // 카메라와 타겟의 거리
	const _float fHeightOffset = 3.0f;  // 타겟 위의 높이
	const _float fSmoothFactor = 5.0f;  // 카메라 이동 부드러움 (높을수록 빠름)

	// 1. 카메라 방향 벡터 계산 (현재 카메라 방향 기반)
	_vector vCameraRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_vector vCameraUp = m_pTransformCom->Get_State(STATE::UP);
	_vector vCameraLook = m_pTransformCom->Get_State(STATE::LOOK);

	// 2. 타겟 기준으로 카메라 이상적 위치 계산
	_vector vIdealPosition = vTargetPosition;

	// 타겟 뒤로 이동
	vIdealPosition = XMVectorAdd(vIdealPosition, XMVectorScale(XMVector3Normalize(XMVectorSet(
		-XMVectorGetX(vCameraLook), 0.0f, -XMVectorGetZ(vCameraLook), 0.0f)), fDistance));

	// 높이 오프셋 적용
	vIdealPosition = XMVectorAdd(vIdealPosition, XMVectorSet(0.0f, fHeightOffset, 0.0f, 0.0f));

	// 3. 카메라 충돌 처리 (선택적)
	// 여기서는 간단한 예시로 생략했지만, 필요하다면 레이캐스트를 사용하여 벽과의 충돌을 체크하고
	// 충돌 시 카메라 위치를 조정할 수 있습니다.

	// 4. 현재 카메라 위치를 이상적 위치로 부드럽게 이동 (Lerp)
	_vector vCurrentPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vNewPosition = XMVectorLerp(vCurrentPosition, vIdealPosition, fSmoothFactor * fTimeDelta);

	// 5. 카메라 위치 및 시선 방향 업데이트
	m_pTransformCom->Set_State(STATE::POSITION, vNewPosition);

	// 시선은 항상 타겟을 향하도록 (약간 위쪽을 보도록 오프셋 적용 가능)
	_vector vLookAt = XMVectorAdd(vTargetPosition, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)); // 타겟의 머리 위치 정도를 보도록
	m_pTransformCom->LookAt(vLookAt);
	return;
}
#pragma endregion

#pragma region Save&Load

HRESULT CCamera_TPS::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CCamera_TPS"] = data;
	outJson["CCamera_TPS"]["PrototypeTag"] = "Prototype_GameObject_Camera_TPS";
	return S_OK;
}

HRESULT CCamera_TPS::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCamera_TPS::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CCamera_TPS";
	return S_OK;
}

HRESULT CCamera_TPS::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion
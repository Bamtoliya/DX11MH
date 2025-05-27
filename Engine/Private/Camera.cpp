#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

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
	if (nullptr == pArg)
		return S_OK;
	CAMERA_COM_DESC* pDesc = static_cast<CAMERA_COM_DESC*>(pArg);
	memcpy(&m_tDesc, pDesc, sizeof(CAMERA_COM_DESC));
	return S_OK;
}

HRESULT CCamera::Late_Initialize()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_tDesc.fAspect = static_cast<_float>(ViewportDesc.Width) / ViewportDesc.Height;

	CTransform* pTransformCom = reinterpret_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));

	if (XMVector3Length(pTransformCom->Get_State(STATE::POSITION)).m128_f32[0] == 0.f)
	{
		pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 20.f, -15.f, 1.0f));
	}

	pTransformCom->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	return S_OK;
}
#pragma endregion

#pragma region Loops

void CCamera::Priority_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	CTransform* pTransformCom = reinterpret_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));

	if (nullptr == pTransformCom)
		return;
	
	Bind_Matrices(pTransformCom);
}

void CCamera::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

void CCamera::Late_Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
}

HRESULT CCamera::Render()
{
	if (!m_bActive)
		return S_OK;
	return S_OK;
}

void CCamera::Bind_Matrices(CTransform* pTransformCom)
{
	m_pGameInstance->Set_Transform(D3DTS::VIEW, pTransformCom->Get_WorldMatrix_Inverse());
	if (m_tDesc.bOrthographic)
		m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixOrthographicLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
	else
		m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_tDesc.fFovy, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar));
}

void CCamera::ZoomIn(_float fTimeDelta)
{
	m_tDesc.fFovy = max(m_tDesc.fFovy - m_tDesc.fZoomPerSec * fTimeDelta, m_tDesc.fMinFovy);
}

void CCamera::ZoomOut(_float fTimeDelta)
{
	m_tDesc.fFovy = min(m_tDesc.fFovy + m_tDesc.fZoomPerSec * fTimeDelta, m_tDesc.fMaxFovy);
}

void CCamera::ResetZoom(_float fTimeDelta)
{
	m_tDesc.fFovy = m_tDesc.fDefaultFovy;
}

void CCamera::LookAt(_fvector position)
{
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	pTransform->LookAt(position);
}

#pragma endregion


#pragma region Save&Load

HRESULT CCamera::Serialize(json& outJson)
{
	json data;
	if(FAILED(__super::Serialize(data)))
		return E_FAIL;
	data["bOrthographic"] = m_tDesc.bOrthographic;
	data["fZoomPerSec"] = m_tDesc.fZoomPerSec;
	data["fFovy"] = XMConvertToDegrees(m_tDesc.fFovy);
	data["fNear"] = m_tDesc.fNear;
	data["fFar"] = m_tDesc.fFar;
	data["vAt"] = { m_tDesc.vAt.x, m_tDesc.vAt.y, m_tDesc.vAt.z, m_tDesc.vAt.w };
	outJson["CCamera"] = data;
	return S_OK;
}

HRESULT CCamera::Deserialize(simdjson::ondemand::object & jsonData)
{
	if (FAILED(__super::Deserialize(jsonData))) // 부모 클래스의 Deserialize 호출
		return E_FAIL;

	simdjson::ondemand::object cameraData;
	if (jsonData["CCamera"].get_object().get(cameraData)) // "CCamera" 객체 가져오기
		return E_FAIL; // 또는 오류 처리

	// 각 속성 역직렬화
	if (cameraData["bOrthographic"].get_bool().get(m_tDesc.bOrthographic))
		return E_FAIL; // 또는 기본값 설정 및 오류 로그

	double tempDouble; // simdjson은 double로 숫자 값을 가져옵니다.
	if (cameraData["fZoomPerSec"].get_double().get(tempDouble))
		return E_FAIL;
	m_tDesc.fZoomPerSec = static_cast<_float>(tempDouble);

	if (cameraData["fFovy"].get_double().get(tempDouble))
		return E_FAIL;
	m_tDesc.fFovy = XMConvertToRadians(static_cast<_float>(tempDouble)); // JSON에서는 Degree, 내부에서는 Radian 사용

	if (cameraData["fNear"].get_double().get(tempDouble))
		return E_FAIL;
	m_tDesc.fNear = static_cast<_float>(tempDouble);

	if (cameraData["fFar"].get_double().get(tempDouble))
		return E_FAIL;
	m_tDesc.fFar = static_cast<_float>(tempDouble);

	simdjson::ondemand::array vAtArray;
	if (cameraData["vAt"].get_array().get(vAtArray))
		return E_FAIL;

	size_t index = 0;
	for (auto value : vAtArray)
	{
		if (value.get_double().get(tempDouble))
			return E_FAIL;
		switch (index)
		{
		case 0: m_tDesc.vAt.x = static_cast<_float>(tempDouble); break;
		case 1: m_tDesc.vAt.y = static_cast<_float>(tempDouble); break;
		case 2: m_tDesc.vAt.z = static_cast<_float>(tempDouble); break;
		case 3: m_tDesc.vAt.w = static_cast<_float>(tempDouble); break;
		default: break; // 배열 크기가 예상과 다를 경우의 처리
		}
		index++;
	}
	if (index != 4) // vAt는 4개의 요소를 가져야 함
	{
		// 오류 처리: vAt 배열의 크기가 올바르지 않음
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CCamera";
	return S_OK;
}

HRESULT CCamera::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{

	return S_OK;
}

#pragma endregion


#pragma region ImGui
HRESULT CCamera::RenderImGui()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Set this Camera as Active"))
		{
			m_pGameInstance->Set_Camera(this);
		}
		ImGui::Text("Perspective");
		ImGui::SameLine();
		ImGui::Toggle("Orthographic", &m_tDesc.bOrthographic);
		_float fFovDegree = XMConvertToDegrees(m_tDesc.fFovy);
		ImGui::DragFloat("ZoomSpeed", &m_tDesc.fZoomPerSec, 0.01f, 0.0f, 10.f);
		ImGui::DragFloat("FOVy(Zoom)", &fFovDegree, 0.1f, 10.f, 200.f);
		ImGui::DragFloat("Near", &m_tDesc.fNear, 0.01f, 0.01f, 1000.f);
		if (m_tDesc.fNear <= 0.f) m_tDesc.fNear = 0.01f;
		ImGui::DragFloat("Far", &m_tDesc.fFar, 0.01f, 0.2f, 10000.f);
		if (m_tDesc.fFar <= 0.f) m_tDesc.fFar = m_tDesc.fNear + 0.01f;
		ImGui::DragFloat4("Look At", &m_tDesc.vAt.x, 1.0f);
		m_tDesc.fFovy = XMConvertToRadians(fFovDegree);
	}
	return S_OK;
}
#pragma endregion
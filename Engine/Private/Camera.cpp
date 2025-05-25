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

	//
	pTransformCom->LookAt(XMLoadFloat4(&m_tDesc.vAt));

	return S_OK;
}
#pragma endregion

#pragma region Loops

void CCamera::Priority_Update(_float fTimeDelta)
{
	CTransform* pTransformCom = reinterpret_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));

	if (nullptr == pTransformCom)
		return;

	Bind_Matrices(pTransformCom);
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
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

#pragma endregion


#pragma region Save&Load

HRESULT CCamera::Serialize(json& outJson)
{
	outJson["Camera"] = json::object();
	outJson["Camera"]["Class"] = "CCamera";
	outJson["Camera"]["Leve"] = "STATIC";
	outJson["Camera"]["bOrthographic"] = m_tDesc.bOrthographic;
	//json["Camera"]["fZoomPerSec"] = m_tDesc.fZoomPerSec;
	//json["Camera"]["fFovy"] = XMConvertToDegrees(m_tDesc.fFovy);
	//json["Camera"]["fNear"] = m_tDesc.fNear;
	//json["Camera"]["fFar"] = m_tDesc.fFar;
	//json["Camera"]["vAt"] = { m_tDesc.vAt.x, m_tDesc.vAt.y, m_tDesc.vAt.z, m_tDesc.vAt.w };
	return S_OK;
}

HRESULT CCamera::Deserialize(const json& inData)
{
	return S_OK;
}

HRESULT CCamera::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CCamera";
	return S_OK;
}

HRESULT CCamera::Prototype_Deserialize(const simdjson::ondemand::object& jsonData)
{

	return S_OK;
}

#pragma endregion


#pragma region ImGui
HRESULT CCamera::RenderImGui()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("OrthGraphic", &m_tDesc.bOrthographic);
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
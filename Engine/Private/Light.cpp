#include "Light.h"
#include "GameInstance.h"


#pragma region Constructor/ Destructor
CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CLight::CLight(const CLight& Prototype)
	: CComponent{ Prototype }
	, m_tInfo { Prototype.m_tInfo }
{

}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint			iPassIndex = {};

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		/* 빛정보를 쉐이더에 던진다. */
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_tInfo.vDirection, sizeof(_float4))))
			return E_FAIL;

		iPassIndex = 1;
	}
	else
	{


		iPassIndex = 2;
	}


	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_fLightAmbient", &m_LightDesc.fAmbient, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_Buffers();
	pVIBuffer->Render();

	return S_OK;
}

CLight* CLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight* pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight* CLight::Clone(void* pArg)
{
	CLight* pInstance = new CLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		LIGHT_DESC* pDesc = static_cast<LIGHT_DESC*>(pArg);
		memcpy(&m_tInfo, pDesc, sizeof(LIGHT_DESC));
	}
	return S_OK;
}

void CLight::Free()
{
	__super::Free();
}
#pragma endregion


#pragma region Save&Load

HRESULT CLight::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	data["Type"] = m_tInfo.eType;
	data["Direction"] = { m_tInfo.vDirection.x, m_tInfo.vDirection.y, m_tInfo.vDirection.z, m_tInfo.vDirection.w };
	data["Position"] = { m_tInfo.vPosition.x, m_tInfo.vPosition.y, m_tInfo.vPosition.z, m_tInfo.vPosition.w };
	data["Range"] = m_tInfo.fRange;
	data["Diffuse"] = { m_tInfo.vDiffuse.x, m_tInfo.vDiffuse.y, m_tInfo.vDiffuse.z, m_tInfo.vDiffuse.w };
	data["Ambient"] = { m_tInfo.vAmbient.x, m_tInfo.vAmbient.y, m_tInfo.vAmbient.z, m_tInfo.vAmbient.w };
	data["Specular"] = { m_tInfo.vSpecular.x, m_tInfo.vSpecular.y, m_tInfo.vSpecular.z, m_tInfo.vSpecular.w };
	outJson["CLight"] = data;
	return S_OK;
}

HRESULT CLight::Deserialize(simdjson::ondemand::object & jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	simdjson::ondemand::object lightObject;
	if(jsonData["CLight"].get_object().get(lightObject) == simdjson::SUCCESS)
	{
			
		m_tInfo.eType = static_cast<LIGHT_DESC::TYPE>(lightObject["Type"].get_int64().value_unsafe());
		m_tInfo.fRange = static_cast<_float>(lightObject["Range"].get_double().value_unsafe());
		simdjson::ondemand::array arr = lightObject["Direction"].get_array();
		auto iter = arr.begin();
		m_tInfo.vDirection.x = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vDirection.y = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vDirection.z = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vDirection.w = static_cast<_float>((*iter).get_double().value_unsafe());

		arr = lightObject["Position"].get_array();
		iter = arr.begin();
		m_tInfo.vPosition.x = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vPosition.y = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vPosition.z = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vPosition.w = static_cast<_float>((*iter).get_double().value_unsafe());

		arr = lightObject["Diffuse"].get_array();
		iter = arr.begin();
		m_tInfo.vDiffuse.x = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vDiffuse.y = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vDiffuse.z = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vDiffuse.w = static_cast<_float>((*iter).get_double().value_unsafe());

		arr = lightObject["Specular"].get_array();
		iter = arr.begin();
		m_tInfo.vSpecular.x = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vSpecular.y = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vSpecular.z = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vSpecular.w = static_cast<_float>((*iter).get_double().value_unsafe());

		arr = lightObject["Ambient"].get_array();
		iter = arr.begin();
		m_tInfo.vAmbient.x = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vAmbient.y = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vAmbient.z = static_cast<_float>((*iter).get_double().value_unsafe()); ++iter;
		m_tInfo.vAmbient.w = static_cast<_float>((*iter).get_double().value_unsafe());

		
	}
	return S_OK;
}

HRESULT CLight::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CLight";
	return S_OK;
}

HRESULT CLight::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{

	return S_OK;
}

#pragma endregion

void CLight::Update()
{
	if (!m_pOwner)
		return;

	XMStoreFloat4(&m_tInfo.vPosition, m_pOwner->Get_Position());
}

HRESULT CLight::RenderImGui()
{
	if(ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Combo("Light Type", reinterpret_cast<int*>(&m_tInfo.eType), "Directional\0Point\0\0");

		if (m_tInfo.eType == LIGHT_DESC::TYPE_DIRECTIONAL)
			ImGui::DragFloat3("Direction", &m_tInfo.vDirection.x, 0.1f, -1.f, 1.f);
		if (m_tInfo.eType == LIGHT_DESC::TYPE_POINT)
			ImGui::DragFloat("Range", &m_tInfo.fRange, 1.0f, 0.f, 1000.f);
		ImGui::ColorPicker4("Diffuse", &m_tInfo.vDiffuse.x);
		ImGui::ColorPicker4("Ambient", &m_tInfo.vAmbient.x);
		ImGui::ColorPicker4("Specular", &m_tInfo.vSpecular.x);
	}
	return S_OK;
}


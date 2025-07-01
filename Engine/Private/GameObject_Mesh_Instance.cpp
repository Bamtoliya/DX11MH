
#include "GameObject_Mesh_Instance.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_Mesh_Instance::CGameObject_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_Mesh_Instance::CGameObject_Mesh_Instance(const CGameObject_Mesh_Instance& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_Mesh_Instance* CGameObject_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Mesh_Instance* pInstance = new CGameObject_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_Mesh_Instance::Clone(void* pArg)
{
	CGameObject_Mesh_Instance* pInstance = new CGameObject_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Mesh_Instance::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Mesh_Instance* pInstance = new CGameObject_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Mesh_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Mesh_Instance::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_Mesh_Instance"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Mesh_Instance::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Mesh_Instance::Ready_Components()
{
	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_GrindRail"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CVIBuffer_Mesh_Instance::MESH_INSTANCE_DESC meshInstanceDesc;
	meshInstanceDesc.pMesh = m_pModelCom->Get_Meshes()[0];
	meshInstanceDesc.iNumInstance = 100;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Mesh_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferInstanceCom), &meshInstanceDesc)))
		return E_FAIL;

	// 인스턴스들의 Transform 생성 (예시: 100개)
	/*for (int i = 0; i < 100; ++i)
	{
		CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
		_float3 vPos = _float3((i % 20) * 2.f, 0.f, (i / 20) * 2.f);
		pTransform->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));
		m_pInstanceTransform.push_back(pTransform);
	}*/

	return S_OK;
}

void CGameObject_Mesh_Instance::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferInstanceCom);
	for (auto& pTransform : m_pInstanceTransform)
	{
		Safe_Release(pTransform);
	}
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Mesh_Instance::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_Mesh_Instance"] = data;
	outJson["CGameObject_Mesh_Instance"]["PrototypeTag"] = "Prototype_GameObject_Mesh_Instance";
	outJson["CGameObject_Mesh_Instance"]["PrototypeModelTag"] = WideToUtf8(m_pModelCom->GetPrototypeTag().c_str()).c_str();
	return S_OK;
}

HRESULT CGameObject_Mesh_Instance::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	// 모델 프로토타입 태그 검색
	string_view modelProtoTagSV;
	jsonData["PrototypeModelTag"].get_string().get(modelProtoTagSV);
	if (jsonData["PrototypeModelTag"].get_string().get(modelProtoTagSV) == simdjson::SUCCESS)
	{
		_wstring modelProtoTag = Utf8ToWide(string(modelProtoTagSV));

		CModel* pOldModel = m_pModelCom;

		// 새 모델 프로토타입 찾아서 클론
		CComponent* pPrototype = m_pGameInstance->Find_Prototype(PROTOTYPE::TYPE_COMPONENT, m_pGameInstance->Get_CurrentLevelIndex(), modelProtoTag);
		if (pPrototype != nullptr)
		{
			CModel* pNewModel = (CModel*)pPrototype->Clone(nullptr);
			if (pNewModel != nullptr)
			{
				// 모델 컴포넌트 교체
				Safe_Release(m_pModelCom);
				m_pModelCom = pNewModel;	
				m_Components[m_pModelCom->GetName()] = m_pModelCom;
				Safe_AddRef(m_pModelCom);

				m_pModelCom->Set_OwnerGameObject(this);
				/*CVIBuffer_Mesh_Instance::MESH_INSTANCE_DESC meshInstanceDesc;
				meshInstanceDesc.pMesh = m_pModelCom->Get_Meshes()[0];*/
			}
		}
	}
	return S_OK;
}

HRESULT CGameObject_Mesh_Instance::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Mesh_Instance";
	return S_OK;
}

HRESULT CGameObject_Mesh_Instance::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Mesh_Instance::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_Mesh_Instance::Update(_float fTimeDelta)
{

}

void CGameObject_Mesh_Instance::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_Mesh_Instance::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}


HRESULT CGameObject_Mesh_Instance::Render()
{
	if (!m_bVisible)
		return S_OK;
	Bind_ShaderResources();

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferInstanceCom->Bind_Buffers(m_pModelCom->Get_Meshes()[i])))
			return E_FAIL;

		if (FAILED(m_pVIBufferInstanceCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}
#pragma endregion
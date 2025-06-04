
#include "Height_Field.h"
#include "GameInstance.h"

#pragma region Constructor & Destructor
CHeight_Field::CHeight_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CHeight_Field::CHeight_Field(const CHeight_Field& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CHeight_Field::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CHeight_Field::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	PxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns = 10; // Example value, set as needed
	heightFieldDesc.nbRows = 10;
	heightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
	heightFieldDesc.flags = PxHeightFieldFlag::eNO_BOUNDARY_EDGES;

	m_pHeightField = PxCreateHeightField(heightFieldDesc);
	m_fHfHeightScale = 1.0f;
	m_fHfRowScale = 1.0f;
	m_fHfColumnScale = 1.0f;
	return S_OK;
}
HRESULT CHeight_Field::Late_Initialize()
{
	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(m_pOwner->Get_Component(L"Com_VIBuffer"));
	if (!pTerrain)
		return E_FAIL;

	_uint numCols = pTerrain->Get_NumVerticeX(); // PhysX는 nbColumns = X
	_uint numRows = pTerrain->Get_NumVerticeZ(); // nbRows = Z

	// 2. 높이 데이터 준비 (예시: float -> short 변환)
	std::vector<physx::PxI16> heights(numCols * numRows);




	return S_OK;
}

CHeight_Field* CHeight_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHeight_Field* pInstance = new CHeight_Field(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHeight_Field");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CHeight_Field::Clone(void* pArg)
{
	CHeight_Field* pInstance = new CHeight_Field(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHeight_Field");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHeight_Field::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region Save&Load
HRESULT CHeight_Field::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CHeight_Field::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}
#pragma endregion

#pragma region Loops
void CHeight_Field::Priority_Update(_float fTimeDelta)
{
}
void CHeight_Field::Update(_float fTimeDelta)
{
}
void CHeight_Field::Late_Update(_float fTimeDelta)
{
}
HRESULT CHeight_Field::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CHeight_Field::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Height_Field", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
	return S_OK;
}
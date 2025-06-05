
#include "HeightField.h"
#include "GameInstance.h"

#pragma region Constructor & Destructor
CHeightField::CHeightField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CHeightField::CHeightField(const CHeightField& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CHeightField::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CHeightField::Initialize(void* pArg)
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
HRESULT CHeightField::Late_Initialize()
{
	CVIBuffer_Terrain* pTerrain = dynamic_cast<CVIBuffer_Terrain*>(m_pOwner->Get_Component(L"Com_VIBuffer"));
	if (!pTerrain)
		return E_FAIL;

	_uint numCols = pTerrain->Get_NumVerticeX(); // PhysX는 nbColumns = X
	_uint numRows = pTerrain->Get_NumVerticeZ(); // nbRows = Z
	// 2. 높이 데이터 준비 (예시: float -> short 변환)
	std::vector<physx::PxI16> heights(numCols * numRows);


	for (_uint row = 0; row < numRows; ++row)
	{
		for (_uint col = 0; col < numCols; ++col)
		{

			_uint index = row * numCols + col;
			
			heights[index] = static_cast<physx::PxI16>(pTerrain->Get_VertexPositions()[index].y); // GetHeightFromTerrain() is a hypothetical function to retrieve height data
			//samples[index].materialIndex0 = 0; // 기본 재질 인덱스
			//samples[index].materialIndex1 = 0; // 기본 재질 인덱스
		}
	}


	PxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns = numCols;
	heightFieldDesc.nbRows = numRows;
	heightFieldDesc.format = PxHeightFieldFormat::eS16_TM;
	heightFieldDesc.flags = PxHeightFieldFlag::eNO_BOUNDARY_EDGES;
	heightFieldDesc.samples.data = heights.data();
	//heightFieldDesc.samples.


	m_pHeightField = PxCreateHeightField(heightFieldDesc);
	m_fHfHeightScale = 1.0f;
	m_fHfRowScale = 1.0f;
	m_fHfColumnScale = 1.0f;

	m_pGameInstance->Get_PhysicsScene()->addActor(&m_pHeightField)
	return S_OK;
}

CHeightField* CHeightField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHeightField* pInstance = new CHeightField(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHeightField");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CHeightField::Clone(void* pArg)
{
	CHeightField* pInstance = new CHeightField(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHeightField");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHeightField::Free()
{
	__super::Free();
	Safe_Release(m_pHeightField);
}
#pragma endregion

#pragma region Save&Load
HRESULT CHeightField::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CHeightField::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}
#pragma endregion

#pragma region Loops
void CHeightField::Priority_Update(_float fTimeDelta)
{
}
void CHeightField::Update(_float fTimeDelta)
{
}
void CHeightField::Late_Update(_float fTimeDelta)
{
}
HRESULT CHeightField::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CHeightField::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("HeightField", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderFloat("Height Scale", &m_fHfHeightScale, 0.0f, 10.0f);
		ImGui::SliderFloat("Row Scale", &m_fHfRowScale, 0.0f, 10.0f);
		ImGui::SliderFloat("Column Scale", &m_fHfColumnScale, 0.0f, 10.0f);
	}
	ImGui::PopID();
	return S_OK;
}
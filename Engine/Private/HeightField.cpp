#pragma once
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
	return S_OK;
}
HRESULT CHeightField::Late_Initialize()
{
	CVIBuffer_Terrain* pTerrain = dynamic_cast<CVIBuffer_Terrain*>(m_pOwner->Get_Component(L"Com_VIBuffer"));
	if (!pTerrain)
		return E_FAIL;

	_uint numVerticesX = pTerrain->Get_NumVerticeX();
	_uint numVerticesZ = pTerrain->Get_NumVerticeZ();

	// 그리드 당 버텍스 수 계산
	_uint gridVerticesX = (numVerticesX / m_iTileCountX) + ((numVerticesX % m_iTileCountX) ? 1 : 0);
	_uint gridVerticesZ = (numVerticesZ / m_iTileCountZ) + ((numVerticesZ % m_iTileCountZ) ? 1 : 0);

	const _float3* pVertexPositions = pTerrain->Get_VertexPositions();
    m_HeightFieldDescs.resize(m_iTileCountX * m_iTileCountZ);

    Create_HeightField(pVertexPositions, numVerticesX, numVerticesZ, 0, 0, gridVerticesX, gridVerticesZ, 0); // 첫 번째 타일 생성

	// 각 타일마다 HeightField 생성
	//for (_uint z = 0; z < m_iTileCountZ; ++z)
	//{
	//	for (_uint x = 0; x < m_iTileCountX; ++x)
	//	{
	//		_uint tileIndex = z * m_iTileCountX + x;
	//		_uint startX = x * (gridVerticesX - 1);
	//		_uint startZ = z * (gridVerticesZ - 1);

	//		// 경계를 넘어가지 않도록 조정
	//		if (startX >= numVerticesX || startZ >= numVerticesZ)
	//			continue;

	//		// 현재 그리드에 대한 실제 버텍스 수 계산 (경계 처리)
	//		_uint actualGridVerticesX = min(gridVerticesX, numVerticesX - startX);
	//		_uint actualGridVerticesZ = min(gridVerticesZ, numVerticesZ - startZ);

	//		if (FAILED(Create_HeightField(pVertexPositions, numVerticesX, numVerticesZ,
	//			startX, startZ, actualGridVerticesX, actualGridVerticesZ,
	//			tileIndex)))
	//		{
	//			return E_FAIL;
	//		}
	//	}
	//}

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

    for (auto& desc : m_HeightFieldDescs)
    {
        m_pGameInstance->Get_PhysicsScene()->removeActor(*desc.pActor);
        if(desc.pActor != nullptr)
            desc.pActor->release();
        if(desc.pHeightField != nullptr)
            desc.pHeightField->release();
        if(desc.pShape != nullptr)
            desc.pShape->release();
    }
    m_HeightFieldDescs.clear();
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


HRESULT CHeightField::Create_HeightField(const _float3* pVertexPositions, _uint totalVerticesX, _uint totalVerticesZ, _uint startX, _uint startZ, _uint gridVerticesX, _uint gridVerticesZ, _uint tileIndex)
{
    // PhysX에서 사용하는 Height Field는 열(column)=X축, 행(row)=Z축 방향으로 정의됨
    _uint numCols = gridVerticesX;
    _uint numRows = gridVerticesZ;

    if (numCols == 0 || numRows == 0)
        return S_OK; // 너무 작은 그리드는 생성하지 않음

    // 높이 데이터와 재질 데이터를 담을 샘플 배열 생성        
    vector<PxHeightFieldSample> samples(numCols * numRows);

    // 높이 데이터 설정
    for (_uint row = 0; row < numRows; ++row)
    {
        for (_uint col = 0; col < numCols; ++col)
        {
            _uint localIndex = row * numCols + col;
            _uint terrainX = startX + row; // row가 X축
            _uint terrainZ = startZ + col; // col이 Z축
            _uint globalIndex = terrainZ * totalVerticesX + terrainX;
            //_uint globalIndex = (startZ + row) * totalVerticesX + (startX + col);

            // PhysX의 높이 필드는 16비트 정수를 사용하므로, 높이값을 적절히 스케일링
            // 예: 높이값 범위가 0-100이면 65535로 매핑하여 정밀도 확보
            PxI16 height = static_cast<PxI16>(pVertexPositions[globalIndex].y * m_fHfHeightScale);

            samples[localIndex].height = height;
            samples[localIndex].materialIndex0 = 0;
            samples[localIndex].materialIndex1 = 0;
            samples[localIndex].setTessFlag();  // 삼각형 테셀레이션 플래그 설정
        }
    }

    // HeightField 설명자 생성
    PxHeightFieldDesc heightFieldDesc;
    heightFieldDesc.nbColumns = gridVerticesZ;
    heightFieldDesc.nbRows = gridVerticesX;
    heightFieldDesc.format = physx::PxHeightFieldFormat::eS16_TM;
    heightFieldDesc.samples.data = samples.data();
    heightFieldDesc.samples.stride = sizeof(physx::PxHeightFieldSample);
    heightFieldDesc.flags = physx::PxHeightFieldFlag::eNO_BOUNDARY_EDGES;

    // PhysX 시스템에서 HeightField 생성
    PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
    if (!pPhysics)
        return E_FAIL;

    
    PxHeightField* pHeightField = PxCreateHeightField(heightFieldDesc);
    if (!pHeightField)
        return E_FAIL;

    // HeightField 형상 생성
    physx::PxHeightFieldGeometry hfGeom(
        pHeightField,
        physx::PxMeshGeometryFlags(),
        m_fHfHeightScale,
        m_fHfRowScale,
        m_fHfColumnScale
    );

    // 액터 생성 및 설정
    /*physx::PxTransform transform(
        physx::PxVec3(startX * m_fHfColumnScale, 0.0f, startZ * m_fHfRowScale)
    );*/

    physx::PxTransform transform(physx::PxIdentity);

    physx::PxRigidStatic* pActor = pPhysics->createRigidStatic(transform);
    if (!pActor)
    {
        pHeightField->release();
        return E_FAIL;
    }

    // 형상 재질 설정
    physx::PxMaterial* pMaterial = pPhysics->createMaterial(0.5f, 0.5f, 0.1f);  // 마찰, 반발 계수 설정

    // 1. 이미 구해놓은 값들을 사용해 지형의 전체 너비와 깊이를 계산합니다.
    _float fWidth = (heightFieldDesc.nbColumns - 1) * m_fHfColumnScale;
    _float fDepth = (heightFieldDesc.nbRows - 1) * m_fHfRowScale;

    // 2. 중앙에 맞추기 위한 Local Pose를 생성합니다.
    physx::PxTransform localPose(physx::PxVec3(-fWidth * 0.5f, 0.f, -fDepth * 0.5f));
    physx::PxTransform shapeLocalPose(physx::PxIdentity);

    // 형상을 액터에 추가
    physx::PxShape* pShape = pPhysics->createShape(hfGeom, *pMaterial, true);
    // The fix! Enable Scene Query Shape
    //pShape->setLocalPose(localPose);
    pShape->setLocalPose(shapeLocalPose); // 로컬 포즈 설정
    pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    pShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);  // 시각화 비활성화로 성능 향상
    pShape->setSimulationFilterData(physx::PxFilterData(1, 0, 0, 0));  // 필터링 데이터 설정
    pShape->setContactOffset(0.1f);
    pActor->attachShape(*pShape);
    pShape->release();
    pMaterial->release();

    // 물리 씬에 액터 추가
    m_pGameInstance->Get_PhysicsScene()->addActor(*pActor);

    // 생성된 리소스 관리
    HEIGHTFIELD_DESC desc;
    desc.pHeightField = pHeightField;
    desc.pActor = pActor;

    m_HeightFieldDescs[tileIndex] = desc;

    return S_OK;
}

HRESULT CHeightField::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("HeightField", ImGuiTreeNodeFlags_DefaultOpen))
	{

        ImGui::SliderFloat("Height Scale", &m_fHfHeightScale, 0.001f, 10.0f);
        ImGui::SliderFloat("Row Scale", &m_fHfRowScale, 0.1f, 10.0f);
        ImGui::SliderFloat("Column Scale", &m_fHfColumnScale, 0.1f, 10.0f);

        // GPU 물리 상태 표시
        PxScene* pScene = m_pGameInstance->Get_PhysicsScene();
        if (pScene)
        {
            bool isGpuEnabled = (pScene->getFlags() & PxSceneFlag::eENABLE_GPU_DYNAMICS);
            ImGui::TextColored(
                isGpuEnabled ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
                isGpuEnabled ? "GPU Physics: Enabled" : "GPU Physics: Disabled (Using CPU)"
            );
        }
	}
	ImGui::PopID();
	return S_OK;
}
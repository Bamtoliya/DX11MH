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
    m_TriangleMeshDescs.resize(m_iTileCountX * m_iTileCountZ);
    //m_HeightFieldDescs.resize(m_iTileCountX * m_iTileCountZ);

    //Create_TriangleMesh();

	// 각 타일마다 HeightField 생성
	for (_uint z = 0; z < m_iTileCountZ; ++z)
	{
		for (_uint x = 0; x < m_iTileCountX; ++x)
		{
			_uint tileIndex = z * m_iTileCountX + x;
			_uint startX = x * (gridVerticesX - 1);
			_uint startZ = z * (gridVerticesZ - 1);
    
			// 경계를 넘어가지 않도록 조정
			if (startX >= numVerticesX || startZ >= numVerticesZ)
				continue;
    
			// 현재 그리드에 대한 실제 버텍스 수 계산 (경계 처리)
			_uint actualGridVerticesX = min(gridVerticesX, numVerticesX - startX);
			_uint actualGridVerticesZ = min(gridVerticesZ, numVerticesZ - startZ);

            if (FAILED(Create_TriangleMesh(pVertexPositions, numVerticesX, numVerticesZ,
                startX, startZ, actualGridVerticesX, actualGridVerticesZ,
                tileIndex)))
            {
                return E_FAIL;
            }
    
			//if (FAILED(Create_HeightField(pVertexPositions, numVerticesX, numVerticesZ,
			//	startX, startZ, actualGridVerticesX, actualGridVerticesZ,
			//	tileIndex)))
			//{
			//	return E_FAIL;
			//}
		}
	}

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


    for (auto& desc : m_TriangleMeshDescs)
    {
        m_pGameInstance->Get_PhysicsScene()->removeActor(*desc.pActor);
        if (desc.pActor != nullptr)
            desc.pActor->release();
        if (desc.pTriangleMesh != nullptr)
            desc.pTriangleMesh->release();
    }
    m_TriangleMeshDescs.clear();
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
    //heightFieldDesc.flags = physx::PxHeightFieldFlag::eNO_BOUNDARY_EDGES;

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
    physx::PxTransform transform(
        physx::PxVec3(startX * m_fHfColumnScale, 0.0f, startZ * m_fHfRowScale)
    );

    //physx::PxTransform transform(physx::PxIdentity);

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

HRESULT CHeightField::Create_TriangleMesh(const _float3* pVertexPositions, _uint totalVerticesX, _uint totalVerticesZ, _uint startX, _uint startZ, _uint gridVerticesX, _uint gridVerticesZ, _uint tileIndex)
{
    _uint numCols = gridVerticesX;
    _uint numRows = gridVerticesZ;

    PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
    if (!pPhysics) return E_FAIL;

    // 1. 타일에 해당하는 버텍스 데이터 추출
    
    PxVec3* vertices = new PxVec3[numCols * numRows];

    for (_uint z = 0; z < numRows; ++z)
    {
        for (_uint x = 0; x < numCols; ++x)
        {
            _uint localIndex = z * numCols + x;
            _uint terrainX = startX + z; // row가 X축
            _uint terrainZ = startZ + x; // col이 Z축
            _uint globalIndex = terrainZ * totalVerticesX + terrainX;
            vertices[localIndex] = (PxVec3(
                (pVertexPositions[globalIndex].x) * m_fHfColumnScale,
                (pVertexPositions[globalIndex].y) * m_fHfHeightScale,
                (pVertexPositions[globalIndex].z) * m_fHfRowScale
            ));
        }
    }


    std::vector<PxU32> indices;
    for (_uint z = 0; z < numRows - 1; ++z)
    {
        for (_uint x = 0; x < numCols - 1; ++x)
        {

            PxU32 i0 = z * numCols + x;         // 현재 로컬 타일의 (z, x)
            PxU32 i1 = z * numCols + (x + 1);   // 현재 로컬 타일의 (z, x+1)
            PxU32 i2 = (z + 1) * numCols + x;   // 현재 로컬 타일의 (z+1, x)
            PxU32 i3 = (z + 1) * numCols + (x + 1); // 현재 로컬 타일의 (z+1, x+1)

            //PxU32 i0 = z * numVerticesX + x;
            //PxU32 i1 = z * numVerticesX + (x + 1);
            //PxU32 i2 = (z + 1) * numVerticesX + x;
            //PxU32 i3 = (z + 1) * numVerticesX + (x + 1);
    

            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);

            //indices.push_back(i2); indices.push_back(i3); indices.push_back(i1);
            //indices.push_back(i2); indices.push_back(i1); indices.push_back(i0);
        }
    }
    if (indices.empty()) return S_OK; // 인덱스가 없으면 생성하지 않음


    // 3. TriangleMesh 설명자 생성
    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = static_cast<PxU32>(numCols * numRows);
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices;

    meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data = indices.data();

    // 4. 메쉬 쿠킹
    PxCookingParams cookingParams(pPhysics->getTolerancesScale());
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
    //cookingParams.meshWeldTolerance = 0.001f; // 필요에 따라 설정
    //cookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES); // 필요에 따라 설정
    //cookingParams.midphaseDesc = PxMeshMidPhase::eBVH34; // 성능 향상을 위해 BVH34 사용 고려

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;
    bool bSuccess = PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer, &result);
    if (!bSuccess) {
        MSG_BOX("PxCookTriangleMesh failed.");
        switch (result)
        {
        case PxTriangleMeshCookingResult::eSUCCESS:
            // 이 경우는 bSuccess가 false인데 eSUCCESS가 오는 경우는 거의 없음
            MSG_BOX("Cooking failed but result is eSUCCESS (unexpected).");
            break;
        case PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
            MSG_BOX("Cooking failed: A triangle is too large. Increase PxCookingParams::areaTestEpsilon or subdivide the triangle.");
            // 이 경우, 스케일 값(m_fHfColumnScale 등)이 너무 크거나, 특정 삼각형의 면적이 과도하게 클 수 있습니다.
            // 또는 PxTolerancesScale 값을 조정해야 할 수도 있습니다.
            break;
        case PxTriangleMeshCookingResult::eFAILURE: // 일반적인 실패
            MSG_BOX("Cooking failed: Generic failure (eFAILURE). Check input mesh data for degeneracies (zero-area triangles, non-manifold geometry, etc.).");
            // "internal error: BV4 tree failed to build"는 종종 이 eFAILURE와 함께 발생합니다.
            // 입력 데이터(vertices, indices)의 유효성을 철저히 검사해야 합니다.
            // - 정점 중복 (같은 위치에 여러 정점)
            // - 매우 좁거나 긴 삼각형 (degenerate triangles)
            // - 인덱스가 잘못되어 삼각형이 제대로 정의되지 않는 경우
            // - Non-manifold geometry (메쉬가 물리적으로 불가능한 형태)
            break;
            // 추가적인 PxTriangleMeshCookingResult::Enum 값들에 대한 case를 넣어줄 수 있습니다.
        default:
            MSG_BOX("Cooking failed: Unknown cooking result enum.");
            break;
        }
        return E_FAIL;
    }

    // 5. 쿠킹된 데이터로 TriangleMesh 생성
    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    PxTriangleMesh* pTriangleMesh = pPhysics->createTriangleMesh(readBuffer);
    if (!pTriangleMesh) {
        return E_FAIL;
    }

    // 6. 액터 및 형상 생성
    // 타일의 월드 위치 계산: HeightField와 유사하게 startX, startZ와 스케일을 사용
    // 단, 버텍스 자체에 스케일을 적용했으므로, 액터의 트랜스폼은 타일의 시작점을 기준으로 해야 합니다.
    // 버텍스 좌표가 이미 월드 좌표 기준이라면 PxIdentity 사용 가능
    // 여기서는 버텍스 좌표가 로컬(0,0 시작)이라고 가정하고, 액터 트랜스폼으로 위치를 잡습니다.
    // 하지만 위에서 pAllVertexPositions를 사용했으므로, 각 타일의 버텍스는 이미 월드 공간상의 상대적 위치를 가집니다.
    // 따라서 액터의 위치는 (0,0,0) 또는 타일링을 하지 않는 경우 지형의 원점이 됩니다.
    // 타일링을 한다면, 각 타일의 버텍스는 해당 타일의 로컬 원점을 기준으로 생성하고, 액터 트랜스폼으로 배치해야 합니다.
    // 현재 코드는 전체 지형에서 타일 부분의 버텍스를 '잘라내어' 사용하므로, 버텍스 좌표는 이미 월드 공간 기준입니다.
    // 따라서 액터의 Transform은 PxIdentity가 되어야 합니다.
    // 만약 각 타일의 버텍스를 (0,0)에서 시작하도록 재계산한다면, 아래 Transform이 필요합니다.
    
    /*physx::PxTransform transform(
        physx::PxVec3(startX * m_fHfColumnScale, 0.0f, startZ * m_fHfRowScale)
    );*/
    
    physx::PxTransform transform(physx::PxIdentity); // 버텍스가 이미 월드 좌표 기준이므로 Identity 사용

    physx::PxRigidStatic* pActor = pPhysics->createRigidStatic(transform);
    if (!pActor) {
        pTriangleMesh->release();
        return E_FAIL;
    }

    physx::PxMaterial* pMaterial = pPhysics->createMaterial(0.5f, 0.5f, 0.1f); // 기본 재질
    // TriangleMeshGeometry 생성 시 스케일링은 이미 버텍스에 적용했으므로 PxVec3(1.f) 사용
    physx::PxTriangleMeshGeometry triGeom(pTriangleMesh, PxVec3(1.0f, 1.0f, 1.0f));


    physx::PxShape* pShape = pPhysics->createShape(triGeom, *pMaterial, true);
    if (!pShape) {
        pMaterial->release();
        pActor->release();
        pTriangleMesh->release();
        return E_FAIL;
    }
    // pShape->setLocalPose(physx::PxIdentity); // 액터 내 로컬 포즈, 보통 Identity
    pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    pShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
    pShape->setSimulationFilterData(physx::PxFilterData(1, 0, 0, 0));
    pShape->setContactOffset(0.1f); // 필요에 따라 조정
    pActor->attachShape(*pShape);

    pShape->release(); // 액터에 attach 후 release
    pMaterial->release(); // 액터에 attach 후 release

    // 7. 물리 씬에 액터 추가
    m_pGameInstance->Get_PhysicsScene()->addActor(*pActor);

    m_TriangleMeshDescs[tileIndex].pTriangleMesh = pTriangleMesh; // m_HeightFieldDescs 대신 사용
    m_TriangleMeshDescs[tileIndex].pActor = pActor;
    Safe_Delete_Array(vertices);

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
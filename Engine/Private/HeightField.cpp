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
	m_HeightFieldDescs.resize(m_iTileCountX * m_iTileCountZ);

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
        
		if (desc.pActor != nullptr)
		{
			m_pGameInstance->Get_PhysicsScene()->removeActor(*desc.pActor);
			desc.pActor->release();
		}
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

//HRESULT CHeightField::Create_TriangleMesh(const _float3* pVertexPositions, _uint totalVerticesX, _uint totalVerticesZ, _uint startX, _uint startZ, _uint gridVerticesX, _uint gridVerticesZ, _uint tileIndex)
//{
//    _uint numCols = gridVerticesX;
//    _uint numRows = gridVerticesZ;
//
//    PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
//    if (!pPhysics) return E_FAIL;
//
//    // 1. 타일에 해당하는 버텍스 데이터 추출
//    
//    PxVec3* vertices = new PxVec3[numCols * numRows];
//
//    for (_uint z = 0; z < numRows; ++z)
//    {
//        for (_uint x = 0; x < numCols; ++x)
//        {
//            _uint localIndex = z * numCols + x;
//            _uint terrainX = startX + z; // row가 X축
//            _uint terrainZ = startZ + x; // col이 Z축
//            _uint globalIndex = terrainZ * totalVerticesX + terrainX;
//            vertices[localIndex] = (PxVec3(
//                (pVertexPositions[globalIndex].x) * m_fHfColumnScale,
//                (pVertexPositions[globalIndex].y) * m_fHfHeightScale,
//                (pVertexPositions[globalIndex].z) * m_fHfRowScale
//            ));
//        }
//    }
//
//
//    std::vector<PxU32> indices;
//    for (_uint z = 0; z < numRows - 1; ++z)
//    {
//        for (_uint x = 0; x < numCols - 1; ++x)
//        {
//
//            PxU32 i0 = z * numCols + x;         // 현재 로컬 타일의 (z, x)
//            PxU32 i1 = z * numCols + (x + 1);   // 현재 로컬 타일의 (z, x+1)
//            PxU32 i2 = (z + 1) * numCols + x;   // 현재 로컬 타일의 (z+1, x)
//            PxU32 i3 = (z + 1) * numCols + (x + 1); // 현재 로컬 타일의 (z+1, x+1)
//
//            //PxU32 i0 = z * numVerticesX + x;
//            //PxU32 i1 = z * numVerticesX + (x + 1);
//            //PxU32 i2 = (z + 1) * numVerticesX + x;
//            //PxU32 i3 = (z + 1) * numVerticesX + (x + 1);
//    
//
//            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
//            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
//
//            //indices.push_back(i2); indices.push_back(i3); indices.push_back(i1);
//            //indices.push_back(i2); indices.push_back(i1); indices.push_back(i0);
//        }
//    }
//    if (indices.empty()) return S_OK; // 인덱스가 없으면 생성하지 않음
//
//
//    // 3. TriangleMesh 설명자 생성
//    PxTriangleMeshDesc meshDesc;
//    meshDesc.points.count = static_cast<PxU32>(numCols * numRows);
//    meshDesc.points.stride = sizeof(PxVec3);
//    meshDesc.points.data = vertices;
//
//    meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
//    meshDesc.triangles.stride = 3 * sizeof(PxU32);
//    meshDesc.triangles.data = indices.data();
//
//    // 4. 메쉬 쿠킹
//    PxCookingParams cookingParams(pPhysics->getTolerancesScale());
//    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
//    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
//    //cookingParams.meshWeldTolerance = 0.001f; // 필요에 따라 설정
//    //cookingParams.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES); // 필요에 따라 설정
//    //cookingParams.midphaseDesc = PxMeshMidPhase::eBVH34; // 성능 향상을 위해 BVH34 사용 고려
//
//    PxDefaultMemoryOutputStream writeBuffer;
//    PxTriangleMeshCookingResult::Enum result;
//    bool bSuccess = PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer, &result);
//    if (!bSuccess) {
//        MSG_BOX("PxCookTriangleMesh failed.");
//        switch (result)
//        {
//        case PxTriangleMeshCookingResult::eSUCCESS:
//            // 이 경우는 bSuccess가 false인데 eSUCCESS가 오는 경우는 거의 없음
//            MSG_BOX("Cooking failed but result is eSUCCESS (unexpected).");
//            break;
//        case PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
//            MSG_BOX("Cooking failed: A triangle is too large. Increase PxCookingParams::areaTestEpsilon or subdivide the triangle.");
//            // 이 경우, 스케일 값(m_fHfColumnScale 등)이 너무 크거나, 특정 삼각형의 면적이 과도하게 클 수 있습니다.
//            // 또는 PxTolerancesScale 값을 조정해야 할 수도 있습니다.
//            break;
//        case PxTriangleMeshCookingResult::eFAILURE: // 일반적인 실패
//            MSG_BOX("Cooking failed: Generic failure (eFAILURE). Check input mesh data for degeneracies (zero-area triangles, non-manifold geometry, etc.).");
//            // "internal error: BV4 tree failed to build"는 종종 이 eFAILURE와 함께 발생합니다.
//            // 입력 데이터(vertices, indices)의 유효성을 철저히 검사해야 합니다.
//            // - 정점 중복 (같은 위치에 여러 정점)
//            // - 매우 좁거나 긴 삼각형 (degenerate triangles)
//            // - 인덱스가 잘못되어 삼각형이 제대로 정의되지 않는 경우
//            // - Non-manifold geometry (메쉬가 물리적으로 불가능한 형태)
//            break;
//            // 추가적인 PxTriangleMeshCookingResult::Enum 값들에 대한 case를 넣어줄 수 있습니다.
//        default:
//            MSG_BOX("Cooking failed: Unknown cooking result enum.");
//            break;
//        }
//        return E_FAIL;
//    }
//
//    // 5. 쿠킹된 데이터로 TriangleMesh 생성
//    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
//    PxTriangleMesh* pTriangleMesh = pPhysics->createTriangleMesh(readBuffer);
//    if (!pTriangleMesh) {
//        return E_FAIL;
//    }
//
//    // 6. 액터 및 형상 생성
//    // 타일의 월드 위치 계산: HeightField와 유사하게 startX, startZ와 스케일을 사용
//    // 단, 버텍스 자체에 스케일을 적용했으므로, 액터의 트랜스폼은 타일의 시작점을 기준으로 해야 합니다.
//    // 버텍스 좌표가 이미 월드 좌표 기준이라면 PxIdentity 사용 가능
//    // 여기서는 버텍스 좌표가 로컬(0,0 시작)이라고 가정하고, 액터 트랜스폼으로 위치를 잡습니다.
//    // 하지만 위에서 pAllVertexPositions를 사용했으므로, 각 타일의 버텍스는 이미 월드 공간상의 상대적 위치를 가집니다.
//    // 따라서 액터의 위치는 (0,0,0) 또는 타일링을 하지 않는 경우 지형의 원점이 됩니다.
//    // 타일링을 한다면, 각 타일의 버텍스는 해당 타일의 로컬 원점을 기준으로 생성하고, 액터 트랜스폼으로 배치해야 합니다.
//    // 현재 코드는 전체 지형에서 타일 부분의 버텍스를 '잘라내어' 사용하므로, 버텍스 좌표는 이미 월드 공간 기준입니다.
//    // 따라서 액터의 Transform은 PxIdentity가 되어야 합니다.
//    // 만약 각 타일의 버텍스를 (0,0)에서 시작하도록 재계산한다면, 아래 Transform이 필요합니다.
//    
//    /*physx::PxTransform transform(
//        physx::PxVec3(startX * m_fHfColumnScale, 0.0f, startZ * m_fHfRowScale)
//    );*/
//    
//    physx::PxTransform transform(physx::PxIdentity); // 버텍스가 이미 월드 좌표 기준이므로 Identity 사용
//
//    physx::PxRigidStatic* pActor = pPhysics->createRigidStatic(transform);
//    if (!pActor) {
//        pTriangleMesh->release();
//        return E_FAIL;
//    }
//
//    physx::PxMaterial* pMaterial = pPhysics->createMaterial(0.5f, 0.5f, 0.1f); // 기본 재질
//    // TriangleMeshGeometry 생성 시 스케일링은 이미 버텍스에 적용했으므로 PxVec3(1.f) 사용
//    physx::PxTriangleMeshGeometry triGeom(pTriangleMesh, PxVec3(1.0f, 1.0f, 1.0f));
//
//
//    physx::PxShape* pShape = pPhysics->createShape(triGeom, *pMaterial, true);
//    if (!pShape) {
//        pMaterial->release();
//        pActor->release();
//        pTriangleMesh->release();
//        return E_FAIL;
//    }
//    // pShape->setLocalPose(physx::PxIdentity); // 액터 내 로컬 포즈, 보통 Identity
//    pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
//    pShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
//    pShape->setSimulationFilterData(physx::PxFilterData(1, 0, 0, 0));
//    pShape->setContactOffset(0.1f); // 필요에 따라 조정
//    pActor->attachShape(*pShape);
//
//    pShape->release(); // 액터에 attach 후 release
//    pMaterial->release(); // 액터에 attach 후 release
//
//    // 7. 물리 씬에 액터 추가
//    m_pGameInstance->Get_PhysicsScene()->addActor(*pActor);
//
//    m_TriangleMeshDescs[tileIndex].pTriangleMesh = pTriangleMesh; // m_HeightFieldDescs 대신 사용
//    m_TriangleMeshDescs[tileIndex].pActor = pActor;
//    Safe_Delete_Array(vertices);
//
//    return S_OK;
//}

// 새로 작성된 Create_TriangleMesh 함수
HRESULT CHeightField::Create_TriangleMesh(
	const _float3* pAllGlobalVertexPositions,
	_uint totalGlobalVerticesX,
	_uint totalGlobalVerticesZ,
	_uint tileWorldStartX,
	_uint tileWorldStartZ,
	_uint numVerticesXInCurrentTile,
	_uint numVerticesZInCurrentTile,
	_uint tileDescIndex)
{
	if (numVerticesXInCurrentTile < 2 || numVerticesZInCurrentTile < 2) {
		// 이 타일은 너무 작아서 유효한 삼각형 메쉬를 만들 수 없습니다.
		return S_OK;
	}

	PxPhysics* pPhysics = m_pGameInstance->Get_Physics();
	if (!pPhysics) {
		MSG_BOX("PhysX SDK PxPhysics not initialized.");
		return E_FAIL;
	}

	// 1. 현재 타일에 해당하는 로컬 정점 데이터 추출 및 생성
	//    정점 좌표는 (0,0,0)을 기준으로 생성하고, 액터의 Transform으로 타일의 실제 월드 위치와 스케일을 적용합니다.
	std::vector<PxVec3> localVertices;
	localVertices.reserve(numVerticesXInCurrentTile * numVerticesZInCurrentTile);

	for (_uint z_local = 0; z_local < numVerticesZInCurrentTile; ++z_local) {
		for (_uint x_local = 0; x_local < numVerticesXInCurrentTile; ++x_local) {
			_uint globalX = tileWorldStartX + x_local;
			_uint globalZ = tileWorldStartZ + z_local;
			// global 인덱스가 전체 지형 범위를 벗어나지 않도록 방어
			if (globalX >= totalGlobalVerticesX || globalZ >= totalGlobalVerticesZ) {
				// 이 경우는 Late_Initialize에서 이미 필터링되어야 하지만, 안전을 위해 추가
				// 예를 들어, 마지막 타일의 정점 수가 정확히 계산되지 않은 경우 발생 가능
				// 여기서는 간단히 잘못된 정점 대신 (0,0,0) 또는 이전 정점 사용 등의 대체 로직이 필요할 수 있으나,
				// 우선은 유효한 데이터만 들어온다고 가정.
				// 실제로는 이 상황이 발생하면 Late_Initialize의 타일 분할 로직 점검 필요.
				continue;
			}
			_uint globalIndex = globalZ * totalGlobalVerticesX + globalX;

			// 로컬 정점: X, Z는 (0,0)에서 시작, Y는 원본 높이 사용
			// 스케일은 나중에 PxTriangleMeshGeometry 또는 Actor Transform에서 적용
			localVertices.push_back(PxVec3(
				static_cast<float>(x_local), // 로컬 X 좌표 (0부터 시작)
				pAllGlobalVertexPositions[globalIndex].y, // 원본 Y 높이
				static_cast<float>(z_local)  // 로컬 Z 좌표 (0부터 시작)
			));
		}
	}
	// 로컬 정점 수가 예상과 다르면 (위의 continue 등으로 인해) 처리
	if (localVertices.size() != numVerticesXInCurrentTile * numVerticesZInCurrentTile) {
		// MSG_BOX("Vertex count mismatch for tile.");
		// 이 경우, 인덱싱 문제 발생 가능성이 높으므로 실패 처리 또는 빈 메쉬 처리
		return E_FAIL; // 또는 S_OK로 빈 메쉬 처리
	}


	// 2. 로컬 인덱스 데이터 생성 (현재 타일의 로컬 정점 기준)
	std::vector<PxU32> localIndices;
	_uint numCellsXInTile = numVerticesXInCurrentTile - 1;
	_uint numCellsZInTile = numVerticesZInCurrentTile - 1;
	localIndices.reserve(numCellsXInTile * numCellsZInTile * 6);

	for (_uint z_local_cell = 0; z_local_cell < numCellsZInTile; ++z_local_cell) {
		for (_uint x_local_cell = 0; x_local_cell < numCellsXInTile; ++x_local_cell) {
			PxU32 i0 = z_local_cell * numVerticesXInCurrentTile + x_local_cell;
			PxU32 i1 = z_local_cell * numVerticesXInCurrentTile + (x_local_cell + 1);
			PxU32 i2 = (z_local_cell + 1) * numVerticesXInCurrentTile + x_local_cell;
			PxU32 i3 = (z_local_cell + 1) * numVerticesXInCurrentTile + (x_local_cell + 1);

			// CCW 와인딩 순서 (DirectX 스타일과 유사)
			localIndices.push_back(i0); localIndices.push_back(i2); localIndices.push_back(i1);
			localIndices.push_back(i1); localIndices.push_back(i2); localIndices.push_back(i3);
		}
	}

	if (localIndices.empty()) {
		// MSG_BOX("No indices generated for triangle mesh tile (tile too thin).");
		return S_OK;
	}

	// 3. TriangleMesh 설명자 생성
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(localVertices.size());
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = localVertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(localIndices.size() / 3);
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = localIndices.data();

	// 4. 메쉬 쿠킹
	PxCookingParams cookingParams(pPhysics->getTolerancesScale());
	// cookingParams.midphaseDesc = PxMeshMidPhase::eBVH34; // 성능 향상을 위해 사용 고려
	// 기본값으로 먼저 테스트, 문제 발생 시 플래그 조정
	// cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	// cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;


	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum cookResult;
	bool bSuccess = PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer, &cookResult);

	if (!bSuccess) {
		char msg[512];
		sprintf_s(msg, sizeof(msg), "PxCookTriangleMesh failed for tile index %u. Result: %d. Vertices: %u, Triangles: %u",
			tileDescIndex, static_cast<int>(cookResult), meshDesc.points.count, meshDesc.triangles.count);
		// 실패 시 생성된 로컬 버텍스/인덱스 정보 등을 로그로 남기면 디버깅에 도움됨
		return E_FAIL;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* pTriangleMesh = pPhysics->createTriangleMesh(readBuffer);
	if (!pTriangleMesh) {
		MSG_BOX("Failed to create PxTriangleMesh from cooked data.");
		return E_FAIL;
	}

	// 5. 액터 및 형상 생성
	// 액터의 위치: 현재 타일의 월드 시작 위치.
	// CVIBuffer_Terrain의 정점 좌표가 (0,0)에서 시작하고 간격이 1이라고 가정.
	// 이 시작 위치에 m_fHfColumnScale과 m_fHfRowScale을 곱하여 실제 월드 위치를 계산.
	// Y 좌표는 지형의 실제 높이에 따라 달라지지만, 여기서는 편의상 0으로 설정하거나,
	// pAllGlobalVertexPositions에서 해당 타일의 (0,0) 정점의 Y값을 가져와 m_fHfHeightScale을 곱한 값을 사용.
	_float3 tileWorldOriginPos = pAllGlobalVertexPositions[tileWorldStartZ * totalGlobalVerticesX + tileWorldStartX];

	physx::PxTransform actorPose(
		PxVec3(tileWorldOriginPos.x * m_fHfColumnScale,
			0.f, // 액터의 Y 위치는 타일의 (0,0) 정점의 스케일된 높이
			tileWorldOriginPos.z * m_fHfRowScale),
		PxQuat(PxIdentity)
	);

	physx::PxRigidStatic* pActor = pPhysics->createRigidStatic(actorPose);
	if (!pActor) {
		pTriangleMesh->release();
		MSG_BOX("Failed to create PxRigidStatic actor for tile.");
		return E_FAIL;
	}

	physx::PxMaterial* pMaterial = pPhysics->createMaterial(0.5f, 0.5f, 0.1f); // 기본 재질

	// TriangleMeshGeometry 생성 시 스케일 적용
	// 로컬 정점은 (0,0) 기준으로 생성되었고, 각 정점 간 간격은 1.0으로 가정.
	// 여기에 m_fHfColumnScale, m_fHfHeightScale, m_fHfRowScale을 곱하여 실제 크기 및 높이 비율을 맞춤.
	physx::PxMeshScale geomScale(
		PxVec3(m_fHfColumnScale, m_fHfHeightScale, m_fHfRowScale),
		PxQuat(PxIdentity)
	);
	physx::PxTriangleMeshGeometry triGeom(pTriangleMesh, geomScale);

	physx::PxShape* pShape = pPhysics->createShape(triGeom, *pMaterial, true);
	if (!pShape) {
		pMaterial->release();
		pActor->release();
		pTriangleMesh->release();
		MSG_BOX("Failed to create PxShape for tile.");
		return E_FAIL;
	}

	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pShape->setFlag(PxShapeFlag::eVISUALIZATION, false); // 디버깅 시 true로 변경 가능
	pActor->attachShape(*pShape);

	pShape->release();
	pMaterial->release();

	// 6. 물리 씬에 액터 추가
	m_pGameInstance->Get_PhysicsScene()->addActor(*pActor);

	// 7. 생성된 리소스 관리
	m_TriangleMeshDescs[tileDescIndex].pTriangleMesh = pTriangleMesh;
	m_TriangleMeshDescs[tileDescIndex].pActor = pActor;

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
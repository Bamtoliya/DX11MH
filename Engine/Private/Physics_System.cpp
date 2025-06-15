#pragma once
#include "Physics_System.h"
#include "GameInstance.h"

#pragma region OmniPvd
#include "../../External Libraries/physx/include/omnipvd/PxOmniPvd.h"
#include "../../External Libraries/physx/include/OmniPvdWriter.h"
#include "../../External Libraries/physx/include/OmniPvdFileWriteStream.h"  
#pragma endregion



#pragma region Constructor/Destructor
CPhysics_System::CPhysics_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance { CGameInstance::Get_Instance() }
    , m_pDevice { pDevice }
    , m_pContext { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

CPhysics_System* CPhysics_System::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPhysics_System* pInstance = new CPhysics_System(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CPhysics_System");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPhysics_System::Free()
{
    __super::Free(); // 만약 CBase에 가상 Free가 있고 호출해야 한다면

    // PhysX 객체 해제 (생성 역순으로 하는 것이 안전)
    PX_RELEASE(m_pDispatcher);
    if (m_pCudaContextManager) // CUDA 컨텍스트 매니저 해제 추가
        PX_RELEASE(m_pCudaContextManager);
    PX_RELEASE(m_pPhysics); // 6. Physics 객체 해제
    PX_RELEASE(m_pPvd); // 7. PVD 해제 (연결했다면)
    PX_RELEASE(m_pFoundation) // 8. Foundation 해제 (가장 마지막)

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}

#pragma endregion

HRESULT CPhysics_System::Initialize()
{
    // Foundation 생성
    m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
    if (!m_pFoundation)
        return E_FAIL;

    // PVD (PhysX Visual Debugger) 연결
    m_pPvd = PxCreatePvd(*m_pFoundation);
    PxPvdTransport* pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    m_pPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL);

    // CUDA Context Manager 생성 (GPU 물리 사용 시)
    physx::PxCudaContextManagerDesc cudaContextManagerDesc;
	
    // 필요에 따라 cudaContextManagerDesc 설정 (예: 그래픽 장치 지정)
    // cudaContextManagerDesc.graphicsDevice = m_pDevice; // DirectX 장치 전달 (D3D11Device 사용 시)
    // 주의: PxCudaContextManagerDesc에 D3D 장치를 직접 할당하는 것은 PhysX 버전에 따라 다를 수 있습니다.
    //       일반적으로 PhysX는 자체적으로 CUDA 장치를 찾거나, 사용자가 명시적으로 인터페이스를 통해 설정합니다.
    //       PxCreateCudaContextManager는 PxFoundation을 인자로 받습니다.

    #if defined(PX_WINDOWS) && PX_SUPPORT_GPU_PHYSX
        // Direct3D 장치와 CUDA 연결
        cudaContextManagerDesc.graphicsDevice = m_pDevice;
        cudaContextManagerDesc.deviceOrdinal = 0; // 기본 CUDA 디바이스 사용 (여러 GPU가 있는 경우 적절히 설정 필요)
    #endif
        // 추가 CUDA 설정
        cudaContextManagerDesc.appGUID = "Sonic_Frontier_Test"; // 고유한 GUID 설정 (선택사항)
    m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc);
    if (!m_pCudaContextManager) {
        // CUDA 컨텍스트 매니저 생성 실패 (예: CUDA 지원 GPU 없음, 드라이버 문제 등)
        // 이 경우 GPU 물리 기능을 사용할 수 없으므로, 경고를 로깅하고 CPU 폴백을 고려할 수 있습니다.
        MSG_BOX("Warning: Failed to create PxCudaContextManager. GPU physics will be unavailable.");
        // m_pCudaContextManager는 nullptr로 유지됩니다.
    }

    // Physics 생성
    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
    if (!m_pPhysics)
        return E_FAIL;


    if (!PxInitExtensions(*m_pPhysics, m_pPvd)) // PVD는 디버그 빌드에서만 유효할 수 있음
    {
        MSG_BOX("PxInitExtensions failed!");
        // ... (Physics, Foundation, PVD 해제) ...
        return E_FAIL;
    }

    // CPU Dispatcher 생성 (멀티스레딩 지원)
    unsigned int numThreads = std::thread::hardware_concurrency();
    m_pDispatcher = PxDefaultCpuDispatcherCreate(numThreads > 0 ? numThreads : 2); // 스레드 2개 사용
    if (!m_pDispatcher)
        return E_FAIL;

    m_pScene = Create_Scene();
    if (!m_pScene) return E_FAIL;

    return S_OK;
}

void CPhysics_System::Priority_Update(_float fTimeDelta)
{
}

void CPhysics_System::Update(_float fTimeDelta)
{
#ifdef _DEBUG
    if (KEY_DOWN(DIK_F3))
        m_bPhysicsDebugDraw = !m_bPhysicsDebugDraw;
#endif // _DEBUG

    if (m_pScene)
    {
        m_pScene->simulate(fTimeDelta);
        m_pScene->fetchResults(true);

        Collect_DebugRenderData(); // 매 프레임 디버그 데이터 수집
    }
}

void CPhysics_System::Late_Update(_float fTimeDelta)
{
}

physx::PxFilterFlags CustomFilterShader(
    physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
    // 충돌 콜백을 받기 위한 플래그 설정
    pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_TOUCH_LOST;
    return physx::PxFilterFlag::eDEFAULT;
}

PxScene* CPhysics_System::Create_Scene()
{
    if (m_pPhysics == nullptr || m_pDispatcher == nullptr)
        return nullptr;

    PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = m_pDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
    sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
    sceneDesc.simulationEventCallback = this;
    sceneDesc.filterShader = CustomFilterShader;

    // GPU 물리 시뮬레이션을 위한 설정
    if (m_pCudaContextManager && m_pCudaContextManager->contextIsValid()) // CUDA 컨텍스트 매니저가 유효한지 확인
    {
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS; // GPU 다이나믹스 활성화 플래그 (버전에 따라 다를 수 있음, 일반적으로는 gpuDispatcher 설정으로 충분)
        //sceneDesc.flags |= PxSceneFlag::eENABLE_DIRECT_GPU_API; // GPU 다이나믹스 활성화 (버전에 따라 이 플래그가 없거나, cudaContextManager 설정만으로 충분할 수 있음)
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;    // GPU 브로드페이즈 사용
        sceneDesc.cudaContextManager = m_pCudaContextManager;
        sceneDesc.solverType = PxSolverType::eTGS; // GPU 가속에 최적화된 솔버

        //sceneDesc.gpuMaxNumPartitions = 8;  // 병렬 처리 파티션 최적화
        //sceneDesc.gpuDynamicsConfig.capa *= 2;  // 제약 버퍼 증가
    }
    else
    {
        // GPU 사용 불가 시 CPU 브로드페이즈 사용 (예: SAP, MBP)
        sceneDesc.broadPhaseType = PxBroadPhaseType::eSAP; // 예시: Sweep and Prune
        MSG_BOX("Info: CUDA Context Manager not available or invalid. Using CPU broadphase.");
    }


    m_pScene = m_pPhysics->createScene(sceneDesc);

    if (m_pScene)
    {
        PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
        if (pvdClient)
        {
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }

        // 디버그 시각화 파라미터 설정
        m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
        // 가장 중요한 eCOLLISION_SHAPES 활성화
        m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 0.0f);
        // 필요에 따라 다른 파라미터도 활성화 가능
        // m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
        // m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_POINT, 1.0f);
        // m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);   
    }

    // GPU 물리 시각화 최적화
    if (m_pScene && m_pCudaContextManager && m_pCudaContextManager->contextIsValid()) {
        // GPU 시뮬레이션에 최적화된 시각화 설정
        m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
        m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
        m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 0.5f);
        m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_EDGES, 0.0f);
    }

    //m_pScene->getRenderBuffer();

    return m_pScene;
}

_float3 FromPxVec3(const physx::PxVec3& vec)
{
    return _float3(vec.x, vec.y, vec.z);
}

void CPhysics_System::Collect_DebugRenderData()
{
    m_DebugLines.clear();
    m_DebugTriangles.clear();

    if (!m_pScene)
        return;

    //const physx::PxRenderBuffer& rb = m_pScene->getRenderBuffer();

    // 1. 씬의 모든 액터 가져오기
    PxU32 actorCount = m_pScene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC);
    std::vector<physx::PxActor*> actors(actorCount);
    m_pScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC | physx::PxActorTypeFlag::eRIGID_DYNAMIC, actors.data(), actorCount);

    for (PxU32 i = 0; i < actorCount; ++i)
    {
        physx::PxRigidActor* rigidActor = static_cast<physx::PxRigidActor*>(actors[i]);
        PxU32 shapeCount = rigidActor->getNbShapes();
        std::vector<physx::PxShape*> shapes(shapeCount);
        rigidActor->getShapes(shapes.data(), shapeCount);

        for (PxU32 j = 0; j < shapeCount; ++j)
        {
            physx::PxShape* shape = shapes[j];
            physx::PxGeometryType::Enum geoType = shape->getGeometry().getType();
            if (geoType == physx::PxGeometryType::eHEIGHTFIELD)
                continue; // HeightField는 디버그 드로우 제외

            // 액터의 전역 포즈와 형상의 로컬 포즈 결합
            PxTransform shapePose = PxShapeExt::getGlobalPose(*shape, *rigidActor);

            // 디버그 색상 설정 (예: 정적 액터는 초록색, 동적 액터는 빨간색)
            _float4 debugColor = (rigidActor->getType() == PxActorType::eRIGID_STATIC)
                ? _float4(0.0f, 1.0f, 0.0f, 1.0f)  // 초록색 (정적)
                : _float4(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색 (동적)

            // 형상 타입에 따라 적절한 디버그 정보 생성
            switch (geoType)
            {
            case physx::PxGeometryType::eBOX:
            {
                // 박스 형상 정보 가져오기
                const PxBoxGeometry boxGeom = static_cast<const PxBoxGeometry&>(shape->getGeometry());

                // 박스의 8개 꼭지점 계산
                PxVec3 vertices[8];
                vertices[0] = PxVec3(-boxGeom.halfExtents.x, -boxGeom.halfExtents.y, -boxGeom.halfExtents.z);
                vertices[1] = PxVec3(boxGeom.halfExtents.x, -boxGeom.halfExtents.y, -boxGeom.halfExtents.z);
                vertices[2] = PxVec3(boxGeom.halfExtents.x, -boxGeom.halfExtents.y, boxGeom.halfExtents.z);
                vertices[3] = PxVec3(-boxGeom.halfExtents.x, -boxGeom.halfExtents.y, boxGeom.halfExtents.z);
                vertices[4] = PxVec3(-boxGeom.halfExtents.x, boxGeom.halfExtents.y, -boxGeom.halfExtents.z);
                vertices[5] = PxVec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, -boxGeom.halfExtents.z);
                vertices[6] = PxVec3(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);
                vertices[7] = PxVec3(-boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);

                // 각 꼭지점을 전역 좌표로 변환
                for (int v = 0; v < 8; v++)
                    vertices[v] = shapePose.transform(vertices[v]);

                // 박스의 12개 에지 (라인) 추가
                // 아래 면
                AddDebugLine(vertices[0], vertices[1], debugColor);
                AddDebugLine(vertices[1], vertices[2], debugColor);
                AddDebugLine(vertices[2], vertices[3], debugColor);
                AddDebugLine(vertices[3], vertices[0], debugColor);
                // 위 면
                AddDebugLine(vertices[4], vertices[5], debugColor);
                AddDebugLine(vertices[5], vertices[6], debugColor);
                AddDebugLine(vertices[6], vertices[7], debugColor);
                AddDebugLine(vertices[7], vertices[4], debugColor);
                // 수직 연결 에지
                AddDebugLine(vertices[0], vertices[4], debugColor);
                AddDebugLine(vertices[1], vertices[5], debugColor);
                AddDebugLine(vertices[2], vertices[6], debugColor);
                AddDebugLine(vertices[3], vertices[7], debugColor);
                break;
            }
            case physx::PxGeometryType::eSPHERE:
            {
                // 구체 형상 정보 가져오기
                const PxSphereGeometry sphereGeom = static_cast<const PxSphereGeometry&>(shape->getGeometry());

                const int numSegments = 16; // 원 분할 수
                const float stepAngle = PxTwoPi / numSegments;

                // XY 평면에 원 그리기
                for (int i = 0; i < numSegments; i++)
                {
                    float angle1 = i * stepAngle;
                    float angle2 = (i + 1) * stepAngle;

                    PxVec3 p1(sphereGeom.radius * PxCos(angle1), sphereGeom.radius * PxSin(angle1), 0.0f);
                    PxVec3 p2(sphereGeom.radius * PxCos(angle2), sphereGeom.radius * PxSin(angle2), 0.0f);

                    AddDebugLine(shapePose.transform(p1), shapePose.transform(p2), debugColor);
                }

                // XZ 평면에 원 그리기
                for (int i = 0; i < numSegments; i++)
                {
                    float angle1 = i * stepAngle;
                    float angle2 = (i + 1) * stepAngle;

                    PxVec3 p1(sphereGeom.radius * PxCos(angle1), 0.0f, sphereGeom.radius * PxSin(angle1));
                    PxVec3 p2(sphereGeom.radius * PxCos(angle2), 0.0f, sphereGeom.radius * PxSin(angle2));

                    AddDebugLine(shapePose.transform(p1), shapePose.transform(p2), debugColor);
                }

                // YZ 평면에 원 그리기
                for (int i = 0; i < numSegments; i++)
                {
                    float angle1 = i * stepAngle;
                    float angle2 = (i + 1) * stepAngle;

                    PxVec3 p1(0.0f, sphereGeom.radius * PxCos(angle1), sphereGeom.radius * PxSin(angle1));
                    PxVec3 p2(0.0f, sphereGeom.radius * PxCos(angle2), sphereGeom.radius * PxSin(angle2));

                    AddDebugLine(shapePose.transform(p1), shapePose.transform(p2), debugColor);
                }
                break;
            }
            case physx::PxGeometryType::eCAPSULE:
            {
                // 캡슐 형상 정보 가져오기
                const PxCapsuleGeometry capsuleGeom = static_cast<const PxCapsuleGeometry&>(shape->getGeometry());

                const int numSegments = 16; // 원 분할 수
                const float stepAngle = PxTwoPi / numSegments;
                const float halfHeight = capsuleGeom.halfHeight;
                const float radius = capsuleGeom.radius;

                // 캡슐의 원통 부분 (XY 평면에 직사각형으로 표현)
                for (int i = 0; i < numSegments; i++)
                {
                    float angle1 = i * stepAngle;
                    float angle2 = (i + 1) * stepAngle;

                    // 아래쪽 원
                    PxVec3 p1(radius * PxCos(angle1), radius * PxSin(angle1), -halfHeight);
                    PxVec3 p2(radius * PxCos(angle2), radius * PxSin(angle2), -halfHeight);
                    AddDebugLine(shapePose.transform(p1), shapePose.transform(p2), debugColor);

                    // 위쪽 원
                    PxVec3 p3(radius * PxCos(angle1), radius * PxSin(angle1), halfHeight);
                    PxVec3 p4(radius * PxCos(angle2), radius * PxSin(angle2), halfHeight);
                    AddDebugLine(shapePose.transform(p3), shapePose.transform(p4), debugColor);

                    // 수직 연결선
                    if (i % 4 == 0) // 4개마다 하나씩만 그림 (너무 많지 않게)
                    {
                        AddDebugLine(shapePose.transform(p1), shapePose.transform(p3), debugColor);
                    }
                }

                // 양쪽 반구 (반원으로 표현)
                for (int i = 0; i < numSegments / 2; i++)
                {
                    float angle1 = i * stepAngle;
                    float angle2 = (i + 1) * stepAngle;

                    // 아래쪽 반구 (XZ 평면)
                    PxVec3 p1(radius * PxCos(angle1), 0.0f, -halfHeight - radius * PxSin(angle1));
                    PxVec3 p2(radius * PxCos(angle2), 0.0f, -halfHeight - radius * PxSin(angle2));
                    AddDebugLine(shapePose.transform(p1), shapePose.transform(p2), debugColor);

                    // 위쪽 반구 (XZ 평면)
                    PxVec3 p3(radius * PxCos(angle1), 0.0f, halfHeight + radius * PxSin(angle1));
                    PxVec3 p4(radius * PxCos(angle2), 0.0f, halfHeight + radius * PxSin(angle2));
                    AddDebugLine(shapePose.transform(p3), shapePose.transform(p4), debugColor);

                    // 아래쪽 반구 (YZ 평면)
                    PxVec3 p5(0.0f, radius * PxCos(angle1), -halfHeight - radius * PxSin(angle1));
                    PxVec3 p6(0.0f, radius * PxCos(angle2), -halfHeight - radius * PxSin(angle2));
                    AddDebugLine(shapePose.transform(p5), shapePose.transform(p6), debugColor);

                    // 위쪽 반구 (YZ 평면)
                    PxVec3 p7(0.0f, radius * PxCos(angle1), halfHeight + radius * PxSin(angle1));
                    PxVec3 p8(0.0f, radius * PxCos(angle2), halfHeight + radius * PxSin(angle2));
                    AddDebugLine(shapePose.transform(p7), shapePose.transform(p8), debugColor);
                }
                break;
            }
            // TODO: 다른 형상 타입 추가 (CONVEX_MESH, TRIANGLE_MESH 등)
            default:
                break;
            }
        }
    }
}


// 헬퍼 함수: 디버그 라인 추가
void CPhysics_System::AddDebugLine(const PxVec3& p0, const PxVec3& p1, const _float4& color)
{
    DEBUG_LINE line;
    line.vP0 = FromPxVec3(p0);
    line.vP1 = FromPxVec3(p1);
    line.vColor = color;
    m_DebugLines.push_back(line);
}

// 헬퍼 함수: 디버그 삼각형 추가
void CPhysics_System::AddDebugTriangle(const PxVec3& p0, const PxVec3& p1, const PxVec3& p2, const _float4& color)
{
    DEBUG_TRIANGLE tri;
    tri.vP0 = FromPxVec3(p0);
    tri.vP1 = FromPxVec3(p1);
    tri.vP2 = FromPxVec3(p2);
    tri.vColor = color;
    m_DebugTriangles.push_back(tri);
}


#pragma region Collision Event
void CPhysics_System::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void CPhysics_System::onWake(PxActor** actors, PxU32 count)
{
}

void CPhysics_System::onSleep(PxActor** actors, PxU32 count)
{
}

void CPhysics_System::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
        cout << "Physics_System OnContact Called\n";
}

void CPhysics_System::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void CPhysics_System::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
    cout << "Physics_System OnAdvance Called\n";
}

#pragma endregion
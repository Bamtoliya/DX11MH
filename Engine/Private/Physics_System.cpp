#include "Physics_System.h"
#include "GameInstance.h"

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

    return S_OK;
}

void CPhysics_System::Priority_Update(_float fTimeDelta)
{
}

void CPhysics_System::Update(_float fTimeDelta)
{
    if (m_pScene)
    {
        m_pScene->simulate(fTimeDelta);
        m_pScene->fetchResults(true);

        CollectDebugRenderData(); // 매 프레임 디버그 데이터 수집
    }
}

void CPhysics_System::Late_Update(_float fTimeDelta)
{
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

    // GPU 물리 시뮬레이션을 위한 설정
    if (m_pCudaContextManager && m_pCudaContextManager->contextIsValid()) // CUDA 컨텍스트 매니저가 유효한지 확인
    {
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS; // GPU 다이나믹스 활성화 플래그 (버전에 따라 다를 수 있음, 일반적으로는 gpuDispatcher 설정으로 충분)
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;    // GPU 브로드페이즈 사용
        sceneDesc.cudaContextManager = m_pCudaContextManager;
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS; // GPU 다이나믹스 활성화 (버전에 따라 이 플래그가 없거나, cudaContextManager 설정만으로 충분할 수 있음)
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;    // GPU 브로드페이즈 사용
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
        // 필요에 따라 다른 파라미터도 활성화 가능
        // m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 1.0f);
        // m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_POINT, 1.0f);
        // m_pScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 1.0f);
    }

    //m_pScene->getRenderBuffer();

    return m_pScene;
}

_float3 FromPxVec3(const physx::PxVec3& vec)
{
    return _float3(vec.x, vec.y, vec.z);
}

void CPhysics_System::CollectDebugRenderData()
{
    m_DebugLines.clear();
    m_DebugTriangles.clear();

    if (!m_pScene)
        return;

    const physx::PxRenderBuffer& rb = m_pScene->getRenderBuffer();

    // 라인 정보 수집
    for (physx::PxU32 i = 0; i < rb.getNbLines(); ++i)
    {
        const physx::PxDebugLine& pxLine = rb.getLines()[i];
        DEBUG_LINE line;
        line.vP0 = FromPxVec3(pxLine.pos0); // FromPxVec3 헬퍼 함수 필요 (PxVec3 -> _float3)
        line.vP1 = FromPxVec3(pxLine.pos1);
        // PhysX 색상은 0xAARRGGBB (uint32) 또는 각 정점별 색상. 여기서는 pos0의 색상만 사용.
        // 필요시 color0, color1 모두 고려.
        line.vColor = _float4(
            ((pxLine.color0 >> 16) & 0xFF) / 255.f, // R
            ((pxLine.color0 >> 8) & 0xFF) / 255.f,  // G
            (pxLine.color0 & 0xFF) / 255.f,         // B
            1.f                                     // A (Alpha - 항상 1로 설정 또는 필요에 따라 ((pxLine.color0 >> 24) & 0xFF) / 255.f)
        );
        m_DebugLines.push_back(line);
    }

    // 삼각형 정보 수집 (필요한 경우)
    for (physx::PxU32 i = 0; i < rb.getNbTriangles(); ++i)
    {
        const physx::PxDebugTriangle& pxTri = rb.getTriangles()[i];
        DEBUG_TRIANGLE tri;
        tri.vP0 = FromPxVec3(pxTri.pos0);
        tri.vP1 = FromPxVec3(pxTri.pos1);
        tri.vP2 = FromPxVec3(pxTri.pos2);
        tri.vColor = _float4(
            ((pxTri.color0 >> 16) & 0xFF) / 255.f,
            ((pxTri.color0 >> 8) & 0xFF) / 255.f,
            (pxTri.color0 & 0xFF) / 255.f,
            1.f
        );
        m_DebugTriangles.push_back(tri);
    }
}

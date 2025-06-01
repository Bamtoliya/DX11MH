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

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
    // PhysX 객체 해제 (생성 역순으로 하는 것이 안전)
    PX_RELEASE(m_pDispatcher); // 4. Dispatcher 해제
    PX_RELEASE(m_pPhysics); // 6. Physics 객체 해제
    PX_RELEASE(m_pPvd); // 7. PVD 해제 (연결했다면)
    PX_RELEASE(m_pFoundation) // 8. Foundation 해제 (가장 마지막)
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

    // Physics 생성
    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
    if (!m_pPhysics)
        return E_FAIL;

    // CPU Dispatcher 생성 (멀티스레딩 지원)
    m_pDispatcher = PxDefaultCpuDispatcherCreate(2); // 스레드 2개 사용
    if (!m_pDispatcher)
        return E_FAIL;

    return S_OK;
}

void CPhysics_System::Priority_Update(_float fTimeDelta)
{
}

void CPhysics_System::Update(_float fTimeDelta)
{
}

void CPhysics_System::Late_Update(_float fTimeDelta)
{
}

PxScene* CPhysics_System::CreateScene()
{
    // 씬 생성이 불가능한 경우 확인
    if (m_pPhysics == nullptr || m_pDispatcher == nullptr)
        return nullptr;

    // 씬 descriptor 생성
    PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());

    // 물리 속성 설정
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // 중력 설정
    sceneDesc.cpuDispatcher = m_pDispatcher;        // CPU 디스패처 설정

    // 충돌 필터링 콜백 설정 (필요시 커스텀 필터링 셰이더 구현)
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // 씬 플래그 설정 (선택적)
    sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;        // 연속 충돌 감지
    sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION; // 안정화 기능

    // GPU 물리 시뮬레이션을 위한 설정 (선택적)
    // sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
    // sceneDesc.gpuDispatcher = cudaContextManager->getGpuDispatcher();

    // 씬 생성
    m_pScene = m_pPhysics->createScene(sceneDesc);

    // 디버깅을 위한 시각화 설정 (선택적)
    if (m_pScene)
    {
        PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
        if (pvdClient)
        {
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
    }

    return m_pScene;
}

#include "Physics_System.h"


#pragma region Constructor/Destructor
CPhysics_System::CPhysics_System()
{
}

CPhysics_System* CPhysics_System::Create()
{
    CPhysics_System* pInstance = new CPhysics_System();

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
    return nullptr;
}

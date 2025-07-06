#pragma once
#include "Base.h"


NS_BEGIN(physx)
// PhysX 클래스 전방 선언 (헤더 의존성 감소 목적)
class PxFoundation;
class PxPhysics;
class PxPvd; // PhysX Visual Debugger
class PxDefaultCpuDispatcher;
class PxMaterial;
class PxScene; // 씬은 보통 별도로 관리하거나 나중에 생성
class PxDefaultAllocator;
class PxDefaultErrorCallback;
NS_END

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CPhysics_System final : public CBase, public PxSimulationEventCallback
{
#pragma region Constructor/Destructor
private:
	CPhysics_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysics_System() = default;
public:
	static CPhysics_System* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Free();
#pragma endregion
#pragma region Intialize
public:
	virtual HRESULT Initialize();
#pragma endregion
public:

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

#pragma region Public Accessors (예시)
public:
	PxPhysics*	Get_Physics() const { return m_pPhysics; }
	PxScene*	Get_PhysicsScene(_uint iIndex = 0) const { return m_pScene; }
	PxScene*	Create_Scene(); // 씬 생성 함수 (예시)
#pragma endregion

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	// PhysX Core Objects
	PxDefaultAllocator      m_Allocator;
	PxDefaultErrorCallback   m_ErrorCallback;

	PxScene* m_pScene = nullptr;
	PxFoundation* m_pFoundation = nullptr;
	PxPhysics* m_pPhysics = nullptr;
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	PxCudaContextManager*	m_pCudaContextManager = nullptr;
	PxControllerManager*	m_pControllerManager = nullptr;
	PxPvd* m_pPvd = nullptr;

	list<PxActor*> m_ActorsInScene; // 물리 객체 리스트

public:
	void Collect_DebugRenderData(); // 디버그 데이터 수집 함수
	void AddDebugLine(const physx::PxVec3& p0, const physx::PxVec3& p1, const _float4& color);
	void AddDebugTriangle(const physx::PxVec3& p0, const physx::PxVec3& p1, const physx::PxVec3& p2, const _float4& color);
	const vector<DEBUG_LINE>& Get_DebugLines() const { return m_DebugLines; }
	const vector<DEBUG_TRIANGLE>& Get_DebugTriangles() const { return m_DebugTriangles; }
	const _bool	Get_IsDebugDraw() const { return m_bPhysicsDebugDraw; }

public:
	void AddActorToScene(PxActor* pActor);
	void RemoveActorFromScene(PxActor* pActor);

public:
	void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
	void onWake(PxActor** actors, PxU32 count) override;
	void onSleep(PxActor** actors, PxU32 count) override;
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;
	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
	void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;

private:
	vector<DEBUG_LINE>      m_DebugLines;
	vector<DEBUG_TRIANGLE>  m_DebugTriangles;
	_bool					m_bPhysicsDebugDraw = { false };

	
};

NS_END
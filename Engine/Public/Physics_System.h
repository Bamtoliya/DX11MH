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
class CPhysics_System final : public CBase
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
	// 다른 시스템에서 PhysX 객체에 접근해야 할 경우 getter 함수 제공
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
	PxCudaContextManager* m_pCudaContextManager = nullptr;
	PxPvd* m_pPvd = nullptr;

public:
	void CollectDebugRenderData(); // 디버그 데이터 수집 함수
	const vector<DEBUG_LINE>& GetDebugLines() const { return m_DebugLines; }
	const vector<DEBUG_TRIANGLE>& GetDebugTriangles() const { return m_DebugTriangles; }

private:
	vector<DEBUG_LINE>      m_DebugLines;
	vector<DEBUG_TRIANGLE>  m_DebugTriangles;
};

NS_END
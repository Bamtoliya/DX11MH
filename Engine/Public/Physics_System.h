	#pragma once
	#include "Base.h"


BEGIN(physx)
// PhysX 클래스 전방 선언 (헤더 의존성 감소 목적)
class PxFoundation;
class PxPhysics;
class PxPvd; // PhysX Visual Debugger
class PxCooking; // Runtime cooking
class PxDefaultCpuDispatcher;
class PxMaterial;
class PxScene; // 씬은 보통 별도로 관리하거나 나중에 생성
class PxDefaultAllocator;
class PxDefaultErrorCallback;
END

BEGIN(Engine)
class CPhysics_System final : public CBase
{
#pragma region Constructor/Destructor
private:
	CPhysics_System();
	virtual ~CPhysics_System() = default;
public:
	static CPhysics_System* Create();
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
	PxPhysics*	GetPhysics() const { return m_pPhysics; }
	PxScene*	CreateScene(); // 씬 생성 함수 (예시)

#pragma endregion

private:
	// PhysX Core Objects
	PxDefaultAllocator      m_Allocator;
	PxDefaultErrorCallback   m_ErrorCallback;


	PxScene* m_pScene = nullptr;
	PxFoundation* m_pFoundation = nullptr;
	PxPhysics* m_pPhysics = nullptr;
	PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
	PxPvd* m_pPvd = nullptr;
};

END
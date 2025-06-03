
#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CPhysics_System;
class CTransform;
class ENGINE_DLL CRigidBody : public CComponent
{
    // 리지드바디 타입
    enum class EBodyType { STATIC, DYNAMIC, KINEMATIC };

    // 충돌체 모양 타입
    enum class EShapeType { BOX, SPHERE, CAPSULE, CONVEX_MESH, TRIANGLE_MESH };

    // 리지드바디 초기화 정보 구조체
    typedef struct tagRigidBodyDesc
    {
        EBodyType   eBodyType = EBodyType::STATIC;
        EShapeType  eShapeType = EShapeType::BOX;
        _float3     vShapeSize = _float3(1.f, 1.f, 1.f); // Box: (halfExtents), Sphere: (radius,0,0), Capsule: (radius, halfHeight,0)
        _float      fMass = 1.f;                        // Dynamic body의 질량
        _bool       bGravity = true;                    // 중력 적용 여부 (Dynamic body)
        _float      fStaticFriction = 0.5f;
        _float      fDynamicFriction = 0.5f;
        _float      fRestitution = 0.5f;                // 반발 계수
        _bool       bLockRotX = false, bLockRotY = false, bLockRotZ = false; // 회전 축 잠금 (Dynamic body)
        _bool       bKinematicTarget = false;           // 키네마틱 컨트롤 시 사용

        // 메시 타입일 경우 모델 컴포넌트 필요 (추후 구현 시 사용)
        class CModel* pModelForMesh = nullptr;

    } RIGIDBODY_DESC;

#pragma region Constructor/Destructor
protected:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& Prototype);
	virtual ~CRigidBody() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRigidBody* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;
	virtual HRESULT Render() override;
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
public:
    // PhysX Actor 접근
    PxRigidActor* Get_Actor() const { return m_pActor; }

    // 힘 및 속도 제어 (Dynamic Body용)
    void Add_Force(const _float3& vForce, PxForceMode::Enum eForceMode = PxForceMode::eFORCE);
    void Add_Torque(const _float3& vTorque, PxForceMode::Enum eForceMode = PxForceMode::eFORCE);
    void Set_Linear_Velocity(const _float3& vVelocity);
    _float3 Get_Linear_Velocity() const;
    void Set_Angular_Velocity(const _float3& vVelocity);
    _float3 Get_Angular_Velocity() const;
    void Set_Kinematic_Target(const _float4x4& matTargetWorld); // 키네마틱 액터 목표 위치/회전 설정

    // 속성 설정
    void Set_Body_Type(EBodyType eType); // 런타임 타입 변경 (주의해서 사용)
    void Set_Mass(_float fMass);
    void Enable_Gravity(_bool bEnable);
    void Set_Material_Properties(_float fStaticFriction, _float fDynamicFriction, _float fRestitution);
    void Lock_Rotation_Axis(_bool bLockX, _bool bLockY, _bool bLockZ);

private:
    HRESULT Create_Actor();        // PhysX 액터 생성 로직
    void Release_Actor();       // PhysX 액터 해제 로직
    void Synchronize_Transform_To_Physics(); // CTransform -> PxActor
    void Synchronize_Physics_To_Transform(); // PxActor -> CTransform

private:
    PxRigidActor*    m_pActor = nullptr;           // PhysX 액터 (Static 또는 Dynamic)
    PxShape*         m_pShape = nullptr;           // PhysX 충돌 형태
    PxMaterial*      m_pMaterial = nullptr;        // PhysX 재질

    RIGIDBODY_DESC          m_tRigidBodyDesc;             // 리지드바디 설정값

    _bool                   m_bIsLateInitialized = false;
    _bool                   m_bIsTransformDirty = true; // Transform이 변경되었는지 여부
};
NS_END

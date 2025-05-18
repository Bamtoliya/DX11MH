#pragma once

#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTransform final : public CComponent
{	
public:
	typedef struct tagTrasformDesc : public tagComponentDesc {
		_float4x4		WorldMatrix 	= {};
		_float			fSpeedPerSec 	= {};
		_float			fRotationPerSec = {};

		_float			fCurRotationX = 0.f;
		_float			fCurRotationY = 0.f;
		_float			fCurRotationZ = 0.f;

		_float4			qOrientation = { 0.f, 0.f, 0.f, 1.f }; // 단위 쿼터니언으로 초기화
		_bool			bRelative = { false }; // 상대좌표계인지 여부
	}TRANSFORM_DESC;
#pragma region Constructor/Destructor
private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
#pragma endregion
#pragma region Get/Set
	//Matrix
public:
	_vector Get_State(STATE eState) { return XMLoadFloat4x4(&m_tDesc.WorldMatrix).r[ENUM_CLASS(eState)]; }
	//_matrix Get_WorldMatrix() const { return XMLoadFloat4x4(&m_tDesc.WorldMatrix); }
	_matrix Get_WorldMatrix() const;
	_matrix Get_LocalMatrix() const { return XMLoadFloat4x4(&m_tDesc.WorldMatrix); }
	_matrix Get_WorldMatrix_Inverse() { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_tDesc.WorldMatrix));}
	_float4x4 Get_World4x4() const { return m_tDesc.WorldMatrix; }
	const _float4x4* Get_World4x4Ptr() const { return &m_tDesc.WorldMatrix; }
	void Set_State(STATE eState, _fvector vState) { XMStoreFloat4(reinterpret_cast<_float4*>(&m_tDesc.WorldMatrix.m[ENUM_CLASS(eState)]), vState); }

public:
	void Scaling(_float fX = 1.f, _float fY = 1.f, _float fZ = 1.f);
	void Scaling(const _float3& vScale);

	//void Set_WorldMatrix(_matrix mWorldMatrix) { XMStoreFloat4x4(&m_tDesc.WorldMatrix, mWorldMatrix); }
	void Set_WorldMatrix(const _matrix& WorldMatrix);
	_float3 Get_Scaled();
	_vector Get_Scale_Vector() const;
	_float Get_Scale(STATE eState) { return XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_tDesc.WorldMatrix).r[ENUM_CLASS(eState)])); }

	void Set_Scale(STATE eState, _float fScale);
	void Set_TransformDesc(const TRANSFORM_DESC& tDesc) { m_tDesc = tDesc; }
	void Change_Speed(_float fSpeed) { m_tDesc.fSpeedPerSec = fSpeed; }
	void Change_RotationPerSec(_float fRotationPerSec) { m_tDesc.fRotationPerSec = fRotationPerSec; }

	_float Get_CurrentRotationX() { return m_tDesc.fCurRotationX; }
	_float Get_CurrentRotationY() { return m_tDesc.fCurRotationY; }
	_float Get_CurrentRotationZ() { return m_tDesc.fCurRotationZ; }

	_bool  Get_Relative() { return m_tDesc.bRelative; }
#pragma endregion

#pragma region Movement
public: /* Not Use Sliding */
	bool Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Go_Up(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);

public: /* Use Sliding */
	bool Sliding_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Sliding_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Sliding_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);
	bool Sliding_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fRadius = 1.f);

public: /* Use Sliding while Animation */
	bool Sliding_Anim(_vector vecMove, _float fRotation, class CNavigation* pNavigation = nullptr);
	bool Sliding_Anim_To_Target(_vector vAt, _vector vecMove, _float fRotation, class CNavigation* pNavigation = nullptr);

public:
	void Jump(_float fTimeDelta, _float fVelocity, _float fGravity, _float fStartiHeight);
	bool Go_PosTarget(_float fTimeDelta, _vector TargetPos, class CNavigation* pNavigation = nullptr);
	bool Go_PosDir(_float fTimeDelta, _vector vecDir, class CNavigation* pNavigation = nullptr);
	bool Go_PosLerp(_float fTimeDelta, _vector TargetPos, _float fVecOffset);

public:/* Rotations */
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Set_Rotation(_float3 fAngle);
	void Set_RotationY(_float fAngleY);

	void Rotate_Quaternion_Accumulate(_fvector vAxis, _float fRadian);
	void Update_WorldMatrix_From_SQT();
	void Turn_Quaternion(_fvector vAxis, _float fTimeDelta);

public:
	void LookAt(_fvector vAt);
	void LookDir(_fvector vDir);
	void LookAtExceptY(_fvector vAt);

public:
	void RoamingTurn(_matrix WorldMatrix, _float fRadian);
#pragma endregion
	
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, class CVIBuffer* pVIBuffer);
	virtual HRESULT RenderImGui() override;
	virtual void* Dump() override;
#pragma region Variables
private:
	TRANSFORM_DESC		m_tDesc;
#pragma endregion
};

NS_END
#pragma once
#include "Base.h"
BEGIN(Engine)
class ENGINE_DLL CRay : public CBase
{
public:
	typedef struct tagRayDesc
	{
		_float4			vOrigin;
		_float4			vDirection;
	}RAY_DESC;
#pragma region Constructor/Destructor
private:
	CRay();
	virtual ~CRay() = default;
public:
	static CRay* Create();
	void Free();
#pragma endregion
#pragma region Intialize
public:
	virtual HRESULT Initialize(void* pArg = nullptr);
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
#pragma endregion

#pragma region Get/Set
	// Ray의 시작점
	_float3 Get_Origin() const { return *(_float3*)(&m_tDesc.vOrigin); }
	// Ray의 시작점 (SIMD 연산용)
	_vector Get_Origin_Vector() const { return XMLoadFloat4(&m_tDesc.vOrigin); }
	// Ray의 방향 (정규화됨)
	_float3 Get_Direction_Float3() const { return *(_float3*)(&m_tDesc.vDirection);; }
	// Ray의 방향 (정규화됨, SIMD 연산용)
	_vector Get_Direction_Vector() const { return XMLoadFloat4(&m_tDesc.vDirection); }
	_vector Get_Point(const _float fDistance) const;

	// 멤버 변수 설정 함수들
	void Set_Origin(FXMVECTOR vOrigin);
	void Set_Direction(FXMVECTOR vDirection); // 입력된 방향을 정규화하여 저장
#pragma endregion


private:
	RAY_DESC m_tDesc;
};
END
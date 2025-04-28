#pragma once

#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CTransform final : public CComponent
{	
public:
	typedef struct tagTrasformDesc : public tagComponentDesc {
		_float4x4		WorldMatrix 	= {};
		_float			fSpeedPerSec 	= {};
		_float			fRotationPerSec = {};
	}TRANSFORM_DESC;
	enum STATE { RIGHT, UP, LOOK, POS, T_END };
#pragma region Constructor/Destructor
protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
#pragma endregion

#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion
public:
	_float3 Get_State(STATE eState) const { return *reinterpret_cast<const _float3*>(&m_tDesc.WorldMatrix.m[eState][0]); }
	void Set_State(STATE eState, const _float3& vState) { memcpy(&m_tDesc.WorldMatrix.m[eState][0], &vState, sizeof vState); }
	virtual void* Dump() override;
#pragma region Variables
private:
	TRANSFORM_DESC		m_tDesc;
#pragma endregion
};

END
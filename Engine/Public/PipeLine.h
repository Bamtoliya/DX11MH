#pragma once

#include "Base.h"

BEGIN(Engine)
class CPipeLine final : public CBase
{

#pragma region Constructor/Destructor
private:
	CPipeLine();
	virtual ~CPipeLine() = default;
public:
	static CPipeLine* Create();
	virtual void Free();
#pragma endregion

#pragma region Get/Set
public:
	const _float4x4* Get_Transform_Float4x4(TRANSFORMSTATE eState) const {
		return &m_TransformationMatrix[ENUM_CAST(eState)];
	}

	const _matrix Get_Transform_Matrix(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformationMatrix[ENUM_CAST(eState)]);
	}

	const _float4x4* Get_Transform_Inverse_Float4x4(TRANSFORMSTATE eState) const {
		return &m_TransformationMatrix_Inverse[ENUM_CAST(eState)];
	}

	const _matrix Get_Transform_Inverse_Matrix(TRANSFORMSTATE eState) const {
		return XMLoadFloat4x4(&m_TransformationMatrix_Inverse[ENUM_CAST(eState)]);
	}

	const _float4* Get_CamPosition() const {
		return &m_vCamPosition;
	}

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix StateMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix[ENUM_CAST(eState)], StateMatrix);
	}
#pragma endregion

public:
	void Update();

private:
	_float4x4				m_TransformationMatrix[ENUM_CAST(TRANSFORMSTATE::D3DTS_END)] = {};
	_float4x4				m_TransformationMatrix_Inverse[ENUM_CAST(TRANSFORMSTATE::D3DTS_END)] = {};
	_float4					m_vCamPosition = {};
};

END
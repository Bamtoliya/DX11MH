#include "Transform.h"
#pragma region Constructor/Destructor
CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent{ Prototype }
{
}
CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
#pragma endregion

#pragma region Initalize
HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_tDesc.fSpeedPerSec = pDesc->fSpeedPerSec;
	m_tDesc.fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

#pragma endregion

_vector CTransform::Get_Scale_Vector() const
{
	// 1. 월드 행렬을 XMMATRIX 타입으로 로드합니다.
	XMMATRIX worldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	// 2. Right 벡터(첫 번째 행)의 3D 길이를 계산합니다. (X 스케일)
	// XMVector3Length는 결과 벡터의 모든 요소(x,y,z,w)에 길이를 담아 반환합니다.
	XMVECTOR vLengthRight = XMVector3Length(worldMatrix.r[CTransform::STATE::RIGHT]); // STATE::RIGHT가 0이라고 가정

	// 3. Up 벡터(두 번째 행)의 3D 길이를 계산합니다. (Y 스케일)
	XMVECTOR vLengthUp = XMVector3Length(worldMatrix.r[CTransform::STATE::UP]);    // STATE::UP이 1이라고 가정

	// 4. Look 벡터(세 번째 행)의 3D 길이를 계산합니다. (Z 스케일)
	XMVECTOR vLengthLook = XMVector3Length(worldMatrix.r[CTransform::STATE::LOOK]);  // STATE::LOOK이 2라고 가정

	// 5. 각 결과 벡터에서 스케일 값(길이)을 _float 타입으로 추출합니다.
	// XMVectorGetX, Y, Z, W 중 아무거나 사용해도 길이를 얻을 수 있습니다. 보통 X를 사용합니다.
	_float scaleX = XMVectorGetX(vLengthRight);
	_float scaleY = XMVectorGetX(vLengthUp);
	_float scaleZ = XMVectorGetX(vLengthLook);

	// 6. 추출된 스케일 값들로 _float3 구조체를 생성하여 반환합니다.
	XMVECTOR vScaleVector = XMVectorSet(scaleX, scaleY, scaleZ, 0.f); // W는 0으로 설정
	_float3 vScaleResult;
	XMStoreFloat3(&vScaleResult, vScaleVector);
	return vScaleVector;
}

void CTransform::Set_Scale(STATE eState, _float fScale)
{
	_matrix			WorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	WorldMatrix.r[eState] = XMVector3Normalize(WorldMatrix.r[eState]) * fScale;

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, WorldMatrix);
}


void* CTransform::Dump()
{
	return &m_tDesc;
}

#pragma region Movement
bool CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vLook = Get_State(CTransform::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vLook = Get_State(CTransform::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vRight = Get_State(CTransform::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vRight = Get_State(CTransform::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vUp = Get_State(CTransform::UP);

	vPosition += XMVector3Normalize(vUp) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vUp) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Sliding_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vLook = Get_State(CTransform::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;

	/*if (pNavigation)
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vLook + vNormal);
		_vector vPos = Get_State(CTransform::POS);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vLook) * fRadius))
			Set_State(CTransform::POS, vPos);
		return false;
	}*/
	Set_State(CTransform::POS, vPosition);
	return true;
}

bool CTransform::Sliding_Backward(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vLook = Get_State(CTransform::LOOK) * -1.f;

	vPosition += XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;

	/*if (pNavigation)
	{
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);
	}

	if (nullptr == pNavigation)
		Set_State(CTransform::POS, vPosition);
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vLook + vNormal);
		_vector vPos = Get_State(CTransform::POS);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vLook) * fRadius))
			Set_State(CTransform::POS, vPos);
		return false;
	}*/
	Set_State(CTransform::POS, vPosition);
	return true;
}

bool CTransform::Sliding_Left(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vRight = Get_State(CTransform::RIGHT) * -1.f;

	vPosition += XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;

	/*if (pNavigation)
	{
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);
	}

	if (nullptr == pNavigation)
		Set_State(CTransform::POS, vPosition);
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vRight, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vRight + vNormal);
		_vector vPos = Get_State(CTransform::POS);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vRight) * fRadius))
			Set_State(CTransform::POS, vPos);
		return false;
	}*/
	Set_State(CTransform::POS, vPosition);
	return true;
}

bool CTransform::Sliding_Right(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(CTransform::POS);
	_vector		vRight = Get_State(CTransform::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(CTransform::POS, vPosition);
	/*if (pNavigation)
	{
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);
	}

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		Set_State(CTransform::POS, vPosition);
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vRight, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vRight + vNormal);
		_vector vPos = Get_State(CTransform::POS);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vRight) * fRadius))
			Set_State(CTransform::POS, vPos);
		return false;
	}*/

	return true;
}

bool CTransform::Sliding_Anim(_vector vecMove, _float fRotation, class CNavigation* pNavigation)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	_vector vWorldScale, vWorldRot, vWorldPos;
	XMMatrixDecompose(&vWorldScale, &vWorldRot, &vWorldPos, WorldMatrix);
	vWorldRot = XMQuaternionNormalize(vWorldRot);

	_vector RotationQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (fRotation * -1.f));

	WorldMatrix = XMMatrixRotationQuaternion(XMQuaternionNormalize(XMQuaternionMultiply(vWorldRot, RotationQuat)));

	_vector vTranslation = XMVector3TransformNormal(vecMove, XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix);

	WorldMatrix = XMMatrixScalingFromVector(vWorldScale) * WorldMatrix * XMMatrixTranslationFromVector(vWorldPos);

	_vector vPos = WorldMatrix.r[3] + vTranslation;
	WorldMatrix.r[3] = vWorldPos;

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, WorldMatrix);
	Set_State(CTransform::POS, vPos);
	/*if (pNavigation)
		pNavigation->Compute_CurrentIndex_byXZ(vPos);

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vPos))
		Set_State(CTransform::POS, vPos);*/

	return true;
}

bool CTransform::Sliding_Anim_To_Target(_vector vAt, _vector vecMove, _float fRotation, CNavigation* pNavigation)
{
	_matrix WorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	_vector vWorldPos = WorldMatrix.r[3];
	WorldMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector vWorldRot = XMQuaternionNormalize(XMQuaternionRotationMatrix(WorldMatrix));

	_vector RotationQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (fRotation * -1.f));

	WorldMatrix = XMMatrixRotationQuaternion(XMQuaternionNormalize(XMQuaternionMultiply(vWorldRot, RotationQuat)));

	_vector vTranslation = XMVector3TransformCoord(vecMove, XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix);

	WorldMatrix.r[0] = XMVector4Normalize(WorldMatrix.r[0]) * Get_Scale(CTransform::RIGHT);
	WorldMatrix.r[1] = XMVector4Normalize(WorldMatrix.r[1]) * Get_Scale(CTransform::UP);
	WorldMatrix.r[2] = XMVector4Normalize(WorldMatrix.r[2]) * Get_Scale(CTransform::LOOK);
	WorldMatrix.r[3] = vWorldPos;

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, WorldMatrix);

	_vector		vPosition = Get_State(CTransform::POS);

	_vector		vAfterPosition = vPosition + vTranslation;
	vAfterPosition = XMVectorSetW(vAfterPosition, 1.f);
	Set_State(CTransform::POS, vAfterPosition);
	/*if (pNavigation)
		pNavigation->Compute_CurrentIndex_byXZ(vAfterPosition);

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vAfterPosition))
		Set_State(CTransform::POS, vAfterPosition);*/

	return true;
}

void CTransform::Jump(_float fTimeDelta, _float fVelocity, _float fGravity, _float fStartiHeight)
{
	_vector		vPosition = Get_State(CTransform::POS);
	float fPosY = fStartiHeight + fVelocity * fTimeDelta - (0.5f * fGravity * fTimeDelta * fTimeDelta);

	vPosition = XMVectorSetY(vPosition, fPosY);
	float y = XMVectorGetY(vPosition);
	Set_State(CTransform::POS, vPosition);
}

bool CTransform::Go_PosTarget(_float fTimeDelta, _vector TargetPos, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(CTransform::POS);
	_vector vTargetPos = TargetPos;
	_vector vDir = vTargetPos - vPosition;
	_float fLength = XMVectorGetX(XMVector3Length(vDir));
	vDir = XMVector3Normalize(vDir);

	if (fLength < 0.5f)
	{
		Set_State(CTransform::POS, vTargetPos);
		return true;
	}

	vPosition += vDir * fTimeDelta * m_tDesc.fSpeedPerSec;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		Set_State(CTransform::POS, vPosition);
	else if (true == pNavigation->isMove(vPosition))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_PosDir(_float fTimeDelta, _vector vecDir, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(CTransform::POS);
	_vector vDir = vecDir;

	vDir = XMVector3Normalize(vDir);

	vPosition += vDir * fTimeDelta * m_tDesc.fSpeedPerSec;
	Set_State(CTransform::POS, vPosition);
	/*if (nullptr == pNavigation)
		Set_State(CTransform::POS, vPosition);
	else if (true == pNavigation->isMove(vPosition))
		Set_State(CTransform::POS, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_PosLerp(_float fTimeDelta, _vector TargetPos, _float fVecOffset)
{
	_vector vPos = Get_State(CTransform::POS);
	_vector vDir = TargetPos - vPos;

	_float fLength = XMVectorGetX(XMVector3Length(vPos - TargetPos));
	_float  fVec = (1 - fTimeDelta) * 0 + fTimeDelta * fLength;

	vPos += XMVector3Normalize(vDir) * fVec * m_tDesc.fSpeedPerSec * fVecOffset;

	Set_State(CTransform::POS, vPos);

	if (fVec == 0.0f || fLength < 0.1f)
		return true;

	return false;
}


void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_tDesc.fRotationPerSec * fTimeDelta);

	Set_State(CTransform::RIGHT, XMVector3TransformNormal(Get_State(CTransform::RIGHT), RotationMatrix));
	Set_State(CTransform::UP, XMVector3TransformNormal(Get_State(CTransform::UP), RotationMatrix));
	Set_State(CTransform::LOOK, XMVector3TransformNormal(Get_State(CTransform::LOOK), RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(CTransform::RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scale(CTransform::RIGHT), RotationMatrix));
	Set_State(CTransform::UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scale(CTransform::UP), RotationMatrix));
	Set_State(CTransform::LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scale(CTransform::LOOK), RotationMatrix));
}

/* This is NOT an additive rotation like the "Turn()" function above.
This function set the specific rotation on the specific axis. */
void CTransform::Set_Rotation(_float3 fAngle)
{
	// Get current RotationMatrix from the WorldMatrix by decomposition.
	_vector vScale, vRotationQuat, vTranslation;
	XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, Get_WorldMatrix());
	_matrix RotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);

	// Multiply the WorldMatrix by the Inverse of current RotationMatrix (to get a WorldMatrix without any Rotation)
	_matrix InverseRotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
	_matrix WorldMatrixWithoutRotation = XMMatrixMultiply(Get_WorldMatrix(), InverseRotationMatrix);

	// Make a NewRotationMatrix with new angle values
	_matrix NewRotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fAngle.x), XMConvertToRadians(fAngle.y), XMConvertToRadians(fAngle.z));
	m_tDesc.fCurRotationX = fAngle.x;
	m_tDesc.fCurRotationY = fAngle.y;
	m_tDesc.fCurRotationZ = fAngle.z;

	// Set NewRotationMatrix to WorldMatrixWithoutRotation
	Set_State(CTransform::RIGHT, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[0], NewRotationMatrix));
	Set_State(CTransform::UP, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[1], NewRotationMatrix));
	Set_State(CTransform::LOOK, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[2], NewRotationMatrix));
}

void CTransform::Set_RotationY(_float fAngleY)
{
	Set_Rotation(_float3(m_tDesc.fCurRotationX, fAngleY, m_tDesc.fCurRotationZ));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vPosition = Get_State(CTransform::POS);

	_vector		vLook = vAt - vPosition;
	_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vRight = XMVector3Cross(vAxisY, vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(RIGHT, XMVector3Normalize(vRight) * Get_Scale(CTransform::RIGHT));
	Set_State(UP, XMVector3Normalize(vUp) * Get_Scale(CTransform::UP));
	Set_State(LOOK, XMVector3Normalize(vLook) * Get_Scale(CTransform::LOOK));
}

void CTransform::LookDir(_fvector vDir)
{
	_vector		vPosition = Get_State(CTransform::POS);

	_vector		vLook = vDir;
	_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vRight = XMVector3Cross(vAxisY, vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(RIGHT, XMVector3Normalize(vRight) * Get_Scale(CTransform::RIGHT));
	Set_State(UP, XMVector3Normalize(vUp) * Get_Scale(CTransform::UP));
	Set_State(LOOK, XMVector3Normalize(vLook) * Get_Scale(CTransform::LOOK));
}

void CTransform::LookAtExceptY(_fvector vAt)
{
	_vector		vLook = XMVectorSetY(vAt, m_tDesc.WorldMatrix.m[3][1]) - Get_State(POS);
	if (XMVectorGetX(XMVectorEqual(vLook, XMVectorSet(0.f, 0.f, 0.f, 0.f))) != 0)
		return;

	_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vRight = XMVector3Cross(vAxisY, vLook);

	Set_State(RIGHT, XMVector3Normalize(vRight) * Get_Scale(RIGHT));
	Set_State(LOOK, XMVector3Normalize(vLook) * Get_Scale(LOOK));
}

void CTransform::RoamingTurn(_matrix WorldMatrix, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadian);

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, XMLoadFloat4x4(&m_tDesc.WorldMatrix) * RotationMatrix);
}

void CTransform::Rotate_Quaternion_Accumulate(_fvector vAxis, _float fRadian)
{
	// 1. 현재 방향 쿼터니언 로드
	XMVECTOR currentOrientation = XMLoadFloat4(&m_tDesc.qOrientation);

	// 2. 이번에 적용할 회전을 나타내는 델타 쿼터니언 생성
	XMVECTOR deltaRotation = XMQuaternionRotationAxis(vAxis, fRadian);

	// 3. 현재 방향에 델타 회전을 곱하여 누적
	// 순서 중요: new = current * delta 는 월드 축 기준 회전 또는 마지막에 적용되는 회전과 유사
	// 순서 중요: new = delta * current 는 로컬 축 기준 회전 또는 먼저 적용되는 회전과 유사
	// 여기서는 객체를 vAxis 기준으로 회전시킨다고 가정하고 current * delta 사용
	XMVECTOR newOrientation = XMQuaternionMultiply(currentOrientation, deltaRotation);

	// 4. 결과 쿼터니언 정규화 (부동 소수점 오차 누적 방지)
	newOrientation = XMQuaternionNormalize(newOrientation);

	// 5. 계산된 새로운 방향 쿼터니언 저장
	XMStoreFloat4(&m_tDesc.qOrientation, newOrientation);

	// 중요: 쿼터니언이 변경되었으므로, 월드 행렬도 업데이트해야 함
	// 이 함수 내에서 직접 업데이트하거나, 플래그를 설정하여 나중에 업데이트
	// 예: Update_WorldMatrix_From_SQT(); // Scale, Quaternion, Translation으로 월드 행렬 재계산
}

/**
 * @brief 사원수를 사용하여 월드 행렬을 재계산하는 함수 (예시)
 * (Scale, Position 정보도 필요함)
 */
void CTransform::Update_WorldMatrix_From_SQT()
{
	// 스케일 행렬 생성 (Get_ScaleVector() 함수가 있다고 가정)
	_vector scale =
	{ Get_Scale(CTransform::RIGHT),
	Get_Scale(CTransform::UP),
	Get_Scale(CTransform::LOOK), 0.f };
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);

	// 회전 쿼터니언 로드 및 회전 행렬 생성
	XMVECTOR rotationQuat = XMLoadFloat4(&m_tDesc.qOrientation);
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);

	// 위치 벡터 로드 (Get_State(POSITION) 함수가 있다고 가정)
	XMVECTOR position = Get_State(CTransform::POS);

	// 월드 행렬 계산 (일반적으로 Scale * Rotation * Translation 순서)
	_matrix worldMatrix = scaleMatrix * rotationMatrix;
	// 이동(Translation)은 행렬의 마지막 행(r[3])에 직접 설정
	worldMatrix.r[3] = XMVectorSetW(position, 1.f); // W 요소를 1로 설정

	// 계산된 월드 행렬 저장
	XMStoreFloat4x4(&m_tDesc.WorldMatrix, worldMatrix);

	// 만약 Get_State(RIGHT/UP/LOOK)이 m_tDesc.WorldMatrix를 직접 참조하지 않고
	// 별도의 멤버 변수에 저장한다면, 여기서 업데이트 필요
	// Set_State(CTransform::RIGHT, worldMatrix.r[0]);
	// Set_State(CTransform::UP,    worldMatrix.r[1]);
	// Set_State(CTransform::LOOK,  worldMatrix.r[2]);
}

// 기존 Turn 함수와 유사한 기능을 하는 쿼터니언 버전
void CTransform::Turn_Quaternion(_fvector vAxis, _float fTimeDelta)
{
	// 초당 회전 속도를 사용하여 이번 프레임의 회전 각도 계산
	_float fRadian = m_tDesc.fRotationPerSec * fTimeDelta;
	// 누적 회전 함수 호출
	Rotate_Quaternion_Accumulate(vAxis, fRadian);
}
#pragma endregion
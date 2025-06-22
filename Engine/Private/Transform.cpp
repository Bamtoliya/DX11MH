#pragma once
#include "Transform.h"
#include "Shader.h"
#include "VIBuffer.h"
#include "GameObject.h"
#include "Navigation.h"

#pragma region Constructor/Destructor
CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent{ Prototype }
	, m_tDesc { Prototype.m_tDesc }
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

#pragma region Initalize
HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_tDesc.WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);
	XMStoreFloat4x4(&m_tDesc.WorldMatrix, XMMatrixIdentity());
	if (pArg != nullptr)
		Set_State(STATE::POSITION, XMLoadFloat4(reinterpret_cast<_float4*>(pDesc->WorldMatrix.m[3])));

	m_tDesc.WorldMatrix._44 = 1.f;
	m_tDesc.fSpeedPerSec = pDesc->fSpeedPerSec;
	m_tDesc.fRotationPerSec = pDesc->fRotationPerSec;
	m_tDesc.bRelativePosition = pDesc->bRelativePosition;
	m_tDesc.bRelativeRotation = pDesc->bRelativeRotation;
	m_tDesc.bRelativeScale = pDesc->bRelativeScale;

	return S_OK;
}

#pragma endregion

void CTransform::Free()
{
	__super::Free();

}
#pragma endregion



#pragma region Save & Load

HRESULT CTransform::Serialize(json& outJson)
{
	json data;
	// 1. 부모 클래스(CComponent)의 Serialize 호출
	if (FAILED(__super::Serialize(outJson)))
		return E_FAIL;	

	// 3. TRANSFORM_DESC 멤버 변수 저장
	// WorldMatrix는 직접 저장하기보다 SQT(Scale, Quaternion, Translation)로 분해하여 저장하는 것이 일반적입니다.
	// 또는, WorldMatrix 자체를 float 배열로 저장할 수도 있습니다.
	// 여기서는 SQT와 추가 속성들을 저장합니다.

	_vector scaleVec, rotationQuatVec, translationVec;
	XMMatrixDecompose(&scaleVec, &rotationQuatVec, &translationVec, XMLoadFloat4x4(&m_tDesc.WorldMatrix));

	_float3 scale, translation;
	_float4 rotation; // qOrientation을 직접 사용하거나 분해된 rotationQuatVec 사용

	XMStoreFloat3(&scale, scaleVec);
	XMStoreFloat4(&rotation, XMLoadFloat4(&m_tDesc.qOrientation)); // 저장된 쿼터니언 사용
	XMStoreFloat3(&translation, translationVec);

	data["Position"] = { translation.x, translation.y, translation.z };
	data["Scale"] = { scale.x, scale.y, scale.z };
	data["Orientation"] = { rotation.x, rotation.y, rotation.z, rotation.w }; // 쿼터니언

	// 오일러 각도 (주로 UI 표시 및 편집 편의를 위함, 쿼터니언이 주 회전 데이터)
	data["EulerRotation"] = { m_tDesc.fCurRotationX, m_tDesc.fCurRotationY, m_tDesc.fCurRotationZ };

	data["SpeedPerSec"] = m_tDesc.fSpeedPerSec;
	data["RotationPerSec"] = m_tDesc.fRotationPerSec;

	// 세분화된 상대 플래그 저장
	data["IsRelativePosition"] = m_tDesc.bRelativePosition;
	data["IsRelativeRotation"] = m_tDesc.bRelativeRotation;
	data["IsRelativeScale"] = m_tDesc.bRelativeScale;

	// WorldMatrix 자체를 저장하고 싶다면 (예: 디버깅용 또는 특정 상황)
	// json worldMatJson = json::array();
	// for(int i=0; i<4; ++i) {
	//     for(int j=0; j<4; ++j) {
	//         worldMatJson.push_back(m_tDesc.WorldMatrix.m[i][j]);
	//     }
	// }
	// outJson["WorldMatrix_Raw"] = worldMatJson;

	outJson["CTransform"] = data;

	return S_OK;
}
HRESULT CTransform::Deserialize(simdjson::ondemand::object& jsonData)
{
	// 1. 부모 클래스(CComponent)의 Deserialize 호출
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	// 2. TRANSFORM_DESC 멤버 변수 복원
	_float3 position = { 0.f, 0.f, 0.f };
	_float3 scale = { 1.f, 1.f, 1.f };
	_float4 orientation = { 0.f, 0.f, 0.f, 1.f }; // 단위 쿼터니언
	_float3 eulerRotation = { 0.f, 0.f, 0.f };

	simdjson::ondemand::object transformObject;

	if (jsonData["CTransform"].get_object().get(transformObject) == simdjson::SUCCESS)
	{
		if (auto posNode = transformObject["Position"]; posNode.error() == simdjson::SUCCESS) {
			simdjson::ondemand::array arr = posNode.get_array();
			if (arr.count_elements() == 3) {
				auto iter = arr.begin();
				position.x = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				position.y = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				position.z = static_cast<_float>((*iter).get_double().value_unsafe());
			}
		}

		if (auto scaleNode = transformObject["Scale"]; scaleNode.error() == simdjson::SUCCESS) {
			simdjson::ondemand::array arr = scaleNode.get_array();
			if (arr.count_elements() == 3) {
				auto iter = arr.begin();
				scale.x = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				scale.y = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				scale.z = static_cast<_float>((*iter).get_double().value_unsafe());
			}
		}

		if (auto oriNode = transformObject["Orientation"]; oriNode.error() == simdjson::SUCCESS) {
			simdjson::ondemand::array arr = oriNode.get_array();
			if (arr.count_elements() == 4) {
				auto iter = arr.begin();
				orientation.x = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				orientation.y = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				orientation.z = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				orientation.w = static_cast<_float>((*iter).get_double().value_unsafe());
				m_tDesc.qOrientation = orientation; // 쿼터니언 직접 저장
			}
		}

		if (auto eulerNode = transformObject["EulerRotation"]; eulerNode.error() == simdjson::SUCCESS) {
			simdjson::ondemand::array arr = eulerNode.get_array();
			if (arr.count_elements() == 3) {
				auto iter = arr.begin();
				eulerRotation.x = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				eulerRotation.y = static_cast<_float>((*iter).get_double().value_unsafe());
				++iter;
				eulerRotation.z = static_cast<_float>((*iter).get_double().value_unsafe());
				m_tDesc.fCurRotationX = eulerRotation.x;
				m_tDesc.fCurRotationY = eulerRotation.y;
				m_tDesc.fCurRotationZ = eulerRotation.z;
			}
		}

		// SQT로부터 월드 행렬 재구성
		_matrix worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationQuaternion(XMLoadFloat4(&m_tDesc.qOrientation)) *
			XMMatrixTranslation(position.x, position.y, position.z);
		XMStoreFloat4x4(&m_tDesc.WorldMatrix, worldMatrix);


		if (auto val = transformObject["SpeedPerSec"]; val.error() == simdjson::SUCCESS) { // 또는 get_uint64 등 타입에 맞게
			m_tDesc.fSpeedPerSec = static_cast<_float>(val.get_double().value_unsafe());
		}
		if (auto val = transformObject["RotationPerSec"]; val.error() == simdjson::SUCCESS) {
			m_tDesc.fRotationPerSec = static_cast<_float>(val.get_double().value_unsafe());
		}

		// 세분화된 상대 플래그 로드
		if (auto val = transformObject["IsRelativePosition"]; val.error() == simdjson::SUCCESS) {
			m_tDesc.bRelativePosition = val.get_bool().value_unsafe();
		}
		if (auto val = transformObject["IsRelativeRotation"]; val.error() == simdjson::SUCCESS) {
			m_tDesc.bRelativeRotation = val.get_bool().value_unsafe();
		}
		if (auto val = transformObject["IsRelativeScale"]; val.error() == simdjson::SUCCESS) {
			m_tDesc.bRelativeScale = val.get_bool().value_unsafe();
		}
		// 이전 IsRelative 호환성 (선택 사항)
		if (auto val = transformObject["IsRelative"]; val.error() == simdjson::SUCCESS) {
		 	bool oldRelative = val.get_bool().value_unsafe();
		 	m_tDesc.bRelativePosition = oldRelative;
		 	m_tDesc.bRelativeRotation = oldRelative;
			m_tDesc.bRelativeScale = oldRelative;
		}
	}

	

	// WorldMatrix_Raw를 저장했다면 여기서 복원할 수도 있습니다.
	// 하지만 SQT로 복원하는 것이 더 일반적이고 유연합니다.

	return S_OK;
}

#pragma endregion

#pragma region Loops
void CTransform::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CTransform::Update(_float fTimeDelta) 
{
	__super::Update(fTimeDelta);
}
void CTransform::Late_Update(_float fTimeDelta) 
{
	__super::Late_Update(fTimeDelta);
}
HRESULT CTransform::Render()
{ 
	return S_OK;
}
#pragma endregion
_matrix CTransform::Get_WorldMatrix() const
{
	// 1. 로컬 변환 행렬 가져오기
	_matrix localWorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	if (nullptr == m_pOwner || nullptr == m_pOwner->Get_Parent())
	{
		return localWorldMatrix; // 부모가 없으면 로컬 월드 행렬이 최종 월드 행렬
	}

	CTransform* pParentTransform = static_cast<CTransform*>(m_pOwner->Get_Parent()->Get_Component(g_strTransformTag));
	if (nullptr == pParentTransform)
	{
		return localWorldMatrix; // 부모는 있지만 트랜스폼 컴포넌트가 없으면 로컬 월드 행렬 반환
	}

	_matrix parentWorldMatrix = pParentTransform->Get_WorldMatrix();

	// 로컬 SQT 분해
	_vector localScaleVec, localRotQuatVec, localTransVec;
	XMMatrixDecompose(&localScaleVec, &localRotQuatVec, &localTransVec, localWorldMatrix);

	// 부모 SQT 분해
	_vector parentScaleVec, parentRotQuatVec, parentTransVec;
	XMMatrixDecompose(&parentScaleVec, &parentRotQuatVec, &parentTransVec, parentWorldMatrix);

	// 최종 SQT 계산
	_vector finalScaleVec = localScaleVec;
	_vector finalRotQuatVec = localRotQuatVec;
	_vector finalTransVec = localTransVec;

	if (m_tDesc.bRelativeScale)
	{
		finalScaleVec = XMVectorMultiply(localScaleVec, parentScaleVec);
	}
	// else: finalScaleVec은 localScaleVec 그대로 사용 (월드 기준 절대 크기)
	// 이 경우, 부모의 회전과 크기가 자식의 절대 크기에 영향을 주지 않도록 하려면
	// 최종 행렬 구성 시 주의가 필요합니다. 일반적으로 절대 크기는 월드 행렬의 스케일링 벡터를
	// 직접 설정하는 방식으로 구현될 수 있으나, SQT 결합 방식에서는 복잡합니다.
	// 여기서는 bRelativeScale이 false이면 로컬 스케일을 월드 스케일로 간주하고,
	// 부모의 스케일링 효과를 제거하기 위해 부모 스케일의 역을 곱하는 방식도 고려할 수 있으나,
	// 가장 간단한 것은 "로컬 스케일이 최종 월드 스케일"이라고 가정하는 것입니다.
	// 하지만 일반적인 계층 구조에서는 부모의 변환이 누적되므로,
	// bRelativeScale = false 일 때 "월드에서 이 스케일 값을 가져라"는 의미로 해석하고,
	// 부모의 스케일링을 제거하려면 (finalScaleVec / parentScaleVec) 와 유사한 연산이 필요합니다.
	// 여기서는 bRelativeScale이 true일 때만 부모 스케일을 곱하고, false이면 로컬 스케일을 사용합니다.

	if (m_tDesc.bRelativeRotation)
	{
		finalRotQuatVec = XMQuaternionMultiply(localRotQuatVec, parentRotQuatVec);
	}
	// else: finalRotQuatVec은 localRotQuatVec 그대로 사용 (월드 기준 절대 회전)
	// 이 경우, 부모의 회전을 제거해야 합니다. finalRotQuatVec = XMQuaternionMultiply(localRotQuatVec, XMQuaternionInverse(parentRotQuatVec));
	// 하지만 이는 로컬 회전을 월드 회전으로 직접 설정하는 것과 같습니다.

	if (m_tDesc.bRelativePosition)
	{
		// 로컬 위치를 부모의 월드 공간으로 변환
		finalTransVec = XMVector3TransformCoord(localTransVec, parentWorldMatrix);
	}
	// else: finalTransVec은 localTransVec 그대로 사용 (월드 기준 절대 위치)
	// 이 경우, 부모의 위치 변환을 제거해야 합니다.
	// finalTransVec = XMVectorSubtract(localTransVec, parentTransVec); // 이것은 단순 뺄셈이며, 회전/크기 변환을 고려하지 않음
	// 정확히는, localTransVec을 월드 원점 기준으로 설정하고 싶다면,
	// 부모 변환의 역행렬을 로컬 변환에 곱한 후 위치를 가져오는 방식이 필요할 수 있습니다.
	// 여기서는 bRelativePosition이 false이면 localTransVec을 월드 위치로 간주합니다.

	// --- 재검토된 절대 변환 로직 ---
	// bRelativeXXX가 false일 경우, 해당 컴포넌트는 월드 공간 값을 직접 사용해야 합니다.
	// 이는 부모 변환의 해당 부분을 "무효화"하는 것을 의미합니다.

	_matrix finalMatrix;

	if (!m_tDesc.bRelativePosition && !m_tDesc.bRelativeRotation && !m_tDesc.bRelativeScale) {
		// 모든 것이 절대적이면, 로컬 월드 행렬이 최종 월드 행렬
		finalMatrix = localWorldMatrix;
	}
	else if (m_tDesc.bRelativePosition && m_tDesc.bRelativeRotation && m_tDesc.bRelativeScale) {
		// 모든 것이 상대적이면, 단순 곱셈
		finalMatrix = XMMatrixMultiply(localWorldMatrix, parentWorldMatrix);
	}
	else {
		// 혼합된 경우: 각 SQT 요소를 조건부로 결합
		XMMATRIX scaleMat, rotMat, transMat;

		// 스케일
		if (m_tDesc.bRelativeScale) {
			finalScaleVec = XMVectorMultiply(localScaleVec, parentScaleVec);
		}
		else {
			finalScaleVec = localScaleVec; // 월드 스케일로 사용
		}
		scaleMat = XMMatrixScalingFromVector(finalScaleVec);

		// 회전
		if (m_tDesc.bRelativeRotation) {
			finalRotQuatVec = XMQuaternionMultiply(localRotQuatVec, parentRotQuatVec);
		}
		else {
			finalRotQuatVec = localRotQuatVec; // 월드 회전으로 사용
		}
		rotMat = XMMatrixRotationQuaternion(finalRotQuatVec);

		// 위치
		if (m_tDesc.bRelativePosition) {
			// 로컬 위치를 부모의 (스케일 * 회전) 변환 후, 부모 위치에 더함
			_matrix parentSR = XMMatrixScalingFromVector(parentScaleVec) * XMMatrixRotationQuaternion(parentRotQuatVec);
			finalTransVec = XMVector3TransformCoord(localTransVec, parentSR);
			finalTransVec = XMVectorAdd(finalTransVec, parentTransVec);
		}
		else {
			finalTransVec = localTransVec; // 월드 위치로 사용
		}
		transMat = XMMatrixTranslationFromVector(finalTransVec);

		// 최종 행렬 구성: S * R * T (월드 공간 기준)
		// 또는 로컬(S*R*T)을 기준으로 하고, 절대적인 부분을 부모 변환에서 "제거"
		// 여기서는 최종 S, R, T를 계산하여 월드 행렬을 만듭니다.
		finalMatrix = scaleMat * rotMat * transMat;
	}
	return finalMatrix;
}
_vector CTransform::Get_Scale_Vector() const
{
	// 1. 월드 행렬을 XMMATRIX 타입으로 로드합니다.
	XMMATRIX worldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	// 2. Right 벡터(첫 번째 행)의 3D 길이를 계산합니다. (X 스케일)
	// XMVector3Length는 결과 벡터의 모든 요소(x,y,z,w)에 길이를 담아 반환합니다.
	XMVECTOR vLengthRight = XMVector3Length(worldMatrix.r[ENUM_CLASS(STATE::RIGHT)]); // STATE::RIGHT가 0이라고 가정

	// 3. Up 벡터(두 번째 행)의 3D 길이를 계산합니다. (Y 스케일)
	XMVECTOR vLengthUp = XMVector3Length(worldMatrix.r[ENUM_CLASS(STATE::UP)]);    // STATE::UP이 1이라고 가정

	// 4. Look 벡터(세 번째 행)의 3D 길이를 계산합니다. (Z 스케일)
	XMVECTOR vLengthLook = XMVector3Length(worldMatrix.r[ENUM_CLASS(STATE::LOOK)]);  // STATE::LOOK이 2라고 가정

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

void CTransform::Scaling(_float fX, _float fY, _float fZ)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * fX);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * fY);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * fZ);
}

void CTransform::Scaling(const _float3& vScale)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * vScale.z);
}

void CTransform::Set_WorldMatrix(const _matrix& WorldMatrix)
{
	_matrix matrixToStore = WorldMatrix;

	if (nullptr != m_pOwner && nullptr != m_pOwner->Get_Parent())
	{
		CTransform* pParentTransform = static_cast<CTransform*>(m_pOwner->Get_Parent()->Get_Component(g_strTransformTag));
		if (pParentTransform)
		{
			_matrix parentWorldMatrix = pParentTransform->Get_WorldMatrix();
			_matrix invParentWorldMatrix = XMMatrixInverse(nullptr, parentWorldMatrix);

			// NewWorldMatrix는 최종 월드 공간 변환으로 가정합니다.
			// 로컬 변환을 계산해야 합니다.

			_vector newWorldScale, newWorldRot, newWorldPos;
			XMMatrixDecompose(&newWorldScale, &newWorldRot, &newWorldPos, WorldMatrix);

			_vector parentScale, parentRot, parentPos;
			XMMatrixDecompose(&parentScale, &parentRot, &parentPos, parentWorldMatrix);

			_vector localScale = newWorldScale;
			_vector localRot = newWorldRot;
			_vector localPos = newWorldPos;

			if (m_tDesc.bRelativeScale) {
				// newWorldScale = localScale * parentScale  => localScale = newWorldScale / parentScale
				// XMVectorDivide는 없으므로, 역수를 곱합니다. (1/parentScale.x, ...)
				// 또는 XMMatrixDecompose가 반환하는 스케일 벡터가 이미 적절히 분리되어 있다면,
				// matrixToStore = NewWorldMatrix * invParentWorldMatrix; 에서 스케일이 올바르게 계산될 수 있습니다.
				// 여기서는 단순화하여, 전체가 상대적이면 invParentWorldMatrix를 곱합니다.
			}
			else {
				// 로컬 스케일은 newWorldScale과 동일해야 함.
			}

			if (m_tDesc.bRelativeRotation) {
				// newWorldRot = localRot * parentRot => localRot = newWorldRot * invParentRot
			}
			else {
				// 로컬 회전은 newWorldRot과 동일해야 함.
			}

			if (m_tDesc.bRelativePosition) {
				// newWorldPos = XMVector3TransformCoord(localPos, parentWorldMatrix)
				// => localPos = XMVector3TransformCoord(newWorldPos, invParentWorldMatrix)
			}
			else {
				// 로컬 위치는 newWorldPos와 동일해야 함.
			}

			// 모든 것이 상대적인 경우의 단순화된 로직:
			if (m_tDesc.bRelativePosition && m_tDesc.bRelativeRotation && m_tDesc.bRelativeScale) {
				matrixToStore = XMMatrixMultiply(WorldMatrix, invParentWorldMatrix);
			}
			// TODO: 각 bRelative 플래그에 따라 S/R/T를 개별적으로 계산하고 로컬 행렬을 재구성하는
			//       더 정교한 로직이 필요합니다.
			//       예: 로컬 S = bRelativeScale ? newS/parentS : newS
			//           로컬 R = bRelativeRotation ? newR*invParentR : newR
			//           로컬 T = bRelativePosition ? XMVector3TransformCoord(newP, invParentSR) : newP
			//       그런 다음 로컬 S, R, T로 matrixToStore를 재구성합니다.
			// 현재는 모든 플래그가 true일 때만 부모 변환을 제거합니다.
			// 그렇지 않으면 NewWorldMatrix를 로컬로 간주합니다 (이는 의도와 다를 수 있음).
			// 사용자의 요구사항에 따라 이 부분을 더 정교하게 만들어야 합니다.
			// 가장 일반적인 경우는 NewWorldMatrix가 "최종 목표 월드 변환"이고,
			// 우리는 m_tDesc.WorldMatrix (로컬 변환)를 저장해야 한다는 것입니다.

			// 임시 단순화: 모든 것이 절대적이면 NewWorldMatrix를 그대로 사용,
			// 그렇지 않고 하나라도 상대적이면 부모의 역행렬을 곱하여 로컬화 시도.
			// 이는 정확하지 않을 수 있으며, 각 요소별 처리가 필요합니다.
			bool anyRelative = m_tDesc.bRelativePosition || m_tDesc.bRelativeRotation || m_tDesc.bRelativeScale;
			if (anyRelative) {
				// 이 부분은 Get_WorldMatrix와 대칭적으로 S, R, T를 분리하고 재계산해야 합니다.
				// 지금은 전체 행렬에 대해 역변환을 적용합니다.
				matrixToStore = XMMatrixMultiply(WorldMatrix, invParentWorldMatrix);
			}
			else { // 모든 것이 절대적이면, NewWorldMatrix가 로컬이자 월드.
				matrixToStore = WorldMatrix;
			}
		}
	}

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, matrixToStore);
	SynchronizeRotationRepresentations(); // 월드 행렬 변경 시 SQT 및 오일러 각 동기화
}

_float3 CTransform::Get_Scaled()
{	
	return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))));		
}

void CTransform::Set_Scale(STATE eState, _float fScale)
{
	_matrix			WorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);

	WorldMatrix.r[ENUM_CLASS(eState)] = XMVector3Normalize(WorldMatrix.r[ENUM_CLASS(eState)]) * fScale;

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	_float4x4 finalMatrixToBind;
	XMStoreFloat4x4(&finalMatrixToBind, Get_WorldMatrix()); // Get_WorldMatrix가 이미 상대성을 처리

	return pShader->Bind_Matrix(pConstantName, &finalMatrixToBind);
	
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, CVIBuffer* pVIBuffer)
{
	if (nullptr == pShader)
		return E_FAIL;

	_matrix transformWorldMatrix = Get_WorldMatrix(); // CTransform의 최종 월드 행렬 (상대성 처리됨)

	if (pVIBuffer != nullptr)
	{
		_float3 bufferLocalSize = pVIBuffer->Get_LocalSize();
		_matrix vibufferScaleMatrix = XMMatrixScaling(bufferLocalSize.x, bufferLocalSize.y, bufferLocalSize.z);
		// VIBuffer의 스케일은 CTransform의 로컬 공간에 적용되어야 합니다.
		// Get_WorldMatrix()가 반환하는 것은 이미 부모 변환이 적용된 것일 수 있으므로,
		// VIBuffer 스케일 적용 시점을 고려해야 합니다.
		// 만약 VIBuffer가 객체의 "모델" 크기를 나타낸다면, 로컬 변환에 먼저 곱해져야 합니다.
		// _matrix localWorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);
		// _matrix scaledLocalWorldMatrix = vibufferScaleMatrix * localWorldMatrix;
		// 그런 다음 이 scaledLocalWorldMatrix를 기준으로 Get_WorldMatrix와 유사한 상대성 계산을 해야 합니다.

		// 현재 Get_WorldMatrix()는 m_tDesc.WorldMatrix를 기반으로 계산합니다.
		// VIBuffer 스케일을 적용하려면, Get_WorldMatrix 로직 내에서 처리하거나,
		// 여기서 Get_WorldMatrix 결과를 후처리해야 합니다.
		// 여기서는 Get_WorldMatrix() 결과를 사용하고, VIBuffer 스케일을 곱합니다.
		// 이는 VIBuffer 스케일이 최종 월드 변환 후 적용되는 것처럼 보일 수 있습니다.
		// 정확한 적용 순서는 의도에 따라 달라집니다.
		// 일반적으로 모델 스케일은 가장 먼저 적용됩니다.
		// _matrix finalWorldMatrixForShader = vibufferScaleMatrix * transformWorldMatrix; (이것은 월드 스케일링처럼 동작)

		// 수정: VIBuffer 스케일은 로컬 스케일에 먼저 적용되어야 합니다.
		_matrix localMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);
		_matrix scaledLocalMatrix = vibufferScaleMatrix * localMatrix; // 로컬 스케일 * 모델 스케일

		// 이제 이 scaledLocalMatrix를 기반으로 부모와의 상대성을 계산해야 합니다.
		// Get_WorldMatrix 로직을 여기에 다시 적용하거나, Get_WorldMatrix가 로컬 행렬을 인자로 받도록 수정해야 합니다.
		// 임시로, Get_WorldMatrix가 m_tDesc.WorldMatrix를 사용한다고 가정하고,
		// VIBuffer 스케일은 최종 결과에 곱하는 것으로 유지 (이는 시각적으로 다를 수 있음)
		// TODO: 이 로직은 VIBuffer 스케일 적용 시점을 명확히 해야 합니다.
		// 가장 간단한 방법은 VIBuffer를 렌더링할 때만 이 스케일을 적용하는 것입니다.
		// 셰이더에 전달되는 월드 행렬은 객체의 월드 행렬이어야 합니다.
		// VIBuffer의 크기는 모델 변환의 일부로 간주될 수 있습니다.

		_float4x4 finalMatrixToBind;
		XMStoreFloat4x4(&finalMatrixToBind, scaledLocalMatrix); // 우선 객체의 월드 행렬 사용
		// 만약 pVIBuffer 스케일을 여기에 곱한다면:
		// _matrix finalMatrixWithBufferScale = vibufferScaleMatrix * transformWorldMatrix;
		// XMStoreFloat4x4(&finalMatrixToBind, finalMatrixWithBufferScale);
		return pShader->Bind_Matrix(pConstantName, &finalMatrixToBind);
	}

	_float4x4 worldMat;
	XMStoreFloat4x4(&worldMat, transformWorldMatrix);
	return pShader->Bind_Matrix(pConstantName, &worldMat);
}

HRESULT CTransform::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 세분화된 상대성 체크박스
		bool changedRelative = false;
		if (ImGui::Checkbox("Relative Position", &m_tDesc.bRelativePosition)) changedRelative = true;
		ImGui::SameLine();
		if (ImGui::Checkbox("Relative Rotation", &m_tDesc.bRelativeRotation)) changedRelative = true;
		ImGui::SameLine();
		if (ImGui::Checkbox("Relative Scale", &m_tDesc.bRelativeScale)) changedRelative = true;

		// 로컬 월드 행렬 (m_tDesc.WorldMatrix)을 분해하여 표시 및 편집
		_matrix localWorldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);
		_vector scaleVec, currentRotationQuatVec, translationVec;

		if (XMMatrixDecompose(&scaleVec, &currentRotationQuatVec, &translationVec, localWorldMatrix))
		{
			// UI 편집을 위해 m_tDesc의 SQT 값 사용 또는 분해된 값 사용
			// m_tDesc.qOrientation이 항상 최신이라고 가정
			currentRotationQuatVec = XMLoadFloat4(&m_tDesc.qOrientation);

			_float3 position, scale;
			XMStoreFloat3(&position, translationVec); // 로컬 위치
			XMStoreFloat3(&scale, scaleVec);         // 로컬 스케일

			bool changedSQT = false;

			if (ImGui::DragFloat3("Position (Local)", &position.x, 0.1f))
			{
				translationVec = XMLoadFloat3(&position);
				changedSQT = true;
			}

			_float3 eulerAnglesDegrees = {
				XMConvertToDegrees(m_tDesc.fCurRotationX),
				XMConvertToDegrees(m_tDesc.fCurRotationY),
				XMConvertToDegrees(m_tDesc.fCurRotationZ)
			};

			if (ImGui::DragFloat3(u8"Rotation (Local Euler °)", &eulerAnglesDegrees.x, 1.0f, -360.0f, 360.0f))
			{
				m_tDesc.fCurRotationX = XMConvertToRadians(eulerAnglesDegrees.x);
				m_tDesc.fCurRotationY = XMConvertToRadians(eulerAnglesDegrees.y);
				m_tDesc.fCurRotationZ = XMConvertToRadians(eulerAnglesDegrees.z);

				currentRotationQuatVec = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&_float3(m_tDesc.fCurRotationX, m_tDesc.fCurRotationY, m_tDesc.fCurRotationZ)));
				XMStoreFloat4(&m_tDesc.qOrientation, XMQuaternionNormalize(currentRotationQuatVec));
				changedSQT = true;
			}

			if (ImGui::DragFloat3("Scale (Local)", &scale.x, 0.01f, 0.001f, 1000.0f))
			{
				scaleVec = XMLoadFloat3(&scale);
				changedSQT = true;
			}

			if (changedSQT)
			{
				// 변경된 로컬 SQT로 로컬 월드 행렬 업데이트
				_matrix newLocalWorldMatrix = XMMatrixScalingFromVector(scaleVec) *
					XMMatrixRotationQuaternion(XMLoadFloat4(&m_tDesc.qOrientation)) *
					XMMatrixTranslationFromVector(translationVec);
				XMStoreFloat4x4(&m_tDesc.WorldMatrix, newLocalWorldMatrix);
				// SynchronizeRotationRepresentations(); // 이미 오일러 각과 쿼터니언이 동기화됨
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "XMMatrixDecompose failed for local matrix.");
		}
		ImGui::DragFloat("Speed", &m_tDesc.fSpeedPerSec, 0.1f, 0.0f, 10.f);
		ImGui::DragFloat("Rotation Speed", &m_tDesc.fRotationPerSec, 0.1f, 0.0f, 10.f);

		// 디버깅: 최종 월드 SQT 표시 (읽기 전용)
		if (ImGui::TreeNode("Computed World Transform (Read-Only)")) {
			_matrix finalWorldMat = Get_WorldMatrix();
			_vector finalS, finalR, finalT;
			XMMatrixDecompose(&finalS, &finalR, &finalT, finalWorldMat);

			_float3 finalScale, finalTranslation;
			_float4 finalOrientation;
			XMStoreFloat3(&finalScale, finalS);
			XMStoreFloat3(&finalTranslation, finalT);
			XMStoreFloat4(&finalOrientation, finalR);

			DirectX::SimpleMath::Quaternion smFinalQuat(finalR);
			DirectX::SimpleMath::Vector3 finalEulerRad = smFinalQuat.ToEuler();
			_float3 finalEulerDeg = { XMConvertToDegrees(finalEulerRad.x), XMConvertToDegrees(finalEulerRad.y), XMConvertToDegrees(finalEulerRad.z) };


			ImGui::InputFloat3("World Pos", &finalTranslation.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("World Euler Deg", &finalEulerDeg.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat4("World Quat", &finalOrientation.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("World Scale", &finalScale.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::TreePop();
		}
	}
	ImGui::PopID();
	return S_OK;
}

void* CTransform::Dump()
{
	return &m_tDesc;
}

#pragma region Movement
bool CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);
		
	vPosition += XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;

	if(nullptr == pNavigation || true == pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);

	return true;
}

bool CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vUp = Get_State(STATE::UP);

	vPosition += XMVector3Normalize(vUp) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vUp) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Sliding_Straight(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;

	/*if (pNavigation)
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vLook + vNormal);
		_vector vPos = Get_State(STATE::POSITION);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vLook) * fRadius))
			Set_State(STATE::POSITION, vPos);
		return false;
	}*/
	Set_State(STATE::POSITION, vPosition);
	return true;
}

bool CTransform::Sliding_Backward(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK) * -1.f;

	vPosition += XMVector3Normalize(vLook) * m_tDesc.fSpeedPerSec * fTimeDelta;

	/*if (pNavigation)
	{
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);
	}

	if (nullptr == pNavigation)
		Set_State(STATE::POSITION, vPosition);
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vLook + vNormal);
		_vector vPos = Get_State(STATE::POSITION);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vLook) * fRadius))
			Set_State(STATE::POSITION, vPos);
		return false;
	}*/
	Set_State(STATE::POSITION, vPosition);
	return true;
}

bool CTransform::Sliding_Left(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT) * -1.f;

	vPosition += XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;

	/*if (pNavigation)
	{
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);
	}

	if (nullptr == pNavigation)
		Set_State(STATE::POSITION, vPosition);
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vRight, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vRight + vNormal);
		_vector vPos = Get_State(STATE::POSITION);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vRight) * fRadius))
			Set_State(STATE::POSITION, vPos);
		return false;
	}*/
	Set_State(STATE::POSITION, vPosition);
	return true;
}

bool CTransform::Sliding_Right(_float fTimeDelta, CNavigation* pNavigation, _float fRadius)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_tDesc.fSpeedPerSec * fTimeDelta;
	Set_State(STATE::POSITION, vPosition);
	/*if (pNavigation)
	{
		pNavigation->Compute_CurrentIndex_byXZ(vPosition);
	}

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else if (false == pNavigation->isMove(vPosition + XMVector3Normalize(vRight) * fRadius))
	{
		_vector vNormal = XMVector3Normalize(pNavigation->Get_LastNormal());
		_float fDot = XMVectorGetX(XMVector3Dot(vRight, vNormal));
		vNormal = vNormal * fDot * -1.f;
		_vector vSliding = XMVector3Normalize(vRight + vNormal);
		_vector vPos = Get_State(STATE::POSITION);
		vPos += vSliding * fTimeDelta * m_tDesc.fSpeedPerSec;
		if (true == pNavigation->isMove(vPos + XMVector3Normalize(vRight) * fRadius))
			Set_State(STATE::POSITION, vPos);
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
	Set_State(STATE::POSITION, vPos);
	/*if (pNavigation)
		pNavigation->Compute_CurrentIndex_byXZ(vPos);

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vPos))
		Set_State(STATE::POSITION, vPos);*/

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

	WorldMatrix.r[0] = XMVector4Normalize(WorldMatrix.r[0]) * Get_Scale(STATE::RIGHT);
	WorldMatrix.r[1] = XMVector4Normalize(WorldMatrix.r[1]) * Get_Scale(STATE::UP);
	WorldMatrix.r[2] = XMVector4Normalize(WorldMatrix.r[2]) * Get_Scale(STATE::LOOK);
	WorldMatrix.r[3] = vWorldPos;

	XMStoreFloat4x4(&m_tDesc.WorldMatrix, WorldMatrix);

	_vector		vPosition = Get_State(STATE::POSITION);

	_vector		vAfterPosition = vPosition + vTranslation;
	vAfterPosition = XMVectorSetW(vAfterPosition, 1.f);
	Set_State(STATE::POSITION, vAfterPosition);
	/*if (pNavigation)
		pNavigation->Compute_CurrentIndex_byXZ(vAfterPosition);

	if (nullptr == pNavigation)
		
	else if (true == pNavigation->isMove(vAfterPosition))
		Set_State(STATE::POSITION, vAfterPosition);*/

	return true;
}

void CTransform::Jump(_float fTimeDelta, _float fVelocity, _float fGravity, _float fStartiHeight)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	float fPosY = fStartiHeight + fVelocity * fTimeDelta - (0.5f * fGravity * fTimeDelta * fTimeDelta);

	vPosition = XMVectorSetY(vPosition, fPosY);
	float y = XMVectorGetY(vPosition);
	Set_State(STATE::POSITION, vPosition);
}

bool CTransform::Go_PosTarget(_float fTimeDelta, _vector TargetPos, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vTargetPos = TargetPos;
	_vector vDir = vTargetPos - vPosition;
	_float fLength = XMVectorGetX(XMVector3Length(vDir));
	vDir = XMVector3Normalize(vDir);

	if (fLength < 0.5f)
	{
		Set_State(STATE::POSITION, vTargetPos);
		return true;
	}

	vPosition += vDir * fTimeDelta * m_tDesc.fSpeedPerSec;
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		Set_State(STATE::POSITION, vPosition);
	else if (true == pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_PosDir(_float fTimeDelta, _vector vecDir, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vDir = vecDir;

	vDir = XMVector3Normalize(vDir);

	vPosition += vDir * fTimeDelta * m_tDesc.fSpeedPerSec;
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		Set_State(STATE::POSITION, vPosition);
	else if (true == pNavigation->isMove(vPosition))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

	return true;
}

bool CTransform::Go_PosLerp(_float fTimeDelta, _vector TargetPos, _float fVecOffset)
{
	_vector vPos = Get_State(STATE::POSITION);
	_vector vDir = TargetPos - vPos;

	_float fLength = XMVectorGetX(XMVector3Length(vPos - TargetPos));
	_float  fVec = (1 - fTimeDelta) * 0 + fTimeDelta * fLength;

	vPos += XMVector3Normalize(vDir) * fVec * m_tDesc.fSpeedPerSec * fVecOffset;

	Set_State(STATE::POSITION, vPos);

	if (fVec == 0.0f || fLength < 0.1f)
		return true;

	return false;
}


void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_tDesc.fRotationPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(Get_State(STATE::RIGHT), RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(Get_State(STATE::UP), RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(Get_State(STATE::LOOK), RotationMatrix));

	SynchronizeRotationRepresentations();
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
	Set_State(STATE::RIGHT, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[0], NewRotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[1], NewRotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[2], NewRotationMatrix));
}

void CTransform::Set_RotationY(_float fAngleY)
{
	Set_Rotation(_float3(m_tDesc.fCurRotationX, fAngleY, m_tDesc.fCurRotationZ));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);	

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));

	SynchronizeRotationRepresentations();
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_vector		vQuaternion = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));

	SynchronizeRotationRepresentations();
}

void CTransform::LookAt(_fvector vAt)
{
	_float3		vScaled = Get_Scaled();

	_vector		vLook = vAt - Get_State(STATE::POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::LookDir(_fvector vDir)
{
	_vector		vPosition = Get_State(STATE::POSITION);

	_vector		vLook = vDir;
	_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vRight = XMVector3Cross(vAxisY, vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * Get_Scale(STATE::RIGHT));
	Set_State(STATE::UP, XMVector3Normalize(vUp) * Get_Scale(STATE::UP));
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * Get_Scale(STATE::LOOK));
}

void CTransform::LookAtExceptY(_fvector vAt)
{
	_vector		vLook = XMVectorSetY(vAt, m_tDesc.WorldMatrix.m[3][1]) - Get_State(STATE::POSITION);
	if (XMVectorGetX(XMVectorEqual(vLook, XMVectorSet(0.f, 0.f, 0.f, 0.f))) != 0)
		return;

	_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector		vRight = XMVector3Cross(vAxisY, vLook);

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * Get_Scale(STATE::RIGHT));
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * Get_Scale(STATE::LOOK));
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
	{ Get_Scale(STATE::RIGHT),
	Get_Scale(STATE::UP),
	Get_Scale(STATE::LOOK), 0.f };
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(scale);

	// 회전 쿼터니언 로드 및 회전 행렬 생성
	XMVECTOR rotationQuat = XMLoadFloat4(&m_tDesc.qOrientation);
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);

	// 위치 벡터 로드 (Get_State(POSITION) 함수가 있다고 가정)
	XMVECTOR position = Get_State(STATE::POSITION);

	// 월드 행렬 계산 (일반적으로 Scale * Rotation * Translation 순서)
	_matrix worldMatrix = scaleMatrix * rotationMatrix;
	// 이동(Translation)은 행렬의 마지막 행(r[3])에 직접 설정
	worldMatrix.r[3] = XMVectorSetW(position, 1.f); // W 요소를 1로 설정

	// 계산된 월드 행렬 저장
	XMStoreFloat4x4(&m_tDesc.WorldMatrix, worldMatrix);

	// 만약 Get_State(RIGHT/UP/LOOK)이 m_tDesc.WorldMatrix를 직접 참조하지 않고
	// 별도의 멤버 변수에 저장한다면, 여기서 업데이트 필요
	// Set_State(STATE::RIGHT, worldMatrix.r[0]);
	// Set_State(STATE::UP,    worldMatrix.r[1]);
	// Set_State(STATE::LOOK,  worldMatrix.r[2]);
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

// CTransform.cpp 파일에 다음 메서드 구현 추가 (이미 있다면 생략)
void CTransform::SynchronizeRotationRepresentations()
{
	// 현재 월드 행렬로부터 스케일, 회전(쿼터니언), 위치를 분해합니다.
	_vector scaleVec, rotationQuatVec, translationVec;
	// Get_WorldMatrix()를 사용하여 부모 변환이 적용된 최종 월드 행렬을 가져올 수도 있지만,
	// 여기서는 m_tDesc.WorldMatrix (로컬 월드 행렬)을 기준으로 동기화합니다.
	// 만약 Get_WorldMatrix()의 결과를 사용해야 한다면, 해당 행렬로 XMMatrixDecompose를 호출해야 합니다.
	// 일반적으로 로컬 변환의 회전 표현을 동기화하는 것이 목적이므로 m_tDesc.WorldMatrix를 사용합니다.
	if (XMMatrixDecompose(&scaleVec, &rotationQuatVec, &translationVec, XMLoadFloat4x4(&m_tDesc.WorldMatrix)))
	{
		// 분해된 회전 쿼터니언으로 m_tDesc.qOrientation 업데이트
		XMStoreFloat4(&m_tDesc.qOrientation, XMQuaternionNormalize(rotationQuatVec));

		// 업데이트된 쿼터니언으로부터 오일러 각(라디안)을 계산하여 업데이트합니다.
		// DirectX::SimpleMath::Quaternion::ToEuler()는 Yaw (Y축), Pitch (X축), Roll (Z축) 순서로 라디안 값을 반환합니다.
		DirectX::SimpleMath::Quaternion smQuat(rotationQuatVec); // XMVECTOR를 SimpleMath::Quaternion으로 변환
		DirectX::SimpleMath::Vector3 eulerAnglesRad = smQuat.ToEuler();

		// CTransform::Set_WorldMatrix 및 RenderImGui에서의 규칙과 일관되게 오일러 각을 저장합니다.
		// m_tDesc.fCurRotationX : Yaw (Y축 회전값) - eulerAnglesRad.x
		// m_tDesc.fCurRotationY : Pitch (X축 회전값) - eulerAnglesRad.y
		// m_tDesc.fCurRotationZ : Roll (Z축 회전값) - eulerAnglesRad.z
		// 이 매핑은 Set_WorldMatrix 및 RenderImGui의 변환 로직과 일치해야 합니다.
		// 현재 파일의 Set_WorldMatrix에서는 다음과 같이 사용하고 있습니다:
		// m_tDesc.fCurRotationX = eulerAnglesRad.x; // Yaw
		// m_tDesc.fCurRotationY = eulerAnglesRad.y; // Pitch
		// m_tDesc.fCurRotationZ = eulerAnglesRad.z; // Roll
		// 이 규칙을 따릅니다.
		m_tDesc.fCurRotationX = eulerAnglesRad.x;
		m_tDesc.fCurRotationY = eulerAnglesRad.y;
		m_tDesc.fCurRotationZ = eulerAnglesRad.z;
	}
	else
	{
		// 행렬 분해 실패 시 (예: 행렬이 비정규적 스케일링/전단 포함)
		OutputDebugStringA("CTransform::SynchronizeRotationRepresentations - XMMatrixDecompose failed.\n");
	}
}
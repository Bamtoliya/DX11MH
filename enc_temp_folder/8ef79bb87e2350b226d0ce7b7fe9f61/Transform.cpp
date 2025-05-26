#pragma once
#include "Transform.h"
#include "Shader.h"
#include "VIBuffer.h"
#include "GameObject.h"

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

void CTransform::Free()
{
	__super::Free();

}
#pragma endregion

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
	m_tDesc.bRelative = pDesc->bRelative;

	return S_OK;
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
	data["IsRelative"] = m_tDesc.bRelative;

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


	if (auto val = jsonData["SpeedPerSec"]; val.error() == simdjson::SUCCESS) { // 또는 get_uint64 등 타입에 맞게
		m_tDesc.fSpeedPerSec = static_cast<_float>(val.get_double().value_unsafe());
	}
	if (auto val = jsonData["RotationPerSec"]; val.error() == simdjson::SUCCESS) {
		m_tDesc.fRotationPerSec = static_cast<_float>(val.get_double().value_unsafe());
	}
	if (auto val = jsonData["IsRelative"]; val.error() == simdjson::SUCCESS) {
		m_tDesc.bRelative = val.get_bool().value_unsafe();
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
	if (m_tDesc.bRelative && nullptr != m_pOwner->Get_Parent())
	{
		CGameObject* pParentGameObject = m_pOwner->Get_Parent();
		_matrix finalMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);
		return XMMatrixMultiply(finalMatrix,
								static_cast<CTransform*>(pParentGameObject->Get_Component(g_strTransformTag))->Get_WorldMatrix());
	}
	else
	{
		return XMLoadFloat4x4(&m_tDesc.WorldMatrix);
	}
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
	if (m_tDesc.bRelative && nullptr != m_pOwner->Get_Parent())
	{
		CGameObject* pParentGameObject = m_pOwner->Get_Parent();
		_matrix finalMatrix = WorldMatrix;
		finalMatrix = XMMatrixMultiply(finalMatrix, static_cast<CTransform*>(pParentGameObject->Get_Component(g_strTransformTag))->Get_WorldMatrix_Inverse());
		/*while (pParentGameObject)
		{
			
			pParentGameObject = pParentGameObject->Get_Parent();
		}*/
		//부모가 없을때까지 반복적으로 월드매트릭스 인버스를 곱하기?
		XMStoreFloat4x4(&m_tDesc.WorldMatrix, finalMatrix);
	}
	else
	{
		XMStoreFloat4x4(&m_tDesc.WorldMatrix, WorldMatrix);
	}
	
	// 월드 행렬로부터 스케일, 회전(쿼터니언), 위치를 분해합니다.
	_vector scaleVec, rotationQuatVec, translationVec;
	if (XMMatrixDecompose(&scaleVec, &rotationQuatVec, &translationVec, WorldMatrix))
	{
		// 정규화된 쿼터니언을 저장합니다.
		XMStoreFloat4(&m_tDesc.qOrientation, XMQuaternionNormalize(rotationQuatVec));

		// (선택 사항) 오일러 각 멤버 변수도 업데이트합니다.
		// 이 오일러 각은 주로 ImGui 표시용으로 사용될 수 있습니다.
		// 정확한 오일러 각 변환 함수 필요 (이전 답변의 예시 참고 또는 직접 구현)
		// 여기서는 간단히 쿼터니언만 업데이트하고, RenderImGui에서 필요시 변환합니다.
		
		// UpdateEulerAnglesFromQuaternion(rotationQuatVec); // 이런 함수를 만들어서 호출 가능
		DirectX::XMVECTOR xmQuat = XMLoadFloat4(&m_tDesc.qOrientation);
		DirectX::SimpleMath::Quaternion smQuat(xmQuat); // XMVECTOR를 SimpleMath::Quaternion으로 변환
		DirectX::SimpleMath::Vector3 eulerAnglesRad = smQuat.ToEuler(); // YXZ 순서의 오일러 각 (Yaw, Pitch, Roll)을 라디안으로 반환

		// XMQuaternionRotationRollPitchYawFromVector는 Roll, Pitch, Yaw 순서를 기대하므로, 값을 올바르게 매핑해야 합니다.
		// eulerAnglesRad.x 가 Yaw, eulerAnglesRad.y 가 Pitch, eulerAnglesRad.z 가 Roll 인 경우:
		m_tDesc.fCurRotationX = eulerAnglesRad.x; // Yaw
		m_tDesc.fCurRotationY = eulerAnglesRad.y; // Pitch
		m_tDesc.fCurRotationZ = eulerAnglesRad.z; // Roll

		//_float3 eulerRad = SimpleMath::Quaternion::ToEuler(rotationQuatVec); // 직접 구현 또는 라이브러리 함수 사용
		//m_tDesc.fCurRotationX = eulerRad.x; // Roll에 해당하는 값
		//m_tDesc.fCurRotationY = eulerRad.y; // Pitch에 해당하는 값
		//m_tDesc.fCurRotationZ = eulerRad.z; // Yaw에 해당하는 값
	}
	else
	{
		// 행렬 분해 실패 시 처리 (예: 로그 출력)
		// 이 경우 qOrientation은 이전 값을 유지하거나, 기본값으로 설정할 수 있습니다.
		OutputDebugStringA("CTransform::Set_WorldMatrix - XMMatrixDecompose failed.\n");
	}
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
	if (m_tDesc.bRelative && nullptr != m_pOwner->Get_Parent())
	{
		_float4x4 finalMatrixToBind = {};
		XMStoreFloat4x4(&finalMatrixToBind,
			XMMatrixMultiply(XMLoadFloat4x4(&m_tDesc.WorldMatrix),
			static_cast<CTransform*>(m_pOwner->Get_Parent()->Get_Component(g_strTransformTag))->Get_WorldMatrix()));
		return pShader->Bind_Matrix(pConstantName, &finalMatrixToBind);
	}
	else
	{
		return pShader->Bind_Matrix(pConstantName, &m_tDesc.WorldMatrix);
	}
	
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, CVIBuffer* pVIBuffer)
{
	if (nullptr == pShader)
		return E_FAIL;

	_matrix worldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix); // CTransform의 현재 월드 행렬 (위치, 회전, 상대 스케일 적용됨)

	if (pVIBuffer != nullptr)
	{
		_float3 bufferLocalSize = pVIBuffer->Get_LocalSize();
		_matrix vibufferScaleMatrix = XMMatrixScaling(bufferLocalSize.x, bufferLocalSize.y, bufferLocalSize.z);
		_matrix finalWorldMatrixForShader = vibufferScaleMatrix * worldMatrix;

		_float4x4 finalMatrixToBind;
		XMStoreFloat4x4(&finalMatrixToBind, finalWorldMatrixForShader);
		if (m_tDesc.bRelative && nullptr != m_pOwner->Get_Parent())
		{
			_float4x4 parentMatrixToBind = {};
			XMStoreFloat4x4(&parentMatrixToBind,
				XMMatrixMultiply(XMLoadFloat4x4(&finalMatrixToBind),
				static_cast<CTransform*>(m_pOwner->Get_Parent()->Get_Component(g_strTransformTag))->Get_WorldMatrix()));
			return pShader->Bind_Matrix(pConstantName, &parentMatrixToBind);
		}
		return pShader->Bind_Matrix(pConstantName, &finalMatrixToBind);
	}
	// pVIBuffer가 nullptr이면, CTransform의 월드 행렬만 사용 (기존 방식)
	return pShader->Bind_Matrix(pConstantName, &m_tDesc.WorldMatrix);
}

HRESULT CTransform::RenderImGui()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Releative(Local)", &m_tDesc.bRelative);
		_matrix worldMatrix = XMLoadFloat4x4(&m_tDesc.WorldMatrix);
		_vector scaleVec, currentRotationQuatVec, translationVec;

		// 1. 월드 행렬 분해 (주로 스케일과 위치를 가져오고, 회전은 m_tDesc.qOrientation 사용)
		//    또는, m_tDesc.qOrientation을 직접 사용하고, 스케일/위치만 행렬에서 가져올 수도 있습니다.
		//    여기서는 일관성을 위해 분해 후 m_tDesc.qOrientation으로 다시 로드합니다.
		if (XMMatrixDecompose(&scaleVec, &currentRotationQuatVec, &translationVec, worldMatrix))
		{
			// 저장된 쿼터니언을 사용 (ImGuizmo에 의해 업데이트되었을 수 있으므로)
			currentRotationQuatVec = XMLoadFloat4(&m_tDesc.qOrientation);

			_float3 position, scale;
			XMStoreFloat3(&position, translationVec);
			XMStoreFloat3(&scale, scaleVec);

			bool changed = false;

			// --- 위치 (Position) ---
			if (ImGui::DragFloat3("Position", &position.x, 1.0f))
			{
				translationVec = XMLoadFloat3(&position);
				changed = true;
			}

			// --- 회전 (Rotation) - 오일러 각으로 표시 및 편집 ---
			// 현재 쿼터니언(m_tDesc.qOrientation)을 오일러 각(도)으로 변환하여 UI에 표시
			_float3 eulerAnglesDegrees;
			// 여기에 쿼터니언 -> 오일러 각(도) 변환 로직 필요
			// 예시: (정확한 변환 함수를 사용해야 합니다)
			//    auto quatToEuler = [](const _float4& q) -> _float3 {
			//        _float3 angles; // 라디안
			//        // ... (쿼터니언 q로부터 오일러 각 angles.x, angles.y, angles.z 계산) ...
			//        // 예: ZYX 순서 (DirectXMath의 Roll, Pitch, Yaw에 대응되도록)
			//        // Roll (x-axis rotation)
			//        double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
			//        double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
			//        angles.x = std::atan2(sinr_cosp, cosr_cosp);
			//        // Pitch (y-axis rotation)
			//        double sinp = 2 * (q.w * q.y - q.z * q.x);
			//        if (std::abs(sinp) >= 1)
			//            angles.y = std::copysign(XM_PI / 2, sinp); // use 90 degrees if out of range
			//        else
			//            angles.y = std::asin(sinp);
			//        // Yaw (z-axis rotation)
			//        double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
			//        double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
			//        angles.z = std::atan2(siny_cosp, cosy_cosp);
			//        return angles;
			//    };
			// _float3 eulerAnglesRadians = quatToEuler(m_tDesc.qOrientation);

			// 임시: 오일러 각 변환이 복잡하므로, 이전처럼 fCurRotationX, Y, Z를 사용하되,
			// ImGuizmo 변경 시 이 값들이 업데이트되도록 Set_WorldMatrix에서 처리해야 합니다.
			// 가장 좋은 방법은 m_tDesc.qOrientation에서 직접 오일러 각을 계산하는 것입니다.
			// 아래는 임시로 fCurRotationX,Y,Z를 계속 사용하는 방식입니다.
			// **이 방식은 Set_WorldMatrix에서 fCurRotationX,Y,Z도 함께 업데이트한다는 가정하에 동작합니다.**
			// **쿼터니언으로 완전 동기화하려면 m_tDesc.qOrientation -> 오일러 각 변환이 필요합니다.**

			// 여기서는 m_tDesc.qOrientation을 오일러 각(라디안)으로 변환한다고 가정합니다.
			// (실제 변환 함수는 프로젝트에 맞게 구현 필요)
			// 예를 들어, `GetEulerAnglesFromQuaternion(XMLoadFloat4(&m_tDesc.qOrientation))` 함수가 있다고 가정하고,
			// 이 함수가 라디안 단위의 _float3(roll, pitch, yaw)를 반환한다고 가정합니다.

			// --- 쿼터니언을 오일러 각으로 변환 (표시용) ---
			// 이 부분은 정확한 변환 함수가 필요합니다. (예: DirectX Tool Kit의 SimpleMath::Quaternion::ToEuler)
			// 여기서는 임시로 fCurRotationX,Y,Z를 사용하지만, 이상적으로는 m_tDesc.qOrientation에서 변환해야 합니다.
			// 만약 fCurRotationX,Y,Z가 Set_WorldMatrix에서 qOrientation과 동기화된다면 이대로 사용 가능.
			XMVECTOR tempQuatForEuler = XMLoadFloat4(&m_tDesc.qOrientation);
			// 오일러 각 추출 (예시 - 정확한 함수로 대체 필요)
			// 다음은 DirectX::SimpleMath (DirectX Tool Kit)의 예시입니다.
			// DirectX::SimpleMath::Vector3 eulerRad = DirectX::SimpleMath::Quaternion(tempQuatForEuler).ToEuler();
			// eulerAnglesDegrees.x = XMConvertToDegrees(eulerRad.x); // Roll
			// eulerAnglesDegrees.y = XMConvertToDegrees(eulerRad.y); // Pitch
			// eulerAnglesDegrees.z = XMConvertToDegrees(eulerRad.z); // Yaw
			// 지금은 fCurRotationX, Y, Z가 qOrientation과 동기화된다고 가정하고 사용합니다.
			eulerAnglesDegrees = {
				XMConvertToDegrees(m_tDesc.fCurRotationX),
				XMConvertToDegrees(m_tDesc.fCurRotationY),
				XMConvertToDegrees(m_tDesc.fCurRotationZ)
			};


			if (ImGui::DragFloat3(u8"Rotation (Euler °)", &eulerAnglesDegrees.x, 1.0f, -360.0f, 360.0f))
			{
				_float3 eulerAnglesRadians = {
					XMConvertToRadians(eulerAnglesDegrees.x),
					XMConvertToRadians(eulerAnglesDegrees.y),
					XMConvertToRadians(eulerAnglesDegrees.z)
				};
				// 오일러 각(라디안)으로부터 새로운 쿼터니언 생성 (X, Y, Z 순서로 Roll, Pitch, Yaw)
				currentRotationQuatVec = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&eulerAnglesRadians));
				XMStoreFloat4(&m_tDesc.qOrientation, XMQuaternionNormalize(currentRotationQuatVec)); // 쿼터니언 업데이트

				// fCurRotationX,Y,Z도 동기화 (UI 표시용)
				m_tDesc.fCurRotationX = eulerAnglesRadians.x;
				m_tDesc.fCurRotationY = eulerAnglesRadians.y;
				m_tDesc.fCurRotationZ = eulerAnglesRadians.z;

				changed = true;
			}

			// --- 크기 (Scale) ---
			if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.001f, 1000.0f))
			{
				/*if (scale.x < 0.001f) scale.x = 0.001f;
				if (scale.y < 0.001f) scale.y = 0.001f;
				if (scale.z < 0.001f) scale.z = 0.001f;*/
				scaleVec = XMLoadFloat3(&scale);
				changed = true;
			}

			// 2. 변경 사항이 있다면 월드 행렬 재구성 및 업데이트
			if (changed)
			{
				_matrix newWorldMatrix = XMMatrixScalingFromVector(scaleVec) *
					XMMatrixRotationQuaternion(currentRotationQuatVec) * // 업데이트된 쿼터니언 사용
					XMMatrixTranslationFromVector(translationVec);

				XMStoreFloat4x4(&m_tDesc.WorldMatrix, newWorldMatrix);
				// Set_WorldMatrix를 호출하여 qOrientation 등도 다시 업데이트할 필요는 없음 (이미 changed 플래그에서 직접 처리)
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "XMMatrixDecompose failed. Matrix might be non-affine.");
			// ... (기존 오류 처리) ...
		}
		ImGui::DragFloat("Speed", &m_tDesc.fSpeedPerSec, 0.1f, 0.0f, 10.f);
		ImGui::DragFloat("Rotation", &m_tDesc.fRotationPerSec, 0.1f, 0.0f, 10.f);
	}
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
	Set_State(STATE::POSITION, vPosition);
	/*if (nullptr == pNavigation)
		

	else if (true == pNavigation->isMove(vPosition + XMVector3Normalize(vLook) * fRadius))
		Set_State(STATE::POSITION, vPosition);
	else
		return false;*/

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
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE::RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scale(STATE::RIGHT), RotationMatrix));
	Set_State(STATE::UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scale(STATE::UP), RotationMatrix));
	Set_State(STATE::LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scale(STATE::LOOK), RotationMatrix));
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
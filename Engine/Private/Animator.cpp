
#include "Animator.h"
#include "GameInstance.h"
#include "State.h"
#include "Model.h"
#include "Animation.h"
#include "Bone.h"
#include "Mesh.h"
#include "Channel.h"

#pragma region Constructor & Destructor
CAnimator::CAnimator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CAnimator::CAnimator(const CAnimator& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CAnimator::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CAnimator::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;


}
HRESULT CAnimator::Late_Initialize()
{
	if (FAILED(__super::Late_Initialize()))
		return E_FAIL;
	CModel* pModelCom = nullptr;
	if (m_pOwner) {
		pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	}

	if (pModelCom) {
		// 모델로부터 최대 뼈 개수를 가져와 변환 벡터들 크기 초기화
		// CModel에 GetNumBones() 또는 GetBones().size() 같은 함수 필요
		_uint numBones = pModelCom->Get_NumBones();
		if (numBones > 0) {
			m_FinalBoneTransforms.resize(numBones);
			m_CurAnimSnapshotTransforms.resize(numBones);
			m_PreAnimSnapshotTransforms.resize(numBones);
			for (_uint i = 0; i < numBones; ++i)
			{
				m_FinalBoneTransforms[i] = *pModelCom->Get_Bones()[i]->Get_TransfomationMatrix();
				m_CurAnimSnapshotTransforms[i] = *pModelCom->Get_Bones()[i]->Get_TransfomationMatrix();
				m_PreAnimSnapshotTransforms[i] = *pModelCom->Get_Bones()[i]->Get_TransfomationMatrix();
			}
		}
	}
	else {
		MSG_BOX("CAnimator::Initialize - Failed to get CModel component.");
		// return E_FAIL; // 모델이 필수는 아닐 수 있으나, 애니메이션 재생 불가
	}
	m_iCurStateIndex = 0; // 또는 유효한 기본값
	m_pCurAnimation = nullptr;
	m_iPreStateIndex = 0;
	m_pPreAnimation = nullptr;
	return S_OK;
}
CAnimator* CAnimator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimator* pInstance = new CAnimator(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CAnimator* CAnimator::Clone(void* pArg)
{
	CAnimator* pInstance = new CAnimator(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CAnimator::Free()
{
	__super::Free();
	m_pCurAnimation = nullptr;
	m_pPreAnimation = nullptr;
}
#pragma endregion
#pragma region Save&Load
HRESULT CAnimator::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CAnimator::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}
#pragma endregion

#pragma region Loops
void CAnimator::Priority_Update(_float fTimeDelta)
{
}
void CAnimator::Update(_float fTimeDelta)
{
	Play_Animation(fTimeDelta);
}
void CAnimator::Late_Update(_float fTimeDelta)
{
}
HRESULT CAnimator::Render()
{
	return S_OK;
}

_bool CAnimator::Play_Animation(_float fTimeDelta)
{
	m_bIsFinished = false;
	if (!m_pOwner)
		return false;

	CModel* pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (!pModelCom)
		return false;

	vector<CBone*>& bones = pModelCom->Get_Bones();

	UpdateAnimationTimes(fTimeDelta);

	// 블렌딩이 활성화된 경우
	if (m_bBlend && m_pPreAnimation && m_pCurAnimation && (m_iCurStateIndex != m_iPreStateIndex))
	{
		m_fBlendElapsedTime += fTimeDelta;
		_float blendFactor = min(m_fBlendElapsedTime / m_fBlendDuration, 1.0f);
		//
		m_pPreAnimation->Sample_At_Time(m_fPreTrackPosition, m_PreAnimSnapshotTransforms);
		//
		m_pCurAnimation->Sample_At_Time(m_fCurTrackPosition, m_CurAnimSnapshotTransforms);
		//
		//
		// 두 애니메이션 스냅샷 블렌딩하여 본의 로컬 변환 업데이트
		for (size_t i = 0; i < bones.size(); ++i)
		{
			// 각 뼈의 이전 애니메이션 변환과 현재 애니메이션 변환을 가져옴
			XMMATRIX preTransformMat = XMLoadFloat4x4(&m_PreAnimSnapshotTransforms[i]);
			XMMATRIX curTransformMat = XMLoadFloat4x4(&m_CurAnimSnapshotTransforms[i]);
		
			// 각 변환 행렬을 스케일, 회전(쿼터니언), 이동으로 분해
			XMVECTOR preScale, preRotQuat, preTranslation;
			XMMatrixDecompose(&preScale, &preRotQuat, &preTranslation, preTransformMat);
		
			XMVECTOR curScale, curRotQuat, curTranslation;
			XMMatrixDecompose(&curScale, &curRotQuat, &curTranslation, curTransformMat);
		
			// blendFactor를 사용하여 선형 보간(Lerp) 및 구면 선형 보간(Slerp) 수행
			XMVECTOR blendedScale = XMVectorLerp(preScale, curScale, blendFactor);
			XMVECTOR blendedRotQuat = XMQuaternionSlerp(preRotQuat, curRotQuat, blendFactor);
			XMVECTOR blendedTranslation = XMVectorLerp(preTranslation, curTranslation, blendFactor);
		
			// 보간된 스케일, 회전, 이동으로 최종 변환 행렬 재구성
			XMMATRIX blendedTransform = XMMatrixAffineTransformation(
				blendedScale, XMVectorZero(), blendedRotQuat, blendedTranslation);
		
			// 계산된 최종 변환을 해당 뼈에 적용
			bones[i]->Set_TransformationMatrix(blendedTransform);
			XMStoreFloat4x4(&m_FinalBoneTransforms[i], blendedTransform);
		}
		
		//// 블렌딩 완료 체크
		if (blendFactor >= 1.0f)
		{
			m_bBlend = false; // 블렌딩 종료
			m_pPreAnimation = nullptr; // 이전 애니메이션 참조 해제
			//m_fPreTrackPosition = 0.f;
		
			// 현재 애니메이션이 루프되지 않고 재생 시간이 총 시간을 초과했는지 확인하여 종료 여부 결정
			if (!m_bCurAnimLoop && m_fCurTrackPosition >= m_pCurAnimation->Get_Duration()) {
				m_bIsFinished = true;
			}
		}

	}
	else // 블렌딩이 아니거나, 블렌딩이 끝난 경우 (단일 애니메이션 재생)
	{
		if (m_pCurAnimation)
		{
			// 현재 애니메이션의 m_fCurAnimTime 시점의 뼈 변환을 m_CurAnimSnapshotTransforms에 저장
			m_pCurAnimation->Sample_At_Time(m_fCurTrackPosition, m_CurAnimSnapshotTransforms);
			//m_pCurAnimation->Update_Bones(m_fCurTrackPosition, Bones, )
			for (size_t i = 0; i < bones.size(); ++i)
			{
				bones[i]->Set_TransformationMatrix(XMLoadFloat4x4(&m_CurAnimSnapshotTransforms[i]));
			}
			

			// 현재 애니메이션이 루프되지 않고 재생 시간이 총 시간을 초과했는지 확인하여 종료 여부 결정
			if (!m_bCurAnimLoop && m_fCurTrackPosition >= m_pCurAnimation->Get_Duration())
			{
				m_bIsFinished = true;
			}
		}
	}

	// 모든 본의 CombinedTransformationMatrix 업데이트
	_float4x4 preTransformMatrix = pModelCom->Get_PreTransformMatrix();
	for (auto& pBone : bones)
	{
		pBone->Update_CombinedTransformationMatrix(bones, XMLoadFloat4x4(&preTransformMatrix));
	}

	return m_bIsFinished;
}

_float CAnimator::Get_AnimationProgress()
{
	if (!m_pCurAnimation) return 1.f;
	return m_fCurTrackPosition / m_pCurAnimation->Get_Duration();
}

void CAnimator::Change_Animation(_uint animationIndex, _float fPlaybackSpeed, _bool bLoop, _float fBlendDuration)
{
	CModel* pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModelCom)
		return;

	CAnimation* pNewAnim = pModelCom->Get_Animation(animationIndex);

	if (fPlaybackSpeed != m_fCurAnimPlaybackSpeed)
		m_fCurAnimPlaybackSpeed = fPlaybackSpeed;

	if (m_pCurAnimation == pNewAnim && m_bCurAnimLoop == bLoop)
		return;

	if (fBlendDuration > 0.f && m_pCurAnimation != nullptr)
	{
		m_iPreStateIndex = m_iCurStateIndex;
		m_pPreAnimation = m_pCurAnimation;
		m_fPreTrackPosition = m_fCurTrackPosition;
		m_fPreAnimPlaybackSpeed = m_fCurAnimPlaybackSpeed;
		m_bPreAnimLoop = m_bCurAnimLoop;

		m_bBlend = true;
		m_fBlendDuration = fBlendDuration;
		m_fBlendElapsedTime = 0.f;
	}
	else
	{
		m_bBlend = false;
	}

	m_iCurStateIndex = animationIndex;
	m_fCurTrackPosition = 0.f;
	m_pCurAnimation = pNewAnim;
	m_pCurAnimation->Reset_KeyFrameIndices();
	m_fCurAnimPlaybackSpeed = fPlaybackSpeed;
	m_bCurAnimLoop = bLoop;
}
void CAnimator::UpdateAnimationTimes(_float fTimeDelta)
{
	CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (!pModel)
		return;
	if (m_pCurAnimation)
	{
		m_fCurTrackPosition += m_fCurAnimPlaybackSpeed * m_pCurAnimation->Get_TickPerSecond() * fTimeDelta;
		// 루프 처리
		if (m_bCurAnimLoop && m_fCurTrackPosition >= m_pCurAnimation->Get_Duration())
			m_fCurTrackPosition = fmod(m_fCurTrackPosition, m_pCurAnimation->Get_Duration());
		else if (!m_bCurAnimLoop && m_fCurTrackPosition > m_pCurAnimation->Get_Duration())
			m_fCurTrackPosition = m_pCurAnimation->Get_Duration();
	}

	if (m_bBlend && m_pPreAnimation)
	{
		m_fPreTrackPosition += m_fPreAnimPlaybackSpeed * m_pPreAnimation->Get_TickPerSecond() * fTimeDelta;

		if (m_bPreAnimLoop && m_fPreTrackPosition >= m_pPreAnimation->Get_Duration())
			m_fPreTrackPosition = fmod(m_fPreTrackPosition, m_pPreAnimation->Get_Duration());
		else if (!m_bPreAnimLoop && m_fPreTrackPosition > m_pPreAnimation->Get_Duration())
			m_fPreTrackPosition = m_pPreAnimation->Get_Duration();
	}
}

#pragma endregion
HRESULT CAnimator::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("AnimatorProps", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			// 테이블 헤더
			ImGui::TableSetupColumn("Property");
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			// 현재 애니메이션 정보
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Current Animation");
			ImGui::TableSetColumnIndex(1);
			if (m_pCurAnimation) {
				ImGui::Text("%s (Index: %u)", m_pCurAnimation->Get_Name(), m_iCurStateIndex);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("  Track Position");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.2f / %.2f", m_fCurTrackPosition, m_pCurAnimation->Get_Duration());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("  Playback Speed");
				ImGui::TableSetColumnIndex(1);
				ImGui::InputFloat("##CurSpeed", &m_fCurAnimPlaybackSpeed, 0.01f, 0.1f, "%.2f");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("  Looping");
				ImGui::TableSetColumnIndex(1);
				ImGui::Checkbox("##CurLoop", &m_bCurAnimLoop);
			}
			else {
				ImGui::Text("None");
			}

			// 블렌딩 정보
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Blending Active");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", m_bBlend ? "Yes" : "No");

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Previous Animation");
			ImGui::TableSetColumnIndex(1);
			if(m_pPreAnimation)
				ImGui::Text("%s (Index: %u)", m_pPreAnimation->Get_Name(), m_iPreStateIndex);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("  Track Position");
			ImGui::TableSetColumnIndex(1);
			if (m_pPreAnimation)
				ImGui::Text("%.2f / %.2f", m_fPreTrackPosition, m_pPreAnimation->Get_Duration());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("  Playback Speed");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.2f", m_fPreAnimPlaybackSpeed); // 이전 애니메이션 속도는 보통 고정

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("  Looping");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", m_bPreAnimLoop ? "Yes" : "No"); // 이전 애니메이션 루프 여부도 고정

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Blend Progress");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.2f / %.2f (%.1f%%)", m_fBlendElapsedTime, m_fBlendDuration, (m_fBlendElapsedTime / m_fBlendDuration) * 100.f);
			ImGui::ProgressBar(m_fBlendElapsedTime / m_fBlendDuration, ImVec2(-FLT_MIN, 0.f));
			

			// 기타 상태 변수 (필요에 따라 추가)
			// ImGui::TableNextRow();
			// ImGui::TableSetColumnIndex(0);
			// ImGui::Text("Delay Time");
			// ImGui::TableSetColumnIndex(1);
			// ImGui::Text("%.2f", m_fDelayTime); // m_fDelayTime 변수가 있다면

			ImGui::EndTable();
		}
	}
	ImGui::PopID();
	return S_OK;
}
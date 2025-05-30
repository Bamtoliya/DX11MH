
#include "Animator.h"
#include "GameInstance.h"
#include "State.h"
#include "Model.h"
#include "Animation.h"

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
HRESULT CAnimator::Late_Initialize()
{
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
	if (m_iCurStateIndex != m_iPreStateIndex)
	{
		if (m_fDelayTime <= m_fElapsedTime)
		{
			m_iPreStateIndex = m_iCurStateIndex;
			CModel* pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));


			//* 여기 수정해야함 Set_Animation이 아니라 블렌딩된 본정보로 업데이트를 돌려야함 *//
			pModelCom->Set_Animation(m_iCurStateIndex, true);
		}
		m_fElapsedTime += fTimeDelta;
		
	}
	else
	{
		m_fElapsedTime = 0.f;
	}
	
}
void CAnimator::Late_Update(_float fTimeDelta)
{
}
HRESULT CAnimator::Render()
{
	return S_OK;
}

void CAnimator::PlayAnimation(_uint animationIndex, _bool bLoop, _float fPlaybackSpeed, _float fBlendDuration)
{
	_bool isFinished = false;
	CModel* pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModelCom)
		return;

	CAnimation* pNewAnim = pModelCom->Get_Animation(animationIndex);

	if (m_pCurAnimation == pNewAnim && m_bCurAnimLoop == bLoop && m_fCurAnimPlaybackSpeed == fPlaybackSpeed)
		return;

	if (fBlendDuration > 0.f && m_pCurAnimation != nullptr)
	{
		m_iPreStateIndex = m_iCurStateIndex;
		m_pPreAnimation = m_pCurAnimation;
		m_fPreAnimTime = m_fCurAnimTime;
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
	m_pCurAnimation = pNewAnim;
	m_fCurAnimTime = 0.f; // 새 애니메이션은 처음부터 재생
	m_fCurAnimPlaybackSpeed = fPlaybackSpeed;
	m_bCurAnimLoop = bLoop;
}
void CAnimator::UpdateAnimationTimes(_float fTimeDelta)
{
	if (m_pCurAnimation)
	{
		_float fCurAnimDuration = m_pCurAnimation->Get_Duration(); // CAnimation에 GetDuration() 필요
		_float fCurTicksPerSecond = m_pCurAnimation->Get_TickPerSecond(); // CAnimation에 GetTicksPerSecond() 필요

		m_fCurAnimTime += fTimeDelta * m_fCurAnimPlaybackSpeed * fCurTicksPerSecond;

		if (m_bCurAnimLoop)
		{
			if (fCurAnimDuration > 0.f)
				m_fCurAnimTime = fmod(m_fCurAnimTime, fCurAnimDuration);
		}
		else
		{
			if (m_fCurAnimTime >= fCurAnimDuration)
				m_fCurAnimTime = fCurAnimDuration; // 마지막 프레임에서 멈춤
		}
	}

	if (m_bBlend && m_pPreAnimation)
	{
		_float fPreAnimDuration = m_pPreAnimation->Get_Duration();
		_float fPreTicksPerSecond = m_pPreAnimation->Get_TickPerSecond();

		m_fPreAnimTime += fTimeDelta * m_fPreAnimPlaybackSpeed * fPreTicksPerSecond;

		// 이전 애니메이션은 블렌딩 중에는 루프하지 않도록 하거나, 현재 루프 상태를 따르도록 할 수 있음
		// 여기서는 루프 여부와 관계없이 시간이 계속 흐르도록 함 (블렌딩 완료 시 더 이상 사용 안 함)
		if (m_bPreAnimLoop) {
			if (fPreAnimDuration > 0.f)
				m_fPreAnimTime = fmod(m_fPreAnimTime, fPreAnimDuration);
		}
		else {
			if (m_fPreAnimTime >= fPreAnimDuration)
				m_fPreAnimTime = fPreAnimDuration;
		}
	}
}
void CAnimator::Add_State(_uint iIndex, CState* pState)
{
	if (m_StateLists[iIndex])
		Safe_Release(m_StateLists[iIndex]);
	m_StateLists[iIndex] = pState;
}

#pragma endregion
HRESULT CAnimator::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
	return S_OK;
}
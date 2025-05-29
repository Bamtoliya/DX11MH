
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
			m_NextAnimSnapshotTransforms.resize(numBones);
		}
	}
	else {
		MSG_BOX("CAnimator::Initialize - Failed to get CModel component.");
		// return E_FAIL; // 모델이 필수는 아닐 수 있으나, 애니메이션 재생 불가
	}
	m_iCurStateIndex = 0; // 또는 유효한 기본값
	m_pCurAnimation = nullptr;
	m_iNextStateIndex = 0;
	m_pNextAnimation = nullptr;


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
	if (m_iCurStateIndex != m_iNextStateIndex)
	{
		if (m_fDelayTime <= m_fElapsedTime)
		{
			m_iCurStateIndex = m_iNextStateIndex;
			CModel* pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));

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
	CModel* pModelCom = static_cast<CModel*>(m_pOwner->Get_Component(L"Com_Model"));
	if (nullptr == pModelCom)
		return;

	CAnimation* pNewAnim = pModelCom->Get_Animation(animationIndex);

	if (m_pCurAnimation == pNewAnim && m_bCurAnimLoop == bLoop && m_fCurAnimPlaybackSpeed == fPlaybackSpeed)
		return;

	if (fBlendDuration > 0.f && m_pCurAnimation != nullptr)
	{
		m_iNextStateIndex = m_iCurStateIndex;
		m_pNextAnimation = m_pCurAnimation;
		m_fNextAnimTime = m_fCurAnimTime;
		m_fNextAnimPlaybackSpeed = m_fCurAnimPlaybackSpeed;
		m_bNextAnimLoop = m_bCurAnimLoop;

		m_bBlend = true;
		m_fBlendDuration = fBlendDuration;
		m_fBlendElapsedTime = 0.f;
	}
	else
	{
		m_bBlend = false;
	}
}
void CAnimator::UpdateAnimationTimes(_float fTimeDelta)
{

}
void CAnimator::CalculateBlendedTransforms()
{
}
void CAnimator::CalculateSingleAnimationTransforms(CAnimation* pAnimation, _float fAnimationTime, _bool bIsLoop, vector<_float4x4>& outTransforms)
{
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
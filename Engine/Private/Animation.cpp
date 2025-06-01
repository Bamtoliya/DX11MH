#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

#pragma region Constructor & Destructor
CAnimation::CAnimation()
{

}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fDuration{ Prototype.m_fDuration }
	, m_fDefaultTickPerSec{ Prototype.m_fDefaultTickPerSec }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	//, m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
{
	strcpy_s(m_szName, Prototype.m_szName);
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

CAnimation* CAnimation::Create(ifstream& ifs, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(ifs, Bones)))
	{
		MSG_BOX("Failed to Created in Binary : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

HRESULT CAnimation::Initialize(ifstream& ifs, const vector<class CBone*>& Bones)
{
	/* 이 애니메이션이 컨트롤해야하는 뼈의 갯수 */
	_uint iNameLength = {};
	ifs.read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&m_szName[0]), sizeof(_char) * iNameLength);
	m_szName[iNameLength] = '\0';
	_double duration = { 0.0 };
	_double tick = { 0.0 };
	ifs.read(reinterpret_cast<char*>(&duration), sizeof(_double));
	ifs.read(reinterpret_cast<char*>(&tick), sizeof(_double));

	m_fDuration = static_cast<_float>(duration);
	m_fTickPerSecond = static_cast<_float>(tick);
	m_fDefaultTickPerSec = m_fTickPerSecond;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	/* 각 뼈의 정보를 새엇ㅇ한다. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(ifs, Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
	return S_OK;
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
	/* 이 애니메이션이 컨트롤해야하는 뼈의 갯수 */
	m_iNumChannels = pAIAnimation->mNumChannels;
	strcpy_s(m_szName, pAIAnimation->mName.C_Str());
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;
	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fDefaultTickPerSec = m_fTickPerSecond;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);
		
	/* 각 뼈의 정보를 새엇ㅇ한다. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}
#pragma endregion


_bool CAnimation::Update_Bones(_float& fTrackPosition, const vector<CBone*>& Bones, _bool isLoop)
{
	if (fTrackPosition >= m_fDuration)
	{
		if (false == isLoop)
		{
			fTrackPosition = m_fDuration;
			return true;
		}		
		
		fTrackPosition = 0.f;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Update_TransformationMatrix(&m_CurrentKeyFrameIndices[i], fTrackPosition, Bones);
	}

	return false; 
}

void CAnimation::Sample_At_Time(_float fTime, vector<_float4x4>& outTransforms)
{
	// 애니메이션 시간이 범위를 벗어나면 처리
	if (fTime < 0.f)
		fTime = 0.f;
	else if (fTime > m_fDuration)
		fTime = m_fDuration;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (m_Channels[i] != nullptr)
		{
			// CChannel의 Sample_Transform 함수를 호출하여 fTime에서의 변환 행렬을 계산합니다.
			// 이 함수는 해당 채널이 어떤 뼈에 영향을 미치는지 알고 있어야 하며,
			// 계산된 변환 행렬을 outTransforms의 적절한 위치에 저장해야 합니다.

			// CChannel이 자신이 담당하는 뼈의 인덱스(Bones 벡터 내의)를 알고 있다고 가정합니다.
			// 또는 CChannel의 이름과 Bones 벡터 내의 CBone 이름을 비교하여 일치하는 뼈를 찾아야 합니다.

			// 예시: CChannel이 대상 뼈의 인덱스를 가지고 있는 경우 (m_iTargetBoneIndex는 CChannel의 멤버 변수라고 가정)
			_uint targetBoneIndex = m_Channels[i]->Get_TargetBoneIndex();
			if (targetBoneIndex < outTransforms.size())
			{
			    m_Channels[i]->Sample_Transform(fTime, &outTransforms[targetBoneIndex]);
			}

			// 예시: CChannel의 이름과 CBone의 이름을 비교하는 경우
			//const char* channelName = m_Channels[i]->Get_Name(); // CChannel에 Get_Name()이 있다고 가정
			//for (size_t boneIdx = 0; boneIdx < Bones.size(); ++boneIdx)
			//{
			//	if (Bones[boneIdx] != nullptr && Bones[boneIdx]->Compare_Name(channelName))
			//	{
			//		// 해당 뼈를 찾았으면, outTransforms의 해당 위치에 변환 행렬을 저장합니다.
			//		// CChannel의 Sample_Transform은 _float4x4*를 인자로 받으므로, 주소를 전달합니다.
			//		m_Channels[i]->Sample_Transform(fTime, &outTransforms[boneIdx]);
			//		break; // 다음 채널로 넘어감
			//	}
			//}
		}
	}
}

HRESULT CAnimation::RenderImGui(_uint i)
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(("[" + to_string(i) + "]" + m_szName).c_str()))
	{
		ImGui::Text("Duration : %.2f", m_fDuration);
		ImGui::PushItemWidth(100.0f); // 원하는 너비 값 (예: 100.0f)으로 설정합니다.
		ImGui::DragFloat("Tick Per Second", &m_fTickPerSecond, 0.01f, 0.0f, 100.f);
		ImGui::PopItemWidth(); // 너비 설정을 원래대로 복원합니다.
		ImGui::SameLine();
		if (ImGui::Button("Reset", ImVec2(150, 0)))
		{
			m_fTickPerSecond = m_fDefaultTickPerSec;
		}
		//ImGui::Text("Current Track Position : %.2f", m_fCurrentTrackPosition);
		ImGui::BeginChild("ChannelsListChild", ImVec2(0, 150), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		for (auto& pChannel : m_Channels)
		{
			if (pChannel != nullptr)
			{
				pChannel->RenderImGui(); // 각 채널의 ImGui 렌더링 함수 호출
				ImGui::Separator();     // 채널 간 구분선 추가
			}
		}
		ImGui::EndChild(); // 채널 정보 자식 창 종료
	}		
	ImGui::PopID();
	return S_OK;
}
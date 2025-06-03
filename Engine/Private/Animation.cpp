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
	for (auto& pChannel : Prototype.m_Channels)
		Safe_AddRef(pChannel);
	/*for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);*/
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
			_uint targetBoneIndex = m_Channels[i]->Get_TargetBoneIndex();
			if (targetBoneIndex < outTransforms.size())
			{
			    m_Channels[i]->Sample_Transform(fTime, &outTransforms[targetBoneIndex]);
			}
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
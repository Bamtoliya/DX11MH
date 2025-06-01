#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CBone;
class CAnimation final : public CBase
{
#pragma region Constructor & Destructor
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;
public:
	static CAnimation* Create(ifstream& ifs, const vector<class CBone*>& Bones);
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	CAnimation* Clone();
	virtual void Free() override;
public:
	HRESULT Initialize(ifstream& ifs, const vector<class CBone*>& Bones);
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
#pragma endregion
#pragma region Variables
private:
	_char					m_szName[MAX_PATH] = {};
	/* 전체 재생 거리. */
	_float					m_fDuration = {};
	_float					m_fDefaultTickPerSec = {};
	_float					m_fTickPerSecond = {};
	//_float					m_fCurrentTrackPosition = {};
	vector<_uint>			m_CurrentKeyFrameIndices;

	/* 이 애니메이션을 표현하기위해서 사용하는 뼈의 갯수 */
	_uint					m_iNumChannels;
	vector<class CChannel*>	m_Channels;
#pragma endregion

public:	
	_bool Update_Bones(_float& fTrackPosition, const vector<CBone*>& Bones, _bool isLoop);
	const _char* Get_Name() const { return m_szName; }

	void Reset_KeyFrameIndices() { m_CurrentKeyFrameIndices.clear(); m_CurrentKeyFrameIndices.resize(m_iNumChannels); }
	
	_float Get_Duration() const { return m_fDuration; }
	_float Get_TickPerSecond() const { return m_fTickPerSecond; }
	/*_float Get_TrackPosition() const { return m_fCurrentTrackPosition; }
	void Set_TrackPosition(_float fPosition) { m_fCurrentTrackPosition = fPosition; }*/
	_uint Get_NumChannels() const { return m_iNumChannels; }
	class CChannel* Get_Channel(_uint iIndex) const {
		if (iIndex >= m_Channels.size()) return nullptr;
		return m_Channels[iIndex];
	}
	void Sample_At_Time(_float fTime, vector<_float4x4>& outTransforms);
	
	HRESULT RenderImGui(_uint i);

};

NS_END
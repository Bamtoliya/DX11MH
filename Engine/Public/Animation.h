#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	typedef struct tagAnimationFromModel
	{
		_float fDuration = 0.f;
		_float fTickPerSecond = 0.f;
		_uint iNumChannels = 0;

	} MODEL_ANIMATION;
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ifstream& ifs, const vector<class CBone*>& Bones);
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);

	void Update_Bones(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop);
private:
	/* 전체 재생 거리. */
	_float					m_fDuration = {};
	_float					m_fTickPerSecond = {};
	_float					m_fCurrentTrackPosition = {};

	/* 이 애니메이션을 표현하기위해서 사용하는 뼈의 갯수 */
	_uint					m_iNumChannels;	
	vector<class CChannel*>	m_Channels;

	_char					m_szName[MAX_PATH] = {};

public:
	static CAnimation* Create(ifstream& ifs, const vector<class CBone*>& Bones);
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

NS_END
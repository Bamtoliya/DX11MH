#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
#pragma region Structures
public:
	typedef struct tagAnimationFromModel
	{
		_float fDuration = 0.f;
		_float fTickPerSecond = 0.f;
		_uint iNumChannels = 0;

	} MODEL_ANIMATION;


#pragma endregion
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
	_float					m_fTickPerSecond = {};
	_float					m_fCurrentTrackPosition = {};
	vector<_uint>			m_CurrentKeyFrameIndices;

	/* 이 애니메이션을 표현하기위해서 사용하는 뼈의 갯수 */
	_uint					m_iNumChannels;
	vector<class CChannel*>	m_Channels;
#pragma endregion



public:
	
	_bool Update_Bones(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop);

	HRESULT RenderImGui();


};

NS_END
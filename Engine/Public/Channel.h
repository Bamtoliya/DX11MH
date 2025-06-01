#pragma once

#include "Base.h"

/* 특정 애니메이션을 구동하기위해서 필요한, 뼈의 시간당 상태정보*/

NS_BEGIN(Engine)

class CChannel final : public CBase
{
#pragma region Structure
public:
	typedef struct tagChannelFromModel
	{
		_char* m_szName;
		_uint iNumKeyFrames;
		KEYFRAME* pKeyFrames;
		_uint iBoneIndex;
	} MODEL_CHANNEL;
#pragma endregion

#pragma region Constructor & Destructor
private:
	CChannel();
	virtual ~CChannel() = default;
public:
	static CChannel* Create(ifstream& ifs, const vector<class CBone*>& Bones);
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
public:
	HRESULT Initialize(ifstream& ifs, const vector<class CBone*>& Bones);
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<class CBone*>& Bones);

#pragma endregion

#pragma region Variables
private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iNumKeyFrames;
	vector<KEYFRAME>	m_KeyFrames;
	_uint				m_iBoneIndex = {};
	//_uint				m_iCurrentKeyFrameIndex = {};
#pragma endregion

public:
	_char* Get_Name() { return m_szName; }
	_uint Get_TargetBoneIndex() { return m_iBoneIndex; }
	void Sample_Transform(_float fTime, _float4x4* pOutMatrix);
	HRESULT RenderImGui();
};

NS_END
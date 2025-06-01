#include "Channel.h"
#include "Bone.h"

#pragma region Constructor & Destructor
CChannel::CChannel()
{
}

CChannel* CChannel::Create(ifstream& ifs, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(ifs, Bones)))
	{
		MSG_BOX("Failed to Created in Binary : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}

HRESULT CChannel::Initialize(ifstream& ifs, const vector<class CBone*>& Bones)
{
	_uint iNameLength = 0;
	ifs.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&m_szName[0]), sizeof(_char) * iNameLength);
	m_szName[iNameLength] = '\0';
	_uint iNumPositionKeys = 0;
	_uint iNumRotationKeys = 0;
	_uint iNumScalingKeys = 0;
	ifs.read(reinterpret_cast<char*>(&iNumPositionKeys), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&iNumRotationKeys), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&iNumScalingKeys), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(_uint));

	_float3		vTranslation = {};
	_float4		vRotation = {};
	_float3		vScale = {};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame = {};
		_double trackPosition = {};

		if (i < iNumPositionKeys)
		{
			ifs.read(reinterpret_cast<char*>(&trackPosition), sizeof(_double));
			ifs.read(reinterpret_cast<char*>(&vTranslation), sizeof(_float3));
		}

		if (i < iNumRotationKeys)
		{
			ifs.read(reinterpret_cast<char*>(&trackPosition), sizeof(_double));
			ifs.read(reinterpret_cast<char*>(&vRotation.x), sizeof(_float));
			ifs.read(reinterpret_cast<char*>(&vRotation.y), sizeof(_float));
			ifs.read(reinterpret_cast<char*>(&vRotation.z), sizeof(_float));
			ifs.read(reinterpret_cast<char*>(&vRotation.w), sizeof(_float));
		}

		if (i < iNumScalingKeys)
		{
			ifs.read(reinterpret_cast<char*>(&trackPosition), sizeof(_double));
			ifs.read(reinterpret_cast<char*>(&vScale), sizeof(_float3));
		}

		KeyFrame.fTrackPosition = static_cast<_float>(trackPosition);
		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(m_szName))
				return true;

			++m_iBoneIndex;

			return false;
		});


	return S_OK;
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.C_Str());
	m_iNumKeyFrames = max(pAIChannel->mNumPositionKeys, pAIChannel->mNumScalingKeys);
	m_iNumKeyFrames = max(pAIChannel->mNumRotationKeys, m_iNumKeyFrames);

	_float3		vScale;
	_float4		vRotation;
	_float3		vTranslation;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(pAIChannel->mNodeName.data))
				return true;

			++m_iBoneIndex;

			return false;
		});


	return S_OK;
}
#pragma endregion

void CChannel::Update_TransformationMatrix(_uint* pCurrentKeyFrameIndex, _float fCurrentTrackPosition, const vector<class CBone*>& Bones)
{
	if (0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	_matrix			TransformationMatrix{};

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale, vRotation, vPosition;

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition  = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	else
	{
		if(fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		_float			fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourTranslation, vDestTranslation;

		vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);

		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);

		vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);
		vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);
		
		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}

	// TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}


void CChannel::Sample_Transform(_float fTime, _float4x4* pOutMatrix)
{
	if (m_KeyFrames.empty())
		return;

	// 위치, 회전, 크기 키프레임 값을 샘플링
	_vector vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	_vector vRotation = XMQuaternionIdentity();
	_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	// 시간에 맞는 키프레임 찾기
	if (fTime <= m_KeyFrames.front().fTrackPosition)
	{
		// 첫 번째 키프레임보다 이전 시간
		vScale = XMLoadFloat3(&m_KeyFrames.front().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.front().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.front().vTranslation);
	}
	else if (fTime >= m_KeyFrames.back().fTrackPosition)
	{
		// 마지막 키프레임보다 이후 시간
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vTranslation);
	}
	else
	{
		// 두 키프레임 사이 시간 - 보간 필요
		_uint iCurrentFrame = 0;

		// 현재 시간이 속한 키프레임 구간 찾기
		for (_uint i = 0; i < m_KeyFrames.size() - 1; ++i)
		{
			if (fTime >= m_KeyFrames[i].fTrackPosition && fTime < m_KeyFrames[i + 1].fTrackPosition)
			{
				iCurrentFrame = i;
				break;
			}
		}

		// 두 키프레임 사이의 보간 비율 계산
		_float t = (fTime - m_KeyFrames[iCurrentFrame].fTrackPosition) /
			(m_KeyFrames[iCurrentFrame + 1].fTrackPosition - m_KeyFrames[iCurrentFrame].fTrackPosition);

		// 스케일 보간 (선형)
		_vector vS0 = XMLoadFloat3(&m_KeyFrames[iCurrentFrame].vScale);
		_vector vS1 = XMLoadFloat3(&m_KeyFrames[iCurrentFrame + 1].vScale);
		vScale = XMVectorLerp(vS0, vS1, t);

		// 회전 보간 (구면 선형)
		_vector vQ0 = XMLoadFloat4(&m_KeyFrames[iCurrentFrame].vRotation);
		_vector vQ1 = XMLoadFloat4(&m_KeyFrames[iCurrentFrame + 1].vRotation);
		vRotation = XMQuaternionSlerp(vQ0, vQ1, t);

		// 위치 보간 (선형)
		_vector vP0 = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[iCurrentFrame].vTranslation), 1.0f);
		_vector vP1 = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[iCurrentFrame + 1].vTranslation), 1.0f);
		vPosition = XMVectorLerp(vP0, vP1, t);
	}

	// 행렬 조합 (크기 -> 회전 -> 이동 순서)
	_matrix TransformationMatrix = XMMatrixAffineTransformation(
		vScale,
		XMVectorSet(0.f, 0.f, 0.f, 1.f), // 회전 중심점
		vRotation,
		vPosition
	);

	// 결과 저장

	XMStoreFloat4x4(pOutMatrix, TransformationMatrix);
}

HRESULT CChannel::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(m_szName))
	{
		ImGui::Text("KeyFrames : %d", m_iNumKeyFrames);
		ImGui::Text("BoneIndex : %d", m_iBoneIndex);
	}
	ImGui::PopID();
	return S_OK;
}
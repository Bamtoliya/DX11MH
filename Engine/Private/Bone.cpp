#include "Bone.h"
#pragma region Constructor & Destructor
CBone::CBone()
{
}

CBone* CBone::Create(ifstream& ifs, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(ifs, iParentIndex)))
	{
		MSG_BOX("Failed to Created in Binary : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}

HRESULT CBone::Initialize(ifstream& ifs, _int iParentBoneIndex)
{
	_uint szNameLength = { 0 };
	ifs.read(reinterpret_cast<char*>(&szNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<char*>(&m_szName[0]), sizeof(_char) * szNameLength);
	m_szName[szNameLength] = '\0';
	ifs.read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof(_float4x4));
	ifs.read(reinterpret_cast<char*>(&m_iParentBoneIndex), sizeof(_int));


	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;
	return S_OK;
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
	strcpy_s(m_szName, pAINode->mName.data);

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;

	return S_OK;
}

#pragma endregion



void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, 
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));

}


HRESULT CBone::RenderImGui()
{
	return S_OK;
}
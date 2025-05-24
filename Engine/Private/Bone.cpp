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


HRESULT CBone::RenderImGuiNode(const std::vector<CBone*>& allBones, _int currentBoneListIndex)
{
	if (nullptr == this) // 혹시 모를 경우를 대비한 방어 코드
		return E_FAIL;

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	// 필요하다면 선택된 뼈에 ImGuiTreeNodeFlags_Selected 플래그를 추가할 수 있습니다.
	// 예: if (IsBoneSelected(this)) node_flags |= ImGuiTreeNodeFlags_Selected;

	const _char* boneName = (m_szName[0] == '\0') ? "[Unnamed Bone]" : m_szName;
	std::string nodeLabel = std::to_string(currentBoneListIndex) + " | " + boneName;

	// ImGui::PushID(this); // 뼈 이름이 고유하지 않을 경우 ID 충돌 방지를 위해 포인터 사용 가능
	bool node_open = ImGui::TreeNodeEx(nodeLabel.c_str(), node_flags);
	// ImGui::PopID();

	// 뼈 노드 클릭 시 특정 동작 (예: 뼈 선택)을 추가할 수 있습니다.
	if (ImGui::IsItemClicked())
	{
		// 예: CGameInstance::Get_Instance()->SelectBone(this);
	}

	if (node_open)
	{
		for (size_t i = 0; i < allBones.size(); ++i)
		{
			CBone* pChildBone = allBones[i];
			if (pChildBone != nullptr && pChildBone->Get_ParentBoneIndex() == currentBoneListIndex)
			{
				// 자식 뼈에 대해 재귀적으로 RenderImGuiNode 호출
				pChildBone->RenderImGuiNode(allBones, static_cast<_int>(i));
			}
		}
		ImGui::TreePop();
	}
	return S_OK;
}
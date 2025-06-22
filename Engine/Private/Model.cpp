#pragma once
#include "Component.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"

namespace fs = std::filesystem;

#pragma region Constructor & Destructor
CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_eType{ Prototype.m_eType }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	, m_Bones { Prototype.m_Bones }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations { Prototype.m_Animations }
{
	strcpy_s(m_szModelFilePath, Prototype.m_szModelFilePath);
	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);
		//m_Bones.push_back(pBone->Clone());

	for (auto& pMaterial : m_Materials)
		Safe_AddRef(pMaterial);

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& pAnim : Prototype.m_Animations)
		//m_Animations.push_back(pAnim->Clone());
		Safe_AddRef(pAnim);
		
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	strncpy_s(m_szModelFilePath, pModelFilePath, MAX_PATH);
	fs::path filePath(pModelFilePath);
	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	if (filePath.extension() == L".model")
	{
		ifstream inputFile(filePath, std::ios::binary);

		if (inputFile.is_open())
		{
			inputFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
			inputFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
			inputFile.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

			if (FAILED(Ready_Bin_Bones(inputFile, -1)))
				return E_FAIL;

			if (FAILED(Ready_Bin_Meshes(inputFile)))
				return E_FAIL;

			if (FAILED(Ready_Bin_Materials(pModelFilePath, inputFile)))
				return E_FAIL;

			/* 각 애니메이션 마다 이용하고 있는 뼈대들의 시간에 맞는 상태값들을 미리 읽어서 저장해둔다. */
			if (FAILED(Ready_Bin_Animations(inputFile)))
				return E_FAIL;
			inputFile.close();
		}
	}
	else
	{
		_uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

		if (MODEL::NONANIM == eType)
			iFlag |= aiProcess_PreTransformVertices;

		m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

		if (nullptr == m_pAIScene)
			return E_FAIL;

		if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
			return E_FAIL;

		if (FAILED(Ready_Meshes()))
			return E_FAIL;

		if (FAILED(Ready_Materials(pModelFilePath)))
			return E_FAIL;

		///* 각 애니메이션 마다 이용하고 있는 뼈대들의 시간에 맞는 상태값들을 미리 읽어서 저장해둔다. */
		if (FAILED(Ready_Animations()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int		iParentIndex = (_int)m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParentIndex);
	}
	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bin_Bones(ifstream& ifs, _int iParentBoneIndex)
{
	
	CBone* pBone = CBone::Create(ifs, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;
	m_Bones.push_back(pBone);
	_uint iNumChildren = {};
	ifs.read(reinterpret_cast<char*>(&iNumChildren), sizeof(_uint));
	_int		iParentIndex = (_int)m_Bones.size() - 1;

	for (_uint i = 0; i < iNumChildren; ++i)
	{
		Ready_Bin_Bones(ifs, iParentIndex);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bin_Meshes(ifstream& ifs)
{
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, ifs, m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bin_Materials(const _char* pModelFilePath, ifstream& ifs)
{
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, ifs);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}
	return S_OK;
}

HRESULT CModel::Ready_Bin_Animations(ifstream& ifs)
{
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(ifs, m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMaterial : m_Materials)
		Safe_Release(pMaterial);
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	m_Importer.FreeScene();
}
#pragma endregion

#pragma region Save&Load

HRESULT CModel::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CModel::Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

HRESULT CModel::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CModel";
	outJson["ModelFilePath"] = string(m_szModelFilePath);
	switch (m_eType)
	{
		case MODEL::ANIM: outJson["Type"] = "ANIM"; break;
		case MODEL::NONANIM: outJson["Type"] = "NONANIM"; break;
	}

	json matrixArray = json::array();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// nlohmann/json은 float를 직접 저장해도 충분한 정밀도를 가집니다.
			// 특정 정밀도의 문자열로 저장하고 싶다면 FloatToString 헬퍼 사용.
			matrixArray.push_back(m_PreTransformMatrix.m[i][j]);
		}
	}
	outJson["PreTransformedMatrix"] = matrixArray;
	return S_OK;
}

HRESULT CModel::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	string_view filePath_sv;
	if (jsonData["ModelFilePath"].get_string().get(filePath_sv)) {
		return E_FAIL;
	}
	strcpy_s(m_szModelFilePath, string(filePath_sv).c_str()); // .c_str() 추가

	// Type 읽기
	string_view type_sv;
	if (jsonData["Type"].get_string().get(type_sv)) {
		// 오류 처리
	}
	string typeStr = string(type_sv);
	if (typeStr == "ANIM") {
		m_eType = MODEL::ANIM;
	}
	else if (typeStr == "NONANIM") {
		m_eType = MODEL::NONANIM;
	}
	else {
		// 알 수 없는 타입에 대한 처리
		return E_FAIL;
	}

	// PreTransformedMatrix 읽기
	simdjson::ondemand::array matrixJsonArray;
	if (jsonData["PreTransformedMatrix"].get_array().get(matrixJsonArray)) {
		// 오류 처리
	}

	_float matrixValues[16];
	size_t index = 0;
	for (auto value : matrixJsonArray) {
		if (index < 16) {
			double val_double; // simdjson은 double로 숫자 값을 가져옵니다.
			if (value.get_double().get(val_double)) {
				// 오류 처리
			}
			matrixValues[index++] = static_cast<_float>(val_double);
		}
		else {
			// 배열 크기 초과 오류 처리
			break;
		}
	}

	if (index != 16) {
		// 행렬 값 개수 부족 오류 처리
		return E_FAIL;
	}

	// XMFLOAT4X4로 변환 (Row-major 순서로 가정)
	// JSON에 저장된 순서가 Column-major라면 그에 맞게 수정해야 합니다.
	// 현재 직렬화 코드에서는 행 우선으로 저장하고 있습니다.
	m_PreTransformMatrix = XMFLOAT4X4(
		matrixValues[0], matrixValues[1], matrixValues[2], matrixValues[3],
		matrixValues[4], matrixValues[5], matrixValues[6], matrixValues[7],
		matrixValues[8], matrixValues[9], matrixValues[10], matrixValues[11],
		matrixValues[12], matrixValues[13], matrixValues[14], matrixValues[15]
	);

	// JSON에서 읽어온 정보로 모델 초기화
	// Initialize_Prototype을 호출하여 모델 데이터를 로드합니다.
	// Initialize_Prototype은 이미 m_szModelFilePath, m_eType, m_PreTransformMatrix를 사용합니다.
	if (FAILED(Initialize_Prototype(m_eType, m_szModelFilePath, XMLoadFloat4x4(&m_PreTransformMatrix))))
	{
		MSG_BOX("Failed to Initialize_Prototype in Prototype_Deserialize");
		return E_FAIL;
	}

	return S_OK;
}

#pragma endregion

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;		

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex]->Bind_ShaderResource(pShader, pConstantName, eType, iTextureIndex);	
}

HRESULT CModel::Bind_Bone_Matrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Bind_Bone_Matrices(pShader, pConstantName, m_Bones);	
}

HRESULT CModel::Render(_uint iMeshIndex)
{	
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();	

	return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	_bool		isFinished = { false };
	/* 1. 현재 애니메이션에 맞는 뼈의 상태를 읽어와서 뼈의 TrnasformationMatrix를 갱신해준다. */
	isFinished  = m_Animations[m_iCurrentAnimIndex]->Update_Bones(fTimeDelta, m_Bones, m_isLoop);

	/* 2. 전체 뼐르 순회하면서 뼈들의 ColmbinedTransformationMatixf를 부모에서부터 자식으로 갱신해주낟. */
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	/*XMMatrixDecompose()*/

	return isFinished;
}

void CModel::Set_Animation(_uint iIndex, _bool isLoop)
{

	if (iIndex != m_iCurrentAnimIndex)
	{
		m_iCurrentAnimIndex = iIndex;
	}
		
	m_isLoop = isLoop;

}

HRESULT CModel::RenderImGui()
{
	if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
	{
		string strType = "Non Anim";
		if(m_eType == MODEL::ANIM)
			strType = "Anim";
		ImGui::Text("Model Type : %s", strType.c_str());
		ImGui::InputText("Model File Path", &m_szModelFilePath[0], MAX_PATH, ImGuiInputTextFlags_ElideLeft);
		
		ImGui::Indent();
		if (ImGui::CollapsingHeader(("Meshes (" + std::to_string(m_iNumMeshes) + ")").c_str()))
		{
			int a = 0;
			for (auto& pMesh : m_Meshes)
			{
				a += pMesh->Get_NumVertices();
				pMesh->RenderImGui();
			}
			ImGui::Text(to_string(a).c_str());

		}
		ImGui::Unindent();
		ImGui::Indent();
		if (ImGui::CollapsingHeader(("Bones (" + std::to_string(m_Bones.size()) + ")").c_str()))
		{
			for (size_t i = 0; i < m_Bones.size(); ++i)
			{
				CBone* pBone = m_Bones[i];
				if (pBone != nullptr && pBone->Get_ParentBoneIndex() == -1)
				{
					pBone->RenderImGuiNode(m_Bones, static_cast<_int>(i));
				}
			}
		}
		ImGui::Unindent();
		ImGui::Indent();
		if (ImGui::CollapsingHeader(("Materials (" + std::to_string(m_iNumMaterials) + ")").c_str()))
		{
			for (auto& pMaterials : m_Materials)
				pMaterials->RenderImGui();
		}
		ImGui::Unindent();
		
		if (m_eType == MODEL::ANIM)
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader(("Animations (" + std::to_string(m_iNumAnimations) + ")").c_str()))
			{
				// 애니메이션 목록을 표시하기 위한 자식 창
				ImGui::BeginChild("AnimationList", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
				for (size_t i = 0; i < m_Animations.size(); ++i)
				{
					if (m_Animations[i] != nullptr)
					{
						// 각 애니메이션을 선택 가능한 항목으로 표시
						// CAnimation의 m_szName을 레이블로 사용합니다.
						if (ImGui::Selectable(("[" + to_string(i) + "]" + m_Animations[i]->Get_Name()).c_str(), m_iCurrentAnimIndex == i))
						{
							m_iCurrentAnimIndex = static_cast<_uint>(i);
						}
					}
				}
				ImGui::EndChild();

				// 현재 선택된 애니메이션의 상세 정보를 표시 (선택 사항)
				if (m_iCurrentAnimIndex < m_Animations.size() && m_Animations[m_iCurrentAnimIndex] != nullptr)
				{
					ImGui::Separator();
					ImGui::Text("Selected Animation Details:");
					// CAnimation::RenderImGui(_uint i)를 호출하여 상세 정보 표시
					// _uint i 파라미터는 ImGui ID 스택 관리를 위해 사용될 수 있습니다.
					m_Animations[m_iCurrentAnimIndex]->RenderImGui(m_iCurrentAnimIndex);
				}
			}
			
			//ImGui::InputScalar("##Current Animation Index", ImGuiDataType_U32, &m_iCurrentAnimIndex);
			//ImGui::SameLine();
			// UTF-8 기호를 사용합니다: ▷ (재생), ■ (정지)
			// 글꼴이 해당 기호를 지원해야 올바르게 표시됩니다.
			if (ImGui::Button("Play", ImVec2(100.f, 0.f))) // "##Play"는 ID 충돌을 피하기 위함입니다.
			{
				if (m_iCurrentAnimIndex < m_Animations.size())
				{
					m_isLoop = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause", ImVec2(100.f, 0.f))) // "##Pause"은 ID 충돌을 피하기 위함입니다.
			{
				m_isLoop = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop", ImVec2(100.f, 0.f))) // "##Stop"은 ID 충돌을 피하기 위함입니다.
			{
				m_isLoop = false;
			}
			ImGui::Unindent();
		}
	} 
	return S_OK;
}
#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
#pragma region Constructor & Destructor
protected:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual HRESULT Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
public:
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Animations();
public:
	HRESULT Ready_Bin_Bones(ifstream& ifs, _int iParentBoneIndex);
	HRESULT Ready_Bin_Meshes(ifstream& ifs);
	HRESULT Ready_Bin_Materials(const _char* pModelFilePath, ifstream& ifs);
	HRESULT Ready_Bin_Animations(ifstream& ifs);
#pragma endregion

#pragma region Variables
private:
	Assimp::Importer			m_Importer;

	/* 모델에 대한 모든 정보르,ㄹ 담고 있는 구조체. */
	const aiScene* m_pAIScene = { nullptr };

	MODEL						m_eType = {};
	_float4x4					m_PreTransformMatrix = {};
	_uint						m_iNumMeshes = {};
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = {};
	vector<class CMaterial*>	m_Materials;

	vector<class CBone*>		m_Bones;

	_bool						m_isLoop = {};
	_uint						m_iCurrentAnimIndex = { };

	_uint						m_iNumAnimations = {};
	vector<class CAnimation*>	m_Animations;

private:
	_char						m_szModelFilePath[MAX_PATH] = {};
#pragma endregion

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object & jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) override;
#pragma endregion

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	_uint Get_NumBones() const { return static_cast<_uint>(m_Bones.size()); }
	vector<class CBone*> Get_Bones() const { return m_Bones; }
	_float4x4 Get_PreTransformMatrix() const { return m_PreTransformMatrix; }
	const _float4x4* Get_BoneMatrix(const _char* pBoneName) const;
	class CAnimation* Get_Animation(_uint iIndex) const { return m_Animations[iIndex]; }
	vector<class CMesh*> Get_Meshes() { return m_Meshes; }
public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eType, _uint iTextureIndex = 0);
	HRESULT Bind_Bone_Matrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	virtual HRESULT Render(_uint iMeshIndex);
public:
	_bool Play_Animation(_float fTimeDelta);
	void Set_Animation(_uint iIndex, _bool isLoop = true);
public:
	virtual HRESULT RenderImGui() override;

};

NS_END
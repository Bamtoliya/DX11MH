#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
#pragma region Structures
public:
	typedef struct tagMeshFromModel
	{
		_char* szName;
		_uint iMaterialIndex;
		_uint iPrimitiveTypes;
		_uint iNumVertices;
		_uint iNumIndices;
		_uint iNumBones;
		_uint iVertexStride;
		_uint iIndexStride;
		_uint* pIndices;
		_float3* pVertices;
		_float3* pNormals;
		_float3* pTangents;
		_float2* pTexcoords;
		_float4x4* pOffsetMatrices;
		_float4x4* pBoneMatrices;
	} MODEL_MESH;
#pragma endregion

#pragma region Constructor & Destructor

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;
public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, ifstream& ifs, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual HRESULT Initialize_Prototype(MODEL eType, ifstream& ifs, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Prototype(MODEL eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);

private:
	HRESULT Ready_NonAnim_Mesh(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Anim_Mesh(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);

	HRESULT Ready_NonAnim_Mesh(ifstream& ifs, _fmatrix PreTransformMatrix);
	HRESULT Ready_Anim_Mesh(ifstream& ifs, const vector<class CBone*>& Bones);
#pragma endregion

#pragma region Variables
private:
	_char			m_szName[MAX_PATH] = {};
	_uint			m_iMaterialIndex = {};

	/* 모델에 선언된 전체뼈(x) */
	/* 전체 뼈들 중, 이 메시에 영향을 주는 뼈들만골라서 모아놓은 컨테이너. */
	_uint			m_iNumBones = { };

	/* 전체를 기준으로 이 메시에 영향을 주는 뼈의 인덱스를 모아놓았다. */
	vector<_int>			m_BoneIndices;
	_float4x4				m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>		m_OffsetMatrices;
#pragma endregion

public:
	HRESULT RenderImGui();
public:
	_uint Get_NumVertices() { return m_iNumVertices; }
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
public:
	HRESULT Bind_Bone_Matrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);




};

NS_END
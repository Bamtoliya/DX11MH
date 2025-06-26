#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)
class CMesh;
class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instance
{
public:
	struct InstanceData {
		_float3 vPosition;
		_float3 vRotation;
		_float3 vScale;
	};
	typedef struct tagMeshInstance : public CVIBuffer_Instance::INSTANCE_DESC
	{
		CMesh* pMesh = { nullptr };
	}MESH_INSTANCE_DESC;
private:
	CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	void Update(const vector<_float4x4>& worldMatrices);
	virtual HRESULT Resize(_uint iNumInstance) override;
	virtual HRESULT ApplyChanges() override;
	virtual void EditInstance(_uint iIndex)  override;
	virtual HRESULT ResetInstances()  override;
protected:
	VTXMESH_INSTANCE* m_pVertexInstances = { nullptr };
	vector<InstanceData> m_EditData;

public:
	static CVIBuffer_Mesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END
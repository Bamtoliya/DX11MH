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
		_float4 qOrientation = { 0.f, 0.f, 0.f, 1.f }; // 쿼터니언 추가
		_float fCurRotationX = 0.f; // 오일러 각 (라디안)
		_float fCurRotationY = 0.f;
		_float fCurRotationZ = 0.f;
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
#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& outJson) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

public:
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Bind_Buffers(CMesh* pMesh);
	void Update(const vector<_float4x4>& worldMatrices);
	virtual HRESULT Resize(_uint iNumInstance) override;
	virtual HRESULT ApplyChanges() override;
	virtual void EditInstance(_uint iIndex)  override;
	virtual HRESULT ResetInstances()  override;
private:
	void ExtractSRTFromMatrix(const VTXMESH_INSTANCE& instance, _float3& outPosition, _float3& outRotation, _float3& outScale);
	void CreateMatrixFromSRT(VTXMESH_INSTANCE& outInstance, const _float3& position, const _float3& rotation, const _float3& scale);
	HRESULT UpdateSingleInstance(_uint iIndex);
protected:
	VTXMESH_INSTANCE*		m_pVertexInstances = { nullptr };
	vector<InstanceData>	m_EditData;

public:
	static CVIBuffer_Mesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END
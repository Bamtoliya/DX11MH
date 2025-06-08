
#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CHeightField : public CComponent
{
public:
#pragma region Structs
	typedef struct tagHeightFieldDesc {
		PxRigidActor* pActor = nullptr;			// PhysX 액터
		PxShape* pShape = nullptr;				// PhysX Shape
		PxHeightField* pHeightField = nullptr;	// PhysX HeightField
		
	}HEIGHTFIELD_DESC;
	typedef struct tagTriangleMeshDesc {
		PxRigidActor* pActor = nullptr;			// PhysX 액터
		PxTriangleMesh* pTriangleMesh = nullptr;	// PhysX TriangleMesh
	}TRIANGLEMESH_DESC;
#pragma endregion

#pragma region Constructor/Destructor
protected:
	CHeightField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeightField(const CHeightField& Prototype);
	virtual ~CHeightField() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CHeightField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;
	virtual HRESULT Render() override;
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
private:
	HRESULT Create_HeightField(const _float3* pVertexPositions, _uint totalVerticesX, _uint totalVerticesZ, _uint startX, _uint startZ, _uint gridVerticesX, _uint gridVerticesZ, _uint tileIndex);		// HeightField 생성 로직
	HRESULT Create_TriangleMesh(const _float3* pVertexPositions, _uint totalVerticesX, _uint totalVerticesZ, _uint startX, _uint startZ, _uint gridVerticesX, _uint gridVerticesZ, _uint tileIndex);
	HRESULT Create_Actor();			// PhysX 액터 생성 로직
	void Release_Actor();			// PhysX 액터 해제 로직
private:
	vector<HEIGHTFIELD_DESC>	m_HeightFieldDescs = {};	// HeightField 정보 벡터
	vector<TRIANGLEMESH_DESC>	m_TriangleMeshDescs = {};	// HeightField 정보 벡터
	_uint						m_iHeightFieldCount = 0;	// HeightField 개수
	_uint						m_iTileSize = 1;			// HeightField 타일 크기
	_uint						m_iTileCountX = 8;
	_uint						m_iTileCountZ = 8;
	_float						m_fHfHeightScale = 1.f;			// HeightField의 Y축 스케일
	_float						m_fHfRowScale = 1.f;				// HeightField의 X축 스케일 (정점 간 간격)
	_float						m_fHfColumnScale = 1.f;			// HeightField의 Z축 스케일 (정점 간 간격)
};
NS_END
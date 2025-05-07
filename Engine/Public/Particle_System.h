#pragma once
#include "Component.h"
#include "Bounding_Box.h"
#include "Bounding_Sphere.h"
#include "Texture.h"

NS_BEGIN(Engine)

class ENGINE_DLL CParticle_System abstract : public CComponent
{
public:
	typedef struct tagParticleSystemDesc
	{		
		DWORD		VBSize;
		_wstring	strShaderPath;
		_wstring	strTexturePath;
		_float		fSize;
		_uint		iNumTextures;
	} PARTICLEDESC;
protected:
	typedef struct tagInstancedData
	{
		_float3 vOffset;
		DWORD	instanceColor;
	}INSTNACEDATA;
	typedef struct tagParticleVertex
	{
		_float3 vPosition;
		DWORD	vColor;
	}PARTICLEVTX;
protected:
	CParticle_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_System(const CParticle_System& Prototype);
	virtual ~CParticle_System() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render();
	virtual HRESULT Post_Render();

public:
	virtual void Add_Particle(void* pArg = nullptr);
	virtual void Reset();
	virtual void Reset_Particle(ATTRIBUTE* pAttribute, void* pArg = nullptr);

public:
	void Set_Origin(const _float3& vOrigin) { m_vPos = vOrigin; }
	bool IsEmpty();
	bool IsDead();
	void Remove_Dead_Particles();

protected:

	_float3						m_vPos = {};
	ID3D11Buffer*			m_PointVB = { nullptr };
	CTexture*						m_pTexture = { nullptr };

	//인스턴싱 변수
	/*ID3D11Buffer*					m_InstanceVB = { nullptr };
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl = { nullptr };
	LPDIRECT3DVERTEXSHADER9			m_pVertexShader = { nullptr };*/

	_float							m_fEmit_Rate = {};
	_float							m_fSize = {};
	_uint							m_iMaxParticles = {};
	list<ATTRIBUTE>					m_Particles = {};
	
	DWORD	m_VBSize;
	DWORD	m_VBOffset;
	DWORD	m_VBBatchSize;
	
public:
	virtual CParticle_System* Clone(void* pArg) override;
	void Free() override;
};

NS_END
#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMyMaterial final : public CComponent
{
public:
	typedef struct tagMaterialInit
	{
		const _wstring MaterialFilePath;
	} MATERIAL_DESC;
	typedef struct tagMaterialInfo
	{
		_float4 Diffuse;
		_float4 Ambient;
		_float4 Specular;
		_float4 Emissive;
		_float	Power;
	} MATERIAL;
private:
	CMyMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyMaterial(const CMyMaterial& Prototype);
	virtual ~CMyMaterial() = default;
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pMaterialFilePath);
	virtual HRESULT Initialize(void* pArg) override;
public:
	HRESULT Bind_Resource();
	HRESULT Bind_Material(class CShader* pShader);
private:
	MATERIAL						m_tMaterial;
public:
	static	CMyMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pMaterialFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
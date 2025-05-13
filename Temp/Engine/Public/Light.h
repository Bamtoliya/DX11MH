#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLight final : public CComponent
{
public:
	enum LIGHTTYPE { LT_UNUSED = 0, LT_POINT = 1, LT_SPOT, LT_DIR, LT_END};
	typedef struct tagLightFilePath
	{
		_wstring strLightPath;
	} LIGHT_INIT;
	typedef struct tagLightDesc{
		LIGHTTYPE	eType			= {};
		_float4		fDiffuse		= {};
		_float4		fAmbient		= {};
		_float4		fSpecular		= {};
		_float3		fPosition		= {};
		_float3		fDirection		= {};
		_float		fAttenuation0	= {};
		_float		fAttenuation1	= {};
		_float		fAttenuation2	= {};
		_float		fFalloff		= {};
		_float		fRange			= {};
		_float		fTheta			= {};
		_float		fPhi			= {};
	}LIGHT_DESC;
	typedef struct tagShaderLight {
		_int eType;
		_float3 padding_after_type; // 12 bytes 패딩
		_float4 fDiffuse;
		_float4 fPosition;    // Use D3DX types which match float3/float4 layout
		_float4 fDirection;
		_float fRange;
		_float fAttenuation0;
		_float fAttenuation1;
		_float fAttenuation2;
		//SpotLight Options
		_float fFallOff;
		_float fTheta; // Match HLSL struct fields
		_float fPhi;
		_float fPadding_End;    // Ensure total size is multiple of 16 bytes if necessary
		// Adjust padding based on exact HLSL struct size/layout.
	}SHADER_LIGHT;
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight(const CLight& Prototype);
	virtual ~CLight() = default;
public:
	_float3 Get_Position() { return m_tInfo.fPosition; }
	void Set_Position(const _float3& position) { m_tInfo.fPosition = { position.x, position.y, position.z }; }

public:
	virtual HRESULT Initialize_Prototype(LIGHTTYPE eType);
	virtual HRESULT Initialize(void* pArg);
public:
	HRESULT Bind_Resouce(_uint iIndex);
	HRESULT Bind_Light(class CShader* pShader);
	SHADER_LIGHT Get_Shader_Light();
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

public:
	void DecreaseIntensity(_uint iFrame);
	
private:
	LIGHT_DESC	m_tInfo = {};
public:
	static CLight*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LIGHTTYPE eType);
	virtual CLight* Clone(void* pArg) override;
	virtual void	Free() override;
};

NS_END


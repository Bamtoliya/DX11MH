#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CCamera : public CComponent
{
#pragma region Structures
public:
	typedef struct tagCameraComponentDesc {
		_float4				vAt = _float4(0.f, 0.f, 0.f, 1.0f);
		_float				fAspect;
		_float				fDefaultFovy = XMConvertToRadians(60.f);    // 기본 FOV 값 (라디안)
		_float				fMinFovy = XMConvertToRadians(10.f);        // 최소 FOV (최대 줌인)
		_float				fMaxFovy = XMConvertToRadians(200.f);        // 최대 FOV (최대 줌아웃)
		_float				fFovy = XMConvertToRadians(60.0f);
		_float				fNear = 0.1f;
		_float				fFar = 500.f;
		_bool				bOrthographic = false;
		_float				fZoomPerSec = 1.f;
	} CAMERA_COM_DESC;
#pragma endregion

#pragma region Constructor/Destructor
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;
public:
	static CCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamera* Clone(void* pArg) override;
	virtual void Free();
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Late_Initialize() override;
#pragma endregion
#pragma region Variables
protected:
	CAMERA_COM_DESC m_tDesc = {};
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Render() override;
public:
	virtual void Bind_Matrices(class CTransform* pTransformCom);
#pragma endregion

#pragma region CameraControl
	void ZoomIn(_float fTimeDelta);
	void ZoomOut(_float fTimeDelta);
	void ResetZoom(_float fTimeDelta);
#pragma endregion

public:
	virtual HRESULT RenderImGui() override;

};
NS_END
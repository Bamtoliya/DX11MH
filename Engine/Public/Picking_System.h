#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CGameObject;
class CGameInstance;
class CPicking_System final : public CBase
{
#pragma region Constructor/Destructor
private:
	CPicking_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking_System() = default;

public:
	static CPicking_System* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
#pragma endregion

public:
	HRESULT ReadPickingTargetPixel(_int screenX, _int screenY, _Out_ _float4* pPickedColor);
	HRESULT PickObjectByColor(_float4* pColor);
	HRESULT PickObjectById(_uint iId);
	CGameObject* Get_PickedObject() const { return m_pPickedObject; }
public:
	virtual HRESULT Initialize();
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };

private:
	ID3D11Texture2D*		m_pTexture		= { nullptr };

private:
	CGameObject*			m_pPickedObject = { nullptr };
	
};
NS_END
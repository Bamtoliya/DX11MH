#pragma once

#include "GameObject.h"

/* CUIObject_2D */
/* CUIObject_3D */

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float2			vPos;
		_float2			vSize;
	}UIOBJECT_DESC;
protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	/* 뷰포트 상의 유아이의 중심위치 fX, fY, 사이즈 fSiuzeX, fSizeY */
	_float			m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};

	/* 직교 투영을 위한 행렬. */
	_float4x4		m_ViewMatrix{}, m_ProjMatrix{};

public:
	virtual CGameObject* Clone(CGameObject* pGameObject = nullptr, void* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END
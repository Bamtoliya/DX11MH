#pragma once
#include "Base.h"
BEGIN(Engine)
class CRenderTarget;
class CTarget_Manager final : public CBase
{
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();

	virtual HRESULT Initialize();


	map<const _tchar*, CRenderTarget*> m_targets;
	
};
END
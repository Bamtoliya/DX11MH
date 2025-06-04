#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };

private:


private:
	list<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDERGROUP::RG_END)];
	ID3D11Buffer* m_pVB_DebugLine = nullptr;      // 디버그 라인용 동적 정점 버퍼
	_uint           m_iNumMaxDebugLineVertices = 0; // 버퍼의 최대 정점 수
	_bool           m_bPhysXDebugRender = true;

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_Debug();
	//HRESULT Render_Diffuse();
	//HRESULT Render_Normal();
	//HRESULT Render_Metalic();
	//HRESULT Render_Specular();
	//HRESULT Render_Picking();
	//HRESULT Render_Lights();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
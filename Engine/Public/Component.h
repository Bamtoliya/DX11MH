#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CComponent abstract : public CBase
{
public:
	typedef struct tagComponentDesc {
	} COMPONENT_DESC;
#pragma region Constructor/Destructor
protected:
	CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
#pragma endregion
public:
	virtual HRESULT RenderImGui() { return S_OK; }
	virtual void* Dump() { return nullptr; }
#pragma region Variables
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
#pragma endregion
protected:
	_bool m_bIsCloned = { false };
};
NS_END
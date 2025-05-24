#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CJson_Loader final : public CBase
{
#pragma region Constructor/Destructor
private:
	CJson_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CJson_Loader() = default;
public:
	static CJson_Loader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
#pragma endregion

public:
	HRESULT Load_Prototypes(const _char* pFilePath)
	
};
NS_END
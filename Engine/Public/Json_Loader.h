#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CJson_Loader final : public CBase
{
public:
	// 팩토리 함수의 시그니처 정의
	// params는 해당 클래스 초기화에 필요한 JSON "params" 객체입니다.
	using PrototypeCreatorFunc = function<CBase* (
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext
		)>;
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
private:
	map<string, PrototypeCreatorFunc> m_PrototypeCreators;

private:
	HRESULT Initialize();
	template<typename T>
	void RegisterBaseCreator(const string& className);
#pragma endregion

public:
	HRESULT Register_Creator_Function(const string& className, PrototypeCreatorFunc creatorFunc);
	HRESULT Load_Prototypes(_uint iLevelIndex, const _tchar* pFilePath);
	HRESULT Load_GameObjects(_uint iLevelIndex, const _tchar* pFilePath);
	
};
NS_END
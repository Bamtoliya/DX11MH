#pragma once

/* 엔진과 클라이언트 간의 링크의 역활을 수행한다. */
/* 엔진 내에 존재하는 유일한 싱글톤 클래스이다. */
/* 엔진 개발자가 클라개밫자에게 보여주고싶은 함수를 ... */
#include "Prototype_Manager.h"

BEGIN(Engine)
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDeviceOut, _Out_ ID3D11DeviceContext** ppContextOut, _Out_ class CSound_Manager** ppSoundOut);
	void Update_Engine(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	HRESULT Begin_Draw();
	HRESULT Draw();
	HRESULT End_Draw();

	_float Compute_Random_Normal();
	_float Compute_Random(_float fMin, _float fMax);

	HRESULT Resize(_uint iWinResizeX, _uint iWinResizeY);

#pragma region GRAPHIC_DEVIE
	ID3D11ShaderResourceView* Get_BackBufferSRV() const;
#pragma endregion


#pragma region LEVEL_MANAGER
public:
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region TIMER_MANAGER
	_float Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT Add_Timer(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region PHYSICS_SYSTEM

#pragma endregion

#pragma region PICKING_SYSTEM

#pragma endregion

//
//#pragma region PICKING
//	void Transform_Picking_ToLocalSpace(const _float4x4& WorldMatrixInverse);
//	_bool Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
//	_bool Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
//#pragma endregion

#pragma region Imgui
	void SetEditor(_bool b) { m_bIsEditor = b; }
	HRESULT Add_ImGui(const _wstring& strKey, class CMyImGui* pMyImGui);
#pragma endregion


private:
#pragma region Device
	class CInput_Device*	m_pInput_Device = { nullptr };
	class CGraphic_Device*	m_pGraphic_Device = { nullptr };
#pragma endregion

#pragma region System
	class CPhysics_System* m_pPhysics_System = { nullptr };
	class CPicking_System* m_pPicking_System = { nullptr };
#pragma endregion

#pragma region Manager
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CMyImGui_Manager*		m_pMyImGui_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
#pragma endregion

#pragma region 
	class CRenderer*	m_pRenderer = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };
#pragma endregion

#pragma region Boolean
	_bool	m_bIsEditor = { true };
#pragma endregion

public:
	void Release_Engine();
	virtual void Free() override;
};

END
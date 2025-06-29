#pragma once

/* 엔진과 클라이언트 간의 링크의 역활을 수행한다. */
/* 엔진 내에 존재하는 유일한 싱글톤 클래스이다. */
/* 엔진 개발자가 클라개밫자에게 보여주고싶은 함수를 ... */
#include "Prototype_Manager.h"

NS_BEGIN(Engine)

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
	ID3D11RenderTargetView* Get_RTV() const;
	ID3D11DepthStencilView* Get_DSV() const;
	ID3D11Texture2D* Get_BackBufferTexture() const;
	ID3D11ShaderResourceView* Get_BackBufferSRV() const;
#pragma endregion


#pragma region PHYSICS_SYSTEM
	PxPhysics* Get_Physics();
	PxScene* Get_PhysicsScene(_uint iIndex = 0);
	PxScene* Create_Scene();
	const vector<DEBUG_LINE>& Get_DebugLines();
	const vector<DEBUG_TRIANGLE>& Get_DebugTriangles();
	const _bool	Get_IsDebugDraw();

#pragma endregion


#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(DIM eMouse);	
	_long	Get_DIMouseMove(DIMM eMouseState);
	_bool	Key_Down(_ubyte eKeyID);
	_bool	Key_Up(_ubyte eKeyID);
	_bool	Key_Pressing(_ubyte eKeyID);
	_bool	Any_KeyDown();
	_bool	Mouse_Down(DIM eMouseKeyID);
	_bool	Mouse_Up(DIM eMouseKeyID);
	_bool	Mouse_Pressing(DIM eMouseKeyID);
#pragma endregion

public:
#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevelIndex();
	class CLevel* Get_CurrentLevel();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype, simdjson::ondemand::object& jsonData);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, simdjson::ondemand::object& jsonData);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CGameObject* pGameObject = nullptr, void* pArg = nullptr);
	CBase* Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CGameObject* pGameObject, simdjson::ondemand::object& jsonData);

	void Get_TexturePrototypeList(_uint iLevelIndex, vector<wstring>& outList);
	CComponent* Find_Prototype(PROTOTYPE eType, _uint iLevelIndex, const wstring& strPrototypeTag);


	HRESULT Save_Prototypes(json& data);
	HRESULT Save_Prototypes(_uint iLevelIndex, json& outJson);
	HRESULT Render_PrototypeManagerImGui(_uint iLevelIndex, const _wstring& PrototypeType, _wstring& strSelectedTag);
#pragma endregion

#pragma region TARGET_MANAGER
	//Render Target
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);
	class CRenderTarget* Get_RenderTarget(const _wstring& strTargetTag) const;
	map<const _wstring, class CRenderTarget*> Get_RenderTargetMap() const;
	HRESULT Clear_RenderTarget(const _wstring& strTargetTag, _float4* pClearColor = nullptr);
	//MRT
	HRESULT Add_MRT(const _wstring& strMRTTag, const std::vector<_wstring>& pTargetTags);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV); // 메인 깊이 버퍼 사용
	HRESULT End_MRT(ID3D11RenderTargetView* pBackBufferRTV, ID3D11DepthStencilView* pMainDSV, const D3D11_VIEWPORT& vp);
	HRESULT Clear_MRT(const _wstring& strMRTTag);
	ID3D11ShaderResourceView* Get_SRV(const _wstring& strTargetTag) const;

	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pContantName);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

	HRESULT Copy_ToBackBuffer(const _wstring& strSourceRenderTargetTag);
#pragma endregion

#pragma region CAMERA_MANAGER
	void Add_Camera(class CCamera* pCamera);
	void Remove_Camera(class CCamera* pCamera);
	void Remove_Camera(_uint iIndex);
	void Set_Camera(class CCamera* pCamera);
	void Set_Camera(_uint iIndex);
	void Change_Camera(class CCamera* pCamera);
	void Change_Camera(_uint iIndex);
	void Clear_CameraManager();
#pragma endregion


#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, simdjson::ondemand::object& jsonData);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, _uint iGameObjectID, void* pArg = nullptr);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject, simdjson::ondemand::object& jsonData);
	
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, _uint iPartID, const _wstring& strComponentTag, _uint iIndex = 0);	
	class CGameObject* Get_GameObjectById(_uint iId);
	class CGameObject* Get_GameObjectByName(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strGameObjectName);
	class CGameObject* Get_GameObjectByName(const _wstring& strGameObjectName);

	HRESULT Move_GameObjectToLayer(_uint iObjectID, _uint iTargetLevelIndex, const _wstring& strTargetLayerTag);
	HRESULT Remove_GameObjectFromLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	HRESULT Remove_GameObjectFromLayer(_uint iLevelIndex, CGameObject* pGameObject); // GameObject의 현재 레이어를 찾아 제거

	HRESULT Object_Manager_RenderImGui();
	HRESULT Render_ImGuiLayerList(_uint iLevelIndex, _char* szLayerBuffer, _uint& iGameObjectID, _uint iBufferSize);

	HRESULT Save_GameObjects(json& outJson);
	HRESULT Save_GameObjects(_uint iLevelIndex, json& outJson);

	HRESULT Clear_GameObjects(_uint iLevelIndex);
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

#pragma region TIMER_MANAGER
	_float Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT Add_Timer(const _wstring& strTimerTag);
	void Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region 	PIPELINE
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);
	const _float4x4* Get_Transform_Float4x4(D3DTS eState) const;
	const _float4x4* Get_Transform_Float4x4Inverse(D3DTS eState) const;
	_matrix Get_Transform_Matrix(D3DTS eState) const;
	_matrix Get_Transform_MatrixInverse(D3DTS eState) const;
	const _float4* Get_CamPosition() const;
#pragma endregion

#pragma region PICKING
	HRESULT ReadPickingTargetPixel(_int screenX, _int screenY, _float4* pPickedColor);
	HRESULT PickObjectByColor(_float4* pColor);
	HRESULT PickObjectById(_uint iId);
	class CGameObject* Get_PickedObject() const;
	void Set_PickedObject(CGameObject* pGameObject);
	void Clear_PickedObject();
//	void Transform_Picking_ToLocalSpace(const _float4x4& WorldMatrixInverse);
//	_bool Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
//	_bool Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_Light(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Add_Light(class CLight* pLight);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region MYIMGUI_MANAGER
	void SetEditor(_bool b) { m_bIsEditor = b; }
	HRESULT Add_ImGui(const _wstring& strKey, class CMyImGui* pMyImGui);
	HRESULT ToggleShowImGui(const _wstring& key);
	HRESULT ShowImGui(const _wstring& key);
	HRESULT HideImGui(const _wstring& key);
	HRESULT HideAll();
	HRESULT ShowAll();
	void Add_RenderObjectID(const _uint& iID);
	size_t Get_RenderObjectIDCount(const _uint& iID);
#pragma endregion

#pragma region JSON_LOADER
	HRESULT Register_Creator_Function(const string& className, function<CBase* (ID3D11Device*, ID3D11DeviceContext*)> creatorFunc);
	HRESULT Register_Clone_Function(const string& className, function<CBase* (ID3D11Device*, ID3D11DeviceContext*)> creatorFunc);
	HRESULT Load_Prototypes(_uint iLevelIndex, const _tchar* pFilePath, PrototypeLoadProgressCallback pCallback = nullptr, void* pUserData = nullptr, ID3D11DeviceContext* pDeferredContext = nullptr); // 콜백 인자 추가
	HRESULT Load_GameObject(_uint iLevelIndex, class CGameObject* pGameObject, simdjson::ondemand::object& jsonData);
	HRESULT Load_GameObjects(_uint iLevelIndex, const _tchar* pFilePath);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void Draw_Font(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), _float fScale = 1.f);
#pragma endregion
//
//#pragma region PICKING
//	void Transform_Picking_ToLocalSpace(const _float4x4& WorldMatrixInverse);
//	_bool Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
//	_bool Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
//#pragma endregion

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
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CCamera_Manager*		m_pCamera_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CMyImGui_Manager*		m_pMyImGui_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };


	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CJson_Loader*			m_pJson_Loader = { nullptr };	
	// class CPicking*				m_pPicking = { nullptr };
#pragma endregion
	


#pragma region Boolean
	_bool	m_bIsEditor = { true };
#pragma endregion
public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
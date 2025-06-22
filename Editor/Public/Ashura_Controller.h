#pragma once
#include "Controller.h"

NS_BEGIN(Engine)
class CState;
class CController;
NS_END

NS_BEGIN(Editor)
class CAshura_Controller : public CController
{
public:
	enum States {
		MOVE,
		COMBAT,
	};
public:
#pragma region Struct
	typedef struct tagAshuraStatus {
		//Default Movement
		_float3 vMoveDirection = {};
		pair<_float, _float> fMoveSpeed = { 10.f, 10.f };
		
		_float	fRotationSpeed = { 50.f };
		_float fSlopeLimitAngle = { 60.f };
		_float fCurSlope = { 0.f };
		_float fFriction = 0.99f;

		//Combat
		_float	fComboDelay = { 3.0f };
		_float	fHitDelay = { 0.5f };
		_bool	bCanHit = { true };
	}ASHURA_STATUS;
#pragma region Constructor/Destructor
private:
	CAshura_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAshura_Controller(const CAshura_Controller& Prototype);
	virtual ~CAshura_Controller() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CAshura_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Variables
private:
	ASHURA_STATUS m_tInfo;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;
	virtual HRESULT Render() override;
	virtual void Change_State(_uint iIndex) override;
	virtual void Change_State(_uint iStateIndex, _uint iAnimIndex, _float fPlaybackSpeed = 1.f, _bool bIsLoop = true, _float fBlendDuration = 0.5f);
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
public:
	ASHURA_STATUS& Get_Status() { return m_tInfo; }
};

NS_END
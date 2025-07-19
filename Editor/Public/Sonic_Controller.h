#pragma once
#include "Controller.h"

NS_BEGIN(Engine)
class CState;
class CController;
class CSpline_Component;
NS_END

NS_BEGIN(Editor)
class CSonic_Controller : public CController
{
public:
	enum States {
		MOVE,
		COMBAT,
		AIR
	};
public:
#pragma region Struct
	typedef struct tagSonicStatus {
		//Default Movement
		_float3 vMoveDirection = {};
		pair<_float, _float> fMoveSpeed = { 10.f, 10.f };
		_float	fMaxBoostSpeed = { 15.f };
		_float	fMoveAccelration = { 1.f };
		_float	fBoostAccelration = { 5.f };
		_float	fRotationSpeed = { 50.f };
		_bool	bOnGround = { false };
		_bool	bCanJump = { true };
		_bool	bCanDoubleJump = { false };
		_bool	bSliding = { false };
		pair<_float, _float> fBoostGauge = { 100.f, 100.f };
		_float fSlopeLimitAngle = { 60.f };
		_float fCurSlope = { 0.f };
		_float fFriction = 0.95f;
		_float fJumpForce = { 10.f };
		pair<_float, _float> fVerticalVelocity = { 0.f, 15.f };
		_bool bIsAir = { false };

		//Rail
		_bool bIsForward = { false };
		_bool bIsOnSpline = { false };
		_float fSplineParameter = { 0.f };

		//Spring
		_bool bIsSpring = { false };

		//Combat
		_bool bIsHomingAttackStarted = { false };
		pair<_float, _float>	fAtkDamage = { 4.f, 6.f };
		_float	fComboDelay = { 3.0f };
		_float	fHitDelay = { 0.5f };
		_bool	bCanHit = { true };
		pair<_uint, _uint> iCombos = { 0, 5 };	
		_bool	bComboInputBuffered = false;
		_float	fComboInputBufferTimer = 0.f;
		const _float fComboInputBufferDuration = 0.3f;

		//Status
		pair<_uint, _uint> iRings = { 0, 400 };

		//Detection
		_float fDetectionRange = { 10.f };
	}SONIC_STATUS;
#pragma endregion

#pragma region Constructor/Destructor
private:
	CSonic_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSonic_Controller(const CSonic_Controller& Prototype);
	virtual ~CSonic_Controller() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CSonic_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Variables
private:
	SONIC_STATUS m_tInfo;
	CSpline_Component* m_pCurrentSpline = { nullptr };
	CGameObject* m_pTarget = nullptr; // 새로운 멤버 변수 추가

#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;
	virtual HRESULT Render() override;
	virtual void Change_State(_uint iIndex) override;
	virtual void Change_State(_uint iStateIndex, _uint iAnimIndex, _float fPlaybackSpeed = 1.f, _bool bIsLoop = true, _float fBlendDuration = 0.5f);
	CGameObject* Get_LockOnTarget() const { return m_pTarget; }
private:
	void Update_Targeting(_float fTimeDelta);
	CGameObject* FindBestTarget();	
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;

	//Combo/Combat
public:
	void BufferComboInput();
	_bool ConsumeComboInput();
	void ClearComboInputBuffer();
	void UpdateInputBuffer(_float fTimeDelta);

#pragma region Interaction
public:
	void HomingAttack();
	void ReboundFromAttack();
	//Spline
	void Register_Spline(CSpline_Component* pSpline) { m_pCurrentSpline = pSpline; }
	void Unregister_Spline() { m_pCurrentSpline = nullptr; }
	CSpline_Component* Get_CurrentSpline() const { return m_pCurrentSpline; }

	void Spring(_fvector vPushDirection);
	void DashPanel(_fvector vDashDirection);
	void DashRing(_fvector vDashDirection);

	void Begin_Pulley();
	void Begin_Dashboard();
#pragma endregion

#pragma region Ring
public:
	void Add_Ring(_uint iNumber);
	void Subtract_Ring(_uint iNumber);
	_uint Get_Current_Ring() { return m_tInfo.iRings.first; }
#pragma endregion

public:
	SONIC_STATUS& Get_Status() { return m_tInfo; }

};
NS_END
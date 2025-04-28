#pragma once
#include "Base.h"
BEGIN(Engine)
class CPhysics_System : public CBase
{
#pragma region Constructor/Destructor
private:
	CPhysics_System();
	virtual ~CPhysics_System() = default;
public:
	static CPhysics_System* Create(HINSTANCE hInstance, HWND hWnd);
	void Free();
#pragma endregion
#pragma region Intialize
public:
	virtual HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
#pragma endregion
public:

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:

};

END
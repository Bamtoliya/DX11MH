#pragma once
#include "Component.h"
BEGIN(Engine)
class CSound_Manager;
class CSound_Event;
class ENGINE_DLL CSound_Source final : public CComponent
{
#pragma region Constructor/Destructor
private:
	CSound_Source(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager);
	CSound_Source(const CSound_Source& Prototype);
	virtual ~CSound_Source() = default;
public:
	static	CSound_Source* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSound_Manager* pSound_Manager);
	CSound_Source* Clone(void* pArg);
	virtual void	Free() override;
#pragma endregion

#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
#pragma endregion

#pragma region Loops
public:
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

#pragma endregion

public:
	_float			Get_Global_Parameter(const string& name);
	void			Set_Global_Parameter(const string& name, _float value);
	CSound_Event*	Play_Event(_wstring strEvent, void* pArg = nullptr);
	void			Stop_All_Event(bool allowFadeOut = true);
private:
	void			Clear();
	
private:
	CSound_Manager*	m_pSound_Manager;
	vector<CSound_Event*>	m_Events2D;
	vector<CSound_Event*>	m_Events3D;

};

END;
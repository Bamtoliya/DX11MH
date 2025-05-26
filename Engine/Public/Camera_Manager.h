
#include "Base.h"
NS_BEGIN(Engine)
class CCamera;
class CCamera_Manager : public CBase
{
#pragma region Constructor/Destructor
private:
	CCamera_Manager();
	virtual  ~CCamera_Manager() = default;
public:
	static CCamera_Manager* Create();
	virtual void Free() override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion
public:
	void Add_Camera(CCamera* pCamera);
	void Remove_Camera(CCamera* pCamera);
	void Remove_Camera(_uint iIndex);
	void Set_Camera(CCamera* pCamera);
	void Set_Camera(_uint iIndex);
	void Change_Camera(CCamera* pCamera);
	void Change_Camera(_uint iIndex);
	void InActiveCameras();
	void Clear();

public:
	list<CCamera*>	m_Cameras = {};
	_uint			m_iCurIndex = {};
};
NS_END
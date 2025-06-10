#pragma once 

#define NS_BEGIN(NAMESPACE) namespace NAMESPACE { 
#define NS_END }

#define ENUM_CLASS(ENUM) static_cast<unsigned int>(ENUM)

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define MSG_BOX(message)								\
::MessageBox(nullptr, TEXT(message), L"error", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned int Destroy_Instance();				\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned int ClassName::Destroy_Instance()				\
{														\
	unsigned int iRefCnt = {};							\
	if(nullptr != m_pInstance)							\
	{													\
		iRefCnt = m_pInstance->Release();				\
		if(0 == iRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return iRefCnt;										\
}

#define BANKPATH ""

#define SOLUTIONPATH "../../"
#define ASSETPATH "G:/Computers/Desktop/Assets/"
#define SAVEPATH SOLUTIONPATH "Saves/"

#define KEY_DOWN(eKey) m_pGameInstance->Key_Down(eKey)
#define KEY_UP(eKey) m_pGameInstance->Key_Up(eKey)
#define KEY_PRESSING(eKey) m_pGameInstance->Key_Pressing(eKey)
#define ANY_KEY	m_pGameInstance->Any_KeyDown()

#define MOUSE_DOWN(eKey) m_pGameInstance->Mouse_Down(eKey)
#define MOUSE_UP(eKey) m_pGameInstance->Mouse_Up(eKey)
#define MOUSE_PRESSING(eKey) m_pGameInstance->Mouse_Pressing(eKey)
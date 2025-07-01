#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
NS_BEGIN(Editor)
class CToolBar : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CToolBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CToolBar() = default;
public:
	static CToolBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
#pragma endregion
public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	void	IntializeFileDialogCallBack();

#pragma region Convert fbx Model to Binary
	HRESULT Convert_Model(const _tchar* pFilePath);
	HRESULT Convert_Models_InFolder(const _tchar* pFolderPath);
	HRESULT Write_BoneNode(ofstream& ofs, aiNode* pAiNode, _int iParentBoneIndex);
#pragma endregion

	
	HRESULT Convert_Texture(const _tchar* pFilePath);

	HRESULT Save_Prototypes(_uint iLevelIndex, const _tchar* pFilePath);
	HRESULT Load_Prototypes(_uint iLevelIndex, const _tchar* pFilePath);

	HRESULT Save_GameObjects(_uint iLevelIndex, const _tchar* pFilePath);
	HRESULT Load_GameObjects(_uint iLevelIndex, const _tchar* pFilePath);

private:
	_bool m_bColliderMesh = { false };
	_bool m_bConvertAnim = { false };
	_bool m_bMergeLoad = { false };
	LEVEL m_eLevelIndex = { LEVEL::STATIC };
};
NS_END
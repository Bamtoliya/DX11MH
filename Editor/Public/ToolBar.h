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

	HRESULT Save_Level();
	HRESULT Save_Scene();
	HRESULT Save_Map();
	HRESULT Save_Prototypes(const _tchar* pFilePath);

	HRESULT Import_Level();
	HRESULT Import_Scene();
	HRESULT Import_Map();
	HRESULT Import_Prototype();

#pragma region Convert fbx Model to Binary
	HRESULT Convert_Model(const _tchar* pFilePath);
	HRESULT Write_BoneNode(ofstream& ofs, aiNode* pAiNode, _int iParentBoneIndex);
#pragma endregion

	
	HRESULT Convert_Texture(const _tchar* pFilePath);

	HRESULT Load_Level();
	HRESULT Load_Scene();
	HRESULT Load_Map();
	HRESULT Load_Prototypes();

private:
	bool m_bConvertAnim = { false };
};
NS_END
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
	HRESULT Save_Prefab();

	HRESULT Import_Level();
	HRESULT Import_Scene();
	HRESULT Import_Map();
	HRESULT Import_Prefab();

	HRESULT Convert_Model(const _tchar* pFilePath);
	HRESULT Write_Mesh(ofstream& ofs, const _uint iNumMesh);
	HRESULT Write_Material(ofstream& ofs, const _uint iNumMaterial);
	HRESULT Write_Animation(ofstream& ofs, const _uint iNumAnimation);
	HRESULT Write_BoneNode(ofstream& ofs, aiNode* pAiNode, _int iParentBoneIndex);
	HRESULT Read_BoneNode(ifstream& ifs, class CModel* pModel = nullptr);
	HRESULT Convert_Texture(const _tchar* pFilePath);

	HRESULT Load_Level();
	HRESULT Load_Scene();
	HRESULT Load_Map();
	HRESULT Load_Prefab();

private:
	bool m_bConvertAnim = { false };
};
NS_END
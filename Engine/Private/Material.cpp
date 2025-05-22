#include "Material.h"
#include "Shader.h"

#pragma region Constructor/Destructor
CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, ifstream& ifs)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pModelFilePath, ifs)))
    {
        MSG_BOX("Failed to Created in Binary: CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
    for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        _uint       iNumSRVs = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

        m_SRVs[i].reserve(iNumSRVs);

        for (size_t j = 0; j < iNumSRVs; j++)
        {
            aiString     strTexturePath;

            if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), (_uint)j, &strTexturePath)))
                return E_FAIL;

            _char       szFullPath[MAX_PATH] = {};
            _char       szDrive[MAX_PATH] = {};
            _char       szDir[MAX_PATH] = {};
            _char       szFileName[MAX_PATH] = {};
            _char       szExt[MAX_PATH] = {};

            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDir);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            _tchar      szTextureFilePath[MAX_PATH] = {};

            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szTextureFilePath, MAX_PATH);

            HRESULT         hr = { };
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (false == strcmp(szExt, ".dds"))
                hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
            else
                hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_SRVs[i].push_back(pSRV);
        }
    }

    return S_OK;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, ifstream& ifs)
{
    for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
    {

        _uint       iNumSRVs = {};
        ifs.read(reinterpret_cast<char*>(&iNumSRVs), sizeof(_uint));

        m_SRVs[i].reserve(iNumSRVs);

        for (size_t j = 0; j < iNumSRVs; j++)
        {
            _uint iTexturePathLength = {};

            ifs.read(reinterpret_cast<char*>(&iTexturePathLength), sizeof(_uint));
            _char* szTexturePath = new _char[iTexturePathLength + 1];
            ifs.read(reinterpret_cast<char*>(&szTexturePath[0]), sizeof(_char) * iTexturePathLength);
            szTexturePath[iTexturePathLength] = '\0';

            _char       szFullPath[MAX_PATH] = {};
            _char       szDrive[MAX_PATH] = {};
            _char       szDir[MAX_PATH] = {};
            _char       szFileName[MAX_PATH] = {};
            _char       szExt[MAX_PATH] = {};

            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(szTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDir);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            _tchar      szTextureFilePath[MAX_PATH] = {};

            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szTextureFilePath, MAX_PATH);

            HRESULT         hr = { };
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (false == strcmp(szExt, ".dds"))
                hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
            else
                hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_SRVs[i].push_back(pSRV);
            Safe_Delete_Array(szTexturePath);
        }
    }

    return S_OK;
}

void CMaterial::Free()
{
    __super::Free();

    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        for (auto& pSRV : m_SRVs[i])
            Safe_Release(pSRV);
        m_SRVs[i].clear();
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
#pragma endregion

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
    if (iTextureIndex >= m_SRVs[eType].size())
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);    
}

HRESULT CMaterial::RenderImGui()
{
    return S_OK;
}
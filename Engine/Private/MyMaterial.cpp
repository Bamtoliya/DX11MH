#include "MyMaterial.h"
//#include "Shader.h"

CMyMaterial::CMyMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :   CComponent { pDevice, pContext }
{
}

CMyMaterial::CMyMaterial(const CMyMaterial& Prototype)
    : CComponent    { Prototype }
    , m_tMaterial   { Prototype.m_tMaterial }
{
}

HRESULT CMyMaterial::Initialize_Prototype(const _tchar* pMaterialFilePath)
{
    ifstream f(pMaterialFilePath);
    json data = json::parse(f);
    m_tMaterial.Diffuse =   { data["Diffuse"]["x"], data["Diffuse"]["y"], data["Diffuse"]["z"], data["Diffuse"]["w"] };
    m_tMaterial.Ambient =   { data["Ambient"]["x"], data["Ambient"]["y"], data["Ambient"]["z"], data["Ambient"]["w"] };
    m_tMaterial.Specular =  { data["Specular"]["x"], data["Specular"]["y"], data["Specular"]["z"], data["Specular"]["w"] };
    m_tMaterial.Power =     { data["Power"] };
    m_tMaterial.Emissive =  { data["Emissive"]["x"], data["Emissive"]["y"], data["Emissive"]["z"], data["Emissive"]["w"] };
    return S_OK;
}

HRESULT CMyMaterial::Initialize(void* pArg)
{
    if (pArg)
    {
        MATERIAL_DESC desc = *reinterpret_cast<MATERIAL_DESC*>(pArg);
        ifstream f(desc.MaterialFilePath);
        json data = json::parse(f);
        m_tMaterial.Diffuse = { data["Diffuse"]["x"], data["Diffuse"]["y"], data["Diffuse"]["z"], data["Diffuse"]["w"] };
        m_tMaterial.Ambient = { data["Ambient"]["x"], data["Ambient"]["y"], data["Ambient"]["z"], data["Ambient"]["w"] };
        m_tMaterial.Specular = { data["Specular"]["x"], data["Specular"]["y"], data["Specular"]["z"], data["Specular"]["w"] };
        m_tMaterial.Power = { data["Power"] };
        m_tMaterial.Emissive = { data["Emissive"]["x"], data["Emissive"]["y"], data["Emissive"]["z"], data["Emissive"]["w"] };
    }
    return S_OK;
}

HRESULT CMyMaterial::Bind_Resource()
{
   /* memcpy(&material.Diffuse,   &m_tMaterial.Diffuse,   sizeof(_float4));
    memcpy(&material.Ambient,   &m_tMaterial.Ambient,   sizeof(_float4));
    memcpy(&material.Specular,  &m_tMaterial.Specular,  sizeof(_float4));
    memcpy(&material.Emissive,  &m_tMaterial.Emissive,  sizeof(_float4));
    memcpy(&material.Power,     &m_tMaterial.Power,     sizeof(_float));
    if (FAILED(m_pGraphic_Device->SetMaterial(&material)))
        return E_FAIL;*/
    return S_OK;
}

HRESULT CMyMaterial::Bind_Material(CShader* pShader)
{
    /*pShader->Bind_Value("g_Material", &m_tMaterial, sizeof(MATERIAL));*/
    return S_OK;
}

CMyMaterial* CMyMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pMaterialFilePath)
{
    CMyMaterial* pInstance = new CMyMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pMaterialFilePath)))
    {
        MSG_BOX("Failed to Created : CMyMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMyMaterial::Clone(void* pArg)
{
    CMyMaterial* pInstance = new CMyMaterial(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMyMaterial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMyMaterial::Free()
{
    __super::Free();
}
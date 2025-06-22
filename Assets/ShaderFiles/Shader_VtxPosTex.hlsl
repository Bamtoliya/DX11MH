#include "Shader_Struct.hlsli"
#include "Shader_Test.hlsl"

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    matrix matWV, matWVP;
    
    /* mul : 모든 행렬의 곱하기를 수행한다. /w연산을 수행하지 않는다. */
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;    

    vector vMtrlDiffuse;
    float2 finalUV = In.vTexcoord;
    
    if (g_bUseTextureRegion)
    {
        finalUV.x = g_TextureRegion.x + (g_TextureRegion.z - g_TextureRegion.x) * In.vTexcoord.x;
        finalUV.y = g_TextureRegion.y + (g_TextureRegion.w - g_TextureRegion.y) * In.vTexcoord.y;
    }
    if (g_bTiling)
    {
        vMtrlDiffuse = g_Texture.Sample(DefaultSampler, finalUV * g_vTilingScale + g_vTilingOffset);
    }
    else
    {
        vMtrlDiffuse = g_Texture.Sample(DefaultSampler, finalUV);
    }
    
    Out.vColor = vMtrlDiffuse;
    float r = (float) ((objectId >> 16) & 0xFF) / 255.0f;
    float g = (float) ((objectId >> 8) & 0xFF) / 255.0f;
    float b = (float) (objectId & 0xFF) / 255.0f;
    if(Out.vColor.a <= 0.3f) 
        discard;
    Out.vPick = float4(r, g, b, 1.0f);
    Out.vFinal = Out.vColor;
    if (Out.vFinal.a <= 0.3f) 
        discard;
    return Out;    
}


technique11 DefaultTechnique
{
    pass Default/* 명암 + 스펙큘러 + 그림자 + ssao + 림라이트 */ 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();        
        PixelShader = compile ps_5_0 PS_MAIN();      
    }
}

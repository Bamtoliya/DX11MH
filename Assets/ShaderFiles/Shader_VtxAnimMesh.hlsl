#include "Shader_Struct.hlsli"
#include "Shader_Test.hlsl"
matrix g_BoneMatrices[512];

VS_NOR_OUT VS_MAIN(VS_ANIM_IN In)
{
    VS_NOR_OUT Out;    
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
        
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
        g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y + 
        g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z + 
        g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);   
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;
    
    /* mul : 모든 행렬의 곱하기를 수행한다. /w연산을 수행하지 않는다. */
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;    
    
    return Out;
}


PS_OUT PS_MAIN(PS_NOR_IN In)
{
    PS_OUT Out;    
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    float4 vShade = max(dot(normalize(g_vLightDir) * -1.f, In.vNormal), 0.f) +
        (g_vLightAmbient * g_vMtrlAmibient);
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * vShade;
    float r = (float) ((objectId >> 16) & 0xFF) / 255.0f;
    float g = (float) ((objectId >> 8) & 0xFF) / 255.0f;
    float b = (float) (objectId & 0xFF) / 255.0f;
    Out.vPick = float4(r, g, b, 1.0f);
    
    if(Out.vColor.a < 0.3f)
        discard;
    
    return Out;    
}



technique11 DefaultTechnique
{
   
    pass Default
    {
        VertexShader = compile vs_5_0 VS_MAIN();        
        PixelShader = compile ps_5_0 PS_MAIN();      
    }
   
}

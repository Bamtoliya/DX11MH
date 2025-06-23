// Shader_DebugLine.hlsl

Texture2D g_DiffuseTexture : register(t0); // 사용 안 함
SamplerState g_SamplerState : register(s0); // 사용 안 함

cbuffer GlobalMatrix : register(b0)
{
    matrix g_WorldMatrix; // PhysX 디버그 라인은 이미 월드 좌표계
    matrix g_ViewMatrix;
    matrix g_ProjMatrix;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR0;
};

struct PS_IN
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR0;
};

PS_IN VS_MAIN(VS_IN In)
{
    PS_IN Out = (PS_IN) 0;

    // 월드 행렬은 항등 행렬로 간주 (또는 셰이더에서 항등 행렬 사용)
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // g_WorldMatrix에 항등행렬 전달
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vColor = In.vColor;
    
    return Out;
}

float4 PS_MAIN(PS_IN In) : SV_Target
{
    return In.vColor;
}


technique11 DefaultTechnique
{
    pass Default /* 명암 + 스펙큘러 + 그림자 + ssao + 림라이트 */
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
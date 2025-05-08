#ifndef _SHADER_STRUCT_INCLUDED_
#define _SHADER_STRUCT_INCLUDED_
/* 상수테이블 ConstantTable */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
};


/* 정점의 기초적인 변환 (월드변환, 뷰, 투영변환) */ 
/* 정점의 구성 정보를 변형할 수 있다. */ 

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    /* SV_ : ShaderValue약자 */
    /* 내가 해야할 연산은 다 했으니 이제 니(장치)가 알아서 추가적인 연산을 해라. */     
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    vector vPick  : SV_Target1;
};
#endif
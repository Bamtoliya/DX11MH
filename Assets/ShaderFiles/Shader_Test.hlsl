/* 상수테이블 ConstantTable */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
uint objectId;
sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
};


uint g_oID;

/* 정점의 기초적인 변환 (월드변환, 뷰, 투영변환) */
/* 정점의 구성 정보를 변형할 수 있다. */

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    /* SV_ : ShaderValue약자 */
    /* 내가 해야할 연산은 다 했으니 이제 니(장치)가 알아서 추가적인 연산을 해라. */
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    matrix matWV, matWVP;

    /* mul : 모든 행렬의 곱하기를 수행한다. /w연산을 수행하지 않는다. */
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

    return Out;
}

//POSITION시멘틱이 붙은
//멤버변수에 대해서

/* W나누기 연산을 수행한다. */
/* 뷰포트로 변환한다. */
/* 모든 성분에 대해서 래스터라이즈. -> 픽셀을 생성한다. */


struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    /*vector vImGui : SV_TARGET1;
    vector vPickColor : SV_TARGET2;*/
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float r = (float) ((objectId >> 16) & 0xFF) / 255.0f;
    float g = (float) ((objectId >> 8) & 0xFF) / 255.0f;
    float b = (float) (objectId & 0xFF) / 255.0f;
    Out.vColor = float4(r, g, b, 1.0f);
    return Out;
}

technique11 DefaultTechnique
{
    /* 패스를 생성하는 기준을 뭘로? */
    /* 같은 모델을 그릴때 각기 다른 렌더스테이츠를 먹여야하거나.
    완전히 다른 쉐이딩 기법을 적용해야하거나 */

    /* 각 쉐이더를 어떤 버젼으로 빌드할건지.  */
    /* 어떤 쉐이더를 사용할건지? */
    /* 진입점 함수 결정 */
    /* 렌더스테이츠에 대한 설정*/
    pass Default/* 명암 + 스펙큘러 + 그림자 + ssao + 림라이트 */
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    //pass Disstortion/* 왜곡 1*/ 
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();
    //    PixelShader = compile ps_5_0 PS_MAIN_Diss();
    //}
    //pass Blend/* 반투명 */
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    PixelShader = compile ps_5_0 PS_MAIN_Blend();
    //}
}

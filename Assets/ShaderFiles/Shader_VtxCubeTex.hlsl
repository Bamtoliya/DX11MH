#include "Shader_Struct.hlsl"
#include "Shader_Test.hlsl"

VS_CUBE_OUT VS_MAIN(VS_CUBE_IN In)
{
    VS_CUBE_OUT Out;
    
    matrix matWV, matWVP;
    
    /* mul : 모든 행렬의 곱하기를 수행한다. /w연산을 수행하지 않는다. */
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;    
    
    return Out;
}

//POSITION시멘틱이 붙은
//멤버변수에 대해서

/* W나누기 연산을 수행한다. */
/* 뷰포트로 변환한다. */
/* 모든 성분에 대해서 래스터라이즈. -> 픽셀을 생성한다. */


PS_OUT PS_MAIN(PS_CUBE_IN In)
{
    PS_OUT Out;    
    
    Out.vColor = g_TextureCube.Sample(DefaultSampler, In.vTexcoord);
    float r = (float) ((objectId >> 16) & 0xFF) / 255.0f;
    float g = (float) ((objectId >> 8) & 0xFF) / 255.0f;
    float b = (float) (objectId & 0xFF) / 255.0f;
    Out.vPick = float4(r, g, b, 1.0f);
    
    return Out;    
}


RasterizerState RS_Default
{
    CullMode = Back; // D3DCULL_CCW (FrontCounterClockwise가 false일 때)
};

RasterizerState RS_Sky
{
    CullMode = None; // D3DRS_CULLMODE, D3DCULL_NONE
                     // 또는 CullMode = Front; (큐브 안에서 볼 때)
    // FillMode = Solid; (기본값)
    // FrontCounterClockwise = false; (기본값)
};

// 스카이박스 렌더링을 위한 DepthStencilState
DepthStencilState DS_Sky
{
    DepthEnable = true;     // D3DRS_ZENABLE, TRUE (또는 FALSE)
                            // Skybox.fx의 VS에서 Out.PosH.z = Out.PosH.w; 트릭을 사용한다면
                            // DepthEnable = true로 하고, DepthFunc = LESS_EQUAL로 설정합니다.
                            // 만약 깊이 테스트를 완전히 무시하고 싶다면 DepthEnable = false;
    DepthWriteMask = ZERO;  // D3DRS_ZWRITEENABLE, FALSE
    DepthFunc = LESS_EQUAL; // 또는 LESS (z = w 트릭 사용 시)
                            // DepthEnable이 false면 DepthFunc은 의미 없음
    
    // Stencil 관련 설정은 필요시 추가
    // StencilEnable = false;
};


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
        SetRasterizerState( RS_Sky );
        SetDepthStencilState( DS_Sky, 0 );
        VertexShader = compile vs_5_0 VS_MAIN();        
        PixelShader = compile ps_5_0 PS_MAIN();      
    }

    pass Picking /* 명암 + 스펙큘러 + 그림자 + ssao + 림라이트 */
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_PICKING_CUBE();
    }
}

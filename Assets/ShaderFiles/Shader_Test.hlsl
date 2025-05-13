#include "Shader_Struct.hlsl"
uint objectId;

PS_OUT PS_PICKING(PS_IN In)
{
    PS_OUT Out;
    float r = (float) ((objectId >> 16) & 0xFF) / 255.0f;
    float g = (float) ((objectId >> 8) & 0xFF) / 255.0f;
    float b = (float) (objectId & 0xFF) / 255.0f;
    Out.vColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Out.vPick = float4(r, g, b, 1.0f);
    
    return Out;
}

PS_OUT PS_PICKING_CUBE(PS_CUBE_IN In)
{
    PS_OUT Out;
    float r = (float) ((objectId >> 16) & 0xFF) / 255.0f;
    float g = (float) ((objectId >> 8) & 0xFF) / 255.0f;
    float b = (float) (objectId & 0xFF) / 255.0f;
    Out.vColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    Out.vPick = float4(r, g, b, 1.0f);
    
    return Out;
}
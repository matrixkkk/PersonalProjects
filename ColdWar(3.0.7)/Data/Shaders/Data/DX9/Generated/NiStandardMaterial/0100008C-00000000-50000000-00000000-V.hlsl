#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\0100008C-00000000-50000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x4 g_World;
float4x4 g_ViewProj;
//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------

/*

    This fragment is responsible for applying the view projection transform
    to the input position. Additionally, this fragment applies the world 
    transform to the input position. 
    
*/

void TransformPosition(float3 Position,
    float4x4 World,
    out float4 WorldPos)
{

    // Transform the position into world space for lighting, and projected 
    // space for display
    WorldPos = mul( float4(Position, 1.0f), World );
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for applying the view projection transform
    to the input world position.
    
*/

void ProjectPositionWorldToProj(float4 WorldPosition,
    float4x4 ViewProjection,
    out float4 ProjPos)
{

    ProjPos = mul(WorldPosition, ViewProjection );
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for applying the world transform to the
    normal, binormal, and tangent.
    
*/

void TransformNBT(float3 Normal,
    float3 Binormal,
    float3 Tangent,
    float4x4 World,
    out float3 WorldNrm,
    out float3 WorldBinormal,
    out float3 WorldTangent)
{

    // Transform the normal into world space for lighting
    WorldNrm      = mul( Normal, (float3x3)World );
    WorldBinormal = mul( Binormal, (float3x3)World );
    WorldTangent  = mul( Tangent, (float3x3)World );
    
    // Should not need to normalize here since we will normalize in the pixel 
    // shader due to linear interpolation across triangle not perserving
    // normality.
    
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Input:
//---------------------------------------------------------------------------

struct Input
{
    float3 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UVSet0 : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Binormal : BINORMAL0;

};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------

struct Output
{
    float4 PosProjected : POSITION0;
    float4 WorldPos : TEXCOORD0;
    float3 WorldNormal : TEXCOORD1;
    float3 WorldBinormal : TEXCOORD2;
    float3 WorldTangent : TEXCOORD3;
    float2 UVSet0 : TEXCOORD4;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    TransformPosition(In.Position, g_World, Out.WorldPos);

	// Function call #1
    ProjectPositionWorldToProj(Out.WorldPos, g_ViewProj, Out.PosProjected);

	// Function call #2
    TransformNBT(In.Normal, In.Binormal, In.Tangent, g_World, Out.WorldNormal, 
        Out.WorldBinormal, Out.WorldTangent);

    Out.UVSet0 = In.UVSet0;
    return Out;
}


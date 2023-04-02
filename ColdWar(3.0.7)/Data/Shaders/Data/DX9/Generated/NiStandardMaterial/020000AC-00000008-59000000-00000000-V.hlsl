#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\020000AC-00000008-59000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x4 g_World;
float4x4 g_ViewProj;
float4 g_EyePos;
float4x4 UVSet1_TexTransform;
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

    This fragment is responsible for applying a transform to the input set
    of texture coordinates.
    
*/

void TexTransformApply(float2 TexCoord,
    float4x4 TexTransform,
    out float2 TexCoordOut)
{

    
    TexCoordOut = mul(float4(TexCoord.x, TexCoord.y, 0.0, 1.0), TexTransform);
    
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
/*

    This fragment is responsible for calculating the camera view vector.
    
*/

void CalculateViewVector(float4 WorldPos,
    float3 CameraPos,
    out float3 WorldViewVector)
{

    WorldViewVector = CameraPos - WorldPos;
    WorldViewVector = normalize(WorldViewVector);
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for transforming a vector from world space
    to tangent space.
    
*/

void WorldToTangent(float3 VectorIn,
    float3 WorldNormalIn,
    float3 WorldBinormalIn,
    float3 WorldTangentIn,
    out float3 VectorOut)
{

    float3x3 xForm = float3x3(WorldTangentIn, WorldBinormalIn, WorldNormalIn);
    VectorOut = mul(xForm, VectorIn.xyz);
    
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
    float4 VertexColors : COLOR0;

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
    float3 TangentSpaceView : TEXCOORD4;
    float2 UVSet0 : TEXCOORD5;
    float2 UVSet1 : TEXCOORD6;
    float4 VertexColors : COLOR0;

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
    TexTransformApply(In.UVSet0, UVSet1_TexTransform, Out.UVSet1);

	// Function call #3
    TransformNBT(In.Normal, In.Binormal, In.Tangent, g_World, Out.WorldNormal, 
        Out.WorldBinormal, Out.WorldTangent);

	// Function call #4
    float3 WorldViewVector_CallOut4;
    CalculateViewVector(Out.WorldPos, g_EyePos, WorldViewVector_CallOut4);

	// Function call #5
    WorldToTangent(WorldViewVector_CallOut4, Out.WorldNormal, Out.WorldBinormal, 
        Out.WorldTangent, Out.TangentSpaceView);

    Out.VertexColors = In.VertexColors;
    Out.UVSet0 = In.UVSet0;
    return Out;
}


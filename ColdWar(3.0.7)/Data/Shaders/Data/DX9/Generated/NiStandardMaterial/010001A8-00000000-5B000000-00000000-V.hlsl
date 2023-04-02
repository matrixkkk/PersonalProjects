#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\010001A8-00000000-5B000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x4 g_World;
float4x4 g_ViewProj;
float4 g_EyePos;
float4 g_MaterialSpecular;
float4 g_MaterialPower;
float4 g_MaterialEmissive;
float4 g_AmbientLight;
//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------

/*

    Separate a float4 into a float3 and a float.   
    
*/

void SplitColorAndOpacity(float4 ColorAndOpacity,
    out float3 Color,
    out float Opacity)
{

    Color.rgb = ColorAndOpacity.rgb;
    Opacity = ColorAndOpacity.a;
    
}
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

    This fragment is responsible for computing the coefficients for the 
    following equations:
    
    Kdiffuse = MatEmissive + 
        MatAmbient * Summation(0...N){LightAmbientContribution[N]} + 
        MatDiffuse * Summation(0..N){LightDiffuseContribution[N]}
        
    Kspecular = MatSpecular * Summation(0..N){LightSpecularContribution[N]}
    
    
*/

void ComputeShadingCoefficients(float3 MatEmissive,
    float3 MatDiffuse,
    float3 MatAmbient,
    float3 MatSpecular,
    float3 LightSpecularAccum,
    float3 LightDiffuseAccum,
    float3 LightAmbientAccum,
    bool Saturate,
    out float3 Diffuse,
    out float3 Specular)
{

    Diffuse = MatEmissive + MatAmbient * LightAmbientAccum + 
        MatDiffuse * LightDiffuseAccum;
    Specular = MatSpecular * LightSpecularAccum;
    
    if (Saturate)
    {
        Diffuse = saturate(Diffuse);
        Specular = saturate(Specular);
    }
    
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

    This fragment is responsible for normalizing a float3.
    
*/

void NormalizeFloat3(float3 VectorIn,
    out float3 VectorOut)
{

    VectorOut = normalize(VectorIn);
    
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

    This fragment is responsible for computing the final RGBA color.
    
*/

void CompositeFinalRGBAColor(float3 FinalColor,
    float FinalOpacity,
    out float4 OutputColor)
{

    OutputColor.rgb = FinalColor.rgb;
    OutputColor.a = saturate(FinalOpacity);
    
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
    float3 WorldNormal : TEXCOORD0;
    float3 WorldBinormal : TEXCOORD1;
    float3 WorldTangent : TEXCOORD2;
    float3 TangentSpaceView : TEXCOORD3;
    float4 DiffuseAccum : TEXCOORD4;
    float3 SpecularAccum : TEXCOORD5;
    float2 UVSet0 : TEXCOORD6;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    float3 Color_CallOut0;
    float Opacity_CallOut0;
    SplitColorAndOpacity(In.VertexColors, Color_CallOut0, Opacity_CallOut0);

	// Function call #1
    float4 WorldPos_CallOut1;
    TransformPosition(In.Position, g_World, WorldPos_CallOut1);

	// Function call #2
    ProjectPositionWorldToProj(WorldPos_CallOut1, g_ViewProj, Out.PosProjected);

	// Function call #3
    float3 Diffuse_CallOut3;
    ComputeShadingCoefficients(g_MaterialEmissive, Color_CallOut0, 
        In.VertexColors, g_MaterialSpecular, float3(0.0, 0.0, 0.0), 
        float3(0.0, 0.0, 0.0), g_AmbientLight, bool(false), Diffuse_CallOut3, 
        Out.SpecularAccum);

	// Function call #4
    TransformNBT(In.Normal, In.Binormal, In.Tangent, g_World, Out.WorldNormal, 
        Out.WorldBinormal, Out.WorldTangent);

	// Function call #5
    float3 VectorOut_CallOut5;
    NormalizeFloat3(Out.WorldNormal, VectorOut_CallOut5);

	// Function call #6
    float3 WorldViewVector_CallOut6;
    CalculateViewVector(WorldPos_CallOut1, g_EyePos, WorldViewVector_CallOut6);

	// Function call #7
    CompositeFinalRGBAColor(Diffuse_CallOut3, Opacity_CallOut0, 
        Out.DiffuseAccum);

	// Function call #8
    WorldToTangent(WorldViewVector_CallOut6, Out.WorldNormal, Out.WorldBinormal, 
        Out.WorldTangent, Out.TangentSpaceView);

    Out.UVSet0 = In.UVSet0;
    return Out;
}


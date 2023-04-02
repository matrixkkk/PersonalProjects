#line 2 "e:\Á¹¾÷ ¿¬±¸\ColdWa(2.4)\Data\Shaders\Generated\NiStandardMaterial\00000400-00000000-42000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x4 g_World;
float4x4 g_View;
float4x4 g_Proj;
float4 g_MaterialEmissive;
float4 g_FogDensity;
float4 g_FogNearFar;
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
    to the input world position.
    
*/

void ProjectPositionWorldToViewToProj(float4 WorldPosition,
    float4x4 ViewTransform,
    float4x4 ProjTransform,
    out float4 ViewPos,
    out float4 ProjPos)
{

    ViewPos = mul(WorldPosition, ViewTransform );
    ProjPos = mul(ViewPos, ProjTransform );
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for handling fogging calculations.
    FogType can be one of 4 values:
    
        NONE   - 0
        EXP    - 1
        EXP2   - 2
        LINEAR - 3
    
*/

void CalculateFog(int FogType,
    float FogDensity,
    float4 ViewPosition,
    bool FogRange,
    float2 FogStartEnd,
    out float FogOut)
{

    float d;
    if (FogRange)
    {
        d = length(ViewPosition);
    }
    else
    {
        d = ViewPosition.z;
    }
    
    if (FogType == 0) // NONE
    {
        FogOut = 1.0;
    }
    else if (FogType == 1) // EXP
    {
        FogOut = 1.0 / exp( d * FogDensity);
    }
    else if (FogType == 2) // EXP2
    {
        FogOut = 1.0 / exp( pow( d * FogDensity, 2));
    }
    else if (FogType == 3) // LINEAR
    {
        FogOut = saturate( (FogStartEnd.y - d) / 
            (FogStartEnd.y - FogStartEnd.x));
    }
    
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
//---------------------------------------------------------------------------
// Input:
//---------------------------------------------------------------------------

struct Input
{
    float3 Position : POSITION0;

};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------

struct Output
{
    float4 PosProjected : POSITION0;
    float4 DiffuseAccum : TEXCOORD0;
    float FogOut : FOG0;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    float4 WorldPos_CallOut0;
    TransformPosition(In.Position, g_World, WorldPos_CallOut0);

	// Function call #1
    float3 Color_CallOut1;
    float Opacity_CallOut1;
    SplitColorAndOpacity(g_MaterialEmissive, Color_CallOut1, Opacity_CallOut1);

	// Function call #2
    float4 ViewPos_CallOut2;
    ProjectPositionWorldToViewToProj(WorldPos_CallOut0, g_View, g_Proj, 
        ViewPos_CallOut2, Out.PosProjected);

	// Function call #3
    CalculateFog(int(3), g_FogDensity, ViewPos_CallOut2, bool(true), 
        g_FogNearFar, Out.FogOut);

	// Function call #4
    float3 Diffuse_CallOut4;
    float3 Specular_CallOut4;
    ComputeShadingCoefficients(Color_CallOut1, float3(1.0, 1.0, 1.0), 
        float3(1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), float3(0.0, 0.0, 0.0), 
        float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), bool(false), 
        Diffuse_CallOut4, Specular_CallOut4);

	// Function call #5
    CompositeFinalRGBAColor(Diffuse_CallOut4, Opacity_CallOut1, 
        Out.DiffuseAccum);

    return Out;
}


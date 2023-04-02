#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\01000140-00000000-5B110000-00000000-V.hlsl"
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
float4 g_DirAmbient0;
float4 g_DirDiffuse0;
float4 g_DirSpecular0;
float4 g_DirWorldPosition0;
float4 g_DirWorldDirection0;
float4 g_SpotAmbient0;
float4 g_SpotDiffuse0;
float4 g_SpotSpecular0;
float4 g_SpotWorldPosition0;
float4 g_SpotAttenuation0;
float4 g_SpotWorldDirection0;
float4 g_SpotSpotAttenuation0;
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

    This fragment is responsible for applying the world transform to the
    normal.
    
*/

void TransformNormal(float3 Normal,
    float4x4 World,
    out float3 WorldNrm)
{

    // Transform the normal into world space for lighting
    WorldNrm = mul( Normal, (float3x3)World );
    WorldNrm = normalize(WorldNrm);
    
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

    This fragment is responsible for accumulating the effect of a light
    on the current pixel.
    
    LightType can be one of three values:
        0 - Directional
        1 - Point 
        2 - Spot
        
    Note that the LightType must be a compile-time variable,
    not a runtime constant/uniform variable on most Shader Model 2.0 cards.
    
    The compiler will optimize out any constants that aren't used.
    
    Attenuation is defined as (const, linear, quad, range).
    Range is not implemented at this time.
    
    SpotAttenuation is stored as (cos(theta/2), cos(phi/2), falloff)
    theta is the angle of the inner cone and phi is the angle of the outer
    cone in the traditional DX manner. Gamebryo only allows setting of
    phi, so cos(theta/2) will typically be cos(0) or 1. To disable spot
    effects entirely, set cos(theta/2) and cos(phi/2) to -1 or lower.
    
*/

void Light(float4 WorldPos,
    float3 WorldNrm,
    int LightType,
    bool SpecularEnable,
    float Shadow,
    float3 WorldViewVector,
    float4 LightPos,
    float3 LightAmbient,
    float3 LightDiffuse,
    float3 LightSpecular,
    float3 LightAttenuation,
    float3 LightSpotAttenuation,
    float3 LightDirection,
    float4 SpecularPower,
    float3 AmbientAccum,
    float3 DiffuseAccum,
    float3 SpecularAccum,
    out float3 AmbientAccumOut,
    out float3 DiffuseAccumOut,
    out float3 SpecularAccumOut)
{
   
    // Get the world space light vector.
    float3 LightVector;
    float DistanceToLight;
        
    if (LightType == 0)
    {
        LightVector = -LightDirection;
    }
    else
    {
        LightVector = LightPos - WorldPos;
        DistanceToLight = length(LightVector);
        LightVector = LightVector / DistanceToLight;
    }
    
    // Take N dot L as intensity.
    float LightNDotL = dot(LightVector, WorldNrm);
    float LightIntensity = max(0, LightNDotL);

    float Attenuate = 1.0;
    
    if (LightType != 0)
    {
        // Attenuate Here
        Attenuate = LightAttenuation.x +
            LightAttenuation.y * DistanceToLight +
            LightAttenuation.z * DistanceToLight * DistanceToLight;
        Attenuate = max(1.0, Attenuate);
        Attenuate = 1.0 / Attenuate;

        if (LightType == 2)
        {
            // Get intensity as cosine of light vector and direction.
            float CosAlpha = dot(-LightVector, LightDirection);

            // Factor in inner and outer cone angles.
            CosAlpha = saturate((CosAlpha - LightSpotAttenuation.y) / 
                (LightSpotAttenuation.x - LightSpotAttenuation.y));

            // Power to falloff.
            CosAlpha = pow(CosAlpha, LightSpotAttenuation.z);

            // Multiply the spot attenuation into the overall attenuation.
            Attenuate *= CosAlpha;
        }

        LightIntensity = LightIntensity * Attenuate;
    }

    // Determine the interaction of diffuse color of light and material.
    // Scale by the attenuated intensity.
    DiffuseAccumOut = DiffuseAccum;
    DiffuseAccumOut.rgb += LightDiffuse.rgb * LightIntensity * Shadow;

    // Determine ambient contribution - not affected by shadow
    AmbientAccumOut = AmbientAccum;
    AmbientAccumOut.rgb += LightAmbient.rgb * Attenuate;

    SpecularAccumOut = SpecularAccum;
    if (SpecularEnable)
    {
        // Get the half vector.
        float3 LightHalfVector = LightVector + WorldViewVector;
        LightHalfVector = normalize(LightHalfVector);

        // Determine specular intensity.
        float LightNDotH = max(0, dot(WorldNrm, LightHalfVector));
        float LightSpecIntensity = pow(LightNDotH, SpecularPower.x);
        
        //if (LightNDotL < 0.0)
        //    LightSpecIntensity = 0.0;
        // Must use the code below rather than code above.
        // Using previous lines will cause the compiler to generate incorrect
        // output.
        float SpecularMultiplier = LightNDotL > 0.0 ? 1.0 : 0.0;
        
        // Attenuate Here
        LightSpecIntensity = LightSpecIntensity * Attenuate * 
            SpecularMultiplier;
        
        // Determine the interaction of specular color of light and material.
        // Scale by the attenuated intensity.
        SpecularAccumOut.rgb += Shadow * LightSpecIntensity * LightSpecular;
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
    float3 Normal : NORMAL0;
    float2 UVSet0 : TEXCOORD0;
    float4 VertexColors : COLOR0;

};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------

struct Output
{
    float4 PosProjected : POSITION0;
    float4 DiffuseAccum : TEXCOORD0;
    float3 SpecularAccum : TEXCOORD1;
    float2 UVSet0 : TEXCOORD2;

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
    float3 WorldNrm_CallOut3;
    TransformNormal(In.Normal, g_World, WorldNrm_CallOut3);

	// Function call #4
    float3 VectorOut_CallOut4;
    NormalizeFloat3(WorldNrm_CallOut3, VectorOut_CallOut4);

	// Function call #5
    float3 WorldViewVector_CallOut5;
    CalculateViewVector(WorldPos_CallOut1, g_EyePos, WorldViewVector_CallOut5);

	// Function call #6
    float3 AmbientAccumOut_CallOut6;
    float3 DiffuseAccumOut_CallOut6;
    float3 SpecularAccumOut_CallOut6;
    Light(WorldPos_CallOut1, VectorOut_CallOut4, int(0), bool(true), float(1.0), 
        WorldViewVector_CallOut5, g_DirWorldPosition0, g_DirAmbient0, 
        g_DirDiffuse0, g_DirSpecular0, float3(0.0, 1.0, 0.0), 
        float3(-1.0, -1.0, 0.0), g_DirWorldDirection0, g_MaterialPower, 
        g_AmbientLight, float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 
        AmbientAccumOut_CallOut6, DiffuseAccumOut_CallOut6, 
        SpecularAccumOut_CallOut6);

	// Function call #7
    float3 AmbientAccumOut_CallOut7;
    float3 DiffuseAccumOut_CallOut7;
    float3 SpecularAccumOut_CallOut7;
    Light(WorldPos_CallOut1, VectorOut_CallOut4, int(2), bool(true), float(1.0), 
        WorldViewVector_CallOut5, g_SpotWorldPosition0, g_SpotAmbient0, 
        g_SpotDiffuse0, g_SpotSpecular0, g_SpotAttenuation0, 
        g_SpotSpotAttenuation0, g_SpotWorldDirection0, g_MaterialPower, 
        AmbientAccumOut_CallOut6, DiffuseAccumOut_CallOut6, 
        SpecularAccumOut_CallOut6, AmbientAccumOut_CallOut7, 
        DiffuseAccumOut_CallOut7, SpecularAccumOut_CallOut7);

	// Function call #8
    float3 Diffuse_CallOut8;
    ComputeShadingCoefficients(g_MaterialEmissive, Color_CallOut0, 
        In.VertexColors, g_MaterialSpecular, SpecularAccumOut_CallOut7, 
        DiffuseAccumOut_CallOut7, AmbientAccumOut_CallOut7, bool(false), 
        Diffuse_CallOut8, Out.SpecularAccum);

	// Function call #9
    CompositeFinalRGBAColor(Diffuse_CallOut8, Opacity_CallOut0, 
        Out.DiffuseAccum);

    Out.UVSet0 = In.UVSet0;
    return Out;
}


#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\00002B1F-00100000-10000000-002D0012-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4 g_AmbientLight;
float4 g_MaterialSpecular;
float4 g_MaterialPower;
float4 g_MaterialEmissive;
sampler Normal;
sampler Base;
sampler Detail;
sampler Gloss;
float4 g_PointAmbient0;
float4 g_PointDiffuse0;
float4 g_PointSpecular0;
float4 g_PointWorldPosition0;
float4 g_PointAttenuation0;
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
float4 g_SpotAmbient;
float4 g_SpotDiffuse;
float4 g_SpotSpecular;
float4 g_SpotWorldPosition;
float4 g_SpotAttenuation;
float4 g_SpotWorldDirection;
float4 g_SpotSpotAttenuation;
//---------------------------------------------------------------------------
// Functions:
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

    This fragment is responsible for sampling a texture and returning its value
    as a RGB value and an A value.
    
*/

void TextureRGBASample(float2 TexCoord,
    sampler Sampler,
    bool Saturate,
    out float4 ColorOut)
{

    ColorOut = tex2D(Sampler, TexCoord);
    if (Saturate)
    {
        ColorOut = saturate(ColorOut);
    }
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for sampling a texture and returning its value
    as a RGB value.
    
*/

void TextureRGBSample(float2 TexCoord,
    sampler Sampler,
    bool Saturate,
    out float3 ColorOut)
{

    ColorOut.rgb = tex2D(Sampler, TexCoord).rgb;
    if (Saturate)
    {
        ColorOut.rgb = saturate(ColorOut.rgb);
    }
    
}
//---------------------------------------------------------------------------
/*
    
    This fragment is responsible for scaling a float3 by a constant.   
    
*/

void ScaleFloat3(float3 V1,
    float Scale,
    out float3 Output)
{

    Output = Scale * V1;
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for sampling a normal map to generate the
    new world-space normal.
    
    The normal map type is an enumerated value that indicates the following:
        0 - Standard (rgb = normal/binormal/tangent)
        1 - DXN (rg = normal.xy need to calculate z)
        2 - DXT5 (ag = normal.xy need to calculate z)
    
*/

void CalculateNormalFromColor(float4 NormalMap,
    float3 WorldNormalIn,
    float3 WorldBinormalIn,
    float3 WorldTangentIn,
    int NormalMapType,
    out float3 WorldNormalOut)
{

    
    NormalMap = NormalMap * 2.0 - 1.0;
    
    // Do nothing extra for Standard
    // Handle compressed types:
    if (NormalMapType == 1) // DXN
    {
        NormalMap.rgb = float3(NormalMap.r, NormalMap.g, 
            sqrt(1 - NormalMap.r * NormalMap.r - NormalMap.g * NormalMap.g));
    }
    else if (NormalMapType == 2) // DXT5
    {
        NormalMap.rg = NormalMap.ag;
        NormalMap.b = sqrt(1 - NormalMap.r*NormalMap.r -  
            NormalMap.g * NormalMap.g);
    }
       
    float3x3 xForm = float3x3(WorldTangentIn, WorldBinormalIn, WorldNormalIn);
    xForm = transpose(xForm);
    WorldNormalOut = mul(xForm, NormalMap.rgb);
    
    WorldNormalOut = normalize(WorldNormalOut);
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for multiplying two float3's.   
    
*/

void MultiplyFloat3(float3 V1,
    float3 V2,
    out float3 Output)
{

    Output = V1 * V2;
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for multiplying two floats.   
    
*/

void MultiplyFloat(float V1,
    float V2,
    out float Output)
{

    Output = V1 * V2;
    
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

    This fragment is responsible for computing the final RGB color.
    
*/

void CompositeFinalRGBColor(float3 DiffuseColor,
    float3 SpecularColor,
    out float3 OutputColor)
{

    OutputColor.rgb = DiffuseColor.rgb + SpecularColor.rgb;
    
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
    float4 PosProjected : POSITION0;
    float4 WorldPos : TEXCOORD0;
    float3 WorldNormal : TEXCOORD1;
    float3 WorldBinormal : TEXCOORD2;
    float3 WorldTangent : TEXCOORD3;
    float3 WorldView : TEXCOORD4;
    float2 UVSet0 : TEXCOORD5;
    float2 UVSet1 : TEXCOORD6;
    float4 VertexColors : COLOR0;

};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------

struct Output
{
    float4 Color : COLOR0;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    float3 VectorOut_CallOut0;
    NormalizeFloat3(In.WorldNormal, VectorOut_CallOut0);

	// Function call #1
    float3 VectorOut_CallOut1;
    NormalizeFloat3(In.WorldTangent, VectorOut_CallOut1);

	// Function call #2
    float3 Color_CallOut2;
    float Opacity_CallOut2;
    SplitColorAndOpacity(In.VertexColors, Color_CallOut2, Opacity_CallOut2);

	// Function call #3
    float3 VectorOut_CallOut3;
    NormalizeFloat3(In.WorldBinormal, VectorOut_CallOut3);

	// Function call #4
    float4 ColorOut_CallOut4;
    TextureRGBASample(In.UVSet0, Normal, bool(false), ColorOut_CallOut4);

	// Function call #5
    float4 ColorOut_CallOut5;
    TextureRGBASample(In.UVSet0, Base, bool(false), ColorOut_CallOut5);

	// Function call #6
    float3 ColorOut_CallOut6;
    TextureRGBSample(In.UVSet1, Detail, bool(false), ColorOut_CallOut6);

	// Function call #7
    float3 ColorOut_CallOut7;
    TextureRGBSample(In.UVSet0, Gloss, bool(false), ColorOut_CallOut7);

	// Function call #8
    float3 VectorOut_CallOut8;
    NormalizeFloat3(In.WorldView, VectorOut_CallOut8);

	// Function call #9
    float3 Color_CallOut9;
    float Opacity_CallOut9;
    SplitColorAndOpacity(ColorOut_CallOut5, Color_CallOut9, Opacity_CallOut9);

	// Function call #10
    float3 Output_CallOut10;
    ScaleFloat3(ColorOut_CallOut6, float(2.0), Output_CallOut10);

	// Function call #11
    float3 WorldNormalOut_CallOut11;
    CalculateNormalFromColor(ColorOut_CallOut4, VectorOut_CallOut0, 
        VectorOut_CallOut3, VectorOut_CallOut1, int(0), 
        WorldNormalOut_CallOut11);

	// Function call #12
    float3 Output_CallOut12;
    MultiplyFloat3(Color_CallOut9, Output_CallOut10, Output_CallOut12);

	// Function call #13
    float Output_CallOut13;
    MultiplyFloat(Opacity_CallOut2, Opacity_CallOut9, Output_CallOut13);

	// Function call #14
    float3 AmbientAccumOut_CallOut14;
    float3 DiffuseAccumOut_CallOut14;
    float3 SpecularAccumOut_CallOut14;
    Light(In.WorldPos, WorldNormalOut_CallOut11, int(1), bool(true), float(1.0), 
        VectorOut_CallOut8, g_PointWorldPosition0, g_PointAmbient0, 
        g_PointDiffuse0, g_PointSpecular0, g_PointAttenuation0, 
        float3(-1.0, -1.0, 0.0), float3(1.0, 0.0, 0.0), g_MaterialPower, 
        g_AmbientLight, float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0), 
        AmbientAccumOut_CallOut14, DiffuseAccumOut_CallOut14, 
        SpecularAccumOut_CallOut14);

	// Function call #15
    float3 AmbientAccumOut_CallOut15;
    float3 DiffuseAccumOut_CallOut15;
    float3 SpecularAccumOut_CallOut15;
    Light(In.WorldPos, WorldNormalOut_CallOut11, int(0), bool(true), float(1.0), 
        VectorOut_CallOut8, g_DirWorldPosition0, g_DirAmbient0, g_DirDiffuse0, 
        g_DirSpecular0, float3(0.0, 1.0, 0.0), float3(-1.0, -1.0, 0.0), 
        g_DirWorldDirection0, g_MaterialPower, AmbientAccumOut_CallOut14, 
        DiffuseAccumOut_CallOut14, SpecularAccumOut_CallOut14, 
        AmbientAccumOut_CallOut15, DiffuseAccumOut_CallOut15, 
        SpecularAccumOut_CallOut15);

	// Function call #16
    float3 AmbientAccumOut_CallOut16;
    float3 DiffuseAccumOut_CallOut16;
    float3 SpecularAccumOut_CallOut16;
    Light(In.WorldPos, WorldNormalOut_CallOut11, int(2), bool(true), float(1.0), 
        VectorOut_CallOut8, g_SpotWorldPosition0, g_SpotAmbient0, 
        g_SpotDiffuse0, g_SpotSpecular0, g_SpotAttenuation0, 
        g_SpotSpotAttenuation0, g_SpotWorldDirection0, g_MaterialPower, 
        AmbientAccumOut_CallOut15, DiffuseAccumOut_CallOut15, 
        SpecularAccumOut_CallOut15, AmbientAccumOut_CallOut16, 
        DiffuseAccumOut_CallOut16, SpecularAccumOut_CallOut16);

	// Function call #17
    float3 AmbientAccumOut_CallOut17;
    float3 DiffuseAccumOut_CallOut17;
    float3 SpecularAccumOut_CallOut17;
    Light(In.WorldPos, WorldNormalOut_CallOut11, int(2), bool(true), float(1.0), 
        VectorOut_CallOut8, g_SpotWorldPosition, g_SpotAmbient, g_SpotDiffuse, 
        g_SpotSpecular, g_SpotAttenuation, g_SpotSpotAttenuation, 
        g_SpotWorldDirection, g_MaterialPower, AmbientAccumOut_CallOut16, 
        DiffuseAccumOut_CallOut16, SpecularAccumOut_CallOut16, 
        AmbientAccumOut_CallOut17, DiffuseAccumOut_CallOut17, 
        SpecularAccumOut_CallOut17);

	// Function call #18
    float3 Diffuse_CallOut18;
    float3 Specular_CallOut18;
    ComputeShadingCoefficients(g_MaterialEmissive, Color_CallOut2, 
        In.VertexColors, g_MaterialSpecular, SpecularAccumOut_CallOut17, 
        DiffuseAccumOut_CallOut17, AmbientAccumOut_CallOut17, bool(false), 
        Diffuse_CallOut18, Specular_CallOut18);

	// Function call #19
    float3 Output_CallOut19;
    MultiplyFloat3(Specular_CallOut18, ColorOut_CallOut7, Output_CallOut19);

	// Function call #20
    float3 Output_CallOut20;
    MultiplyFloat3(Diffuse_CallOut18, Output_CallOut12, Output_CallOut20);

	// Function call #21
    float3 OutputColor_CallOut21;
    CompositeFinalRGBColor(Output_CallOut20, Output_CallOut19, 
        OutputColor_CallOut21);

	// Function call #22
    CompositeFinalRGBAColor(OutputColor_CallOut21, Output_CallOut13, Out.Color);

    return Out;
}


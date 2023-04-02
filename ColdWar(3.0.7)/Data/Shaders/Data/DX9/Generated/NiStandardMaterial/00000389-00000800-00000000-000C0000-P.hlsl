#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\00000389-00000800-00000000-000C0000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

sampler Parallax;
float g_ParallaxOffset;
sampler Normal;
sampler Base;
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

    This fragment is responsible for calculating the UV offset to apply
    as a result of a parallax map.
    
*/

void CalculateParallaxOffset(float2 TexCoord,
    float2 Height,
    float OffsetScale,
    float3 TangentSpaceEyeVec,
    out float2 ParallaxOffsetUV)
{

    // Calculate offset scaling constant bias.
    float2 Bias = float2(OffsetScale, OffsetScale) * -0.5;

    // Calculate offset
    float2 Offset = Height.rg * OffsetScale + Bias;

    // Get texcoord.
    ParallaxOffsetUV = TexCoord + Offset * TangentSpaceEyeVec.xy;
    
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
    float3 WorldNormal : TEXCOORD0;
    float3 WorldBinormal : TEXCOORD1;
    float3 WorldTangent : TEXCOORD2;
    float3 TangentSpaceView : TEXCOORD3;
    float4 DiffuseAccum : TEXCOORD4;
    float2 UVSet0 : TEXCOORD5;

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
    SplitColorAndOpacity(In.DiffuseAccum, Color_CallOut2, Opacity_CallOut2);

	// Function call #3
    float3 VectorOut_CallOut3;
    NormalizeFloat3(In.WorldBinormal, VectorOut_CallOut3);

	// Function call #4
    float3 ColorOut_CallOut4;
    TextureRGBSample(In.UVSet0, Parallax, bool(false), ColorOut_CallOut4);

	// Function call #5
    float3 VectorOut_CallOut5;
    NormalizeFloat3(In.TangentSpaceView, VectorOut_CallOut5);

	// Function call #6
    float2 ParallaxOffsetUV_CallOut6;
    CalculateParallaxOffset(In.UVSet0, ColorOut_CallOut4, g_ParallaxOffset, 
        VectorOut_CallOut5, ParallaxOffsetUV_CallOut6);

	// Function call #7
    float4 ColorOut_CallOut7;
    TextureRGBASample(ParallaxOffsetUV_CallOut6, Base, bool(false), 
        ColorOut_CallOut7);

	// Function call #8
    float4 ColorOut_CallOut8;
    TextureRGBASample(ParallaxOffsetUV_CallOut6, Normal, bool(false), 
        ColorOut_CallOut8);

	// Function call #9
    float3 Color_CallOut9;
    float Opacity_CallOut9;
    SplitColorAndOpacity(ColorOut_CallOut7, Color_CallOut9, Opacity_CallOut9);

	// Function call #10
    float3 Output_CallOut10;
    MultiplyFloat3(Color_CallOut2, Color_CallOut9, Output_CallOut10);

	// Function call #11
    float3 WorldNormalOut_CallOut11;
    CalculateNormalFromColor(ColorOut_CallOut8, VectorOut_CallOut0, 
        VectorOut_CallOut3, VectorOut_CallOut1, int(0), 
        WorldNormalOut_CallOut11);

	// Function call #12
    float3 OutputColor_CallOut12;
    CompositeFinalRGBColor(Output_CallOut10, float3(0.0, 0.0, 0.0), 
        OutputColor_CallOut12);

	// Function call #13
    float Output_CallOut13;
    MultiplyFloat(Opacity_CallOut2, Opacity_CallOut9, Output_CallOut13);

	// Function call #14
    CompositeFinalRGBAColor(OutputColor_CallOut12, Output_CallOut13, Out.Color);

    return Out;
}


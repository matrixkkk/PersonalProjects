#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\00002309-00000800-00000000-00090000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

sampler Normal;
sampler Base;
sampler Gloss;
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
    float4 DiffuseAccum : TEXCOORD3;
    float3 SpecularAccum : TEXCOORD4;
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
    float3 VectorOut_CallOut2;
    NormalizeFloat3(In.WorldBinormal, VectorOut_CallOut2);

	// Function call #3
    float4 ColorOut_CallOut3;
    TextureRGBASample(In.UVSet0, Normal, bool(false), ColorOut_CallOut3);

	// Function call #4
    float4 ColorOut_CallOut4;
    TextureRGBASample(In.UVSet0, Base, bool(false), ColorOut_CallOut4);

	// Function call #5
    float3 ColorOut_CallOut5;
    TextureRGBSample(In.UVSet0, Gloss, bool(false), ColorOut_CallOut5);

	// Function call #6
    float3 Output_CallOut6;
    MultiplyFloat3(In.SpecularAccum, ColorOut_CallOut5, Output_CallOut6);

	// Function call #7
    float3 Color_CallOut7;
    float Opacity_CallOut7;
    SplitColorAndOpacity(In.DiffuseAccum, Color_CallOut7, Opacity_CallOut7);

	// Function call #8
    float3 Color_CallOut8;
    float Opacity_CallOut8;
    SplitColorAndOpacity(ColorOut_CallOut4, Color_CallOut8, Opacity_CallOut8);

	// Function call #9
    float3 Output_CallOut9;
    MultiplyFloat3(Color_CallOut7, Color_CallOut8, Output_CallOut9);

	// Function call #10
    float3 WorldNormalOut_CallOut10;
    CalculateNormalFromColor(ColorOut_CallOut3, VectorOut_CallOut0, 
        VectorOut_CallOut2, VectorOut_CallOut1, int(0), 
        WorldNormalOut_CallOut10);

	// Function call #11
    float3 OutputColor_CallOut11;
    CompositeFinalRGBColor(Output_CallOut9, Output_CallOut6, 
        OutputColor_CallOut11);

	// Function call #12
    float Output_CallOut12;
    MultiplyFloat(Opacity_CallOut7, Opacity_CallOut8, Output_CallOut12);

	// Function call #13
    CompositeFinalRGBAColor(OutputColor_CallOut11, Output_CallOut12, Out.Color);

    return Out;
}


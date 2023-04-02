#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Samples\FullDemos\MOUT\Data\Shaders\Generated\NiStandardMaterial\03001001-00000800-00000000-000C0000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

sampler Bump;
float4 g_BumpMatrix;
sampler EnvironmentMap;
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
    as a result of a bump map.
    
*/

void CalculateBumpOffset(float2 DuDv,
    float4 BumpMatrix,
    out float2 BumpOffset)
{

    BumpOffset.x = DuDv.x * BumpMatrix[0] + DuDv.y * BumpMatrix[2];
    BumpOffset.y = DuDv.x * BumpMatrix[1] + DuDv.y * BumpMatrix[3];
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for applying a UV offset to a texture
    coordinate set.
    
*/

void OffsetUVFloat3(float3 TexCoordIn,
    float2 TexCoordOffset,
    out float3 TexCoordOut)
{

    TexCoordOut = TexCoordIn + float3(TexCoordOffset.x, TexCoordOffset.y, 0.0);
    
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
    float4 DiffuseAccum : TEXCOORD0;
    float2 UVSet0 : TEXCOORD1;
    float3 UVSet1 : TEXCOORD2;

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
    float3 Color_CallOut0;
    float Opacity_CallOut0;
    SplitColorAndOpacity(In.DiffuseAccum, Color_CallOut0, Opacity_CallOut0);

	// Function call #1
    float3 Output_CallOut1;
    MultiplyFloat3(Color_CallOut0, float3(1.0f, 1.0f, 1.0f), Output_CallOut1);

	// Function call #2
    float3 ColorOut_CallOut2;
    TextureRGBSample(In.UVSet0, Bump, bool(false), ColorOut_CallOut2);

	// Function call #3
    float2 BumpOffset_CallOut3;
    CalculateBumpOffset(ColorOut_CallOut2, g_BumpMatrix, BumpOffset_CallOut3);

	// Function call #4
    float3 TexCoordOut_CallOut4;
    OffsetUVFloat3(In.UVSet1, BumpOffset_CallOut3, TexCoordOut_CallOut4);

	// Function call #5
    float3 ColorOut_CallOut5;
    TextureRGBSample(TexCoordOut_CallOut4, EnvironmentMap, bool(false), 
        ColorOut_CallOut5);

	// Function call #6
    float3 OutputColor_CallOut6;
    CompositeFinalRGBColor(Output_CallOut1, ColorOut_CallOut5, 
        OutputColor_CallOut6);

	// Function call #7
    CompositeFinalRGBAColor(OutputColor_CallOut6, Opacity_CallOut0, Out.Color);

    return Out;
}


#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\00000D01-00210800-00000000-000C0000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

sampler Dark;
sampler Base;
sampler Detail;
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
    float2 UVSet1 : TEXCOORD2;
    float2 UVSet2 : TEXCOORD3;

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
    float3 ColorOut_CallOut1;
    TextureRGBSample(In.UVSet1, Dark, bool(false), ColorOut_CallOut1);

	// Function call #2
    float3 ColorOut_CallOut2;
    TextureRGBSample(In.UVSet2, Detail, bool(false), ColorOut_CallOut2);

	// Function call #3
    float4 ColorOut_CallOut3;
    TextureRGBASample(In.UVSet0, Base, bool(false), ColorOut_CallOut3);

	// Function call #4
    float3 Output_CallOut4;
    ScaleFloat3(ColorOut_CallOut2, float(2.0), Output_CallOut4);

	// Function call #5
    float3 Color_CallOut5;
    float Opacity_CallOut5;
    SplitColorAndOpacity(ColorOut_CallOut3, Color_CallOut5, Opacity_CallOut5);

	// Function call #6
    float Output_CallOut6;
    MultiplyFloat(Opacity_CallOut0, Opacity_CallOut5, Output_CallOut6);

	// Function call #7
    float3 Output_CallOut7;
    MultiplyFloat3(ColorOut_CallOut1, Color_CallOut5, Output_CallOut7);

	// Function call #8
    float3 Output_CallOut8;
    MultiplyFloat3(Output_CallOut7, Output_CallOut4, Output_CallOut8);

	// Function call #9
    float3 Output_CallOut9;
    MultiplyFloat3(Color_CallOut0, Output_CallOut8, Output_CallOut9);

	// Function call #10
    float3 OutputColor_CallOut10;
    CompositeFinalRGBColor(Output_CallOut9, float3(0.0, 0.0, 0.0), 
        OutputColor_CallOut10);

	// Function call #11
    CompositeFinalRGBAColor(OutputColor_CallOut10, Output_CallOut6, Out.Color);

    return Out;
}


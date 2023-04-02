#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Samples\FullDemos\MOUT\Data\Shaders\Generated\NiStandardMaterial\00000001-00000800-00000000-00080000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

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
    float3 OutputColor_CallOut2;
    CompositeFinalRGBColor(Output_CallOut1, float3(0.0, 0.0, 0.0), 
        OutputColor_CallOut2);

	// Function call #3
    CompositeFinalRGBAColor(OutputColor_CallOut2, Opacity_CallOut0, Out.Color);

    return Out;
}


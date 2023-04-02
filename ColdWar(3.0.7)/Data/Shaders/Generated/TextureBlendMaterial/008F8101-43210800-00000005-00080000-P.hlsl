#line 2 "d:\게임브리오\ColdWa(2.4)\Data\Shaders\Generated\TextureBlendMaterial\008F8101-43210800-00000005-00080000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

sampler Base;
sampler Shader;
sampler Shader1;
sampler Shader2;
sampler Shader3;
sampler Shader4;
float4 g_FogColor;
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

    Separate a float4 into 4 floats.   
    
*/

void SplitRGBA(float4 ColorAndOpacity,
    out float Red,
    out float Green,
    out float Blue,
    out float Alpha)
{

    Red = ColorAndOpacity.r;
    Green = ColorAndOpacity.g;
    Blue = ColorAndOpacity.b;
    Alpha = ColorAndOpacity.a;
    
}
//---------------------------------------------------------------------------
/*

    This fragment implements the operation:
    Output = float3(Input, Input, Input);
    
*/

void FloatToFloat3(float Input,
    out float3 Output)
{

    Output = Input.rrr;
    
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

    This fragment implements the equation:
    Output = (V1 * V2) + V3
    
*/

void MultiplyAddFloat3(float3 V1,
    float3 V2,
    float3 V3,
    out float3 Output)
{

    Output = (V1 * V2) + V3;
    
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

    This fragment is responsible for applying the fog based on the 
    calculations in the vertex shader.
    
    
*/

void ApplyFog(float3 UnfoggedColor,
    float3 FogColor,
    float FogAmount,
    out float3 FoggedColor)
{

    FoggedColor = lerp(FogColor, UnfoggedColor, FogAmount);
    
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
    float2 UVSet3 : TEXCOORD4;
    float2 UVSet4 : TEXCOORD5;
    float2 UVSet5 : TEXCOORD6;
    float FogDepth : FOG0;

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
    float4 ColorOut_CallOut1;
    TextureRGBASample(In.UVSet0, Base, bool(false), ColorOut_CallOut1);

	// Function call #2
    float4 ColorOut_CallOut2;
    TextureRGBASample(In.UVSet1, Shader, bool(false), ColorOut_CallOut2);

	// Function call #3
    float4 ColorOut_CallOut3;
    TextureRGBASample(In.UVSet3, Shader2, bool(false), ColorOut_CallOut3);

	// Function call #4
    float4 ColorOut_CallOut4;
    TextureRGBASample(In.UVSet5, Shader4, bool(false), ColorOut_CallOut4);

	// Function call #5
    float3 Color_CallOut5;
    float Opacity_CallOut5;
    SplitColorAndOpacity(ColorOut_CallOut1, Color_CallOut5, Opacity_CallOut5);

	// Function call #6
    float4 ColorOut_CallOut6;
    TextureRGBASample(In.UVSet2, Shader1, bool(false), ColorOut_CallOut6);

	// Function call #7
    float Red_CallOut7;
    float Green_CallOut7;
    float Blue_CallOut7;
    float Alpha_CallOut7;
    SplitRGBA(ColorOut_CallOut4, Red_CallOut7, Green_CallOut7, Blue_CallOut7, 
        Alpha_CallOut7);

	// Function call #8
    float3 Output_CallOut8;
    FloatToFloat3(Red_CallOut7, Output_CallOut8);

	// Function call #9
    float3 Output_CallOut9;
    FloatToFloat3(Green_CallOut7, Output_CallOut9);

	// Function call #10
    float3 Output_CallOut10;
    FloatToFloat3(Blue_CallOut7, Output_CallOut10);

	// Function call #11
    float3 Output_CallOut11;
    FloatToFloat3(Alpha_CallOut7, Output_CallOut11);

	// Function call #12
    float Output_CallOut12;
    MultiplyFloat(Opacity_CallOut0, Opacity_CallOut5, Output_CallOut12);

	// Function call #13
    float3 Output_CallOut13;
    MultiplyAddFloat3(ColorOut_CallOut2, Output_CallOut8, float3(0.0, 0.0, 0.0), 
        Output_CallOut13);

	// Function call #14
    float4 ColorOut_CallOut14;
    TextureRGBASample(In.UVSet4, Shader3, bool(false), ColorOut_CallOut14);

	// Function call #15
    float3 Output_CallOut15;
    MultiplyAddFloat3(ColorOut_CallOut6, Output_CallOut9, Output_CallOut13, 
        Output_CallOut15);

	// Function call #16
    float3 Output_CallOut16;
    MultiplyAddFloat3(ColorOut_CallOut3, Output_CallOut10, Output_CallOut15, 
        Output_CallOut16);

	// Function call #17
    float3 Output_CallOut17;
    MultiplyAddFloat3(ColorOut_CallOut14, Output_CallOut11, Output_CallOut16, 
        Output_CallOut17);

	// Function call #18
    float3 Output_CallOut18;
    MultiplyFloat3(Color_CallOut5, Output_CallOut17, Output_CallOut18);

	// Function call #19
    float3 Output_CallOut19;
    MultiplyFloat3(Color_CallOut0, Output_CallOut18, Output_CallOut19);

	// Function call #20
    float3 OutputColor_CallOut20;
    CompositeFinalRGBColor(Output_CallOut19, float3(0.0, 0.0, 0.0), 
        OutputColor_CallOut20);

	// Function call #21
    float3 FoggedColor_CallOut21;
    ApplyFog(OutputColor_CallOut20, g_FogColor, In.FogDepth, 
        FoggedColor_CallOut21);

	// Function call #22
    CompositeFinalRGBAColor(FoggedColor_CallOut21, Output_CallOut12, Out.Color);

    return Out;
}


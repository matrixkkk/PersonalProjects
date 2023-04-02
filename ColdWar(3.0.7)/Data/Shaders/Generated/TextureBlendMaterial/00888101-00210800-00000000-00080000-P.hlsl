#line 2 "e:\Á¹¾÷ ¿¬±¸\ColdWa(2.4)\Data\Shaders\Generated\TextureBlendMaterial\00888101-00210800-00000000-00080000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

sampler Base;
sampler Shader;
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
    float3 Color_CallOut3;
    float Opacity_CallOut3;
    SplitColorAndOpacity(ColorOut_CallOut1, Color_CallOut3, Opacity_CallOut3);

	// Function call #4
    float4 ColorOut_CallOut4;
    TextureRGBASample(In.UVSet2, Shader4, bool(false), ColorOut_CallOut4);

	// Function call #5
    float Output_CallOut5;
    MultiplyFloat(Opacity_CallOut0, Opacity_CallOut3, Output_CallOut5);

	// Function call #6
    float Red_CallOut6;
    float Green_CallOut6;
    float Blue_CallOut6;
    float Alpha_CallOut6;
    SplitRGBA(ColorOut_CallOut4, Red_CallOut6, Green_CallOut6, Blue_CallOut6, 
        Alpha_CallOut6);

	// Function call #7
    float3 Output_CallOut7;
    FloatToFloat3(Red_CallOut6, Output_CallOut7);

	// Function call #8
    float3 Output_CallOut8;
    MultiplyAddFloat3(ColorOut_CallOut2, Output_CallOut7, float3(0.0, 0.0, 0.0), 
        Output_CallOut8);

	// Function call #9
    float3 Output_CallOut9;
    MultiplyFloat3(Color_CallOut3, Output_CallOut8, Output_CallOut9);

	// Function call #10
    float3 Output_CallOut10;
    MultiplyFloat3(Color_CallOut0, Output_CallOut9, Output_CallOut10);

	// Function call #11
    float3 OutputColor_CallOut11;
    CompositeFinalRGBColor(Output_CallOut10, float3(0.0, 0.0, 0.0), 
        OutputColor_CallOut11);

	// Function call #12
    float3 FoggedColor_CallOut12;
    ApplyFog(OutputColor_CallOut11, g_FogColor, In.FogDepth, 
        FoggedColor_CallOut12);

	// Function call #13
    CompositeFinalRGBAColor(FoggedColor_CallOut12, Output_CallOut5, Out.Color);

    return Out;
}


#line 2 "d:\Á¹¾÷ ¿¬±¸\Á¹ÀÛ\ColdWar(2.8)(¼­¹ö)\Data\Shaders\Generated\NiStandardMaterial\00000000-00000800-00000000-00080000-P.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions:
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
    float3 OutputColor_CallOut0;
    CompositeFinalRGBColor(float3(1.0f, 1.0f, 1.0f), float3(0.0, 0.0, 0.0), 
        OutputColor_CallOut0);

	// Function call #1
    CompositeFinalRGBAColor(OutputColor_CallOut0, float(1.0), Out.Color);

    return Out;
}


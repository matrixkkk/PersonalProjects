#line 2 "c:\ColdWar\Data\Shaders\Generated\TextureBlendMaterial\86000040-00088888-52000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x4 g_World;
float4x4 g_ViewProj;
float4 g_MaterialEmissive;
float4 g_MaterialDiffuse;
float4 g_MaterialAmbient;
float4 g_AmbientLight;
float4x4 UVSet0_TexTransform;
float4x4 UVSet1_TexTransform;
float4x4 UVSet2_TexTransform;
float4x4 UVSet3_TexTransform;
float4x4 UVSet4_TexTransform;
float4x4 UVSet5_TexTransform;
//---------------------------------------------------------------------------
// Functions:
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

    This fragment is responsible for applying a transform to the input set
    of texture coordinates.
    
*/

void TexTransformApply(float2 TexCoord,
    float4x4 TexTransform,
    out float2 TexCoordOut)
{

    
    TexCoordOut = mul(float4(TexCoord.x, TexCoord.y, 0.0, 1.0), TexTransform);
    
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

};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------

struct Output
{
    float4 PosProjected : POSITION0;
    float4 DiffuseAccum : TEXCOORD0;
    float2 UVSet0 : TEXCOORD1;
    float2 UVSet1 : TEXCOORD2;
    float2 UVSet2 : TEXCOORD3;
    float2 UVSet3 : TEXCOORD4;
    float2 UVSet4 : TEXCOORD5;
    float2 UVSet5 : TEXCOORD6;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    float4 WorldPos_CallOut0;
    TransformPosition(In.Position, g_World, WorldPos_CallOut0);

	// Function call #1
    ProjectPositionWorldToProj(WorldPos_CallOut0, g_ViewProj, Out.PosProjected);

	// Function call #2
    TexTransformApply(In.UVSet0, UVSet0_TexTransform, Out.UVSet0);

	// Function call #3
    TexTransformApply(In.UVSet0, UVSet2_TexTransform, Out.UVSet2);

	// Function call #4
    TexTransformApply(In.UVSet0, UVSet4_TexTransform, Out.UVSet4);

	// Function call #5
    float3 WorldNrm_CallOut5;
    TransformNormal(In.Normal, g_World, WorldNrm_CallOut5);

	// Function call #6
    float3 VectorOut_CallOut6;
    NormalizeFloat3(WorldNrm_CallOut5, VectorOut_CallOut6);

	// Function call #7
    TexTransformApply(In.UVSet0, UVSet1_TexTransform, Out.UVSet1);

	// Function call #8
    TexTransformApply(In.UVSet0, UVSet5_TexTransform, Out.UVSet5);

	// Function call #9
    float3 Color_CallOut9;
    float Opacity_CallOut9;
    SplitColorAndOpacity(g_MaterialDiffuse, Color_CallOut9, Opacity_CallOut9);

	// Function call #10
    TexTransformApply(In.UVSet0, UVSet3_TexTransform, Out.UVSet3);

	// Function call #11
    float3 Diffuse_CallOut11;
    float3 Specular_CallOut11;
    ComputeShadingCoefficients(g_MaterialEmissive, Color_CallOut9, 
        g_MaterialAmbient, float3(1.0, 1.0, 1.0), float3(0.0, 0.0, 0.0), 
        float3(0.0, 0.0, 0.0), g_AmbientLight, bool(false), Diffuse_CallOut11, 
        Specular_CallOut11);

	// Function call #12
    CompositeFinalRGBAColor(Diffuse_CallOut11, Opacity_CallOut9, 
        Out.DiffuseAccum);

    return Out;
}


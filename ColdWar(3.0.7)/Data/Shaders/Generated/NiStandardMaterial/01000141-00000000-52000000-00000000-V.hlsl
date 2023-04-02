#line 2 "c:\ColdWar\Data\Shaders\Generated\NiStandardMaterial\01000141-00000000-52000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x3 g_BoneMatrix3[30];
float4x4 g_SkinWorld;
float4x4 g_ViewProj;
float4 g_MaterialSpecular;
float4 g_MaterialPower;
float4 g_MaterialEmissive;
float4 g_MaterialDiffuse;
float4 g_MaterialAmbient;
float4 g_AmbientLight;
//---------------------------------------------------------------------------
// Functions:
//---------------------------------------------------------------------------

/*

    This fragment is responsible for applying the view projection and skinning 
    transform to the input position. Additionally, this fragment applies the 
    computed world transform to the input position. The weighted world 
    transform defined by the blendweights is output for use in normals or
    other calculations as the new world matrix.
    
*/

void TransformSkinnedPosition(float3 Position,
    int4 BlendIndices,
    float4 BlendWeights,
    float4x3 Bones[30],
    float4x4 SkinToWorldTransform,
    out float4 WorldPos,
    out float4x4 SkinBoneTransform)
{

    // TransformSkinnedPosition *********************************************
    // Transform the skinned position into world space
    // Composite the skinning transform which will take the vertex
    // and normal to world space.
    float fWeight3 = 1.0 - BlendWeights[0] - BlendWeights[1] - BlendWeights[2];
    float4x3 ShortSkinBoneTransform;
    ShortSkinBoneTransform  = Bones[BlendIndices[0]] * BlendWeights[0];
    ShortSkinBoneTransform += Bones[BlendIndices[1]] * BlendWeights[1];
    ShortSkinBoneTransform += Bones[BlendIndices[2]] * BlendWeights[2];
    ShortSkinBoneTransform += Bones[BlendIndices[3]] * fWeight3;
    SkinBoneTransform = float4x4(ShortSkinBoneTransform[0], 0.0f, 
        ShortSkinBoneTransform[1], 0.0f, 
        ShortSkinBoneTransform[2], 0.0f, 
        ShortSkinBoneTransform[3], 1.0f);

    // Transform into world space.
    WorldPos.xyz = mul(float4(Position, 1.0), ShortSkinBoneTransform);
    WorldPos = mul(float4(WorldPos.xyz, 1.0), SkinToWorldTransform);
    
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
    float4 BlendWeights : BLENDWEIGHT0;
    int4 BlendIndices : BLENDINDICES0;
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
    float3 SpecularAccum : TEXCOORD1;
    float2 UVSet0 : TEXCOORD2;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    float4 WorldPos_CallOut0;
    float4x4 SkinBoneTransform_CallOut0;
    TransformSkinnedPosition(In.Position, In.BlendIndices, In.BlendWeights, 
        g_BoneMatrix3, g_SkinWorld, WorldPos_CallOut0, 
        SkinBoneTransform_CallOut0);

	// Function call #1
    ProjectPositionWorldToProj(WorldPos_CallOut0, g_ViewProj, Out.PosProjected);

	// Function call #2
    float3 WorldNrm_CallOut2;
    TransformNormal(In.Normal, SkinBoneTransform_CallOut0, WorldNrm_CallOut2);

	// Function call #3
    float3 VectorOut_CallOut3;
    NormalizeFloat3(WorldNrm_CallOut2, VectorOut_CallOut3);

	// Function call #4
    float3 Color_CallOut4;
    float Opacity_CallOut4;
    SplitColorAndOpacity(g_MaterialDiffuse, Color_CallOut4, Opacity_CallOut4);

	// Function call #5
    float3 Diffuse_CallOut5;
    ComputeShadingCoefficients(g_MaterialEmissive, Color_CallOut4, 
        g_MaterialAmbient, g_MaterialSpecular, float3(0.0, 0.0, 0.0), 
        float3(0.0, 0.0, 0.0), g_AmbientLight, bool(false), Diffuse_CallOut5, 
        Out.SpecularAccum);

	// Function call #6
    CompositeFinalRGBAColor(Diffuse_CallOut5, Opacity_CallOut4, 
        Out.DiffuseAccum);

    Out.UVSet0 = In.UVSet0;
    return Out;
}


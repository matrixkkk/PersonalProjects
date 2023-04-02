#line 2 "D:\게임브리오\Gamebryo 2.2 Source\Sdk\Win32\Shaders\Data\DX9\\Generated\\NiStandardMaterial\0100009D-00000000-50000000-00000000-V.hlsl"
//---------------------------------------------------------------------------
// Constant variables:
//---------------------------------------------------------------------------

float4x3 g_BoneMatrix3[30];
float4x4 g_SkinWorld;
float4x4 g_ViewProj;
float4 g_EyePos;
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
    normal, binormal, and tangent.
    
*/

void TransformNBT(float3 Normal,
    float3 Binormal,
    float3 Tangent,
    float4x4 World,
    out float3 WorldNrm,
    out float3 WorldBinormal,
    out float3 WorldTangent)
{

    // Transform the normal into world space for lighting
    WorldNrm      = mul( Normal, (float3x3)World );
    WorldBinormal = mul( Binormal, (float3x3)World );
    WorldTangent  = mul( Tangent, (float3x3)World );
    
    // Should not need to normalize here since we will normalize in the pixel 
    // shader due to linear interpolation across triangle not perserving
    // normality.
    
}
//---------------------------------------------------------------------------
/*

    This fragment is responsible for calculating the camera view vector.
    
*/

void CalculateViewVector(float4 WorldPos,
    float3 CameraPos,
    out float3 WorldViewVector)
{

    WorldViewVector = CameraPos - WorldPos;
    WorldViewVector = normalize(WorldViewVector);
    
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
    float3 Tangent : TANGENT0;
    float3 Binormal : BINORMAL0;

};

//---------------------------------------------------------------------------
// Output:
//---------------------------------------------------------------------------

struct Output
{
    float4 PosProjected : POSITION0;
    float4 WorldPos : TEXCOORD0;
    float3 WorldNormal : TEXCOORD1;
    float3 WorldBinormal : TEXCOORD2;
    float3 WorldTangent : TEXCOORD3;
    float3 WorldView : TEXCOORD4;
    float2 UVSet0 : TEXCOORD5;

};

//---------------------------------------------------------------------------
// Main():
//---------------------------------------------------------------------------

Output Main(Input In)
{
    Output Out;
	// Function call #0
    float4x4 SkinBoneTransform_CallOut0;
    TransformSkinnedPosition(In.Position, In.BlendIndices, In.BlendWeights, 
        g_BoneMatrix3, g_SkinWorld, Out.WorldPos, SkinBoneTransform_CallOut0);

	// Function call #1
    ProjectPositionWorldToProj(Out.WorldPos, g_ViewProj, Out.PosProjected);

	// Function call #2
    TransformNBT(In.Normal, In.Binormal, In.Tangent, SkinBoneTransform_CallOut0, 
        Out.WorldNormal, Out.WorldBinormal, Out.WorldTangent);

	// Function call #3
    CalculateViewVector(Out.WorldPos, g_EyePos, Out.WorldView);

    Out.UVSet0 = In.UVSet0;
    return Out;
}


// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// This file based heavily on ShadowMap.fx, from the DirectX 9.0 SDK Update 
// (Summer 2004). Parts of this file are Copyright (c) Microsoft Corporation. 

float4x4 kWorldView             : WORLDVIEW;
float4x4 kView                  : VIEW;
float4x4 kProj                  : PROJECTION;
float4   kMatDiffuse            : MATERIALDIFFUSE;
float4   kMatEmissive           : MATERIALEMISSIVE;

float4x4 kViewToLightView       : GLOBAL;       // Transform from view space 
                                                // to light view space
float4x4 kViewToLightProj       : GLOBAL;       // Transform from view space 
                                                // to light projection space
float3   kLightPos              : GLOBAL;       // Light position in view space
float4   kLightAmbient          : GLOBAL = float4( 0.4f, 0.4f, 0.4f, 1.0f );  
                                                // Use an ambient light of 0.4

static const int MAX_BONES = 20;
float4x3 SkinBone[MAX_BONES] : SKINBONEMATRIX3;

texture BaseTex
< 
    string NTM = "base";
>;

texture ShadowTex
< 
    bool hidden = true;
    string NTM = "shader";
>;

texture SpotTex
< 
    bool hidden = true;
    string NTM = "shader";
    int NTMIndex = 1;
>;

texture ClampTex
< 
    bool hidden = true;
    string NTM = "shader";
    int NTMIndex = 2;
>;

sampler2D BaseSampler =
sampler_state
{
    Texture = <BaseTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler2D ShadowSamplerDS =
sampler_state
{
    Texture = <ShadowTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler2D SpotSampler =
sampler_state
{
    Texture = <SpotTex>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler2D ClampSampler =
sampler_state
{
    Texture = <ClampTex>;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
    AddressU = Clamp;
    AddressV = Clamp;
};

//--------------------------------------------------------------//
// Transform Utility functions
//--------------------------------------------------------------//
void CalculateSkinnedPosNorm(float4 inPos, float3 inNrm, 
    float4 inBlendWeights, float4 inBlendIndices, 
    out float4 outPos, out float3 outNrm)
{
	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(inBlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - inBlendWeights[0] - inBlendWeights[1] - 
        inBlendWeights[2];

    // Calculate bone transform to world space
    float4x3 SkinBoneWorld;
	SkinBoneWorld = inBlendWeights[0] * SkinBone[indices[0]];
	SkinBoneWorld += inBlendWeights[1] * SkinBone[indices[1]];
	SkinBoneWorld += inBlendWeights[2] * SkinBone[indices[2]];
	SkinBoneWorld += weight4 * SkinBone[indices[3]];
	
    //
    // Transform skinned position to view space
    //
	float4 tempPos = float4(mul(inPos, SkinBoneWorld), 1.0);
	outPos = mul(tempPos, kView);
	
    //
    // Compute view space normal
	// (Assume SkinBoneWorldInvTrans = SkinBoneWorld and
	//  kViewInvTrans = kView)
    //
	float3 tempNrm = mul(inNrm, (float3x3)SkinBoneWorld);
	outNrm = mul(tempNrm, (float3x3)kView);
	
}

void CalculateSkinnedPos(float4 inPos, 
    float4 inBlendWeights, float4 inBlendIndices, 
    out float4 outPos)
{
	// Compensate for lack of UBYTE4 on Geforce3
    int4 indices = D3DCOLORtoUBYTE4(inBlendIndices);

    // Calculate normalized fourth bone weight
    float weight4 = 1.0f - inBlendWeights[0] - inBlendWeights[1] - 
        inBlendWeights[2];

    // Calculate bone transform to world space
    float4x3 SkinBoneWorld;
	SkinBoneWorld = inBlendWeights[0] * SkinBone[indices[0]];
	SkinBoneWorld += inBlendWeights[1] * SkinBone[indices[1]];
	SkinBoneWorld += inBlendWeights[2] * SkinBone[indices[2]];
	SkinBoneWorld += weight4 * SkinBone[indices[3]];
	
    //
    // Transform skinned position to view space
    //
	float4 tempPos = float4(mul(inPos, SkinBoneWorld), 1.0);
	outPos = mul(tempPos, kView);
}

void CalculatePosNorm(float4 inPos, float3 inNrm, 
    out float4 outPos, out float3 outNrm)
{
    //
    // Transform position to view space
    //
	outPos = mul(inPos, kWorldView);

    //
    // Compute view space normal
	// (Assume kWorldViewInvTrans = kWorldView)
    //
	outNrm = mul(inNrm, (float3x3)kWorldView);
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertSceneDS
// Desc: Process vertex for scene using depth/stencil method
//-----------------------------------------------------------------------------
void VertSceneDS( float4 iPos : POSITION, 
                  float3 iNormal : NORMAL,
                  float2 iTex : TEXCOORD0,
                  out float4 oPos : POSITION,
                  out float4 Diffuse : COLOR,
                  out float4 vPosLight : TEXCOORD0, 
                  out float4 vSpotLight : TEXCOORD1,
                  out float2 vClampPlane : TEXCOORD2,
                  out float2 Tex : TEXCOORD3  )
{
    float4 vPos;
    float3 vNormal;
    
    CalculatePosNorm(iPos, iNormal, vPos, vNormal);

    //
    // Transform to screen coord
    //
    oPos = mul( vPos, kProj );
    
    //
    // Light it, including the weighting from the ambient component that will
    // be included in the pixel shader
    //
    
    // vLight is the unit vector from the light to this vertex
    float3 vLight = normalize( float3( vPos - kLightPos ) );
    Diffuse = ( saturate( dot( -vLight, normalize( vNormal ) ) ) * 
        ( 1 - kLightAmbient ));
    
    //
    // Transform the position to light projection space, or the
    // projection space as if the camera is looking out from
    // the spotlight.
    // 
    // Also, create some additional coordinates for the spotlight masking
    // texture.
    //
    vPosLight = mul( vPos, kViewToLightProj );
    vSpotLight = vPosLight;

    // And one more coordinates for the clamping texture to prevent
    // back-projection. It's biased slightly from a plane to a wide cone
    // (actually a pyramid) to avoid the artifact the sides of the projection,
    // caused by having veeery large texture coordinates.
    // 
    float4 vClampPlaneTemp = mul( vPos, kViewToLightView);
    vClampPlane = vClampPlaneTemp.zy - 
        (abs(vClampPlaneTemp.xx) + abs(vClampPlaneTemp.yy)) * 0.2;

    // Bias projected texture coordinates to center their respective textures.
    vPosLight.x = vPosLight.x * 0.5 + (0.5 + 0.5/1024.0) * vPosLight.w;
    vPosLight.y = vPosLight.y * -0.5 + (0.5 + 0.5/1024.0) * vPosLight.w;
    
    vSpotLight.x = vSpotLight.x * 0.5 + (0.5 + 0.5/64.0) * vSpotLight.w;
    vSpotLight.y = vSpotLight.y * -0.5 + (0.5 + 0.5/64.0) * vSpotLight.w;
    
    //
    // Propagate texture coord
    //
    Tex = iTex;
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertSceneSkinnedDS
// Desc: Process skinned vertex for scene using depth/stencil method
//-----------------------------------------------------------------------------
void VertSceneSkinnedDS( float4 iPos : POSITION, 
                         float3 iNormal : NORMAL,
                         float4 inBlendWeights : BLENDWEIGHT, 
                         float4 inBlendIndices : BLENDINDICES,
                         float2 iTex : TEXCOORD0,
                         out float4 oPos : POSITION,
                         out float4 Diffuse : COLOR,
                         out float4 vPosLight : TEXCOORD0, 
                         out float4 vSpotLight : TEXCOORD1,
                         out float2 vClampPlane : TEXCOORD2,
                         out float2 Tex : TEXCOORD3 )
{
    float4 vPos;
    float3 vNormal;
    
    CalculateSkinnedPosNorm(iPos, iNormal, inBlendWeights, inBlendIndices, 
        vPos, vNormal);

    //
    // Transform to screen coord
    //
    oPos = mul( vPos, kProj );

    //
    // Light it, including the weighting from the ambient component that will
    // be included in the pixel shader
    //

    // vLight is the unit vector from the light to this vertex
    float3 vLight = normalize( float3( vPos - kLightPos ) );
    
    Diffuse = ( saturate( dot( -vLight, normalize( vNormal ) ) ) * 
        ( 1 - kLightAmbient ));
        
    //
    // Transform the position to light projection space, or the
    // projection space as if the camera is looking out from
    // the spotlight.
    // 
    // Also, create some additional coordinates for the spotlight masking
    // texture.
    //
    vPosLight = mul( vPos, kViewToLightProj );
    vSpotLight = vPosLight;

    // And one more coordinates for the clamping texture to prevent
    // back-projection. It's biased slightly from a plane to a wide cone
    // (actually a pyramid) to avoid the artifact the sides of the projection,
    // caused by having veeery large texture coordinates.
    // 
    float4 vClampPlaneTemp = mul( vPos, kViewToLightView);
    vClampPlane = vClampPlaneTemp.zy - 
        (abs(vClampPlaneTemp.xx) + abs(vClampPlaneTemp.yy)) * 0.2;

    // Bias projected texture coordinates to center their respective textures.
    vPosLight.x = vPosLight.x * 0.5 + (0.5 + 0.5/1024.0) * vPosLight.w;
    vPosLight.y = vPosLight.y * -0.5 + (0.5 + 0.5/1024.0) * vPosLight.w;
    
    vSpotLight.x = vSpotLight.x * 0.5 + (0.5 + 0.5/64.0) * vSpotLight.w;
    vSpotLight.y = vSpotLight.y * -0.5 + (0.5 + 0.5/64.0) * vSpotLight.w;
    
    //
    // Propagate texture coord
    //
    Tex = iTex;    
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixSceneDS
// Desc: Process pixel (do per-pixel lighting) for enabled scene
//       using depth/stencil method 
//-----------------------------------------------------------------------------
float4 PixSceneDS( float4 vDiffuse : COLOR,
                   float4 vPosLight : TEXCOORD0, 
                   float4 vSpotLight : TEXCOORD1,
                   float2 vClampPlane : TEXCOORD2,
                   float2 Tex : TEXCOORD3 ) : COLOR
{
    // Shadow map lookup
    float4 AffectedMask = tex2D(ShadowSamplerDS, vPosLight);

    // Spotlight mask lookup
    float4 SpotMask = tex2D(SpotSampler, vSpotLight);
    float4 ClampPlane = tex2D(ClampSampler, vClampPlane);

    // Calculation of diffuse lighting contribution of spotlight
    float4 Diffuse = kMatDiffuse * 
        ((AffectedMask * SpotMask * ClampPlane * vDiffuse) + kLightAmbient);
        
    // Final texture/lighting calculation        
    return tex2D( BaseSampler, Tex ) * (Diffuse + kMatEmissive);
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadowDS
// Desc: Process vertex for the shadow map using depth/stencil method
//-----------------------------------------------------------------------------
void VertShadowDS( float4 Pos : POSITION,
                   out float4 oPos : POSITION )
{
    //
    // Compute the projected coordinates
    //
    oPos = mul( Pos, kWorldView );
    oPos = mul( oPos, kProj );
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadowSkinnedDS
// Desc: Process vertex for the shadow map using depth/stencil method
//-----------------------------------------------------------------------------
void VertShadowSkinnedDS( float4 Pos : POSITION,
                          float4 inBlendWeights : BLENDWEIGHT, 
                          float4 inBlendIndices : BLENDINDICES,
                          out float4 oPos : POSITION )
{
    float4 vPos;
    CalculateSkinnedPos(Pos, inBlendWeights, inBlendIndices, vPos);

    //
    // Transform to screen coord
    //
    oPos = mul( vPos, kProj );
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixShadowDS
// Desc: Process pixel for the shadow map using depth/stencil method
//-----------------------------------------------------------------------------
void PixShadowDS( out float4 Color : COLOR )
{
    Color = 0.0;
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderSceneDS
// Desc: Renders scene objects for use with shadow maps 
//       using depth/stencil method
//-----------------------------------------------------------------------------
technique ShadowMapRenderSceneDS
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass p0
    {
        DepthBias = 0.0;
        SlopeScaleDepthBias = 0.0;

// This conditional exists to mask a parse error on Xbox 360 that occurs even
// though this shader technique is specific to NVidia hardware and is not used
// on  the console.
#if defined(_DX9)
        TextureTransformFlags[0] = Projected;
        TextureTransformFlags[1] = Projected;
#endif

        VertexShader = compile vs_1_1 VertSceneDS();
        PixelShader = compile ps_1_1 PixSceneDS();
    }
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderSceneSkinnedDS
// Desc: Renders scene objects for use with shadow maps 
//       using depth/stencil method
//-----------------------------------------------------------------------------
technique ShadowMapRenderSceneSkinnedDS
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int BonesPerPartition = MAX_BONES;
>
{
    pass p0
    {
        DepthBias = 0.0;
        SlopeScaleDepthBias = 0.0;

// This conditional exists to mask a parse error on Xbox 360 that occurs even
// though this shader technique is specific to NVidia hardware and is not used
// on  the console.
#if defined(_DX9)
        TextureTransformFlags[0] = Projected;
        TextureTransformFlags[1] = Projected;
#endif

        VertexShader = compile vs_1_1 VertSceneSkinnedDS();
        PixelShader = compile ps_1_1 PixSceneDS();
    }
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderShadowDS
// Desc: Renders the shadow map using depth/stencil method
//-----------------------------------------------------------------------------
technique ShadowMapRenderShadowDS
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass p0
    {
        DepthBias = 0.0005;
        SlopeScaleDepthBias = 0.05;
        
        AlphaBlendEnable = FALSE;
        
        VertexShader = compile vs_1_1 VertShadowDS();
        PixelShader = compile ps_1_1 PixShadowDS();
    }
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderShadowSkinnedDS
// Desc: Renders the shadow map using depth/stencil method
//-----------------------------------------------------------------------------
technique ShadowMapRenderShadowSkinnedDS
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
    int BonesPerPartition = MAX_BONES;
>
{
    pass p0
    {
        DepthBias = 0.0005;
        SlopeScaleDepthBias = 0.05;
        
        AlphaBlendEnable = FALSE;
        
        VertexShader = compile vs_1_1 VertShadowSkinnedDS();
        PixelShader = compile ps_1_1 PixShadowDS();
    }
}

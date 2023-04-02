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

#define SHADOW_EPSILON 0.0005f

float4x4 kWorldView             : WORLDVIEW;
float4x4 kView                  : VIEW;
float4x4 kProj                  : PROJECTION;
float4   kMatDiffuse            : MATERIALDIFFUSE;
float4   kMatEmissive           : MATERIALEMISSIVE;

float    fShadowMapSize         : GLOBAL = 1024;
float    fShadowMapSizeInverse  : GLOBAL = 1.0f / 1024.0f;

float4x4 kViewToLightProj       : GLOBAL;       // Transform from view space 
                                                // to light projection space
float3   kLightPos              : GLOBAL;       // Light position in view space
float3   kLightDir              : GLOBAL;       // Light direction in view space
float4   kLightAmbient          : GLOBAL = float4( 0.4f, 0.4f, 0.4f, 1.0f );  
                                                // Use an ambient light of 0.4
float    fCosTheta              : GLOBAL = 0.0f; // Cosine of theta of the spot light

static const int MAX_BONES = 20;
float4x3 SkinBone[MAX_BONES] : SKINBONEMATRIX3;

texture  BaseTex
< 
    string NTM = "base";
>;

texture  ShadowTex
< 
    bool hidden = true;
    string NTM = "shader";
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

sampler2D ShadowSamplerFP =
sampler_state
{
    Texture = <ShadowTex>;
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
// Vertex Shader: VertSceneFP
// Desc: Process vertex for scene using floating-point method
//-----------------------------------------------------------------------------
void VertSceneFP( float4 iPos : POSITION, 
                  float3 iNormal : NORMAL,
                  float2 iTex : TEXCOORD0,
                  out float4 oPos : POSITION,
                  out float2 Tex : TEXCOORD0,
                  out float4 vPos : TEXCOORD1,
                  out float3 vNormal : TEXCOORD2,
                  out float4 vPosLight : TEXCOORD3 )
{
    CalculatePosNorm(iPos, iNormal, vPos, vNormal);

    //
    // Transform to screen coord
    //
    oPos = mul( vPos, kProj );

    //
    // Transform the position to light projection space, or the
    // projection space as if the camera is looking out from
    // the spotlight.
    //
    vPosLight = mul( vPos, kViewToLightProj );

    //
    // Propagate texture coord
    //
    Tex = iTex;
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertSceneSkinnedFP
// Desc: Process skinned vertex for scene using floating-point method
//-----------------------------------------------------------------------------
void VertSceneSkinnedFP( float4 iPos : POSITION, 
                         float3 iNormal : NORMAL,
                         float4 inBlendWeights : BLENDWEIGHT, 
                         float4 inBlendIndices : BLENDINDICES,
                         float2 iTex : TEXCOORD0,
                         out float4 oPos : POSITION,
                         out float2 Tex : TEXCOORD0,
                         out float4 vPos : TEXCOORD1,
                         out float3 vNormal : TEXCOORD2,
                         out float4 vPosLight : TEXCOORD3 )
{
    CalculateSkinnedPosNorm(iPos, iNormal, inBlendWeights, inBlendIndices, 
        vPos, vNormal);

    //
    // Transform to screen coord
    //
    oPos = mul( vPos, kProj );

    //
    // Transform the position to light projection space, or the
    // projection space as if the camera is looking out from
    // the spotlight.
    //
    vPosLight = mul( vPos, kViewToLightProj );

    //
    // Propagate texture coord
    //
    Tex = iTex;
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixSceneFP
// Desc: Process pixel (do per-pixel lighting) for enabled scene 
//       using floating-point method
//-----------------------------------------------------------------------------
float4 PixSceneFP( float2 Tex : TEXCOORD0,
                   float4 vPos : TEXCOORD1,
                   float3 vNormal : TEXCOORD2,
                   float4 vPosLight : TEXCOORD3 ) : COLOR
{
    // Perform same work as the no-texture version
    float4 Diffuse;

    // vLight is the unit vector from the light to this pixel
    float3 vLight = normalize( float3( vPos - kLightPos ) );

    // Compute diffuse from the light
    if( dot( vLight, kLightDir ) > fCosTheta) // Light must face the pixel 
                                                // (within Theta)
    {
        // Pixel is in lit area. Find out if it's
        // in shadow using 2x2 percentage closest filtering

        //transform from RT space to texture space.
        float2 ShadowTexC = 0.5 * vPosLight.xy / vPosLight.w + 
            float2( 0.5, 0.5 );
        ShadowTexC.y = 1.0f - ShadowTexC.y;

        // transform to texel space
        float2 texelpos = fShadowMapSize * ShadowTexC;
        
        // Determine the lerp amounts           
        float2 lerps = frac( texelpos );

        //read in bilerp stamp, doing the shadow checks
        float afSourcevals[4];
        float fTexRes = tex2D( ShadowSamplerFP, ShadowTexC ).r;
        afSourcevals[0] = (fTexRes + 
            SHADOW_EPSILON < vPosLight.z / vPosLight.w) ? 0.0f: 1.0f;  
        afSourcevals[1] = (tex2D( ShadowSamplerFP, ShadowTexC + 
            float2(fShadowMapSizeInverse, 0) ).r + SHADOW_EPSILON < 
            vPosLight.z / vPosLight.w) ? 0.0f: 1.0f;  
        afSourcevals[2] = (tex2D( ShadowSamplerFP, ShadowTexC + 
            float2(0, fShadowMapSizeInverse) ).r + SHADOW_EPSILON < 
            vPosLight.z / vPosLight.w) ? 0.0f: 1.0f;  
        afSourcevals[3] = (tex2D( ShadowSamplerFP, ShadowTexC + 
            float2(fShadowMapSizeInverse, fShadowMapSizeInverse) ).r + 
            SHADOW_EPSILON < vPosLight.z / vPosLight.w) ? 0.0f: 1.0f;  
        
        // lerp between the shadow values to calculate our light amount
        float LightAmount = lerp(lerp(afSourcevals[0], afSourcevals[1], lerps.x),
                                 lerp(afSourcevals[2], afSourcevals[3], lerps.x),
                                 lerps.y );
        // Light it
        Diffuse = ( saturate( dot( -vLight, normalize( vNormal ) ) ) 
            * LightAmount * ( 1 - kLightAmbient ) + kLightAmbient )
            * kMatDiffuse;
    } 
    else
    {
        Diffuse = kLightAmbient * kMatDiffuse;
    }

    return tex2D( BaseSampler, Tex ) * (Diffuse + kMatEmissive);
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadowFP
// Desc: Process vertex for the shadow map using floating-point method
//-----------------------------------------------------------------------------
void VertShadowFP( float4 Pos : POSITION,
                   out float4 oPos : POSITION,
                   out float2 Depth : TEXCOORD0 )
{
    //
    // Compute the projected coordinates
    //
    oPos = mul( Pos, kWorldView );
    oPos = mul( oPos, kProj );

    //
    // Store z and w in our spare texcoord
    //
    Depth.xy = oPos.zw;
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadowSkinnedFP
// Desc: Process vertex for the shadow map using floating-point method
//-----------------------------------------------------------------------------
void VertShadowSkinnedFP( float4 Pos : POSITION,
                          float4 inBlendWeights : BLENDWEIGHT, 
                          float4 inBlendIndices : BLENDINDICES,
                          out float4 oPos : POSITION,
                          out float2 Depth : TEXCOORD0 )
{
    float4 vPos;
    CalculateSkinnedPos(Pos, inBlendWeights, inBlendIndices, vPos);
    //
    // Transform to screen coord
    //
    oPos = mul( vPos, kProj );
    
    //
    // Store z and w in our spare texcoord
    //
    Depth.xy = oPos.zw;
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixShadowFP
// Desc: Process pixel for the shadow map using floating-point method
//-----------------------------------------------------------------------------
void PixShadowFP( float2 Depth : TEXCOORD0,
                  out float4 Color : COLOR )
{
    //
    // Depth is z / w
    //
    Color = Depth.x / Depth.y;
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderSceneFP
// Desc: Renders scene objects for use with shadow maps
//       using floating-point method
//-----------------------------------------------------------------------------
technique ShadowMapRenderSceneFP
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass p0
    {
        VertexShader = compile vs_1_1 VertSceneFP();
        PixelShader = compile ps_2_0 PixSceneFP();
    }
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderSceneSkinnedFP
// Desc: Renders scene objects for use with shadow maps
//       using floating-point method
//-----------------------------------------------------------------------------
technique ShadowMapRenderSceneSkinnedFP
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
	int BonesPerPartition = MAX_BONES;
>
{
    pass p0
    {
        VertexShader = compile vs_1_1 VertSceneSkinnedFP();
        PixelShader = compile ps_2_0 PixSceneFP();
    }
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderShadowFP
// Desc: Renders the shadow map using floating-point method
//-----------------------------------------------------------------------------
technique ShadowMapRenderShadowFP
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
>
{
    pass p0
    {
        ALPHABLENDENABLE = FALSE;
        VertexShader = compile vs_1_1 VertShadowFP();
        PixelShader = compile ps_2_0 PixShadowFP();
    }
}

//-----------------------------------------------------------------------------
// Technique: ShadowMapRenderShadowSkinnedFP
// Desc: Renders the shadow map using floating-point method
//-----------------------------------------------------------------------------
technique ShadowMapRenderShadowSkinnedFP
<
    bool UsesNiRenderState = true;
    bool UsesNiLightState = false;
	int BonesPerPartition = MAX_BONES;
>
{
    pass p0
    {
        ALPHABLENDENABLE = FALSE;
        VertexShader = compile vs_1_1 VertShadowSkinnedFP();
        PixelShader = compile ps_2_0 PixShadowFP();
    }
}

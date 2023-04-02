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

vs_2_0

dcl_position        v0
dcl_blendweight     v1
dcl_blendindices    v2

;----------------------------------------------------------------------------
; Determine the last blending weight
;----------------------------------------------------------------------------
mov     r0.xyz, v1.xyz              ; r0.xyz = w0, w1, w2
dp3     r0.w, v1.xyz, c5.xxxx       ; r0.w = w0 + w1 + w2
add     r0.w, -r0.w, c5.x           ; r0.w = 1 - (w0 + w1 + w2) = w3

;----------------------------------------------------------------------------
; Decode the index - it is swizzled due to the usage of D3DCOLOR for 
;       packing the indices...
;----------------------------------------------------------------------------
mul     r1, v2.zyxw, c5.wwww        ; r1 = indices w/ offset

;----------------------------------------------------------------------------
; Assemble the transform from the bones' contributions
;----------------------------------------------------------------------------
; Get the index of the bone matrix [0]
mova    a0.x, r1.x
; Calculate the contribution of the second bone
mul     r2, r0.x, c[a0.x + 0 + 10]
mul     r3, r0.x, c[a0.x + 1 + 10]
mul     r4, r0.x, c[a0.x + 2 + 10]

; Get the index of the bone matrix [1]
mova    a0.x, r1.y
; Add in the contribution of the second bone
mad     r2, r0.y, c[a0.x + 0 + 10], r2
mad     r3, r0.y, c[a0.x + 1 + 10], r3
mad     r4, r0.y, c[a0.x + 2 + 10], r4

; Get the index of the bone matrix [2]
mova    a0.x, r1.z
; Add in the contribution of the third bone
mad     r2, r0.z, c[a0.x + 0 + 10], r2
mad     r3, r0.z, c[a0.x + 1 + 10], r3
mad     r4, r0.z, c[a0.x + 2 + 10], r4

; Get the index of the bone matrix [3]
mova    a0.x, r1.w
; Add in the contribution of the fourth bone
mad     r2, r0.w, c[a0.x+0+10], r2
mad     r3, r0.w, c[a0.x+1+10], r3
mad     r4, r0.w, c[a0.x+2+10], r4

;----------------------------------------------------------------------------
; Transform the position using the resulting matrix
;----------------------------------------------------------------------------
dp4     r5.x, v0, r2
dp4     r5.y, v0, r3
dp4     r5.z, v0, r4
mov     r5.w, c5.x
m4x4    oPos, r5, c0

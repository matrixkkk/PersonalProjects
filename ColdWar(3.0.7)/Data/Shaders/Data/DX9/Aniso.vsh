// This file, originally named Aniso.nvv, comes from the Aniso sample from
// nVidia's NVEffectBrowser. The algorithm and the majority of the file 
// are nVidia's work.

; v0  -- position
; v3  -- normal
;
; c0-3   -- world/view/proj matrix
; c4     -- light vector
; c5-8   -- inverse/transpose world matrix
; c9     -- {0.0, 0.5, 1.0, -1.0}
; c10    -- eye point
; c11-14 -- world matrix

vs_1_1

dcl_position v0
dcl_normal v3

;transform position
dp4 oPos.x, v0, c0
dp4 oPos.y, v0, c1
dp4 oPos.z, v0, c2
dp4 oPos.w, v0, c3

;transform normal
dp3 r0.x, v3, c5
dp3 r0.y, v3, c6
dp3 r0.z, v3, c7

;normalize normal
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w

;compute world space position
dp4 r1.x, v0, c11
dp4 r1.y, v0, c12
dp4 r1.z, v0, c13
dp4 r1.w, v0, c14

;vector from point to eye
add r2, c10, -r1

;normalize e
dp3 r2.w, r2, r2
rsq r2.w, r2.w
mul r2, r2, r2.w

;h = Normalize( l + e )
add r1, r2, c4

;normalize h
dp3 r1.w, r1, r1
rsq r1.w, r1.w
mul r1, r1, r1.w

;l dot n
dp3 oT0.x, r0, c4

;h dot n
dp3 oT0.y, r1, r0

/************************************************************************
Copyright (C) 1999, 2000 NVIDIA Corporation
*************************************************************************/

#include "HaloConstants.h"

// v0  -- position
// v1  -- normal

vs_1_1

dcl_position v0
dcl_normal v1

// transform position
dp4 oPos.x, v0, c[CV_WORLDVIEWPROJ_0]
dp4 oPos.y, v0, c[CV_WORLDVIEWPROJ_1]
dp4 oPos.z, v0, c[CV_WORLDVIEWPROJ_2]
dp4 oPos.w, v0, c[CV_WORLDVIEWPROJ_3]

// transform normal
dp3 r0.x, v1, c[CV_WORLD_IT_0]
dp3 r0.y, v1, c[CV_WORLD_IT_1]
dp3 r0.z, v1, c[CV_WORLD_IT_2]

// normalize normal
dp3 r0.w, r0, r0
rsq r0.w, r0.w
mul r0, r0, r0.w

// l dot n
dp3 oD0, r0, c[CV_LIGHT_DIRECTION]

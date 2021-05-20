// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3Math.h"
#include <values.h>

using namespace Wm3;

template<> const GEOM_FT Math<GEOM_FT>::EPSILON = double_to_FT(DBL_EPSILON);
template<> const GEOM_FT Math<GEOM_FT>::ZERO_TOLERANCE = double_to_FT(1e-08);
template<> const GEOM_FT Math<GEOM_FT>::MAX_REAL = double_to_FT(DBL_MAX);
template<> const GEOM_FT Math<GEOM_FT>::PI = double_to_FT(4.0*atan(1.0));
template<> const GEOM_FT Math<GEOM_FT>::TWO_PI = 2*Math<GEOM_FT>::PI;
template<> const GEOM_FT Math<GEOM_FT>::HALF_PI = Math<GEOM_FT>::PI/2;
template<> const GEOM_FT Math<GEOM_FT>::INV_PI = 1/Math<GEOM_FT>::PI;
template<> const GEOM_FT Math<GEOM_FT>::INV_TWO_PI = 1/Math<GEOM_FT>::TWO_PI;
template<> const GEOM_FT Math<GEOM_FT>::DEG_TO_RAD = Math<GEOM_FT>::PI/180;
template<> const GEOM_FT Math<GEOM_FT>::RAD_TO_DEG = 180/Math<GEOM_FT>::PI;

//----------------------------------------------------------------------------
// template <>
// float Math<float>::FastInvSqrt (float fValue)
// {
//     float fHalf = 0.5f*fValue;
//     int i  = *(int*)&fValue;
//     i = 0x5f3759df - (i >> 1);
//     fValue = *(float*)&i;
//     fValue = fValue*(1.5f - fHalf*fValue*fValue);
//     return fValue;
// }
//----------------------------------------------------------------------------
// template <>
// double Math<double>::FastInvSqrt (double dValue)
// {
//     double dHalf = 0.5*dValue;
//     Integer64 i  = *(Integer64*)&dValue;
// #if defined(WM3_USING_VC70) || defined(WM3_USING_VC6)
//     i = 0x5fe6ec85e7de30da - (i >> 1);
// #else
//     i = 0x5fe6ec85e7de30daLL - (i >> 1);
// #endif
//     dValue = *(double*)&i;
//     dValue = dValue*(1.5 - dHalf*dValue*dValue);
//     return dValue;
// }
//----------------------------------------------------------------------------


// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3MATH_H
#define WM3MATH_H

//#include "Wm3System.h"

#include "../cgal_types.h"

namespace Wm3
{

  const GEOM_FT ZERO_FT(0);
  const GEOM_FT UNO_FT(1);
  const GEOM_FT UN_MEDIO_FT(1/2);
  const GEOM_FT DOS_FT(2);
  const GEOM_FT MILESIMA_FT(1/1000);

template <class Real>
class Math
{
public:
    // Wrappers to hide implementations of functions.  The ACos and ASin
    // functions clamp the input argument to [-1,1] to avoid NaN issues
    // when the input is slightly larger than 1 or slightly smaller than -1.
    // Other functions have the potential for using a fast and approximate
    // algorithm rather than calling the standard math library functions.
    static Real ACos (Real fValue);
    static Real ASin (Real fValue);
    static Real ATan (Real fValue);
    static Real ATan2 (Real fY, Real fX);
    static Real Ceil (Real fValue);
    static Real Cos (Real fValue);
    static Real Exp (Real fValue);
    static Real FAbs (Real fValue);
    static Real Floor (Real fValue);
    static Real FMod (Real fX, Real fY);
    static Real InvSqrt (Real fValue);
    static Real Log (Real fValue);
    static Real Pow (Real fBase, Real fExponent);
    static Real Sin (Real fValue);
    static Real Sqr (Real fValue);
    static Real Sqrt (Real fValue);
    static Real Tan (Real fValue);

    // Return -1 if the input is negative, 0 if the input is zero, and +1
    // if the input is positive.
    static int Sign (int iValue);
    static Real Sign (Real fValue);

    // Generate a random number in [0,1).  The random number generator may
    // be seeded by a first call to UnitRandom with a positive seed.
    static Real UnitRandom (unsigned int uiSeed = 0);

    // Generate a random number in [-1,1).  The random number generator may
    // be seeded by a first call to SymmetricRandom with a positive seed.
    static Real SymmetricRandom (unsigned int uiSeed = 0);

    // Generate a random number in [min,max).  The random number generator may
    // be seeded by a first call to IntervalRandom with a positive seed.
    static Real IntervalRandom (Real fMin, Real fMax,
        unsigned int uiSeed = 0);

    // Fast evaluation of trigonometric and inverse trigonometric functions
    // using polynomial approximations.  The speed ups were measured on an
    // AMD 2800 (2.08 GHz) processor using Visual Studion .NET 2003 with a
    // release build.

    // The input must be in [0,pi/2].
    // max error sin0 = 1.7e-04, speed up = 4.0
    // max error sin1 = 1.9e-08, speed up = 2.8
    static Real FastSin0 (Real fAngle);
    static Real FastSin1 (Real fAngle);

    // The input must be in [0,pi/2]
    // max error cos0 = 1.2e-03, speed up = 4.5
    // max error cos1 = 6.5e-09, speed up = 2.8
    static Real FastCos0 (Real fAngle);
    static Real FastCos1 (Real fAngle);

    // The input must be in [0,pi/4].
    // max error tan0 = 8.1e-04, speed up = 5.6
    // max error tan1 = 1.9e-08, speed up = 3.4
    static Real FastTan0 (Real fAngle);
    static Real FastTan1 (Real fAngle);

    // The input must be in [0,1].
    // max error invsin0 = 6.8e-05, speed up = 7.5
    // max error invsin1 = 1.4e-07, speed up = 5.5
    static Real FastInvSin0 (Real fValue);
    static Real FastInvSin1 (Real fValue);

    // The input must be in [0,1].
    // max error invcos0 = 6.8e-05, speed up = 7.5
    // max error invcos1 = 1.4e-07, speed up = 5.7
    static Real FastInvCos0 (Real fValue);
    static Real FastInvCos1 (Real fValue);

    // The input must be in [-1,1]. 
    // max error invtan0 = 1.2e-05, speed up = 2.8
    // max error invtan1 = 2.3e-08, speed up = 1.8
    static Real FastInvTan0 (Real fValue);
    static Real FastInvTan1 (Real fValue);

    // A fast approximation to 1/sqrt.
    static Real FastInvSqrt (Real fValue);

    // common constants
    static const Real EPSILON;
    static const Real ZERO_TOLERANCE;
    static const Real MAX_REAL;
    static const Real PI;
    static const Real TWO_PI;
    static const Real HALF_PI;
    static const Real INV_PI;
    static const Real INV_TWO_PI;
    static const Real DEG_TO_RAD;
    static const Real RAD_TO_DEG;
};

#include "Wm3Math.inl"
#include "Wm3Math.mcr"

typedef Math<GEOM_FT> Mathft;

}

#endif




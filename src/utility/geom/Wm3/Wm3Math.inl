// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::ACos (Real fValue)
{
    if ( -UNO_FT < fValue )
    {
        if ( fValue < UNO_FT )
            return double_to_FT(acos(fValue.to_double()));
        else
            return ZERO_FT;
    }
    else
    {
        return PI;
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::ASin (Real fValue)
{
    if ( -UNO_FT < fValue )
    {
        if ( fValue < UNO_FT )
            return double_to_FT(asin(fValue.to_double()));
        else
            return HALF_PI;
    }
    else
    {
        return -HALF_PI;
    }
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::ATan (Real fValue)
{
    return double_to_FT(atan(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::ATan2 (Real fY, Real fX)
{
    return double_to_FT(atan2(fY.to_double(),fX.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Ceil (Real fValue)
{
    return double_to_FT(ceil(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Cos (Real fValue)
{
    return double_to_FT(cos(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Exp (Real fValue)
{
    return double_to_FT(exp(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FAbs (Real fValue)
  { return double_to_FT(fabs(fValue)); }
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Floor (Real fValue)
{
    return double_to_FT(floor(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FMod (Real fX, Real fY)
{
    return double_to_FT(fmod(fX.to_double(),fY.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::InvSqrt (Real fValue)
{
    return double_to_FT((1.0/sqrt(fValue.to_double())));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Log (Real fValue)
{
    return double_to_FT(log(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Pow (Real fBase, Real fExponent)
{
    return double_to_FT(pow((double)fBase,(double)fExponent));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Sin (Real fValue)
{
    return double_to_FT(sin(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Sqr (Real fValue)
{
    return fValue*fValue;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Sqrt (Real fValue)
  { return double_to_FT(sqrt(fValue)); }
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Tan (Real fValue)
{
    return double_to_FT(tan(fValue.to_double()));
}
//----------------------------------------------------------------------------
template <class Real>
int Math<Real>::Sign (int iValue)
{
    if ( iValue > 0 )
        return 1;

    if ( iValue < 0 )
        return -1;

    return 0;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::Sign (Real fValue)
{
    if ( fValue > ZERO_FT )
        return UNO_FT;

    if ( fValue < ZERO_FT )
        return -UNO_FT;

    return ZERO_FT;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::UnitRandom (unsigned int uiSeed )
{
    if ( uiSeed > 0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return double_to_FT(dRatio);
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::SymmetricRandom (unsigned int uiSeed)
{
    if ( uiSeed > 0.0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return double_to_FT((2.0*dRatio - 1.0));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::IntervalRandom (Real fMin, Real fMax, unsigned int uiSeed)
{
    if ( uiSeed > 0 )
        srand(uiSeed);

    double dRatio = ((double)rand())/((double)(RAND_MAX));
    return fMin+(fMax-fMin)*(double_to_FT(dRatio));
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastSin0 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = double_to_FT(7.61e-03);
    fResult *= fASqr;
    fResult -= double_to_FT(1.6605e-01);
    fResult *= fASqr;
    fResult += double_to_FT(1.0);
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastSin1 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = -double_to_FT(2.39e-08);
    fResult *= fASqr;
    fResult += double_to_FT(2.7526e-06);
    fResult *= fASqr;
    fResult -= double_to_FT(1.98409e-04);
    fResult *= fASqr;
    fResult += double_to_FT(8.3333315e-03);
    fResult *= fASqr;
    fResult -= double_to_FT(1.666666664e-01);
    fResult *= fASqr;
    fResult += double_to_FT(1.0);
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastCos0 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = double_to_FT(3.705e-02);
    fResult *= fASqr;
    fResult -= double_to_FT(4.967e-01);
    fResult *= fASqr;
    fResult += double_to_FT(1.0);
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastCos1 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = -double_to_FT(2.605e-07);
    fResult *= fASqr;
    fResult += double_to_FT(2.47609e-05);
    fResult *= fASqr;
    fResult -= double_to_FT(1.3888397e-03);
    fResult *= fASqr;
    fResult += double_to_FT(4.16666418e-02);
    fResult *= fASqr;
    fResult -= double_to_FT(4.999999963e-01);
    fResult *= fASqr;
    fResult += double_to_FT(1.0);
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastTan0 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = double_to_FT(2.033e-01);
    fResult *= fASqr;
    fResult += double_to_FT(3.1755e-01);
    fResult *= fASqr;
    fResult += double_to_FT(1.0);
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastTan1 (Real fAngle)
{
    Real fASqr = fAngle*fAngle;
    Real fResult = double_to_FT(9.5168091e-03);
    fResult *= fASqr;
    fResult += double_to_FT(2.900525e-03);
    fResult *= fASqr;
    fResult += double_to_FT(2.45650893e-02);
    fResult *= fASqr;
    fResult += double_to_FT(5.33740603e-02);
    fResult *= fASqr;
    fResult += double_to_FT(1.333923995e-01);
    fResult *= fASqr;
    fResult += double_to_FT(3.333314036e-01);
    fResult *= fASqr;
    fResult += double_to_FT(1.0);
    fResult *= fAngle;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastInvSin0 (Real fValue)
{
    Real fRoot = Math<Real>::Sqrt((double_to_FT(1.0))-fValue);
    Real fResult = -double_to_FT(0.0187293);
    fResult *= fValue;
    fResult += double_to_FT(0.0742610);
    fResult *= fValue;
    fResult -= double_to_FT(0.2121144);
    fResult *= fValue;
    fResult += double_to_FT(1.5707288);
    fResult = HALF_PI - fRoot*fResult;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastInvSin1 (Real fValue)
{
    Real fRoot = Math<Real>::Sqrt(FAbs((double_to_FT(1.0))-fValue));
    Real fResult = -double_to_FT(0.0012624911);
    fResult *= fValue;
    fResult += double_to_FT(0.0066700901);
    fResult *= fValue;
    fResult -= double_to_FT(0.0170881256);
    fResult *= fValue;
    fResult += double_to_FT(0.0308918810);
    fResult *= fValue;
    fResult -= double_to_FT(0.0501743046);
    fResult *= fValue;
    fResult += double_to_FT(0.0889789874);
    fResult *= fValue;
    fResult -= double_to_FT(0.2145988016);
    fResult *= fValue;
    fResult += double_to_FT(1.5707963050);
    fResult = HALF_PI - fRoot*fResult;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastInvCos0 (Real fValue)
{
    Real fRoot = Math<Real>::Sqrt((double_to_FT(1.0))-fValue);
    Real fResult = -double_to_FT(0.0187293);
    fResult *= fValue;
    fResult += double_to_FT(0.0742610);
    fResult *= fValue;
    fResult -= double_to_FT(0.2121144);
    fResult *= fValue;
    fResult += double_to_FT(1.5707288);
    fResult *= fRoot;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastInvCos1 (Real fValue)
{
    Real fRoot = Math<Real>::Sqrt(FAbs((double_to_FT(1.0))-fValue));
    Real fResult = -double_to_FT(0.0012624911);
    fResult *= fValue;
    fResult += double_to_FT(0.0066700901);
    fResult *= fValue;
    fResult -= double_to_FT(0.0170881256);
    fResult *= fValue;
    fResult += double_to_FT(0.0308918810);
    fResult *= fValue;
    fResult -= double_to_FT(0.0501743046);
    fResult *= fValue;
    fResult += double_to_FT(0.0889789874);
    fResult *= fValue;
    fResult -= double_to_FT(0.2145988016);
    fResult *= fValue;
    fResult += double_to_FT(1.5707963050);
    fResult *= fRoot;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastInvTan0 (Real fValue)
{
    Real fVSqr = fValue*fValue;
    Real fResult = double_to_FT(0.0208351);
    fResult *= fVSqr;
    fResult -= double_to_FT(0.085133);
    fResult *= fVSqr;
    fResult += double_to_FT(0.180141);
    fResult *= fVSqr;
    fResult -= double_to_FT(0.3302995);
    fResult *= fVSqr;
    fResult += double_to_FT(0.999866);
    fResult *= fValue;
    return fResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real Math<Real>::FastInvTan1 (Real fValue)
{
    Real fVSqr = fValue*fValue;
    Real fResult = double_to_FT(0.0028662257);
    fResult *= fVSqr;
    fResult -= double_to_FT(0.0161657367);
    fResult *= fVSqr;
    fResult += double_to_FT(0.0429096138);
    fResult *= fVSqr;
    fResult -= double_to_FT(0.0752896400);
    fResult *= fVSqr;
    fResult += double_to_FT(0.1065626393);
    fResult *= fVSqr;
    fResult -= double_to_FT(0.1420889944);
    fResult *= fVSqr;
    fResult += double_to_FT(0.1999355085);
    fResult *= fVSqr;
    fResult -= double_to_FT(0.3333314528);
    fResult *= fVSqr;
    fResult += double_to_FT(1.0);
    fResult *= fValue;
    return fResult;
}
//----------------------------------------------------------------------------




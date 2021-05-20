// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3Distance.h"
#include "Wm3Math.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real, class TVector>
Distance<Real,TVector>::Distance ()
{
    MaximumIterations = 8;
    ZeroThreshold = Math<Real>::ZERO_TOLERANCE;
    SetDifferenceStep(MILESIMA_FT);

    m_fContactTime = Math<Real>::MAX_REAL;
    m_bHasMultipleClosestPoints0 = false;
    m_bHasMultipleClosestPoints1 = false;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Distance<Real,TVector>::~Distance ()
{
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real Distance<Real,TVector>::GetDifferenceStep () const
{
    return m_fDifferenceStep;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real Distance<Real,TVector>::GetContactTime () const
{
    return m_fContactTime;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
const TVector& Distance<Real,TVector>::GetClosestPoint0 () const
{
    return m_kClosestPoint0;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
const TVector& Distance<Real,TVector>::GetClosestPoint1 () const
{
    return m_kClosestPoint1;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
bool Distance<Real,TVector>::HasMultipleClosestPoints0 () const
  {  return m_bHasMultipleClosestPoints0; }
//----------------------------------------------------------------------------
template <class Real, class TVector>
bool Distance<Real,TVector>::HasMultipleClosestPoints1 () const
{
    return m_bHasMultipleClosestPoints1;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
void Distance<Real,TVector>::SetDifferenceStep (Real fDifferenceStep)
{
    assert( fDifferenceStep > ZERO_FT );
    if ( fDifferenceStep > ZERO_FT )
        m_fDifferenceStep = fDifferenceStep;
    else
      m_fDifferenceStep = MILESIMA_FT;

    m_fInvTwoDifferenceStep = (UN_MEDIO_FT)/m_fDifferenceStep;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real Distance<Real,TVector>::GetDerivative (Real fT,
    const TVector& rkVelocity0, const TVector& rkVelocity1)
{
    // Use a finite difference approximation:  f'(t) = (f(t+h)-f(t-h))/(2*h)
    Real fFp = Get(fT+m_fDifferenceStep,rkVelocity0,rkVelocity1);
    Real fFm = Get(fT-m_fDifferenceStep,rkVelocity0,rkVelocity1);
    Real fDFApprox = m_fInvTwoDifferenceStep*(fFp-fFm);
    return fDFApprox;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real Distance<Real,TVector>::GetDerivativeSquared (Real fT,
    const TVector& rkVelocity0, const TVector& rkVelocity1)
{
    // A derived class should override this only if there is a faster method
    // to compute the derivative of the squared distance for the specific
    // class.
    Real fDistance = Get(fT,rkVelocity0,rkVelocity1);
    Real fDerivative = GetDerivative(fT,rkVelocity0,rkVelocity1);
    return (GEOM_FT(2))*fDistance*fDerivative;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real Distance<Real,TVector>::Get (Real fTMin, Real fTMax,
    const TVector& rkVelocity0, const TVector& rkVelocity1)
{
    // The assumption is that distance f(t) is a convex function.  If
    // f'(tmin) >= 0, then the minimum occurs at tmin.  If f'(tmax) <= 0,
    // then the minimum occurs at tmax.  Otherwise, f'(0) < 0 and
    // f'(tmax) > 0 and the minimum occurs at some t in (tmin,tmax).

    Real fT0 = fTMin;
    Real fF0 = Get(fT0,rkVelocity0,rkVelocity1);
    if ( fF0 <= ZeroThreshold )
    {
        // The distance is effectively zero.  The objects are initially in
        // contact.
        m_fContactTime = fT0;
        return ZERO_FT;
    }
    Real fDF0 = GetDerivative(fT0,rkVelocity0,rkVelocity1);
    if ( fDF0 >= ZERO_FT )
    {
        // The distance is increasing on [0,tmax].
        m_fContactTime = fT0;
        return fF0;
    }

    Real fT1 = fTMax;
    Real fF1 = Get(fT1,rkVelocity0,rkVelocity1);
    if ( fF1 <= ZeroThreshold )
    {
        // The distance is effectively zero.
        m_fContactTime = fT1;
        return ZERO_FT;
    }
    Real fDF1 = GetDerivative(fT1,rkVelocity0,rkVelocity1);
    if ( fDF1 <= ZERO_FT )
    {
        // The distance is decreasing on [0,tmax].
        m_fContactTime = fT1;
        return fF1;
    }

    // Start the process with Newton's method for computing a time when the
    // distance is zero.  During this process we will switch to a numerical
    // minimizer if we decide that the distance cannot be zero.
    int i;
    for (i = 0; i < MaximumIterations; i++)
    {
        // compute the next Newton's iterate
        Real fT = fT0 - fF0/fDF0;
        if ( fT >= fTMax )
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        Real fF = Get(fT,rkVelocity0,rkVelocity1);
        if ( fF <= ZeroThreshold )
        {
            // The distance is effectively zero.
            m_fContactTime = fT;
            return ZERO_FT;
        }

        Real fDF = GetDerivative(fT,rkVelocity0,rkVelocity1);
        if ( fDF >= ZERO_FT )
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        fT0 = fT;
        fF0 = fF;
        fDF0 = fDF;
    }

    if ( i == MaximumIterations )
    {
        // Failed to converge within desired number of iterations.  To
        // reach here, the derivative values were always negative, so report
        // the distance at the last time.
        m_fContactTime = fT0;
        return fF0;
    }

    // The distance is always positive.  Use bisection to find the root of
    // the derivative function.
    Real fTm = fT0;
    for (i = 0; i < MaximumIterations; i++)
    {
        fTm = (UN_MEDIO_FT)*(fT0 + fT1);
        Real fDFm = GetDerivative(fTm,rkVelocity0,rkVelocity1);
        Real fProduct = fDFm*fDF0;
        if ( fProduct < -ZeroThreshold )
        {
            fT1 = fTm;
            fDF1 = fDFm;
        }
        else if ( fProduct > ZeroThreshold )
        {
            fT0 = fTm;
            fDF0 = fDFm;
        }
        else
        {
            break;
        }
    }

    // This is the time at which the minimum occurs and is not the contact
    // time.  Store it anyway for debugging purposes.
    m_fContactTime = fTm;
    Real fFm = Get(fTm,rkVelocity0,rkVelocity1);
    return fFm;
}
//----------------------------------------------------------------------------
template <class Real, class TVector>
Real Distance<Real,TVector>::GetSquared (Real fTMin, Real fTMax,
    const TVector& rkVelocity0, const TVector& rkVelocity1)
{
    // The assumption is that distance f(t) is a convex function.  If
    // f'(tmin) >= 0, then the minimum occurs at tmin.  If f'(tmax) <= 0,
    // then the minimum occurs at tmax.  Otherwise, f'(0) < 0 and
    // f'(tmax) > 0 and the minimum occurs at some t in (tmin,tmax).

    Real fT0 = fTMin;
    Real fF0 = GetSquared(fT0,rkVelocity0,rkVelocity1);
    if ( fF0 <= ZeroThreshold )
    {
        // The distance is effectively zero.  The objects are initially in
        // contact.
        m_fContactTime = fT0;
        return ZERO_FT;
    }
    Real fDF0 = GetDerivativeSquared(fT0,rkVelocity0,rkVelocity1);
    if ( fDF0 >= ZERO_FT )
    {
        // The distance is increasing on [0,tmax].
        m_fContactTime = fT0;
        return fF0;
    }

    Real fT1 = fTMax;
    Real fF1 = GetSquared(fT1,rkVelocity0,rkVelocity1);
    if ( fF1 <= ZeroThreshold )
    {
        // The distance is effectively zero.
        m_fContactTime = fT1;
        return ZERO_FT;
    }
    Real fDF1 = GetDerivativeSquared(fT1,rkVelocity0,rkVelocity1);
    if ( fDF1 <= ZERO_FT )
    {
        // The distance is decreasing on [0,tmax].
        m_fContactTime = fT1;
        return fF1;
    }

    // Start the process with Newton's method for computing a time when the
    // distance is zero.  During this process we will switch to a numerical
    // minimizer if we decide that the distance cannot be zero.
    int i;
    for (i = 0; i < MaximumIterations; i++)
    {
        // compute the next Newton's iterate
        Real fT = fT0 - fF0/fDF0;
        if ( fT >= fTMax )
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        Real fF = GetSquared(fT,rkVelocity0,rkVelocity1);
        if ( fF <= ZeroThreshold )
        {
            // The distance is effectively zero.
            m_fContactTime = fT;
            return ZERO_FT;
        }

        Real fDF = GetDerivativeSquared(fT,rkVelocity0,rkVelocity1);
        if ( fDF >= ZERO_FT )
        {
            // The convexity of the graph guarantees that when this condition
            // happens, the distance is always positive.  Switch to a
            // numerical minimizer.
            break;
        }

        fT0 = fT;
        fF0 = fF;
        fDF0 = fDF;
    }

    if ( i == MaximumIterations )
    {
        // Failed to converge within desired number of iterations.  To
        // reach here, the derivative values were always negative, so report
        // the distance at the last time.
        m_fContactTime = fT0;
        return fF0;
    }

    // The distance is always positive.  Use bisection to find the root of
    // the derivative function.
    Real fTm = fT0;
    for (i = 0; i < MaximumIterations; i++)
    {
        fTm = (UN_MEDIO_FT)*(fT0 + fT1);
        Real fDFm = GetDerivativeSquared(fTm,rkVelocity0,rkVelocity1);
        Real fProduct = fDFm*fDF0;
        if ( fProduct < -ZeroThreshold )
        {
            fT1 = fTm;
            fDF1 = fDFm;
        }
        else if ( fProduct > ZeroThreshold )
        {
            fT0 = fTm;
            fDF0 = fDFm;
        }
        else
        {
            break;
        }
    }

    // This is the time at which the minimum occurs and is not the contact
    // time.  Store it anyway for debugging purposes.
    m_fContactTime = fTm;
    Real fFm = GetSquared(fTm,rkVelocity0,rkVelocity1);
    return fFm;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------

// GEOM_FT
template class Distance<GEOM_FT,Vector2d>;
template class Distance<GEOM_FT,Vector3d>;
//----------------------------------------------------------------------------
}




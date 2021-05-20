// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm3DistVector3Triangle3.h"
#include "Wm3Math.h"


namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistVector3Triangle3<Real>::DistVector3Triangle3 (
    const Vector3d& rkVector, const Triangle3d& rkTriangle)
    :
    m_rkVector(rkVector),
    m_rkTriangle(rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3d& DistVector3Triangle3<Real>::GetVector () const
{
    return m_rkVector;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle3d& DistVector3Triangle3<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Triangle3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Triangle3<Real>::GetSquared ()
  {
    Vector3d kDiff = m_rkTriangle.Vertice0(0).VectorPos() - m_rkVector;
    Vector3d kEdge0 = m_rkTriangle.Vertice0(1) - m_rkTriangle.Vertice0(0);
    Vector3d kEdge1 = m_rkTriangle.Vertice0(2) - m_rkTriangle.Vertice0(0);
    Real fA00 = kEdge0.GetModulus2();
    Real fA01 = kEdge0.GetDot(kEdge1);
    Real fA11 = kEdge1.GetModulus2();
    Real fB0 = kDiff.GetDot(kEdge0);
    Real fB1 = kDiff.GetDot(kEdge1);
    Real fC = kDiff.GetModulus2();
    Real fDet = Math<Real>::FAbs(fA00*fA11-fA01*fA01);
    Real fS = fA01*fB1-fA11*fB0;
    Real fT = fA01*fB0-fA00*fB1;
    Real fSqrDistance;

    if ( fS + fT <= fDet )
    {
      if ( fS < ZERO_FT )
        {
            if ( fT < ZERO_FT )  // region 4
            {
                if ( fB0 < ZERO_FT )
                {
                    fT = ZERO_FT;
                    if ( -fB0 >= fA00 )
                    {
                        fS = UNO_FT;
                        fSqrDistance = fA00+(DOS_FT)*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDistance = fB0*fS+fC;
                    }
                }
                else
                {
                    fS = ZERO_FT;
                    if ( fB1 >= ZERO_FT )
                    {
                        fT = ZERO_FT;
                        fSqrDistance = fC;
                    }
                    else if ( -fB1 >= fA11 )
                    {
                        fT = UNO_FT;
                        fSqrDistance = fA11+(DOS_FT)*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDistance = fB1*fT+fC;
                    }
                }
            }
            else  // region 3
            {
                fS = ZERO_FT;
                if ( fB1 >= ZERO_FT )
                {
                    fT = ZERO_FT;
                    fSqrDistance = fC;
                }
                else if ( -fB1 >= fA11 )
                {
                    fT = UNO_FT;
                    fSqrDistance = fA11+(DOS_FT)*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        }
        else if ( fT < ZERO_FT )  // region 5
        {
            fT = ZERO_FT;
            if ( fB0 >= ZERO_FT )
            {
                fS = ZERO_FT;
                fSqrDistance = fC;
            }
            else if ( -fB0 >= fA00 )
            {
                fS = UNO_FT;
                fSqrDistance = fA00+(DOS_FT)*fB0+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fSqrDistance = fB0*fS+fC;
            }
        }
        else  // region 0
        {
            // minimum at interior point
            Real fInvDet= (UNO_FT)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDistance = fS*(fA00*fS+fA01*fT+(DOS_FT)*fB0) +
                fT*(fA01*fS+fA11*fT+(DOS_FT)*fB1)+fC;
        }
    }
    else
    {
        Real fTmp0, fTmp1, fNumer, fDenom;

        if ( fS < ZERO_FT )  // region 2
        {
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if ( fTmp1 > fTmp0 )
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fS = UNO_FT;
                    fT = ZERO_FT;
                    fSqrDistance = fA00+(DOS_FT)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = UNO_FT - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+2*fB0) +
                        fT*(fA01*fS+fA11*fT+(DOS_FT)*fB1)+fC;
                }
            }
            else
            {
                fS = ZERO_FT;
                if ( fTmp1 <= ZERO_FT )
                {
                    fT = UNO_FT;
                    fSqrDistance = fA11+(DOS_FT)*fB1+fC;
                }
                else if ( fB1 >= ZERO_FT )
                {
                    fT = ZERO_FT;
                    fSqrDistance = fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        }
        else if ( fT < ZERO_FT )  // region 6
        {
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if ( fTmp1 > fTmp0 )
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-(DOS_FT)*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fT = UNO_FT;
                    fS = ZERO_FT;
                    fSqrDistance = fA11+(DOS_FT)*fB1+fC;
                }
                else
                {
                    fT = fNumer/fDenom;
                    fS = UNO_FT - fT;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+(DOS_FT)*fB0) +
                        fT*(fA01*fS+fA11*fT+(DOS_FT)*fB1)+fC;
                }
            }
            else
            {
                fT = ZERO_FT;
                if ( fTmp1 <= ZERO_FT )
                {
                    fS = UNO_FT;
                    fSqrDistance = fA00+(DOS_FT)*fB0+fC;
                }
                else if ( fB0 >= ZERO_FT )
                {
                    fS = ZERO_FT;
                    fSqrDistance = fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDistance = fB0*fS+fC;
                }
            }
        }
        else  // region 1
        {
            fNumer = fA11 + fB1 - fA01 - fB0;
            if ( fNumer <= ZERO_FT )
            {
                fS = ZERO_FT;
                fT = UNO_FT;
                fSqrDistance = fA11+(DOS_FT)*fB1+fC;
            }
            else
            {
                fDenom = fA00-2*fA01+fA11;
                if ( fNumer >= fDenom )
                {
                    fS = UNO_FT;
                    fT = ZERO_FT;
                    fSqrDistance = fA00+(DOS_FT)*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = UNO_FT - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+(DOS_FT)*fB0) +
                        fT*(fA01*fS+fA11*fT+(DOS_FT)*fB1)+fC;
                }
            }
        }
    }

    // account for numerical round-off error
    if ( fSqrDistance < ZERO_FT )
        fSqrDistance = ZERO_FT;

    m_kClosestPoint0 = m_rkVector;
    m_kClosestPoint1 = m_rkTriangle.Vertice0(0).VectorPos() + fS*kEdge0 + fT*kEdge1;

    return fSqrDistance;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Triangle3<Real>::Get (Real fT,
    const Vector3d& rkVelocity0, const Vector3d& rkVelocity1)
{
    Vector3d kMVector = m_rkVector + fT*rkVelocity0;
    Vector3d kMV0 = m_rkTriangle.Vertice0(0).VectorPos() + fT*rkVelocity1;
    Vector3d kMV1 = m_rkTriangle.Vertice0(1).VectorPos() + fT*rkVelocity1;
    Vector3d kMV2 = m_rkTriangle.Vertice0(2).VectorPos() + fT*rkVelocity1;
    Triangle3d kMTriangle(Pos3d()+kMV0,Pos3d()+kMV1,Pos3d()+kMV2);
    return DistVector3Triangle3<Real>(kMVector,kMTriangle).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistVector3Triangle3<Real>::GetSquared (Real fT,
    const Vector3d& rkVelocity0, const Vector3d& rkVelocity1)
{
    Vector3d kMVector = m_rkVector + fT*rkVelocity0;
    Vector3d kMV0 = m_rkTriangle.Vertice0(0).VectorPos() + fT*rkVelocity1;
    Vector3d kMV1 = m_rkTriangle.Vertice0(1).VectorPos() + fT*rkVelocity1;
    Vector3d kMV2 = m_rkTriangle.Vertice0(2).VectorPos() + fT*rkVelocity1;
    Triangle3d kMTriangle(Pos3d()+kMV0,Pos3d()+kMV1,Pos3d()+kMV2);
    return DistVector3Triangle3<Real>(kMVector,kMTriangle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------

// GEOM_FT 
template class DistVector3Triangle3<GEOM_FT>;
//----------------------------------------------------------------------------
}




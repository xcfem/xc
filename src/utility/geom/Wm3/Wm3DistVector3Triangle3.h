// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM3DISTVECTOR3TRIANGLE3_H
#define WM3DISTVECTOR3TRIANGLE3_H

#include "Wm3Distance.h"
#include "../d2/Triangle3d.h"

namespace Wm3
{

template <class Real>
class DistVector3Triangle3: public Distance<Real,Vector3d >
  {
  public:
    DistVector3Triangle3 (const Vector3d& rkVector,
        const Triangle3d& rkTriangle);

    // object access
    const Vector3d& GetVector () const;
    const Triangle3d& GetTriangle () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3d& rkVelocity0,
        const Vector3d& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3d& rkVelocity0,
        const Vector3d& rkVelocity1);

  private:
    using Distance<Real,Vector3d >::m_kClosestPoint0;
    using Distance<Real,Vector3d >::m_kClosestPoint1;

    const Vector3d& m_rkVector;
    const Triangle3d& m_rkTriangle;
  };

typedef DistVector3Triangle3<GEOM_FT> DistVector3Triangle3ft;

}

#endif




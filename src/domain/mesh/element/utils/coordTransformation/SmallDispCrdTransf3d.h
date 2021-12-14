// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//SmallDispCrdTransf3d.h

#ifndef SmallDispCrdTransf3d_h
#define SmallDispCrdTransf3d_h

#include "CrdTransf3d.h"

namespace XC {

//! @ingroup ElemCT
//
//! @brief Base class for small displacements 3D coordinate transformations.
class SmallDispCrdTransf3d: public CrdTransf3d
  {
    const Matrix &computeRW(const Vector &nodeOffset) const;
  protected:
    virtual int computeElemtLengthAndOrient(void) const;
    virtual int computeLocalAxis(void) const;
    Vector &basic_to_local_resisting_force(const Vector &pb, const Vector &p0) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
    Matrix &basic_to_local_stiff_matrix(const Matrix &KB) const;
    const Matrix &local_to_global_stiff_matrix(const Matrix &kl) const;
    DbTagData &getDbTagData(void) const;
  public:
    SmallDispCrdTransf3d(int tag, int classTag);
    SmallDispCrdTransf3d(int tag, int classTag, const Vector &vecInLocXZPlane);

    double getInitialLength(void) const;
    double getDeformedLength(void) const;

    const Vector &getBasicTrialDisp(void) const;
    const Vector &getBasicIncrDisp(void) const;
    const Vector &getBasicIncrDeltaDisp(void) const;
    const Vector &getBasicTrialVel(void) const;
    const Vector &getBasicTrialAccel(void) const;

    const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif

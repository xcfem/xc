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
//ShellUpBasisCrdTransf3d.h

#ifndef ShellUpBasisCrdTransf3d_h
#define ShellUpBasisCrdTransf3d_h

#include "ShellLinearCrdTransf3d.h"

namespace XC {

class Matrix;

//! @ingroup PlaneElements
//
//! @brief Shell 3D coordinate transformations that updates
//! vector directions (needs debugging).
class ShellUpBasisCrdTransf3d: public ShellLinearCrdTransf3d
  {
  public:
    ShellUpBasisCrdTransf3d(void);
    ShellUpBasisCrdTransf3d(const Vector &,const Vector &,const Vector &);
    ShellUpBasisCrdTransf3d(const NodePtrs &t);
    virtual ShellCrdTransf3dBase *getCopy(void) const;

    virtual int update(void);
  };

} // end of XC namespace

#endif

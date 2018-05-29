//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//beam3dBase.h

#ifndef beam3dBase_h
#define beam3dBase_h

#include "domain/mesh/element/truss_beam_column/ProtoBeam3d.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup OneDimensionalElem
//
//! @brief Base class for 3D beam elements.
class beam3dBase : public ProtoBeam3d
  {
  protected:
    double theta;
    mutable double L;
    mutable Vector rForce;
    mutable bool isStiffFormed;

    static Matrix k; // the stiffness matrix
    static Matrix m; // the mass matrix	
    static Matrix d; // the damping matrix

    virtual const Matrix &getStiff(void) const= 0;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    beam3dBase(int classTag, int tag= 0);        
    beam3dBase(int tag, int classTag, double A, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2, double theta);

    int revertToLastCommit(void);        
    
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            
    
    void Print(std::ostream &s, int flag =0);    
  };
} // end of XC namespace

#endif



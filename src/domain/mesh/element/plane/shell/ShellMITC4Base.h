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
//ShellMITC4Base.h

#ifndef ShellMITC4Base_h
#define ShellMITC4Base_h

#include "Shell4NBase.h"

namespace XC {

//! @ingroup PlaneElements
//
//! @brief Base class for MIT C4 shell elements.
class ShellMITC4Base: public Shell4NBase
  {
  protected:
    double Ktt; //!<drilling stiffness
    FVectorShell p0; //!< Reactions in the basic system due to element loads

    static ShellBData BData; //!< B-bar data


    void formResidAndTangent(int tang_flag) const;
    const Matrix calculateG(void) const;
    double *computeBdrill(int node, const double shp[3][4]) const;
    const Matrix& assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const;
    const Matrix& computeBmembrane(int node, const double shp[3][4] ) const;
    const Matrix& computeBbend(int node, const double shp[3][4] ) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    //null constructor
    ShellMITC4Base(int classTag,const ShellCrdTransf3dBase *);
    ShellMITC4Base(int tag,int classTag,const SectionForceDeformation *ptr_mat,const ShellCrdTransf3dBase *); 
    //full constructor
    ShellMITC4Base(int tag,int classTag, int node1, int node2, int node3, int node4, const SectionFDPhysicalProperties &,const ShellCrdTransf3dBase *);

    //set domain because frank is a dumb ass 
    void setDomain(Domain *theDomain);
  
    //return stiffness matrix 
    const Matrix &getInitialStiff(void) const;

    // methods for applying loads
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);

    void alive(void);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

  }; 

} // end of XC namespace
#endif

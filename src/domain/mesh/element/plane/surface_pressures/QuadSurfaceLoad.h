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
                                                                        
#ifndef QuadSurfaceLoad_h
#define QuadSurfaceLoad_h

// Written: Chris McGann
// Created: 04.2009
// Modified: Chris McGann, 11.2010
//           Chris McGann, 02.2011 -> added elemental load

// Description: This file contains the class definition for QuadSurfaceLoad. 

#include "domain/mesh/element/plane/surface_pressures/SurfaceLoadBase.h"

namespace XC {

// number of nodes per element
const int QSL_NUM_NODE= 2;
// d.o.f. per node
const int QSL_NUM_NDF= 2;
// degrees of freedom per element
const int QSL_NUM_DOF(QSL_NUM_NODE*QSL_NUM_NDF);
// displacement degrees of freedom per element
const int QSL_NUM_DDOF(QSL_NUM_DOF);

class QuadSurfaceLoad: public SurfaceLoadBase<QSL_NUM_NODE>
  {
  private:
    // method to update base vector g
    int UpdateBase(double Xi) const;

    mutable Vector internalForces; //!< vector of Internal Forces
    //Vector theVector; //!< vector to return the residual

    mutable Vector g; //!< tangent vector  = d(x_Xi)/d_xi
    mutable Vector myNhat; //!< normal Vector 

    static Matrix tangentStiffness; //!< Tangent Stiffness matrix
  protected:   
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    QuadSurfaceLoad(int tag= 0);
    QuadSurfaceLoad(int tag, int Nd1, int Nd2, double pressure);
    ~QuadSurfaceLoad(void);
    Element *getCopy(void) const;

    // public methods to obtain information about dof & connectivity    
    int getNumDOF(void) const;        
    int resetNodalCoordinates(void);
    void setDomain(Domain *theDomain);

    size_t getDimension(void) const;
    double getLength(bool initialGeometry= true) const;
    int getVtkCellType(void) const;
    Matrix getLocalAxes(bool) const;
    
    // public methods to obtain stiffness, mass, damping and 
    // residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;    

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &, int flag =0);
  };

} //end of XC namespace
#endif

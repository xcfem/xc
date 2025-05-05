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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
#ifndef BrickSurfaceLoad_h
#define BrickSurfaceLoad_h

// Written: Chris McGann
// Created: 04.2009
// Modified: Chris McGann, 11.2010
//           Chris McGann, 02.2011 -> added elemental load

// Description: This file contains the class definition for BrickSurfaceLoad. 

#include "domain/mesh/element/plane/surface_pressures/SurfaceLoadBase.h"
#include "utility/geom/d2/Polygon3d.h"

namespace XC {
class ShellLinearCrdTransf3d;

// number of nodes per element
const int SL_NUM_NODE= 4;
// d.o.f. per node
const int SL_NUM_NDF= 3;
// degrees of freedom per element
const int SL_NUM_DOF(SL_NUM_NODE*SL_NUM_NDF);
// displacement degrees of freedom per element
const int SL_NUM_DDOF(SL_NUM_DOF);

class BrickSurfaceLoad: public SurfaceLoadBase<SL_NUM_NODE>
  {
  private:
    // method to update base vectors g1 & g2
    int UpdateBase(double Xi, double Eta) const;

    mutable Vector internalForces; //!< vector of Internal Forces
    //Vector theVector; //!< vector to return the residual

    mutable Vector g1; //!< tangent vector  = d(x_Xi)/d_xi
    mutable Vector g2; //!< tangent vector  = d(x_Xi)/d_eta 
    mutable Vector myNhat; //!< normal Vector 

    mutable Vector myNI; //!< shape functions

    Vector dcrd1; //!< current coordinates of node 1
    Vector dcrd2; //!< current coordinates of node 2
    Vector dcrd3; //!< current coordinates of node 3
    Vector dcrd4; //!< current coordinates of node 4

    const double oneOverRoot3= 1.0/sqrt(3.0);
    static double GsPts[SL_NUM_NODE][2];

    static Matrix tangentStiffness; //!< Tangent Stiffness matrix
  protected:   
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    BrickSurfaceLoad(int tag= 0);
    BrickSurfaceLoad(int tag, int Nd1, int Nd2, int Nd3, int Nd4, double pressure);
    ~BrickSurfaceLoad(void);
    Element *getCopy(void) const;
    
    size_t getDimension(void) const;
    ShellLinearCrdTransf3d *getCoordTransf(void);
    const ShellLinearCrdTransf3d *getCoordTransf(void) const;
    Polygon3d getPolygon(bool initialGeometry= true) const;
    std::deque<Pos3d> getNodePositions(bool initialGeometry= true) const;
    double getArea(bool initialGeometry) const;
    int getVtkCellType(void) const;

    // public methods to obtain information about dof & connectivity    
    int getNumDOF(void) const;        
    int resetNodalCoordinates(void);
    void setDomain(Domain *theDomain);

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

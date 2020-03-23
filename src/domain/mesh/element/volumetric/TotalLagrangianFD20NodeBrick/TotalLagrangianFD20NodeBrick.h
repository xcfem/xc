// -*-c++-*-
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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              Sept2003
//# UPDATE HISTORY:    28May2004, Zhao Optimized the Stiffness Tensor
//#
//#
//===============================================================================
#ifndef TOTALLAGRANGIANFD20BRICK_H
#define TOTALLAGRANGIANFD20BRICK_H

#include <domain/mesh/element/ElemWithMaterial.h>
#include "domain/mesh/element/utils/physical_properties/NDMaterialPhysicalProperties.h"
#include "domain/mesh/element/utils/body_forces/BodyForces3D.h"

namespace XC {
class BJtensor;
class NDMaterial;

//! @ingroup ElemVol
//
//! @brief Twenty node hexahedron with lagrangian formulation.
class TotalLagrangianFD20NodeBrick: public ElemWithMaterial<20,NDMaterialPhysicalProperties>
  {
  private:
    static Matrix K; //!< Element stiffness Matrix
//    static Matrix C; //!< Element damping matrix
    static Matrix M; //!< Element mass matrix
    static Vector P; //!< Element resisting force vector
    static const double pts[3]; //!< Stores quadrature points
    static const double wts[3]; //!< Stores quadrature weights
    BodyForces3D bf; //!< Body forces

    double det_of_Jacobian;
    
    mutable Matrix *Ki;

    static const int  NumIntegrationPts; //!< Number of integration points.
    static const int  NumTotalGaussPts;
    static const int  NumNodes; //!< Number of nodes.
    static const int  NumDof; //!< Number of degrees of freedom.
    static const int  NumElemDof;
    
    static BJtensor shapeFunction(double , double , double );
    static BJtensor shapeFunctionDerivative(double , double , double );

    BJtensor Jacobian_3D(BJtensor dh) const;
    BJtensor Jacobian_3Dinv(BJtensor dh) const;
    BJtensor dh_Global(BJtensor dh) const;
    BJtensor getNodesCrds(void) const;
    BJtensor getNodesDisp(void) const;

    BJtensor getStiffnessTensor(void) const;
    BJtensor getRtensor(void) const;
    BJtensor getBodyForce(void) const;
    BJtensor getSurfaceForce(void) const;
    BJtensor getForces(void) const;
  public:
    TotalLagrangianFD20NodeBrick(int tag,
    int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,
    int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,
    int node_numb_9,  int node_numb_10, int node_numb_11, int node_numb_12,
    int node_numb_13, int node_numb_14, int node_numb_15, int node_numb_16,
    int node_numb_17, int node_numb_18, int node_numb_19, int node_numb_20,
    NDMaterial &m, const BodyForces3D &bForces= BodyForces3D());
    TotalLagrangianFD20NodeBrick (void);
    Element *getCopy(void) const;
    ~TotalLagrangianFD20NodeBrick(void);

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    int update();

    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);

//    int setParameter(const std::vector<std::string> &argv, Parameter &param);
//    int updateParameter(int parameterID, Information &info);
  };
} // end of XC namespace


#endif


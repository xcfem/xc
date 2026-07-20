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
//FrictionElementBase.h,v $

#ifndef FrictionElementBase_h
#define FrictionElementBase_h

#include "domain/mesh/element/Element0D.h"

#include "domain/mesh/element/utils/physical_properties/UniaxialMatPhysicalProperties.h"
#include "frictionModel/FrictionModels.h"

namespace XC {
class UniaxialMaterial;
class Response;

//! @ingroup Elem
//!
//! @defgroup FrictionElementGrp Element used to represent <a href="http://en.wikipedia.org/wiki/Friction">frictional</a> contacts.
//
//
//! @brief Base class for friction elements.
//! @ingroup FrictionElementGrp
class FrictionElementBase: public Element0D
  {
  protected:    
    FrictionModels frictionModels; //!< pointer to friction model
    UniaxialMatPhysicalProperties physicalProperties;  //!<array of uniaxial materials
    
    // parameters
    Vector x; //!< local x direction.
    Vector y; //!< local y direction.
    double mass; //!< mass of element.
    int maxIter; //!< maximum number of iterations.
    double tol; //!< tolerance for convergence criterion.
    double L; //!< element length.
    
    // state variables
    Vector ub; //!< displacements in basic system.
    Vector qb; //!< forces in basic system.
    Matrix kb; //!< stiffness matrix in basic system.
    Vector ul; //!< displacements in local system.
    Matrix Tgl; //!< transformation matrix from global to local system.
    Matrix Tlb; //!< transformation matrix from local to basic system.
    
    // committed history variables
    
    Matrix kbInit; //!< initial stiffness matrix in basic system.
    
    static double sgn(const double &x);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    FrictionElementBase(int tag, int classTag, int Nd1, int Nd2, const size_t &dim, const FrictionModels &, const UniaxialMatPhysicalProperties &, const Vector &y= Vector(), const Vector &x= Vector(), const double &mass = 0.0, const int &maxIter= 20, const double &tol= 1E-8);
    FrictionElementBase(int classTag,const size_t &);
    
    // parameters
    void setLocalXDirection(const Vector &x);
    const Vector &getLocalXDirection(void) const;
    void setLocalYDirection(const Vector &x);
    const Vector &getLocalYDirection(void) const;
    void setBearingElementMass(const double &);
    const double &getBearingElementMass(void) const;
    void setMaxIter(const int &);
    int getMaxIter(void) const;
    void setTol(const double &);
    const double &getTol(void) const;
    void setLength(const double &);
    const double &getLength(void) const;
    
    
    // state variables
    void setDisplacementsInBasicSystem(const Vector &);
    const Vector &getDisplacementsInBasicSystem(void) const;
    void setForcesInBasicSystem(const Vector &);
    const Vector &getForcesInBasicSystem(void) const;
    void setStiffnessInBasicSystem(const Matrix &);
    const Matrix &getStiffnessInBasicSystem(void) const;
    void setDisplacementsInLocalSystem(const Vector &);
    const Vector &getDisplacementsInLocalSystem(void) const;
    void setTransformationGlobalToLocalSystem(const Matrix &);
    const Matrix &getTransformationGlobalToLocalSystem(void) const;
    void setTransformationLocalToBasicSystem(const Matrix &);
    const Matrix &getTransformationLocalToBasicSystem(void) const;
  };
} // end of XC namespace

#endif

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

// $Revision: 1.2 $
// $Date: 2009/11/03 23:12:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/FlatSliderSimple2d.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the implementation of the
// FlatSliderSimple2d class.

#include "FrictionElementBase.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "frictionModel/FrictionModel.h"
#include "utility/actor/actor/CommMetaData.h"

XC::FrictionElementBase::FrictionElementBase(int tag, int classTag,int Nd1, int Nd2,const size_t &dim,
					     const FrictionModels &thefrnmdls,
					     const UniaxialMatPhysicalProperties &physProp,
					     const Vector &_y, const Vector &_x,
					     const double &m,
					     const int &maxiter,
					     const double &_tol)
  : Element0D(tag, classTag,Nd1,Nd2, dim),
    frictionModels(thefrnmdls), physicalProperties(physProp),
    x(_x), y(_y), mass(m), maxIter(maxiter), tol(_tol),
    L(0.0), ub(dim), qb(dim), kb(dim,dim), ul(dim*2),
    Tgl(dim*2,dim*2), Tlb(dim,dim*2), kbInit(dim,dim)
  {
  }


XC::FrictionElementBase::FrictionElementBase(int classTag,const size_t &dim)
  : Element0D(0,classTag,0,0,dim),
    frictionModels(), physicalProperties(0,nullptr),
    x(), y(), mass(0.0), maxIter(20), tol(1E-8),
    L(0.0), ub(dim), qb(dim), kb(dim,dim), ul(dim*2),
    Tgl(dim*2,dim*2), Tlb(dim,dim*2), kbInit(dim,dim)
  {}

// parameters
//! @brief Set local x direction vector.
void XC::FrictionElementBase::setLocalXDirection(const Vector &v)
  { this->x= v; }
//! @brief Get local x direction vector.
const XC::Vector &XC::FrictionElementBase::getLocalXDirection(void) const
  { return x; }
//! @brief Set local y direction vector.
void XC::FrictionElementBase::setLocalYDirection(const Vector &v)
  { this->y= v; }
//! @brief Get local y direction vector.
const XC::Vector &XC::FrictionElementBase::getLocalYDirection(void) const
  { return this->y; }
//! @brief Set the mass of the element.
void XC::FrictionElementBase::setBearingElementMass(const double &d)
  { this->mass= d; }
//! @brief Get the mass of the element.
const double &XC::FrictionElementBase::getBearingElementMass(void) const
  { return this->mass; }
//! @brief Set the maximum number of iterations.
void XC::FrictionElementBase::setMaxIter(const int &i)
  { this->maxIter= i; }
//! @brief Set the maximum number of iterations.
int XC::FrictionElementBase::getMaxIter(void) const
  { return this->maxIter; }
//! @brief Set the tolerance for convergence criterion.
void XC::FrictionElementBase::setTol(const double &d)
  { this->tol= d; }
//! @brief Get the tolerance for convergence criterion.
const double &XC::FrictionElementBase::getTol(void) const
  { return this->tol; }
//! @brief Set the element length.
void XC::FrictionElementBase::setLength(const double &d)
  { this->L= d; }
//! @brief Get the element length.
const double &XC::FrictionElementBase::getLength(void) const
  { return this->L; }


// state variables

//! @brief Set the displacements in basic system. 
void XC::FrictionElementBase::setDisplacementsInBasicSystem(const Vector &v)
  { this->ub= v; }
//! @brief Get the displacements in basic system. 
const XC::Vector &XC::FrictionElementBase::getDisplacementsInBasicSystem(void) const
  { return this->ub; }
//! @brief Set the forces in basic system. 
void XC::FrictionElementBase::setForcesInBasicSystem(const Vector &v)
  { this->qb= v; }
//! @brief Get the forces in basic system.
const XC::Vector &XC::FrictionElementBase::getForcesInBasicSystem(void) const
  { return this->qb; }
//! @brief Get the stiffness matrix in basic system.
void XC::FrictionElementBase::setStiffnessInBasicSystem(const Matrix &m)
  { this->kb= m; }
//! @brief Set the stiffness matrix in basic system.
const XC::Matrix &XC::FrictionElementBase::getStiffnessInBasicSystem(void) const
  { return this->kb; }


//! @brief Set the displacements in local system.
void XC::FrictionElementBase::setDisplacementsInLocalSystem(const Vector &v)
  { this->ul= v; }
//! @brief Get the displacements in local system.
const XC::Vector &XC::FrictionElementBase::getDisplacementsInLocalSystem(void) const
  { return this->ul; }
//! @brief Set the transformation matrix from global to local system.
void XC::FrictionElementBase::setTransformationGlobalToLocalSystem(const Matrix &m)
  { this->Tgl= m; }
//! @brief Get the transformation matrix from global to local system.
const XC::Matrix &XC::FrictionElementBase::getTransformationGlobalToLocalSystem(void) const
  { return this->Tgl; }
//! @brief Set the transformation matrix from local to basic system.
void XC::FrictionElementBase::setTransformationLocalToBasicSystem(const Matrix &m)
  { this->Tlb= m; }
//! @brief Get the transformation matrix from local to basic system.
const XC::Matrix &XC::FrictionElementBase::getTransformationLocalToBasicSystem(void) const
  { return this->Tlb; }

//! @brief Send members through the communicator argument.
int XC::FrictionElementBase::sendData(Communicator &comm)
  {
    int res= Element0D::sendData(comm);
    res+= comm.sendMovable(frictionModels, getDbTagData(),CommMetaData(9));
    res+= comm.sendMovable(physicalProperties, getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(x,getDbTagData(),CommMetaData(11));
    res+= comm.sendVector(y,getDbTagData(),CommMetaData(12));
    res+= comm.sendInt(maxIter,getDbTagData(),CommMetaData(13));
    res+= comm.sendVector(ub,getDbTagData(),CommMetaData(14));
    res+= comm.sendVector(qb,getDbTagData(),CommMetaData(15));
    res+= comm.sendMatrix(kb,getDbTagData(),CommMetaData(16));
    res+= comm.sendVector(ul,getDbTagData(),CommMetaData(17));
    res+= comm.sendMatrix(Tgl,getDbTagData(),CommMetaData(18));
    res+= comm.sendMatrix(Tlb,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::FrictionElementBase::recvData(const Communicator &comm)
  {
    int res= Element0D::recvData(comm);
    res+= comm.receiveMovable(frictionModels, getDbTagData(),CommMetaData(9));
    res+= comm.receiveMovable(physicalProperties, getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(x,getDbTagData(),CommMetaData(11));
    res+= comm.receiveVector(y,getDbTagData(),CommMetaData(12));
    res+= comm.receiveInt(maxIter,getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(ub,getDbTagData(),CommMetaData(14));
    res+= comm.receiveVector(qb,getDbTagData(),CommMetaData(15));
    res+= comm.receiveMatrix(kb,getDbTagData(),CommMetaData(16));
    res+= comm.receiveVector(ul,getDbTagData(),CommMetaData(17));
    res+= comm.receiveMatrix(Tgl,getDbTagData(),CommMetaData(18));
    res+= comm.receiveMatrix(Tlb,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Specialized version for the sign of the given number
//! (it considers the zero has no sign).
double XC::FrictionElementBase::sgn(const double &x)
  { 
    if(x > 0)
      return 1.0;
    else if (x < 0)
      return -1.0;
    else
      return 0.0;
  }

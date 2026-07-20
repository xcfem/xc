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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/SimpleBearingBase.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the implementation of the
// SimpleBearingBase class.

#include "SimpleBearingBase.h"

XC::SimpleBearingBase::SimpleBearingBase(int tag, int classTag, int Nd1, int Nd2, const size_t &dim,
					 const FrictionModels &thefrnmdls, double kInit,
					 const std::vector<UniaxialMaterial *> &materials,
					 const Vector &_y, const Vector &_x,
					 const double &sdI, const int &addRay,
					 const double &m,
					 const int &maxiter,
					 const double &_tol,
					 const double &kfactuplift)
  : FrictionElementBase(tag, classTag,Nd1,Nd2, dim,thefrnmdls,UniaxialMatPhysicalProperties(materials),_y,_x,m,maxiter,tol),
    k0(kInit), shearDistI(sdI), addRayleigh(addRay), onP0(true),
    kFactUplift(kfactuplift)
  {}


XC::SimpleBearingBase::SimpleBearingBase(int classTag, const size_t &dim)
  : FrictionElementBase(classTag, dim),
    k0(0.0), shearDistI(0.0), addRayleigh(0), onP0(false),
    kFactUplift(1E-12)
  {}

//! @brief Set initial stiffness of hysteretic component.
void XC::SimpleBearingBase::setInitialStiffnessOfHystereticComponent(const double &d)
  { this->k0= d;}
//! @brief Get initial stiffness of hysteretic component.
const double &XC::SimpleBearingBase::getInitialStiffnessOfHystereticComponent(void) const
  { return this->k0; }
//! @brief Set shear distance from node I as fraction of length.
void XC::SimpleBearingBase::setShearDistanceFromNodeIAsFractionOfLength(const double &d)
  { this->shearDistI= d;}
//! @brief Get shear distance from node I as fraction of length.
const double &XC::SimpleBearingBase::getShearDistanceFromNodeIAsFractionOfLength(void) const
  {return this->shearDistI;}
//! @brief Set flag to add Rayleigh damping.
void XC::SimpleBearingBase::setAddRayleighDampingFlag(const int &i)
  { this->addRayleigh= i; }
//! @brief Get flag to add Rayleigh damping.
int XC::SimpleBearingBase::getAddRayleighDampingFlag(void) const
  { return this->addRayleigh; }
//! @brief Set flag to indicate if the element is on P0.
void XC::SimpleBearingBase::setElementIsOnP0Flag(const bool &b)
  { this->onP0= b; }
//! @brief Get flag to indicate if the element is on P0.
bool XC::SimpleBearingBase::getElementIsOnP0Flag(void) const
  { return  this->onP0; }

//! @brief Set the stiffness factor when uplift is encountered.
void XC::SimpleBearingBase::setStiffnessFactorWhenUplift(const double &d)
  { this->kFactUplift= d; }

//! @brief Get the stiffness factor when uplift is encountered.
const double &XC::SimpleBearingBase::getStiffnessFactorWhenUplift(void) const
  { return this->kFactUplift; }

//! @brief Send members through the communicator argument.
int XC::SimpleBearingBase::sendData(Communicator &comm)
  {
    int res= FrictionElementBase::sendData(comm);
    res+= comm.sendDoubles(k0, shearDistI, kFactUplift, getDbTagData(),CommMetaData(20));
    res+= comm.sendInt(addRayleigh, getDbTagData(), CommMetaData(21));
    res+= comm.sendBool(onP0, getDbTagData(), CommMetaData(22));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::SimpleBearingBase::recvData(const Communicator &comm)
  {
    int res= FrictionElementBase::recvData(comm);
    res+= comm.receiveDoubles(k0, shearDistI, kFactUplift, getDbTagData(),CommMetaData(20));
    res+= comm.receiveInt(addRayleigh, getDbTagData(), CommMetaData(21));
    res+= comm.receiveBool(onP0, getDbTagData(), CommMetaData(22));
    return res;
  }

void XC::SimpleBearingBase::Print(std::ostream &s, int flag) const
  {
    if(flag == 0)
      {
        // print everything
	s << "Element: " << this->getTag(); 
	//s << "  type: SimpleBearingBase  iNode: " << connectedExternalNodes(0);
	//s << "  jNode: " << connectedExternalNodes(1) << std::endl;
	frictionModels.Print(s, flag);
        s << std::endl
	  << "  Material ux: " << physicalProperties[0]->getTag() << std::endl
	  << "  Material rz: " << physicalProperties[1]->getTag() << std::endl
	  << "  shearDistI: " << shearDistI
	  << "  addRayleigh: "<< addRayleigh
	  << "  mass: " << mass << "  maxIter: " << maxIter
	  << "  tol: " << tol << std::endl;
        // determine resisting forces in global system
        s << "  resisting force: " << this->getResistingForce() << std::endl;
      }
    else if(flag == 1)
      {
	// does nothing
      }
  }

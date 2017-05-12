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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/GenericSection1d.cpp,v $
                                                                        
                                                                        
// File: ~/material/GenericSection1d.C
//
// Written: MHS 
// Created: Apr 2000
// Revision: A
//
// Description: This file contains the class implementation for XC::GenericSection1d.
//
// What: "@(#) GenericSection1d.C, revA"

#include "GenericSection1d.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include <domain/mesh/element/utils/Information.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cstring>
#include "material/section/ResponseId.h"

XC::Vector XC::GenericSection1d::s(1);
XC::Matrix XC::GenericSection1d::ks(1,1);
XC::ResponseId XC::GenericSection1d::c(1);

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d(int tag, UniaxialMaterial &m, int type)
  :PrismaticBarCrossSection(tag,SEC_TAG_Generic1d), code(type)
  {
    theModel = m.getCopy();

    if(!theModel)
      {
        std::cerr << "XC::GenericSection1d::GenericSection1d  -- failed to get copy of material model\n";
        exit(-1);
      }
  }

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d(int tag)
:PrismaticBarCrossSection(0,SEC_TAG_Generic1d), theModel(0), code(0)
  {}

//! @brief Constructor.
XC::GenericSection1d::GenericSection1d()
:PrismaticBarCrossSection(0,SEC_TAG_Generic1d), theModel(0), code(0)
  {}

//! @brief Destructor.
XC::GenericSection1d::~GenericSection1d(void)
  { if (theModel) delete theModel; }

//! @brief Sets the cross-section initial strain
//! (generalized: axial and bending).
int XC::GenericSection1d::setInitialSectionDeformation(const Vector &def)
  { return theModel->setInitialStrain(def(0)); }

//! @brief Sets the cross-section trial strain
//! (generalized: axial and bending).
int XC::GenericSection1d::setTrialSectionDeformation(const Vector &def)
  { return theModel->setTrialStrain(def(0)); }

//! @brief Zeroes initial strain.
void XC::GenericSection1d::zeroInitialSectionDeformation(void)
  { theModel->setInitialStrain(0.0); }

//! @brief Returns the cross-section initial strain
//! (generalized: axial and bending).
const XC::Vector &XC::GenericSection1d::getInitialSectionDeformation(void) const
  {
    static Vector e(1); // static for class-wide returns
    e(0) = theModel->getInitialStrain();
    return e;
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::GenericSection1d::getSectionDeformation(void) const
  {
    static Vector e(1); // static for class-wide returns
    e(0) = theModel->getStrain();
    return e;
  }

//! @brief Return the stress resultant over the section.
const XC::Vector &XC::GenericSection1d::getStressResultant(void) const
  {
    s(0) = theModel->getStress();
    return s;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::GenericSection1d::getSectionTangent(void) const
  {
    ks(0,0) = theModel->getTangent();
    return ks;
  }

//! @brief Returns the valor inicial de la tangent stiffness matrix.
const XC::Matrix &XC::GenericSection1d::getInitialTangent(void) const
  {
    ks(0,0) = theModel->getInitialTangent();
    return ks;
  }

//! @brief Return the matriz de flexibilidad.
const XC::Matrix &XC::GenericSection1d::getSectionFlexibility(void) const
  {
    double tangent = theModel->getTangent();
    if(tangent != 0.0)
      ks(0,0) = 1.0/tangent;
    else
      ks(0,0) = 1.0e12;
    return ks;
  }

//! @brief Returns the valor inicial la matriz de flexibilidad.
const XC::Matrix &XC::GenericSection1d::getInitialFlexibility(void) const
  {
    double tangent = theModel->getInitialTangent();
    ks(0,0) = 1.0/tangent;
    return ks;
  }

//! @brief Returns the index of the commited state.
int XC::GenericSection1d::commitState(void)
  { return theModel->commitState(); }

//! @brief Returns to the last commited state.
int XC::GenericSection1d::revertToLastCommit ()
  { return theModel->revertToLastCommit(); }

//! @brief Returns to the initial state.
int XC::GenericSection1d::revertToStart ()
  { return theModel->revertToStart(); }

//! @brief Returns the tipo de respuesta.
const XC::ResponseId &XC::GenericSection1d::getType(void) const
  {
    c(0)= code;
    return c;
  }

int XC::GenericSection1d::getOrder(void) const
  { return 1; }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::GenericSection1d::getCopy(void) const
  { return new GenericSection1d(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::GenericSection1d::sendData(CommParameters &cp)
  {
    int res= PrismaticBarCrossSection::sendData(cp);
    setDbTagDataPos(5,code);
    res+= cp.sendBrokedPtr(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::GenericSection1d::recvData(const CommParameters &cp)
  {
    int res= PrismaticBarCrossSection::recvData(cp);
    code= getDbTagDataPos(5);
    theModel= cp.getBrokedMaterial(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    return res;
  }

//! @brief Sends object members through the channel being passed as parameter.
int XC::GenericSection1d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::GenericSection1d::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::GenericSection1d::Print (std::ostream &s, int flag)
{
    s << "GenericSection1d (Uniaxial), tag: " << this->getTag() << std::endl;
    s << "\tResponse code: " << code << std::endl;
    s << "\tUniaxialMaterial: " << theModel->getTag() << std::endl;
}

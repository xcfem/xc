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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/GenericSectionNd.cpp,v $
                                                                        
                                                                        
// File: ~/material/GenericSectionNd.C
//
// Written: MHS 
// Created: Apr 2000
// Revision: A
//
// Description: This file contains the class implementation for XC::GenericSectionNd.
//
// What: "@(#) GenericSectionNd.C, revA"

#include "GenericSectionNd.h"
#include <material/nD/NDMaterial.h>
#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include <domain/mesh/element/Information.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

#include <classTags.h>

#include <cstring>
#include "material/section/ResponseId.h"

XC::GenericSectionNd::GenericSectionNd(int tag, XC::NDMaterial &m, const XC::ResponseId &mCode)
  : SectionForceDeformation(tag,SEC_TAG_GenericNd), theModel(0), code(0)
  {
    theModel = m.getCopy();

    if(theModel == 0)
      {
        std::cerr << "XC::GenericSectionNd::GenericSectionNd -- failed to get copy of material model"
                  << std::endl;
      }

    order= theModel->getOrder();
    code= new XC::ResponseId(mCode);

    if(!code)
      {
        std::cerr << "XC::GenericSectionNd::GenericSectionNd -- failed to allocate section XC::ID"
                  << std::endl;
      }

    if(order != code->Size())
      {
        std::clog << "¡Ojo!, XC::GenericSectionNd::GenericSectionNd -- code size does not match order of material model" << std::endl;
      }
  }

XC::GenericSectionNd::GenericSectionNd(int tag)
  :SectionForceDeformation(tag,SEC_TAG_GenericNd), theModel(0), code(0), order(0)
  {}

XC::GenericSectionNd::GenericSectionNd()
:SectionForceDeformation(0,SEC_TAG_GenericNd), theModel(0), code(0), order(0)
  {}

XC::GenericSectionNd::~GenericSectionNd(void)
  {
    if(theModel) delete theModel;
    if(code) delete code;
  }

int XC::GenericSectionNd::setInitialSectionDeformation(const Vector& def)
  {
    std::cerr << "GenericSectionNd::setInitialSectionDeformation no implementada." << std::endl;
    //return theModel->setInitialStrain(def);
    return 0;
  }

int XC::GenericSectionNd::setTrialSectionDeformation(const Vector& def)
  { return theModel->setTrialStrain(def); }

const XC::Vector &XC::GenericSectionNd::getInitialSectionDeformation(void) const
  {
    std::cerr << "GenericSectionNd::getInitialStrain no implementada." << std::endl;
    return theModel->getStrain();
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::GenericSectionNd::getSectionDeformation(void) const
  { return theModel->getStrain(); }


//! @brief Returns strain at position being passed as parameter.
double XC::GenericSectionNd::getStrain(const double &,const double &) const
  {
    std::cerr << "getStrain not implemented for class: "
              << nombre_clase() << std::endl;
    return 0.0;
  }

const XC::Vector &XC::GenericSectionNd::getStressResultant(void) const
  { return theModel->getStress(); }

const XC::Matrix &XC::GenericSectionNd::getSectionTangent(void) const
  { return theModel->getTangent(); }

const XC::Matrix &XC::GenericSectionNd::getInitialTangent(void) const
  { return theModel->getInitialTangent(); }

int XC::GenericSectionNd::commitState()
  { return theModel->commitState(); }

int XC::GenericSectionNd::revertToLastCommit()
  { return theModel->revertToLastCommit(); }

int XC::GenericSectionNd::revertToStart()
  { return theModel->revertToStart(); }

const XC::ResponseId &XC::GenericSectionNd::getType(void) const
  { return *code; }

int XC::GenericSectionNd::getOrder(void) const
  { return order; }

XC::SectionForceDeformation *XC::GenericSectionNd::getCopy(void) const
  { return new XC::GenericSectionNd(*this); }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::GenericSectionNd::sendData(CommParameters &cp)
  {
    int res= SectionForceDeformation::sendData(cp);
    setDbTagDataPos(5,order);
    res+= cp.sendBrokedPtr(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    res+= cp.sendIDPtr(code,getDbTagData(),ArrayCommMetaData(9,10,11));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::GenericSectionNd::recvData(const CommParameters &cp)
  {
    int res= SectionForceDeformation::recvData(cp);
    order= getDbTagDataPos(5);
    theModel= cp.getBrokedMaterial(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    code= cp.receiveResponseIdPtr(code,getDbTagData(),ArrayCommMetaData(9,10,11));
    return res;
  }

int XC::GenericSectionNd::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(12);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::GenericSectionNd::recvSelf(const CommParameters &cp)
  {
    inicComm(12);
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

void XC::GenericSectionNd::Print (std::ostream &s, int flag)
  {
    s << "Generic Section Nd, tag: " << this->getTag() << std::endl;
    s << "\tsection code: " << code << std::endl;
  }

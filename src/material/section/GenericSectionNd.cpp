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
#include <domain/mesh/element/utils/Information.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

#include <classTags.h>

#include <cstring>
#include "material/section/ResponseId.h"

//! @brief Frees memory.
void XC::GenericSectionNd::free(void)
  {
    if(theModel)
      {
	delete theModel;
	theModel= nullptr;
      }
    order= 0;
    if(code)
      {
	delete code;
	code= nullptr;
      }
  }

//! @brief Allocates memory.
void XC::GenericSectionNd::alloc(const NDMaterial *m, const ResponseId *mCode)
  {
    free();
    if(m && mCode)
      {
        theModel= m->getCopy();

        if(!theModel)
          std::clog << getClassName() << "::" << __FUNCTION__
                    << "; failed to get copy of material model."
                    << std::endl;

	order= theModel->getOrder();
	code= new ResponseId(*mCode);

	if(!code)
	  std::clog << getClassName() << "::" << __FUNCTION__
		    << "; failed to allocate section ID"
		    << std::endl;

	if(order != code->Size())
	  std::clog << getClassName() << "::" << __FUNCTION__
		    << "; Warning! code size does not match"
		    << " order of material model" << std::endl;
      }
  }

//! @brief Constructor.
//!
//! @param tag: object identifier.
//! @param m: ND material.
//! @param mCode: material response identifiers. 
XC::GenericSectionNd::GenericSectionNd(int tag, const NDMaterial &m, const ResponseId &mCode)
  : SectionForceDeformation(tag,SEC_TAG_GenericNd),
    theModel(nullptr), code(nullptr)
  { alloc(&m,&mCode); }

//! @brief Constructor.
XC::GenericSectionNd::GenericSectionNd(int tag)
  :SectionForceDeformation(tag,SEC_TAG_GenericNd),
   theModel(nullptr), code(nullptr), order(0)
  {}

//! @brief Copy constructor.
XC::GenericSectionNd::GenericSectionNd(const GenericSectionNd &otro)
  : SectionForceDeformation(otro), theModel(nullptr), code(nullptr),
    order(0)
  { alloc(otro.theModel,otro.code); }

//! @brief Assignment operator.
XC::GenericSectionNd &XC::GenericSectionNd::operator=(const GenericSectionNd &otro)
  {
    SectionForceDeformation::operator=(otro);
    alloc(otro.theModel,otro.code);
    return *this;
  }

//! @brief Destructor.
XC::GenericSectionNd::~GenericSectionNd(void)
  { free(); }

//! @brief Set initial generalized strain.
int XC::GenericSectionNd::setInitialSectionDeformation(const Vector& def)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    //return theModel->setInitialStrain(def);
    return 0;
  }

//! @brief Set trial generalized strain.
int XC::GenericSectionNd::setTrialSectionDeformation(const Vector& def)
  { return theModel->setTrialStrain(def); }

//! @brief Zeroes initial generalized strain.
void XC::GenericSectionNd::zeroInitialSectionDeformation(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << ";not implemented." << std::endl;
  }

//! @brief Return the initial generalized strain.
const XC::Vector &XC::GenericSectionNd::getInitialSectionDeformation(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << ";not implemented." << std::endl;
    return theModel->getStrain();
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::GenericSectionNd::getSectionDeformation(void) const
  { return theModel->getStrain(); }


//! @brief Returns strain at position being passed as parameter.
double XC::GenericSectionNd::getStrain(const double &,const double &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << ";not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the the generalized stress vector.
const XC::Vector &XC::GenericSectionNd::getStressResultant(void) const
  { return theModel->getStress(); }

//! @brief Return the stiffness matrix.
const XC::Matrix &XC::GenericSectionNd::getSectionTangent(void) const
  { return theModel->getTangent(); }

//! @brief Return the initial stiffness matrix.
const XC::Matrix &XC::GenericSectionNd::getInitialTangent(void) const
  { return theModel->getInitialTangent(); }

//! @brief Commits the current state.
int XC::GenericSectionNd::commitState()
  { return theModel->commitState(); }

//! @brief Returns the material to its last commited state.
int XC::GenericSectionNd::revertToLastCommit()
  { return theModel->revertToLastCommit(); }

//! @brief Returns the material to its initial state.
int XC::GenericSectionNd::revertToStart()
  { return theModel->revertToStart(); }

//! @brief Returns the section ID code that indicates the type
//! of response quantities returned by this instance of GenericSectionND.
const XC::ResponseId &XC::GenericSectionNd::getType(void) const
  { return *code; }

//! @brief Returns the result of invoking getOrder() on the NDMaterial.
int XC::GenericSectionNd::getOrder(void) const
  { return order; }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::GenericSectionNd::getCopy(void) const
  { return new GenericSectionNd(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::GenericSectionNd::sendData(CommParameters &cp)
  {
    int res= SectionForceDeformation::sendData(cp);
    setDbTagDataPos(5,order);
    res+= cp.sendBrokedPtr(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    res+= cp.sendIDPtr(code,getDbTagData(),ArrayCommMetaData(9,10,11));
    return res;
  }

//! @brief Receives object members through the channel
//! being passed as parameter.
int XC::GenericSectionNd::recvData(const CommParameters &cp)
  {
    int res= SectionForceDeformation::recvData(cp);
    order= getDbTagDataPos(5);
    theModel= cp.getBrokedMaterial(theModel,getDbTagData(),BrokedPtrCommMetaData(6,7,8));
    code= cp.receiveResponseIdPtr(code,getDbTagData(),ArrayCommMetaData(9,10,11));
    return res;
  }

//! @brief Sends the object
int XC::GenericSectionNd::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(12);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives the object.
int XC::GenericSectionNd::recvSelf(const CommParameters &cp)
  {
    inicComm(12);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::GenericSectionNd::Print(std::ostream &s, int flag)
  {
    s << "Generic Section Nd, tag: " << this->getTag() << std::endl;
    s << "\tsection code: " << code << std::endl;
  }

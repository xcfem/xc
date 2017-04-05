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
// $Date: 2003/03/04 00:48:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/DomainComponent.cpp,v $
                                                                        
                                                                        
// File: ~/domain/component//DomainComponent.C
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class implementation for XC::DomainComponent
//
// What: "@(#) DomainComponent.h, revA"


#include <domain/component/DomainComponent.h>
#include <domain/domain/Domain.h>
#include "preprocessor/Preprocessor.h"
#include "boost/any.hpp"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::DomainComponent::DomainComponent(int tag, int classTag)
  :TaggedObject(tag), MovableObject(classTag), theDomain(nullptr), idx(0)
  {}

//! @brief Destructor.
XC::DomainComponent::~DomainComponent(void)
  { setDomain(nullptr); }

//! @brief Asigna a pointer al domain. 
void XC::DomainComponent::setDomain(Domain *model)
  { theDomain = model; }

//! @brief Asigna el índice al objeto (ver numera en Set).
void XC::DomainComponent::set_indice(const size_t &i)
  { idx= i; }

//! @brief Returns a pointer to the domain.
XC::Domain *XC::DomainComponent::getDomain(void) const
  { return theDomain; }

//! @brief Returns (if possible) a pointer to the preprocessor.
const XC::Preprocessor *XC::DomainComponent::GetPreprocessor(void) const
  {
    const Preprocessor *retval= nullptr;
    if(theDomain)
      retval= theDomain->GetPreprocessor();
    return retval;
  }

//! @brief Returns (if possible) a pointer to the preprocessor.
XC::Preprocessor *XC::DomainComponent::GetPreprocessor(void)
  {
    Preprocessor *retval= nullptr;
    if(theDomain)
      retval= theDomain->GetPreprocessor();
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::DomainComponent::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendSzt(idx,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::DomainComponent::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveSzt(idx,getDbTagData(),CommMetaData(1));
    return res;
  }

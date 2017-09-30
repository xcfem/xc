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
** (C) Copyright 2001, The Regents of the University of California    **
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
// $Date: 2007/10/31 17:49:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/MatParameter.cpp,v $

// written: fmk

#include "MatParameter.h"
#include "classTags.h"
#include "domain/component/DomainComponent.h"

#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"
#include <utility/actor/message/Message.h>
#include <utility/matrix/ID.h>

XC::MatParameter::MatParameter(int theTag, int materialTag,const std::string &materialParameterName)
:Parameter(theTag, PARAMETER_TAG_MatParameter),
 theParameterName(materialParameterName), theMaterialTag(materialTag), theParameterID(-1) {}

XC::MatParameter::MatParameter(void)
  :Parameter(), 
   theParameterName(""), theMaterialTag(0), theParameterID(-1)
{

}

void XC::MatParameter::Print(std::ostream &s, int flag)  
{
  s << "MaterialParameter, tag = " << this->getTag() << std::endl;
}

void XC::MatParameter::setDomain(Domain *theDomain)  
{
  Element *theEle;
  ElementIter &theEles = theDomain->getElements();

  int theResult = -1;

  
    std::vector<std::string> theString; 
  char materialIdTag[10];
  sprintf(materialIdTag,"%d",theMaterialTag);
  theString[0] = theParameterName;
  theString[1] = materialIdTag;

  // note because of the way this parameter is updated only need to find one in the domain
  while((theEle = theEles()) != 0)
    {
      int result = theEle->setParameter(theString,*this);
      if(result != -1)
	theResult = result;
    }
	
  if(theResult == -1) 
    std::cerr << "MatParameter::setDomain(Domain *theDomain) - NO RESULT\n";
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::MatParameter::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendMovable(theMatInfo,getDbTagData(),CommMetaData(1));
    res+= cp.sendString(theParameterName,getDbTagData(),CommMetaData(2));
    res+= cp.sendInts(theMaterialTag,theParameterID,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::MatParameter::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveMovable(theMatInfo,getDbTagData(),CommMetaData(1));
    res+= cp.receiveString(theParameterName,getDbTagData(),CommMetaData(2));
    res+= cp.receiveInts(theMaterialTag,theParameterID,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::MatParameter::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::MatParameter::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

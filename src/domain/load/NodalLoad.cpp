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
// $Date: 2005/11/23 22:48:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/NodalLoad.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of XC::NodalLoad

#include "NodalLoad.h"
#include "domain/component/Parameter.h"
#include <domain/mesh/node/Node.h>
#include <cstdlib>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/utils/Information.h>
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"

#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

// AddingSensitivity:BEGIN /////////////////////////////////////
 XC::Vector XC::NodalLoad::gradientVector(1);
// AddingSensitivity:END ///////////////////////////////////////

//! @brief Constructor.
XC::NodalLoad::NodalLoad(int tag, int theClassTag)
  : Load(tag,theClassTag), myNode(0), myNodePtr(nullptr), load(), konstant(false)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////
  }

//! @brief Constructor.
XC::NodalLoad::NodalLoad(int tag, int node, int theClassTag)
  : Load(tag,theClassTag), myNode(node), myNodePtr(nullptr), load(), konstant(false)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////
  }


//! @brief Constructor.
XC::NodalLoad::NodalLoad(int tag, int node, const Vector &theLoad, bool isLoadConstant)
  :Load(tag, LOAD_TAG_NodalLoad), myNode(node), myNodePtr(nullptr), load(theLoad), konstant(isLoadConstant)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////
  }


void  XC::NodalLoad::setDomain(Domain *newDomain)
  {
    // first get myNodePtr
    if(newDomain == 0)
      return;

    // invoke the ancestor class method
    this->DomainComponent::setDomain(newDomain);    

    /*
    if(newDomain)
      {
        myNodePtr = newDomain->getNode(myNode);
        if(myNodePtr == 0)
          {
            std::cerr << *newDomain;
            std::cerr << "WARNING XC::NodalLoad::setDomain() - No associated XC::Node node " ;
            std::cerr << " for XC::NodalLoad " << *this;
            //        std::cerr << *newDomain;
            return;
          }
      }
    */
  }

int XC::NodalLoad::getNodeTag(void) const
  { return myNode; }

//! @brief Returns the componentes del vector fuerza.
const XC::Vector &XC::NodalLoad::getForce(void) const
  {
    static Vector retval(3);
    retval.Zero();
    if(!myNodePtr)
      myNodePtr= const_cast<NodalLoad *>(this)->get_node_ptr();
    if(!load.Nulo() && myNodePtr)
      {
        const size_t numGdl= myNodePtr->getNumberDOF();
        const size_t dim= myNodePtr->getCrds().Size();
        switch(numGdl)
          {
          case 1:
            retval(0)= load(0);
            break;
          case 2:
            retval(0)= load(0);
            retval(1)= load(1);
            break;
          case 3:
            if(dim == 2)
              {
                retval(0)= load(0);
                retval(1)= load(1);
              }
            else if(dim == 3)
              {
                retval(0)= load(0);
                retval(1)= load(1);
                retval(2)= load(2);
              }
            break;
          case 6:
            retval(0)= load(0);
            retval(1)= load(1);
            retval(2)= load(2);
            break;
          default:
            std::cerr << "Error en BeamMecLoad::getForce." << std::endl;
            break;
          }
      }
    else
      {
        if(load.Nulo())
          std::cerr << "NodalLoad::getForce; no se ha definido la carga." << std::endl;
        if(!myNodePtr)
          std::cerr << "NodalLoad::getForce; the pointer a nodo es nulo." << std::endl;
      }
    return retval;
  }

//! @brief Returns the componentes del vector fuerza.
const XC::Vector &XC::NodalLoad::getMoment(void) const
  {
    static Vector retval(3);
    retval.Zero();
    if(!myNodePtr)
      myNodePtr= const_cast<NodalLoad *>(this)->get_node_ptr();
    if(!load.Nulo() && myNodePtr)
      {
        const size_t numGdl= myNodePtr->getNumberDOF();
        const size_t dim= myNodePtr->getCrds().Size();
        switch(numGdl)
          {
          case 3:
            if(dim == 2)
              retval(0)= load(2);
            break;
          case 6:
            retval(0)= load(3);
            retval(1)= load(4);
            retval(2)= load(5);
            break;
          default:
            std::cerr << "Error en BeamMecLoad::getMoment." << std::endl;
            break;
          }
      }
    else
      {
        if(load.Nulo())
          std::cerr << "NodalLoad::getMoment; no se ha definido la carga." << std::endl;
        if(!myNodePtr)
          std::cerr << "NodalLoad::getMoment; the pointer a nodo es nulo." << std::endl;
      }
    return retval;
  }

//! @brief Returns a pointer to the loaded node.
const XC::Node *XC::NodalLoad::get_node_ptr(void) const
  {
    const Node *retval= nullptr;
    Domain *theDomain= getDomain();
    if(!theDomain)
      std::cerr << "NodalLoad::get_node_ptr(), pointer to domain nulo.";
    else
      {
        retval= theDomain->getNode(myNode);
        if(!retval)
          {
            std::cerr << "NodalLoad::get_node_ptr() - No se encontró el nodo:"
                      << myNode << std::endl;
          }
      }
    return retval;
  }

//! @brief Returns a pointer to the loaded node.
XC::Node *XC::NodalLoad::get_node_ptr(void)
  {
    return const_cast<Node *>(static_cast<const NodalLoad*>(this)->getNode());
  }


void XC::NodalLoad::applyLoad(double loadFactor)
  {
    if(!myNodePtr)
      myNodePtr= get_node_ptr();

    // add the load times the load factor to nodal unbalanced load
    if(konstant == false)
      myNodePtr->addUnbalancedLoad(load,loadFactor);
    else
      myNodePtr->addUnbalancedLoad(load,1.0);
  } 

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodalLoad::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Send members del objeto a través del
//! canal being passed as parameter.
int XC::NodalLoad::sendData(CommParameters &cp)
  {
    int res= Load::sendData(cp);
    res+= cp.sendInts(myNode,parameterID,getDbTagData(),CommMetaData(2));
    res+= cp.sendVector(load,getDbTagData(),CommMetaData(3));
    res+= cp.sendBool(konstant,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives members del objeto a través del
//! canal being passed as parameter.
int XC::NodalLoad::recvData(const CommParameters &cp)
  {
    int res= Load::recvData(cp);
    res+= cp.receiveInts(myNode,parameterID,getDbTagData(),CommMetaData(2));
    res+= cp.receiveVector(load,getDbTagData(),CommMetaData(3));
    res+= cp.receiveBool(konstant,getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::NodalLoad::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int result= sendData(cp);
    
    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "NodalLoad::sendSelf - failed to send data\n";
    return result;
  }

int XC::NodalLoad::recvSelf(const CommParameters &cp)
  {        
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NodalLoad::recvSelf() - failed to recv data\n";
    else
      res+= recvData(cp);
    return res;
  }


void XC::NodalLoad::Print(std::ostream &s, int flag)
  {
     s << "Nodal Load: " << myNode;
     s << " load : " << load;
  }

// AddingSensitivity:BEGIN /////////////////////////////////////
int XC::NodalLoad::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv.size() < 1)
      return -1;
    else if(argv[0] == "1")
      return param.addObject(1, this);
    else if(argv[0] == "2")
      return param.addObject(2, this);
    else if(argv[0] == "3")
      return param.addObject(3, this);
    else if(argv[0] == "4")
      return param.addObject(4, this);
    else if(argv[0] == "5")
      return param.addObject(5, this);
    else if(argv[0] == "6")
      return param.addObject(6, this);
    else
      return -1;
  }

int XC::NodalLoad::updateParameter(int parameterID, Information &info)
  {
    //int nn; 
    //Domain *theDomain;
    switch (parameterID)
      {
      case -1:
        return -1;
      case 1:
        load(0) = info.theDouble;
        return 0;
      case 2:
        load(1) = info.theDouble;
        return 0;
      case 3:
        load(2) = info.theDouble;
        return 0;
      default:
        return -1;
      }
  }


int XC::NodalLoad::activateParameter(int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }


const XC::Vector &XC::NodalLoad::getExternalForceSensitivity(int gradNumber)
  {
    gradientVector(0) = (double)parameterID;
    return gradientVector;
  }


// AddingSensitivity:END //////////////////////////////////////


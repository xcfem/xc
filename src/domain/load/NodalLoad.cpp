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
//!
//! @param tag: nodal load identifier.
//! @param classTag: identifier of the class.
XC::NodalLoad::NodalLoad(int tag, int classTag)
  : Load(tag,classTag), loadedNode(0), loadedNodePtr(nullptr),
    load(), konstant(false)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////
  }

//! @brief Constructor.
//!
//! @param tag: nodal load identifier.
//! @param nodeTag: identifier of the loaded node.
//! @param classTag: identifier of the class.
XC::NodalLoad::NodalLoad(int tag, int nodeTag, int classTag)
  : Load(tag,classTag), loadedNode(nodeTag), loadedNodePtr(nullptr),
    load(), konstant(false)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////
  }


//! @brief Constructor.
//!
//! To create a NodalLoad object with tag \p tag acting on Node \p node with a
//! reference load given by \p load. \p tag and {\em LOAD\_TAG\_NodalLoad}
//! (defined in \f$<\f$classTags.h\f$>\f$)are passed to the Load constructor.
//! A new vector object is created using
//! the vector \p load as the argument. (THIS MAY CHANGE - may associate
//! the load Vector with \p load, allowing identical loads on diff
//! nodes to be created without the need for each to have its own
//! vector) If no enough memory is available an error message is printed
//! and the program terminates. The boolean \p isLoadConstant is used
//! to indicate whether the value of the load applies to the Node is
//! independent of the load factor.
//!
//! @param tag: nodal load identifier.
//! @param nodeTag: identifier of the loaded node.
//! @param theLoad: load value.
//! @param isLoadConstant: true if load doesn't change with time.
XC::NodalLoad::NodalLoad(int tag, int nodeTag, const Vector &theLoad, bool isLoadConstant)
  : Load(tag, LOAD_TAG_NodalLoad), loadedNode(nodeTag), loadedNodePtr(nullptr),
    load(theLoad), konstant(isLoadConstant)
  {
    // AddingSensitivity:BEGIN /////////////////////////////////////
    parameterID = 0;
    // AddingSensitivity:END ///////////////////////////////////////
  }


//! @brief To set the associated Domain object.
//! 
//! To set the associated Domain object.
void  XC::NodalLoad::setDomain(Domain *newDomain)
  {
    // first get loadedNodePtr
    if(newDomain)
      this->DomainComponent::setDomain(newDomain); // invoke the ancestor class method.

    /*
    if(newDomain)
      {
        loadedNodePtr = newDomain->getNode(loadedNode);
        if(loadedNodePtr == 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; WARNING - No associated node " 
                      << " for NodalLoad " << *this
                      << *newDomain;
            return;
          }
      }
    */
  }

//! Returns the identifier of the loaded node.
int XC::NodalLoad::getNodeTag(void) const
  { return loadedNode; }

//! @brief Returns the componentes of the force vector.
const XC::Vector &XC::NodalLoad::getForce(void) const
  {
    static Vector retval(3);
    retval.Zero();
    if(!loadedNodePtr)
      loadedNodePtr= const_cast<NodalLoad *>(this)->get_node_ptr();
    if(!load.isEmpty() && loadedNodePtr)
      {
        const size_t numGdl= loadedNodePtr->getNumberDOF();
        const size_t dim= loadedNodePtr->getCrds().Size();
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
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; ERROR numGdl= " << numGdl
	              << " out of range." << std::endl;
            break;
          }
      }
    else
      {
        if(load.isEmpty())
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; load not defined." << std::endl;
        if(!loadedNodePtr)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; pointer to node is NULL." << std::endl;
      }
    return retval;
  }

//! @brief Returns the components of the moment.
const XC::Vector &XC::NodalLoad::getMoment(void) const
  {
    static Vector retval(3);
    retval.Zero();
    if(!loadedNodePtr)
      loadedNodePtr= const_cast<NodalLoad *>(this)->get_node_ptr();
    if(!load.isEmpty() && loadedNodePtr)
      {
        const size_t numGdl= loadedNodePtr->getNumberDOF();
        const size_t dim= loadedNodePtr->getCrds().Size();
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
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; ERROR numGdl= " << numGdl
	              << " out of range." << std::endl;
            break;
          }
      }
    else
      {
        if(load.isEmpty())
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; load not defined." << std::endl;
        if(!loadedNodePtr)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; the pointer to node is NULL." << std::endl;
      }
    return retval;
  }

//! @brief Returns a pointer to the loaded node.
const XC::Node *XC::NodalLoad::get_node_ptr(void) const
  {
    const Node *retval= nullptr;
    Domain *theDomain= getDomain();
    if(!theDomain)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; domain not set.";
    else
      {
        retval= theDomain->getNode(loadedNode);
        if(!retval)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; node identified by: "
                      << loadedNode << " not found." << std::endl;
          }
      }
    return retval;
  }

//! @brief Returns a pointer to the loaded node.
XC::Node *XC::NodalLoad::get_node_ptr(void)
  {
    return const_cast<Node *>(static_cast<const NodalLoad*>(this)->getNode());
  }

//! @brief Applies the load on the node.
//! 
//! To it's associated Node it invokes addUnbalancedLoad() with it's
//! load and a factor of \p loadFactor if \p isLoadConstant was specified
//! as \p false in the constructor or \f$1.0\f$ if it was specified
//! as \p true. 
void XC::NodalLoad::applyLoad(double loadFactor)
  {
    if(!loadedNodePtr)
      loadedNodePtr= get_node_ptr();

    // add the load times the load factor to nodal unbalanced load
    if(konstant == false)
      loadedNodePtr->addUnbalancedLoad(load,loadFactor);
    else
      loadedNodePtr->addUnbalancedLoad(load,1.0);
  } 

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodalLoad::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Sends object member through the communicator
//! being passed as parameter.
int XC::NodalLoad::sendData(CommParameters &cp)
  {
    int res= Load::sendData(cp);
    res+= cp.sendInts(loadedNode,parameterID,getDbTagData(),CommMetaData(2));
    res+= cp.sendVector(load,getDbTagData(),CommMetaData(3));
    res+= cp.sendBool(konstant,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object member through the communicator
//! being passed as parameter.
int XC::NodalLoad::recvData(const CommParameters &cp)
  {
    int res= Load::recvData(cp);
    res+= cp.receiveInts(loadedNode,parameterID,getDbTagData(),CommMetaData(2));
    res+= cp.receiveVector(load,getDbTagData(),CommMetaData(3));
    res+= cp.receiveBool(konstant,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends the object.
int XC::NodalLoad::sendSelf(CommParameters &cp)
  {
    inicComm(7);
    int result= sendData(cp);
    
    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return result;
  }

//! @brief Receives.
int XC::NodalLoad::recvSelf(const CommParameters &cp)
  {        
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to recv data.\n";
    else
      res+= recvData(cp);
    return res;
  }

//! @brief Print stuff.
void XC::NodalLoad::Print(std::ostream &s, int flag)
  {
     s << "Nodal Load: " << loadedNode;
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


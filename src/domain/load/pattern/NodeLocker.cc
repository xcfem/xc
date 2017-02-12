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

// $Revision: 1.13 $
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/NodeLocker.cpp,v $

// Written: fmk 07/99
// Revised:
//
// Purpose: This file contains the method definitions for class NodeLocker.
// NodeLocker is a container class.
//
// The XC::NodeLocker interface:


#include "NodeLocker.h"
#include <cstdlib>
#include <utility/matrix/ID.h>
#include <domain/constraints/SFreedom_Constraint.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/domain/single/SingleDomSFreedom_Iter.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>



//! @brief Reserva memoria para almacenar las cargas.
void XC::NodeLocker::alloc_contenedores(void)
  {
    theSPs= new ArrayOfTaggedObjects(this,32,"SPs");

    if(!theSPs)
      {
        std::cerr << " XC::NodeLocker::NodeLocker() - ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Reserva memoria para almacenar los iteradores.
void XC::NodeLocker::alloc_iteradores(void)
  {
    theSpIter= new SingleDomSFreedom_Iter(theSPs);

    if(theSpIter == 0)
      {
        std::cerr << " XC::NodeLocker::NodeLocker() - ran out of memory\n";
        exit(-1);
      }
  }

//! @brief Constructor
XC::NodeLocker::NodeLocker(int tag, int clasTag)
  :ForceReprComponent(tag,clasTag), nextTag(0), currentGeoTag(0), lastGeoSendTag(-1),
   theSPs(nullptr), theSpIter(nullptr)
  {
    // constructor for subclass
    alloc_contenedores();
    alloc_iteradores();
  }


//! @brief Default constructor.
XC::NodeLocker::NodeLocker(void)
  :ForceReprComponent(0,PATTERN_TAG_NodeLocker), nextTag(0),currentGeoTag(0), lastGeoSendTag(-1),
   theSPs(nullptr), theSpIter(nullptr)
  {
    alloc_contenedores();
    alloc_iteradores();
  }

//! @brief Constructor.
XC::NodeLocker::NodeLocker(int tag)
  :ForceReprComponent(tag,PATTERN_TAG_NodeLocker),nextTag(0),
   currentGeoTag(0), lastGeoSendTag(-1), theSPs(nullptr), theSpIter(nullptr)
  {
    alloc_contenedores();
    alloc_iteradores();
  }

//! @brief Destructor.
XC::NodeLocker::~NodeLocker(void)
  {
    if(theSPs) delete theSPs;
    if(theSpIter) delete theSpIter;
  }


//! @brief Establece el domain para las cargas.
void XC::NodeLocker::setDomain(Domain *theDomain)
  {
    if(theSPs)
      {
        SFreedom_Constraint *theSP= nullptr;
        SFreedom_ConstraintIter &theSpConstraints= getSPs();
        while((theSP = theSpConstraints()) != 0)
          theSP->setDomain(theDomain);
      }

    // now we set this load patterns domain
    ForceReprComponent::setDomain(theDomain);
  }

//! @brief Agrega una la coacción mononodal being passed as parameter.
XC::SFreedom_Constraint *XC::NodeLocker::addSFreedom_Constraint(const int &tagNodo,const int &id_gdl,const double &valor)
  {
    SFreedom_Constraint *theSPC= new SFreedom_Constraint(nextTag,tagNodo,id_gdl,valor);
    if(theSPC)
      addSFreedom_Constraint(theSPC);
    return theSPC;
  }

//! @brief Agrega una la coacción mononodal being passed as parameter.
bool XC::NodeLocker::addSFreedom_Constraint(SFreedom_Constraint *theSp)
  {
    Domain *theDomain= this->getDomain();

    bool result= theSPs->addComponent(theSp);
    if(result == true)
      {
        nextTag++;
        if(theDomain)
          theSp->setDomain(theDomain);
        theSp->setLoadPatternTag(this->getTag());
        currentGeoTag++;
      }
    else
      std::cerr << "WARNING: XC::NodeLocker::addSFreedom_Constraint() - load could not be added\n";
    return result;
  }

//! @brief Devuelve un iterador a las coacciones mononodales.
XC::SFreedom_ConstraintIter &XC::NodeLocker::getSPs(void)
  {
    theSpIter->reset();
    return *theSpIter;
  }

//! @brief Devuelve el número de coacciones monopunto.
int XC::NodeLocker::getNumSPs(void) const
  {
    int retval= 0;
    if(theSPs)
      retval= theSPs->getNumComponents();
    return retval;
  }

//! @brief Borra todas las cargas y coacciones.
void XC::NodeLocker::clearAll(void)
  {
    theSPs->clearAll();
    currentGeoTag++;
  }

XC::SFreedom_Constraint *XC::NodeLocker::newSPConstraint(const int &tag_nod,const int &id_gdl,const double &valor)
  { return addSFreedom_Constraint(tag_nod,id_gdl,valor); }

//! @brief Elimina la coacción mononodal cuyo tag se pasa como parámetro.
bool XC::NodeLocker::removeSFreedom_Constraint(int tag)
  {
    const bool retval= theSPs->removeComponent(tag);
    if(!retval)
      std::cerr << "NodeLocker::removeSFreedom_Constraint; no se pudo eliminar la coaccion: "
                << tag << std::endl;
    return retval;
  }

//! @brief Aplica la carga en el instante being passed as parameter.
void XC::NodeLocker::applyLoad(const double &pseudoTime,const double &factor)
  {
    SFreedom_Constraint *sp= nullptr;
    SFreedom_ConstraintIter &theIter = this->getSPs();
    while((sp = theIter()) != 0)
      sp->applyConstraint(factor);
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodeLocker::getDbTagData(void) const
  {
    static DbTagData retval(3);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::NodeLocker::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendInts(nextTag,currentGeoTag,lastGeoSendTag,getDbTagData(),CommMetaData(1));
    res+= cp.sendMovable(*theSPs,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::NodeLocker::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveInts(nextTag,currentGeoTag,lastGeoSendTag,getDbTagData(),CommMetaData(1));
    const int dbTagSPs= getDbTagDataPos(2);
    res+= theSPs->recibe<SFreedom_Constraint>(dbTagSPs,cp,&FEM_ObjectBroker::getNewSP);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::NodeLocker::sendSelf(CommParameters &cp)
  {
    inicComm(3);
    int res= sendData(cp);
    const int dataTag= getDbTag(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NodeLocker::sendSelf() - failed to send extra data\n";    
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::NodeLocker::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "NodeLocker::recvSelf() - data could not be received.\n" ;
    else
      res+=recvData(cp);
    return res;
  }

//! @brief Imprime el caso de carga.
void XC::NodeLocker::Print(std::ostream &s, int flag)
  {
    s << "SPC Pattern: " << this->getTag() << "\n";
    std::cerr << "\n  Single Point Constraints: \n";
    theSPs->Print(s, flag);
  }

//! @brief Returns the constraint that affect the node and DOF
//! being passed as parameter.
//! @param theNode: node identifier.
//! @param theDOF: identifier or the degree of freedom.
std::deque<int> XC::NodeLocker::getTagsSPsNode(int theNode, int theDOF) const
  {
    std::deque<int> retval; 

    int nodeTag= 0;
    int dof= 0;
    NodeLocker *this_no_const= const_cast<NodeLocker *>(this);
    SFreedom_ConstraintIter &theSPs = this_no_const->getSPs();
    SFreedom_Constraint *theSP;
    while(((theSP = theSPs()) != 0))
      {
        nodeTag = theSP->getNodeTag();
        dof = theSP->getDOF_Number();
        if(nodeTag == theNode && dof == theDOF)
          retval.push_back(theSP->getTag());
      }
    return retval;
  }

//! @brief Returns the constraint that affect the node
//! being passed as parameter.
//! @param theNode: node identifier.
std::deque<int> XC::NodeLocker::getTagsSPsNode(int theNode) const
  {
    std::deque<int> retval; 

    int nodeTag= 0;
    NodeLocker *this_no_const= const_cast<NodeLocker *>(this);
    SFreedom_ConstraintIter &theSPs = this_no_const->getSPs();
    SFreedom_Constraint *theSP;
    while(((theSP = theSPs()) != 0))
      {
        nodeTag = theSP->getNodeTag();
        if(nodeTag == theNode)
          retval.push_back(theSP->getTag());
      }
    return retval;
  }

//! @brief Devuelve verdadero si las SPs afectan al nodo being passed as parameter.
bool XC::NodeLocker::nodeAffectedBySPs(int tagNodo) const
  {
    bool retval= false;
    NodeLocker *this_no_const= const_cast<NodeLocker *>(this);
    SFreedom_ConstraintIter &theSPs = this_no_const->getSPs();
    SFreedom_Constraint *theSP;
    while((theSP = theSPs()) != 0)
      if(theSP->getNodeTag() == tagNodo)
        {
          retval= true;
          break;
        }
    return retval;
  }


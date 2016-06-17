//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
#include <domain/constraints/SP_Constraint.h>
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/domain/single/SingleDomSP_Iter.h>
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
    theSpIter= new SingleDomSP_Iter(theSPs);

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


//! @brief Constructor por defecto.
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


//! @brief Establece el dominio para las cargas.
void XC::NodeLocker::setDomain(Domain *theDomain)
  {
    if(theSPs)
      {
        SP_Constraint *theSP= nullptr;
        SP_ConstraintIter &theSpConstraints= getSPs();
        while((theSP = theSpConstraints()) != 0)
          theSP->setDomain(theDomain);
      }

    // now we set this load patterns domain
    ForceReprComponent::setDomain(theDomain);
  }

//! @brief Agrega una la coacción mononodal being passed as parameter.
XC::SP_Constraint *XC::NodeLocker::addSP_Constraint(const int &tagNodo,const int &id_gdl,const double &valor)
  {
    SP_Constraint *theSPC= new SP_Constraint(nextTag,tagNodo,id_gdl,valor);
    if(theSPC)
      addSP_Constraint(theSPC);
    return theSPC;
  }

//! @brief Agrega una la coacción mononodal being passed as parameter.
bool XC::NodeLocker::addSP_Constraint(SP_Constraint *theSp)
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
      std::cerr << "WARNING: XC::NodeLocker::addSP_Constraint() - load could not be added\n";
    return result;
  }

//! @brief Devuelve un iterador a las coacciones mononodales.
XC::SP_ConstraintIter &XC::NodeLocker::getSPs(void)
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

XC::SP_Constraint *XC::NodeLocker::newSPConstraint(const int &tag_nod,const int &id_gdl,const double &valor)
  { return addSP_Constraint(tag_nod,id_gdl,valor); }

//! @brief Elimina la coacción mononodal cuyo tag se pasa como parámetro.
bool XC::NodeLocker::removeSP_Constraint(int tag)
  {
    const bool retval= theSPs->removeComponent(tag);
    if(!retval)
      std::cerr << "NodeLocker::removeSP_Constraint; no se pudo eliminar la coaccion: "
                << tag << std::endl;
    return retval;
  }

//! @brief Aplica la carga en el instante being passed as parameter.
void XC::NodeLocker::applyLoad(const double &pseudoTime,const double &factor)
  {
    SP_Constraint *sp= nullptr;
    SP_ConstraintIter &theIter = this->getSPs();
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
    res+= theSPs->recibe<SP_Constraint>(dbTagSPs,cp,&FEM_ObjectBroker::getNewSP);
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

//! @brief Devuelve las coacciones correspondientes al nodo
//! y grado de libertad que se pasan como parámetro.
std::deque<int> XC::NodeLocker::getTagsSPsNodo(int theNode, int theDOF) const
  {
    std::deque<int> retval; 

    int nodeTag= 0;
    int dof= 0;
    NodeLocker *this_no_const= const_cast<NodeLocker *>(this);
    SP_ConstraintIter &theSPs = this_no_const->getSPs();
    SP_Constraint *theSP;
    while(((theSP = theSPs()) != 0))
      {
        nodeTag = theSP->getNodeTag();
        dof = theSP->getDOF_Number();
        if(nodeTag == theNode && dof == theDOF)
          retval.push_back(theSP->getTag());
      }
    return retval;
  }

//! @brief Devuelve las coacciones correspondientes al nodo
//! y grado de libertad que se pasan como parámetro.
std::deque<int> XC::NodeLocker::getTagsSPsNodo(int theNode) const
  {
    std::deque<int> retval; 

    int nodeTag= 0;
    NodeLocker *this_no_const= const_cast<NodeLocker *>(this);
    SP_ConstraintIter &theSPs = this_no_const->getSPs();
    SP_Constraint *theSP;
    while(((theSP = theSPs()) != 0))
      {
        nodeTag = theSP->getNodeTag();
        if(nodeTag == theNode)
          retval.push_back(theSP->getTag());
      }
    return retval;
  }

//! @brief Devuelve verdadero si las SPs afectan al nodo being passed as parameter.
bool XC::NodeLocker::nodoAfectadoSPs(int tagNodo) const
  {
    bool retval= false;
    NodeLocker *this_no_const= const_cast<NodeLocker *>(this);
    SP_ConstraintIter &theSPs = this_no_const->getSPs();
    SP_Constraint *theSP;
    while((theSP = theSPs()) != 0)
      if(theSP->getNodeTag() == tagNodo)
        {
          retval= true;
          break;
        }
    return retval;
  }


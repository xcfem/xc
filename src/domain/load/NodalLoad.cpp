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
#include <domain/mesh/element/Information.h>
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/base/any_const_ptr.h"
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

//! @brief Devuelve las componentes del vector fuerza.
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
          std::cerr << "NodalLoad::getForce; el puntero a nodo es nulo." << std::endl;
      }
    return retval;
  }

//! @brief Devuelve las componentes del vector fuerza.
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
          std::cerr << "NodalLoad::getMoment; el puntero a nodo es nulo." << std::endl;
      }
    return retval;
  }

//! @brief Devuelve el puntero a nodo que corresponde al tag
//! que se guarda en myNode.
XC::Node *XC::NodalLoad::get_node_ptr(void)
  {
    Node *retval= nullptr;
    Domain *theDomain= getDomain();
    if(!theDomain)
      std::cerr << "NodalLoad::get_node_ptr(), puntero a dominio nulo.";
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

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodalLoad::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Envía los miembros del objeto a través del
//! canal que se pasa como parámetro.
int XC::NodalLoad::sendData(CommParameters &cp)
  {
    int res= Load::sendData(cp);
    res+= cp.sendInts(myNode,parameterID,getDbTagData(),CommMetaData(2));
    res+= cp.sendVector(load,getDbTagData(),CommMetaData(3));
    res+= cp.sendBool(konstant,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del
//! canal que se pasa como parámetro.
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


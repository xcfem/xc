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
                                                                        
// $Revision: 1.7 $
// $Date: 2005/11/07 21:36:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/UniformExcitation.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/UniformExcitation.h
//
// Written: fmk 11/98
// Revised:
//
// Purpose: This file contains the class definition for XC::UniformExcitation.
// UniformExcitation is an abstract class.

#include <domain/load/pattern/load_patterns/UniformExcitation.h>
#include <domain/load/groundMotion/GroundMotion.h>
#include <domain/load/groundMotion/GroundMotionRecord.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "xc_utils/src/base/any_const_ptr.h"

XC::UniformExcitation::UniformExcitation(int tag)
  :EarthquakePattern(tag, PATTERN_TAG_UniformExcitation),
    theMotion(nullptr), theDof(0), vel0(0.0) {}

XC::UniformExcitation::UniformExcitation(GroundMotion &_theMotion, int dof, int tag, double velZero)
  :EarthquakePattern(tag, PATTERN_TAG_UniformExcitation), theMotion(&_theMotion), theDof(dof), vel0(velZero)
  {
    // add the motion to the list of ground motions
    addMotion(*theMotion);
  }

XC::GroundMotion &XC::UniformExcitation::getGroundMotionRecord(void)
  {
    if(!theMotion)
      {
        theMotion= new GroundMotionRecord();
        assert(theMotion);
        addMotion(*theMotion);
      }
    return *theMotion;
  }

void XC::UniformExcitation::setDomain(Domain *theDomain) 
  {
    XC::EarthquakePattern::setDomain(theDomain);

    // now we go through and set all the node velocities to be vel0
    if(vel0 != 0.0)
      {
        NodeIter &theNodes = theDomain->getNodes();
        Node *theNode;
        Vector newVel(1);
        int currentSize = 1;
        while ((theNode = theNodes()) != 0)
          {
            int numDOF = theNode->getNumberDOF();
            if(numDOF != currentSize) 
	      newVel.resize(numDOF);
      
            newVel = theNode->getVel();
            newVel(theDof) = vel0;
            theNode->setTrialVel(newVel);
            theNode->commitState();
          }
      }
  }

void XC::UniformExcitation::applyLoad(double time)
  {
    Domain *theDomain = getDomain();
    if(theDomain)
      {
        //if (numNodes != theDomain->getNumNodes()) {
        NodeIter &theNodes = theDomain->getNodes();
        Node *theNode;
        while((theNode = theNodes()) != 0)
          {
            theNode->setNumColR(1);
            theNode->setR(theDof, 0, 1.0);
          }
        //  }
        EarthquakePattern::applyLoad(time);
      }
    return;
  }

void XC::UniformExcitation::applyLoadSensitivity(double time)
  {
    Domain *theDomain = this->getDomain();
    if(theDomain)
      {

        //if (numNodes != theDomain->getNumNodes()) {
        NodeIter &theNodes = theDomain->getNodes();
        Node *theNode;
        while((theNode = theNodes()) != 0)
          {
            theNode->setNumColR(1);
            theNode->setR(theDof, 0, 1.0);
          }
        //  }
        EarthquakePattern::applyLoadSensitivity(time);
      }
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::UniformExcitation::sendData(CommParameters &cp)
  {
    int res= EarthquakePattern::sendData(cp);
    res+= cp.sendInt(theDof,getDbTagData(),CommMetaData(23));
    res+= cp.sendDouble(vel0,getDbTagData(),CommMetaData(24));
    res+= sendGroundMotionPtr(theMotion,getDbTagData(),cp,BrokedPtrCommMetaData(25,26,27));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::UniformExcitation::recvData(const CommParameters &cp)
  {
    int res= EarthquakePattern::recvData(cp);
    res+= cp.receiveInt(theDof,getDbTagData(),CommMetaData(23));
    res+= cp.receiveDouble(vel0,getDbTagData(),CommMetaData(24));
    theMotion= receiveGroundMotionPtr(theMotion,getDbTagData(),cp,BrokedPtrCommMetaData(25,26,27));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::UniformExcitation::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::UniformExcitation::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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

void XC::UniformExcitation::Print(std::ostream &s, int flag)
  {
    s << "UniformExcitation  " << this->getTag() 
      << " - Not Printing the XC::GroundMotion\n";
  }

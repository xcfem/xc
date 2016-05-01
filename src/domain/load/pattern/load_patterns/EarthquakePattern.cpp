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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/02/17 01:15:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/EarthquakePattern.cpp,v $
                                                                        
// Written: fmk 11/98
// Revised:
//
// Purpose: This file contains the class definition for XC::EarthquakePattern.
// EarthquakePattern is an abstract class.

#include <domain/load/pattern/load_patterns/EarthquakePattern.h>
#include <domain/load/groundMotion/GroundMotion.h>

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <cstdlib>
#include <utility/handler/ErrorHandler.h>

#include "utility/actor/actor/ArrayCommMetaData.h"

void XC::EarthquakePattern::libera(void)
  {
    if(uDotG)
      delete uDotG;
    uDotG= nullptr;
    if(uDotDotG)
      delete uDotDotG;
    uDotDotG= nullptr;
  }

void XC::EarthquakePattern::alloc(const size_t &sz)
  {
    libera();
    if(sz)
      {
        uDotG= new Vector(sz);
        uDotDotG= new Vector(sz);
        if(uDotDotG == 0 || uDotDotG->Size() == 0 || uDotG == 0 || uDotG->Size() == 0)
          std::cerr << "EarthquakePattern::alloc - ran out of memory creating vectors\n";
      }
  }

void XC::EarthquakePattern::copia(const Vector *a,const Vector *b)
  {
    libera();
    if(a)
      {
        uDotG= new Vector(*a);
        if(!uDotG)
          {
            std::cerr << "EarthquakePattern::copia - ran out of memory constructing";
            std::cerr << " vector uDotG of size: " << a->Size() << std::endl;
          }
      }
    if(b)
      {
        uDotDotG= new Vector(*b);
        if(!uDotDotG)
          {
            std::cerr << "EarthquakePattern::copia - ran out of memory constructing";
            std::cerr << " vector uDotDotG of size: " << b->Size() << std::endl;
          }
      }
  }

XC::EarthquakePattern::EarthquakePattern(int tag, int _classTag)
  :EQBasePattern(tag, _classTag), uDotG(nullptr), uDotDotG(nullptr), currentTime(0.0), parameterID(0) {}


XC::EarthquakePattern::~EarthquakePattern(void)
  { libera(); }


void XC::EarthquakePattern::applyLoad(double time)
  {
    // see if XC::quick return, i.e. no Ground Motions or domain set
    const size_t nMotions= getNumMotions();
    if(nMotions == 0)
      return;

    // check if setLoadConstant() has been called
    if(isConstant != 0)
      currentTime = time;

    Domain *theDomain= getDomain();
    if(!theDomain)
      return;


    // set the vel and accel vector
    for(size_t i=0;i<nMotions;i++)
      {
        //    (*uDotG)(i) = theMotions[i]->getVel(currentTime);
        (*uDotDotG)(i) = theMotions[i]->getAccel(currentTime);
      }

    NodeIter &theNodes= theDomain->getNodes();
    Node *theNode;
    while((theNode = theNodes()) != 0) 
      theNode->addInertiaLoadToUnbalance(*uDotDotG, 1.0);
 

    ElementIter &theElements = theDomain->getElements();
    Element *theElement;
    while ((theElement = theElements()) != 0) 
      theElement->addInertiaLoadToUnbalance(*uDotDotG);
  }
    
void XC::EarthquakePattern::applyLoadSensitivity(double time)
  {
    // see if XC::quick return, i.e. no Ground Motions or domain set
    const size_t nMotions= getNumMotions();
    if(nMotions == 0)
      return;

    Domain *theDomain= getDomain();
    if(!theDomain)
      return;

    // set the vel and accel vector
    for(size_t i=0; i<nMotions; i++)
      {
        (*uDotG)(i) = theMotions[i]->getVel(time);
        if(parameterID != 0)
          { // Something is random in the motions
            (*uDotDotG)(i) = theMotions[i]->getAccelSensitivity(time);
          }
        else
          { (*uDotDotG)(i) = theMotions[i]->getAccel(time); }
      }
    bool somethingRandomInMotions = false;
    if(parameterID != 0)
      { somethingRandomInMotions = true; }


    NodeIter &theNodes = theDomain->getNodes();
    Node *theNode;
    while((theNode = theNodes()) != 0) 
      theNode->addInertiaLoadSensitivityToUnbalance(*uDotDotG, 1.0,  somethingRandomInMotions);

    ElementIter &theElements = theDomain->getElements();
    Element *theElement;
    while((theElement = theElements()) != 0) 
      theElement->addInertiaLoadSensitivityToUnbalance(*uDotDotG,  somethingRandomInMotions);
  }
    
int XC::EarthquakePattern::addMotion(GroundMotion &theMotion)
  {
    theMotions.addMotion(theMotion);
    const size_t nMotions= getNumMotions();
    alloc(nMotions);
    return 0;
  }


bool XC::EarthquakePattern::addSP_Constraint(SP_Constraint *)
  {
    std::cerr << "XC::EarthquakePattern::addSP_Constraint() - cannot add XC::SP_Constraint to EQ pattern\n";
    return false;
  }

bool XC::EarthquakePattern::addNodalLoad(NodalLoad *)
  {
    std::cerr << "XC::EarthquakePattern::addNodalLoad() - cannot add XC::NodalLoad to EQ pattern\n";  
    return false;
  }

bool XC::EarthquakePattern::addElementalLoad(ElementalLoad *)
  {
    std::cerr << "XC::EarthquakePattern::addElementalLoad() - cannot add XC::ElementalLoad to EQ pattern\n";    
    return false;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::EarthquakePattern::sendData(CommParameters &cp)
  {
    int res= EQBasePattern::sendData(cp);
    res+= cp.sendVectorPtr(uDotG,getDbTagData(),ArrayCommMetaData(17,18,19));
    res+= cp.sendVectorPtr(uDotDotG,getDbTagData(),ArrayCommMetaData(20,21,22));
    res+= cp.sendDouble(currentTime,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::EarthquakePattern::recvData(const CommParameters &cp)
  {
    int res= EQBasePattern::recvData(cp);
    uDotG= cp.receiveVectorPtr(uDotG,getDbTagData(),ArrayCommMetaData(17,18,19));
    uDotDotG= cp.receiveVectorPtr(uDotDotG,getDbTagData(),ArrayCommMetaData(20,21,22));
    res+= cp.receiveDouble(currentTime,getDbTagData(),CommMetaData(23));
    return res;
  }


void XC::EarthquakePattern::Print(std::ostream &s, int flag)
  { std::cerr << "XC::EarthquakePattern::Print() - not yet implemented\n"; }

// AddingSensitivity:BEGIN ////////////////////////////////////
int XC::EarthquakePattern::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // This is not general, yet
    if(argv[0] == "-randomProcessDiscretizer")
      {
        std::vector<std::string> argv2(argv);
        argv2.erase(argv2.begin(),argv2.begin()+1);
        return theMotions[0]->setParameter(argv2, param);
      }
    else
      {
        std::cerr << "Unknown parameter in XC::EarthquakePattern. " << std::endl;
        return -1;
      }
  }

int XC::EarthquakePattern::updateParameter(int parameterID, Information &info)
  { return theMotions[0]->updateParameter(parameterID,info); }

int XC::EarthquakePattern::activateParameter(int pparameterID)
  {
    parameterID = pparameterID;
    return theMotions[0]->activateParameter(pparameterID);
  }
// AddingSensitivity:END ////////////////////////////////////

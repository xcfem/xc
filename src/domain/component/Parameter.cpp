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
                                                                        
// $Revision: 1.9 $
// $Date: 2008/08/26 15:43:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/Parameter.cpp,v $

#include "DomainComponent.h"
#include "classTags.h"
#include "Parameter.h"

XC::Parameter::Parameter(int passedTag,DomainComponent *parentObject,const std::vector<std::string> &argv)
  :TaggedObject(passedTag), MovableObject(PARAMETER_TAG_Parameter),
   theComponents(), numComponents(0), maxNumComponents(0),
   theObjects(),numObjects(0), maxNumObjects(0), parameterID(), gradIndex(-1)
  {
    int ok = -1;

    maxNumObjects = initialSize;
    maxNumComponents = initialSize;
    theComponents= std::vector<DomainComponent *>(maxNumComponents,static_cast<DomainComponent *>(nullptr));
    theObjects= std::vector<MovableObject *>(maxNumObjects,static_cast<MovableObject *>(nullptr));
    parameterID= std::vector<int>(maxNumObjects,0);

    if(parentObject!=0)
      {
        ok = parentObject->setParameter(argv, *this);
        theComponents[0] = parentObject;
        numComponents = 1;
      }

    if(numObjects == 0 || ok < 0)
      {
        std::cerr << "Parameter::Parameter "<< this->getTag()
                  <<" -- no objects were able to identify parameter" << std::endl;
        for(size_t i=0;i<argv.size();i++)
          std::cerr << argv[i] << ' ';
        std::cerr << std::endl;
      }
  }

XC::Parameter::Parameter(const Parameter &param)
  : TaggedObject(param), MovableObject(PARAMETER_TAG_Parameter),
    theComponents(param.theComponents), numComponents(param.numComponents), maxNumComponents(param.maxNumComponents),
    theObjects(param.theObjects),numObjects(param.numObjects), maxNumObjects(param.maxNumObjects), parameterID(param.parameterID), gradIndex(param.gradIndex)
  {
    theInfo = param.theInfo;
  }

XC::Parameter::Parameter(int tag, int classTag)
  :TaggedObject(tag), MovableObject(classTag),
   theComponents(), numComponents(0), maxNumComponents(0),
   theObjects(),numObjects(0), maxNumObjects(0), parameterID(0), gradIndex(-1)
  {
    maxNumObjects = 1;
    maxNumComponents = 1;

    theComponents= std::vector<DomainComponent *>(maxNumComponents,static_cast<DomainComponent *>(nullptr));
    theObjects = std::vector<MovableObject *>(maxNumObjects,static_cast<MovableObject *>(nullptr));
    parameterID = std::vector<int>(maxNumObjects);
  }


XC::Parameter::Parameter(void)
  :TaggedObject(0), MovableObject(PARAMETER_TAG_Parameter), 
   theComponents(), numComponents(0), maxNumComponents(0),
   theObjects(),numObjects(0), maxNumObjects(0), parameterID(0), gradIndex(-1)
  {}

int XC::Parameter::addComponent(DomainComponent *parentObject, const std::vector<std::string> &argv)
  {
    if(numComponents == maxNumComponents)
      {
        maxNumComponents+= expandSize;
        theComponents.resize(maxNumComponents, nullptr);
      }

    theComponents[numComponents]= parentObject;
    numComponents++;

    //return (parentObject != 0) ?
    //  parentObject->setParameter(argv, *this) : -1;

    int ok = -1;

    int oldNumObjects = numObjects;
    if(parentObject != 0)
      ok = parentObject->setParameter(argv, *this);

    if(numObjects == oldNumObjects || ok < 0)
      {
        std::cerr << "Parameter::addComponent "<< this->getTag() <<" -- no objects were able to identify parameter" << std::endl;
        for(size_t i=0;i<argv.size();i++)
          std::cerr << argv[i] << ' ';
        std::cerr << std::endl;
        return -1;
      }
    return 0;
  }

int XC::Parameter::update(int newValue)
  {
    theInfo.theInt = newValue;
    int ok = 0;

    for(int i = 0; i < numObjects; i++)
      ok += theObjects[i]->updateParameter(parameterID[i], theInfo);
    return ok;
  }

int XC::Parameter::update(double newValue)
  {
    theInfo.theDouble = newValue;
    int ok = 0;
    for(int i = 0; i < numObjects; i++)
      ok += theObjects[i]->updateParameter(parameterID[i], theInfo);
    return ok;
  }

int XC::Parameter::activate(bool active)
  {
    int ok= 0;
    for(int i = 0; i < numObjects; i++)
      { ok += theObjects[i]->activateParameter(active ? parameterID[i] : 0); }
    return ok;
  }

void XC::Parameter::Print(std::ostream &s, int flag)  
  {
    s << "Parameter, tag = " << this->getTag() << std::endl;
    //s << "\tparameterID = " << parameterID << std::endl;
  }

int XC::Parameter::addObject(int paramID, MovableObject *object)
  {
    if(numObjects == maxNumObjects)
      {
        maxNumObjects+= expandSize;
        theComponents.resize(maxNumObjects, nullptr);
        parameterID.resize(maxNumObjects,0);
      }
    parameterID[numObjects]= paramID;
    theObjects[numObjects]= object;
    numObjects++;
    return 0;
  }

void XC::Parameter::setDomain(Domain *theDomain)
  { return; }

int XC::Parameter::sendSelf(CommParameters &cp)
  { return 0; }

int XC::Parameter::recvSelf(const CommParameters &cp)
  { return 0; }

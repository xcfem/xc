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
//Positioner.cpp

#include <reliability/domain/components/Positioner.h>
#include <classTags.h>
#include "domain/component/DomainComponent.h"


XC::Positioner::Positioner(int passedTag, int classTag,DomainComponent *object,const std::vector<std::string> &argv)
  :ReliabilityDomainComponent(passedTag, classTag), theObject(object), parameterID(-1)
  {
    if(theObject) 
      parameterID = theObject->setParameter(argv, theParam);
    if(parameterID < 0)
      std::cerr << "XC::Positioner::Positioner "<< this->getTag() 
                <<" -- unable to set parameter" << std::endl;
  }


int XC::Positioner::update(double newValue)
  {
    theInfo.theDouble = newValue;
    if(parameterID >= 0)
      return theObject->updateParameter(parameterID, theInfo);
    else
      return -1;
  }

int XC::Positioner::activate(bool active)
  {
    if(active)
      { theObject->activateParameter(parameterID); }
    else
      { theObject->activateParameter(0); }
    return 0;
  }


void XC::Positioner::Print(std::ostream &s, int flag)  
  {}



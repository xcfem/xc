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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Integrator.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/Integrator.C
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::Integrator.
//
// What: "@(#) Integrator.C, revA"

#include <solution/analysis/integrator/Integrator.h>
#include "solution/SoluMethod.h"
#include "solution/analysis/model/AnalysisModel.h"
#include "domain/mesh/element/RayleighDampingFactors.h"

//! @brief Constructor.
XC::Integrator::Integrator(SoluMethod *owr,int clasTag)
  :MovableObject(clasTag), EntCmd(owr) {}

void XC::Integrator::applyLoadModel(double newTime)
  { getAnalysisModelPtr()->applyLoadDomain(newTime); }

int XC::Integrator::updateModel(void)
  { return getAnalysisModelPtr()->updateDomain(); }

int XC::Integrator::updateModel(double newTime, double dT)
  { return getAnalysisModelPtr()->updateDomain(newTime,dT); }

double XC::Integrator::getCurrentModelTime(void)
  { return getAnalysisModelPtr()->getCurrentDomainTime(); }

void XC::Integrator::setCurrentModelTime(const double &t)
  { getAnalysisModelPtr()->setCurrentDomainTime(t); }

void XC::Integrator::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  { getAnalysisModelPtr()->setRayleighDampingFactors(rF); }

int XC::Integrator::commitModel(void)
  { return getAnalysisModelPtr()->commitDomain(); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
XC::SoluMethod *XC::Integrator::getSoluMethod(void)
  { return dynamic_cast<SoluMethod *>(Owner()); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
const XC::SoluMethod *XC::Integrator::getSoluMethod(void) const
  { return dynamic_cast<const SoluMethod *>(Owner()); }


//! @brief Devuelve un puntero al modelo de análisis.
const XC::AnalysisModel *XC::Integrator::getAnalysisModelPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Devuelve un puntero al modelo de análisis.
XC::AnalysisModel *XC::Integrator::getAnalysisModelPtr(void)
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Hace los cambios oportunos cuando se ha producido un cambio en el dominio.
int XC::Integrator::domainChanged(void)
  { return 0; }

void XC::Integrator::Print(std::ostream &s, int flag)
  {
    if(getAnalysisModelPtr())
      s << "\t XC::LoadPath - currentLambda: " << getCurrentModelTime() << std::endl;
    else 
      s << "\t XC::LoadPath - no associated AnalysisModel\n"; 
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::Integrator::sendData(CommParameters &cp)
  {
    return 0;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::Integrator::recvData(const CommParameters &cp)
  {
    return 0;
  }

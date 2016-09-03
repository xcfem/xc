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
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/SolutionAlgorithm.cpp,v $
                                                                        
                                                                        
#include "SolutionAlgorithm.h"
#include "solution/SoluMethod.h"
#include <utility/recorder/Recorder.h>
#include <utility/recorder/DamageRecorder.h>
#include <utility/recorder/DatastoreRecorder.h>
#include <utility/recorder/DriftRecorder.h>
#include <utility/recorder/ElementRecorder.h>
#include <utility/recorder/EnvelopeElementRecorder.h>
#include <utility/recorder/EnvelopeNodeRecorder.h>
#include <utility/recorder/FilePlotter.h>
#include <utility/recorder/NodeRecorder.h>
#include <utility/recorder/GSA_Recorder.h>
#include <utility/recorder/MaxNodeDispRecorder.h>
#include <utility/recorder/PatternRecorder.h>
#include <solution/analysis/model/AnalysisModel.h>


XC::SolutionAlgorithm::SolutionAlgorithm(SoluMethod *owr,int clasTag)
  :MovableObject(clasTag), ObjWithRecorders(owr,nullptr)
  {
    Domain *dom= get_domain_ptr();
    if(dom)
      ObjWithRecorders::setLinks(dom);
  }

//! @brief Realiza los cambios que sean necesarios como consecuencia de un cambio en el dominio.
int XC::SolutionAlgorithm::domainChanged(void)
  { return 0; }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
XC::SoluMethod *XC::SolutionAlgorithm::getSoluMethod(void)
  { return dynamic_cast<SoluMethod *>(Owner()); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
const XC::SoluMethod *XC::SolutionAlgorithm::getSoluMethod(void) const
  { return dynamic_cast<const SoluMethod *>(Owner()); }


//! @brief Devuelve un puntero al modelo de análisis.
const XC::AnalysisModel *XC::SolutionAlgorithm::getAnalysisModelPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Devuelve un puntero al modelo de análisis.
XC::AnalysisModel *XC::SolutionAlgorithm::getAnalysisModelPtr(void)
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Devuelve un puntero al integrador.
XC::Integrator *XC::SolutionAlgorithm::getIntegratorPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Devuelve un puntero al integrador.
const XC::Integrator *XC::SolutionAlgorithm::getIntegratorPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getIntegratorPtr();
  }

//! @brief Devuelve un puntero al sistema de ecuaciones.
XC::LinearSOE *XC::SolutionAlgorithm::getLinearSOEPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Devuelve un puntero al sistema de ecuaciones.
const XC::LinearSOE *XC::SolutionAlgorithm::getLinearSOEPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Devuelve un puntero al sistema de ecuaciones.
XC::EigenSOE *XC::SolutionAlgorithm::getEigenSOEPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

//! @brief Devuelve un puntero al sistema de ecuaciones.
const XC::EigenSOE *XC::SolutionAlgorithm::getEigenSOEPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getEigenSOEPtr();
  }

//! @brief Devuelve un puntero a DomainSolver.
const XC::DomainSolver *XC::SolutionAlgorithm::getDomainSolverPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getDomainSolverPtr();
  }

//! @brief Devuelve un puntero a DomainSolver.
XC::DomainSolver *XC::SolutionAlgorithm::getDomainSolverPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getDomainSolverPtr();
  }

//! @brief Devuelve un puntero al subdominio.
const XC::Subdomain *XC::SolutionAlgorithm::getSubdomainPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getSubdomainPtr();
  }

//! @brief Devuelve un puntero a DomainSolver.
XC::Subdomain *XC::SolutionAlgorithm::getSubdomainPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getSubdomainPtr();
  }

XC::Domain *XC::SolutionAlgorithm::get_domain_ptr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::SolutionAlgorithm::sendData(CommParameters &cp)
  {
    return 0;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::SolutionAlgorithm::recvData(const CommParameters &cp)
  {
    return 0;
  }

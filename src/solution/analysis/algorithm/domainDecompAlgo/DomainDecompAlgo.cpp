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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/analysis/algorithm/DomainDecompAlgo.C
// 
// Written: fmk 
// Created: Sun Sept 15 15:06:47: 1996 
// Revision: A 
//

// Description: This file contains the class definition for 
// DomainDecompAlgo. DomainDecompAlgo is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses deifine
// the sequence of operations to be performed in the analysis by static
// equilibrium of a finite element model.  
// 
// What: "@(#)DomainDecompAlgo.h, revA"

#include <solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include "domain/domain/subdomain/Subdomain.h"

//! @brief Constructor.
XC::DomainDecompAlgo::DomainDecompAlgo(SoluMethod *owr)
  :SolutionAlgorithm(owr,DomDecompALGORITHM_TAGS_DomainDecompAlgo) {}

//! @brief Resuelve el paso actual.
int XC::DomainDecompAlgo::solveCurrentStep(void)
  {
    IncrementalIntegrator *theIntegrator= dynamic_cast<IncrementalIntegrator *>(getIntegratorPtr());
    LinearSOE *theLinearSOE= getLinearSOEPtr();
    DomainSolver *theSolver= getDomainSolverPtr();    
    Subdomain *theSubdomain= getSubdomainPtr();
    
    if(theIntegrator != 0 && theLinearSOE != 0 &&
	theSolver != 0 && theSubdomain != 0 )
      {
	const Vector &extResponse= theSubdomain->getLastExternalSysResponse();

	theSolver->setComputedXext(extResponse);
	theSolver->solveXint();

	theIntegrator->update(theLinearSOE->getX());
	return 0;
      }
    else
      {
	std::cerr << "XC::DomainDecompAlgo::solveCurrentStep() ";
	std::cerr << "no links have been set\n";
	return -1;
      }
  }

int XC::DomainDecompAlgo::sendSelf(CommParameters &cp)
  { return 0; }

int XC::DomainDecompAlgo::recvSelf(const CommParameters &cp)
  { return 0; }


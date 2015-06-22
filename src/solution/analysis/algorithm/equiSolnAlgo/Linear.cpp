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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/Linear.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/analysis/algorithm/Linear.C 
// 
// Written: fmk 
// Created: Sun Sept 15 15:06:47: 1996 
// Revision: A 
//

// Description: This file contains the class definition for 
// Linear. Linear is a class which performs a linear solution algorihm
// to solve the equations. No member functions are declared as virtual as 
// it is not expected that this class will be subclassed.
// 
// What: "@(#)Linear.C, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/Linear.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/StaticIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <utility/Timer.h>

// Constructor
XC::Linear::Linear(SoluMethod *owr)
  :EquiSolnAlgo(owr,EquiALGORITHM_TAGS_Linear) {}


//! @brief Performs the linear solution algorithm.
int XC::Linear::resuelve(void)
  {
    AnalysisModel *theAnalysisModel = getAnalysisModelPtr(); 
    LinearSOE *theSOE = getLinearSOEPtr();
    IncrementalIntegrator *theIncIntegrator= getIncrementalIntegratorPtr(); 

    if((!theAnalysisModel) || (!theIncIntegrator) || (!theSOE))
      {
	std::cerr << "WARNING Linear::solveCurrentStep() -";
	std::cerr << "no se ha asignado modelo, integrador o sistema de ecuaciones.\n";
	return -5;
      }

    if(theIncIntegrator->formTangent()<0) //Forma la matriz de rigidez tangente.
      {
	std::cerr << "WARNING Linear::solveCurrentStep() -";
	std::cerr << "the XC::Integrator failed in formTangent()\n";
	return -1;
      }
    
    if(theIncIntegrator->formUnbalance()<0) //Forma el vector de cargas.
      {
	std::cerr << "WARNING XC::Linear::solveCurrentStep() -";
	std::cerr << "the XC::Integrator failed in formUnbalance()\n";	
	return -2;
      }

    if(theSOE->solve() < 0) //Resuelve el sist. de ecuaciones.
      {
	std::cerr << "WARNING XC::Linear::solveCurrentStep() -";
	std::cerr << "the XC::LinearSOE failed in solve()\n";	
	return -3;
      }

    const Vector &deltaU = theSOE->getX(); //Obtiene el vector de movimientos.

    if(theIncIntegrator->update(deltaU) < 0) //Actualiza movimientos.
      {
	std::cerr << "WARNING XC::Linear::solveCurrentStep() -";
	std::cerr << "the XC::Integrator failed in update()\n";	
	return -4;
      }
    return 0;
  }

//! @brief Performs the linear solution algorithm.
int XC::Linear::solveCurrentStep(void)
  {
    // set up some pointers and check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    return resuelve();
  }

//! @brief Asigna el test de convergencia a emplear en el análisis.
int XC::Linear::setConvergenceTest(ConvergenceTest *theNewTest)
  { return 0; }

int XC::Linear::sendSelf(CommParameters &cp)
  { return 0; }

int XC::Linear::recvSelf(const CommParameters &cp)
  { return 0; }


void XC::Linear::Print(std::ostream &s, int flag)
  { s << "\t XC::Linear algorithm"; }

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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2003/03/04 00:39:23 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/meritFunction/CriteriaReductionMeritFunctionCheck.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/meritFunction/CriteriaReductionMeritFunctionCheck.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.h>
#include <cmath>
#include <utility/matrix/Vector.h>


XC::CriteriaReductionMeritFunctionCheck::CriteriaReductionMeritFunctionCheck(ReliabilityConvergenceCheck *passedReliabilityConvergenceCheck)
:MeritFunctionCheck()
{
	theReliabilityConvergenceCheck = passedReliabilityConvergenceCheck;
}

int XC::CriteriaReductionMeritFunctionCheck::check(Vector u_old, 
								  double g_old, 
								  Vector grad_G_old, 
								  double stepSize,
								  Vector stepDirection,
								  double g_new, 
								  Vector grad_G_new)
{
	// New point in standard normal space
	Vector u_new = u_old + stepSize*stepDirection;


	// Number of convergence criteria
	int numCrit = theReliabilityConvergenceCheck->getNumberOfCriteria();


	// Initial declarations
	int i;
	Vector oldCriteriaValues(numCrit);
	Vector newCriteriaValues(numCrit);


	// Convergence checks for the old point
	theReliabilityConvergenceCheck->check(u_old,g_old,grad_G_old);
	for (i=1; i<=numCrit; i++) {
		oldCriteriaValues(i-1) = theReliabilityConvergenceCheck->getCriteriaValue(i);
	}
	

	// Convergence checks for the new point
	theReliabilityConvergenceCheck->check(u_new,g_new,grad_G_new);
	for (i=1; i<=numCrit; i++) {
		newCriteriaValues(i-1) = theReliabilityConvergenceCheck->getCriteriaValue(i);
	}


	// All criteria must have improved for the step to be OK
	bool OK = true;
	for (i=1; i<=numCrit; i++) {
		if (newCriteriaValues(i-1)>oldCriteriaValues(i-1)) {
			OK = false;
		}
	}


	// Return
	if (OK) {
		return 1;
	}
	else {
		return -1;
	}
}





double
XC::CriteriaReductionMeritFunctionCheck::getMeritFunctionValue(Vector u, 
												  double g, 
												  Vector grad_G)
{
	std::cerr << "XC::CriteriaReductionMeritFunctionCheck::getMeritFunctionValue() -- this method" << std::endl
		<< " is not implemented in this specific class." << std::endl;

	return 0.0;
}

int
XC::CriteriaReductionMeritFunctionCheck::updateMeritParameters(Vector u, 
												  double g, 
												  Vector grad_G)
{
	std::cerr << "XC::CriteriaReductionMeritFunctionCheck::updateMeritParameters() -- this method" << std::endl
		<< " is not implemented in this specific class." << std::endl;

	return 0;
}

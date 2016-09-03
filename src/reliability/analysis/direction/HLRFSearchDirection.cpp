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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/03/04 00:39:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/direction/HLRFSearchDirection.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) during Spring 2000
// Revised: haukaas 06/00 (core code)
//			haukaas 06/01 (made part of official OpenSees)
//

#include <reliability/analysis/direction/HLRFSearchDirection.h>
#include <reliability/analysis/direction/SearchDirection.h>
#include <utility/matrix/Vector.h>


XC::HLRFSearchDirection::HLRFSearchDirection()
:SearchDirection(), searchDirection(1)
{
}

XC::Vector
XC::HLRFSearchDirection::getSearchDirection()
{
	return searchDirection;
}



int
XC::HLRFSearchDirection::computeSearchDirection(
							int stepNumber,
							Vector u, 
							double gFunctionValue, 
							Vector gradientInStandardNormalSpace )
{

	// Compute the norm of the gradient
	double normOfGradient = gradientInStandardNormalSpace.Norm();


	// Check that the norm is not zero
	if (normOfGradient == 0.0) {
		std::cerr << "XC::HLRFSearchDirection::computeSearchDirection() - " << std::endl
			<< " the norm of the gradient is zero. " << std::endl;
		return -1;
	}

	
	// Compute the alpha-vector
	Vector alpha = gradientInStandardNormalSpace * ( (-1) / normOfGradient );


	// Compute the direction vector
	double alpha_times_u = alpha ^ u ;
	Vector direction = alpha * ( gFunctionValue / normOfGradient + alpha_times_u ) - u;

	searchDirection = direction;

	return 0;
}


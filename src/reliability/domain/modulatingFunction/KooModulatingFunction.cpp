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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
// $Date: 2003/10/27 23:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/modulatingFunction/KooModulatingFunction.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/modulatingFunction/KooModulatingFunction.h>
#include <reliability/domain/modulatingFunction/ModulatingFunction.h>
#include <classTags.h>
#include <cmath>
#include <reliability/domain/filter/Filter.h>


XC::KooModulatingFunction::KooModulatingFunction(int tag,
												 Filter *theFilt, 
												 double pt1,
												 double pt2)
:ModulatingFunction(tag,MODULATING_FUNCTION_trapezoidal)
{
	t1 = pt1;
	t2 = pt2;

	if (t1>t2 ) {
		std::cerr << "WARNING: Inconsistent input to Koo Modulating Function" << std::endl;
	}

	theFilter = theFilt;
}

double
XC::KooModulatingFunction::getAmplitude(double time)
{
	double amplitude;
	if (time < 0.0) {
		amplitude = 0.0;
	}
	else if (time < t1) {
		amplitude = time*time/25.0; 
	}
	else if (time < t2) {
		amplitude = 1.0;
	}
	else {
		amplitude = exp(-0.5*(time-10.0));
	}

	return amplitude;
}

 XC::Filter *
XC::KooModulatingFunction::getFilter()
{
	return theFilter;
}

double
XC::KooModulatingFunction::getMaxAmplitude()
{
	return 1.0;
}

void
XC::KooModulatingFunction::Print(std::ostream &s, int flag)  
{
}


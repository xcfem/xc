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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/27 23:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/filter/StandardLinearOscillatorDisplacementFilter.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/filter/StandardLinearOscillatorDisplacementFilter.h>
#include <reliability/domain/filter/Filter.h>
#include <classTags.h>
#include <cmath>


XC::StandardLinearOscillatorDisplacementFilter::StandardLinearOscillatorDisplacementFilter(int tag, double period, double dampingRatio)
:Filter(tag,FILTER_standardLinearOscillator)
{
	double pi = 3.14159265358979;
	wn = 2*pi/period;
	xi = dampingRatio;
}

double
XC::StandardLinearOscillatorDisplacementFilter::getAmplitude(double time)
{
	if (time<0.0) {
		return 0.0;
	}
	else {
		double wd = wn * sqrt(1.0-pow(xi,2.0));
		return ( sin(wd*time) * exp(-xi*wn*time)  );
	}
}

double
XC::StandardLinearOscillatorDisplacementFilter::getMaxAmplitude()
{
	double wd = wn * sqrt(1.0-pow(xi,2.0));

	double result = wd/(xi*wn*sqrt((xi*xi*wn*wn+wd*wd)/(xi*xi*wn*wn)))
		*exp(-xi*wn*(atan(wd/(xi*wn))/wd));

	return result;
}

double
XC::StandardLinearOscillatorDisplacementFilter::getTimeOfMaxAmplitude()
{
	double wd = wn * sqrt(1.0-pow(xi,2.0));

	return (atan(wd/(xi*wn))/wd);
}

void
XC::StandardLinearOscillatorDisplacementFilter::Print(std::ostream &s, int flag)  
{
}

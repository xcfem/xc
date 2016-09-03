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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/27 23:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/spectrum/JonswapSpectrum.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "JonswapSpectrum.h"
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>


XC::JonswapSpectrum::JonswapSpectrum(int tag, double min, double max,
								 double alp, double w, double gam)
:Spectrum(tag,SPECTRUM_jonswap)
{
	minFreq = min;
	maxFreq = max;
	alpha = alp;
	wp = w;
	gamma = gam;
}

void XC::JonswapSpectrum::Print(std::ostream &s, int flag)  
{
}


double
XC::JonswapSpectrum::getMinFrequency()
{
	return minFreq;
}
double
XC::JonswapSpectrum::getMaxFrequency()
{
	return maxFreq;
}
double
XC::JonswapSpectrum::getAmplitude(double frequency)
{
	if (frequency < minFreq  ||  frequency > maxFreq) {
		return 0.0;
	}

	double sigma;
	if (frequency < wp) {
		sigma = 0.07;
	}
	else {
		sigma = 0.09;
	}

	double power = exp(-(frequency-wp)/(2.0*sigma*sigma*wp*wp));
	double GAMMA = pow(gamma,power);

	return GAMMA*alpha*pow(frequency,-5.0)*exp(-5.0/4.0*pow((frequency/wp),-4.0));
}

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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/04 00:44:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/Type2LargestValueRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/Type2LargestValueRV.h>
#include "GammaRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>


XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	setParameters(passedMean,passedStdv);
	startValue = passedStartValue;
}
XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	u = passedParameter1;
	k = passedParameter2;
	startValue = passedStartValue;
}
XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	setParameters(passedMean,passedStdv);
	startValue = getMean();
}
XC::Type2LargestValueRV::Type2LargestValueRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_type2largestvalue)
{
	tag = passedTag ;
	u = passedParameter1;
	k = passedParameter2;
	startValue = getMean();
}


void
XC::Type2LargestValueRV::Print(std::ostream &s, int flag)
{
}


double
XC::Type2LargestValueRV::getPDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = k/u * pow(u/rvValue,k+1) * exp(-pow(u/rvValue,k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::Type2LargestValueRV::getCDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = exp(-pow(u/rvValue,k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::Type2LargestValueRV::getInverseCDFvalue(double probValue)
{
	return u * pow((-log(probValue)),(-1.0/k));
}


const char *
XC::Type2LargestValueRV::getType()
{
	return "TYPE2LARGESTVALUE";
}


double 
XC::Type2LargestValueRV::getMean()
{
	double result;
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	result = u * aGammaRV->gammaFunction(1.0-1.0/k);
	delete aGammaRV;
	return result;
}



double 
XC::Type2LargestValueRV::getStdv()
{
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double a = aGammaRV->gammaFunction(1.0-2.0/k);
	double b = aGammaRV->gammaFunction(1.0-1.0/k);
	delete aGammaRV;
	double result = u*sqrt(a-b*b);
	return result;
}


double 
XC::Type2LargestValueRV::getStartValue()
{
	return startValue;
}

double XC::Type2LargestValueRV::getParameter1()  {return u;}
double XC::Type2LargestValueRV::getParameter2()  {return k;}
double XC::Type2LargestValueRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::Type2LargestValueRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}



void
XC::Type2LargestValueRV::setParameters(double mean, double stdv)
{
	double cov = stdv/mean;
	double xk = 1.5;
	function131(xk, cov,mean);
}


void
XC::Type2LargestValueRV::function131(double xk, double cov, double mean)
{
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	xk = xk + 1.0;
	double x1 = 1.0 - 1.0/xk;
	double x2 = 1.0 - 2.0/xk;
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function131(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function134(xk,cov,mean);
	else 
		function132(xk,cov,mean);
}
void
XC::Type2LargestValueRV::function132(double xk, double cov, double mean)
{
	xk = xk - 0.1;
	double x1 = 1.0 - 1.0/xk;
	double x2 = 1.0 - 2.0/xk;
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function133(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function134(xk,cov,mean);
	else 
		function132(xk,cov,mean);
}
void
XC::Type2LargestValueRV::function133(double xk, double cov, double mean)
{
	xk = xk + 0.01;
	double x1 = 1.0 - 1.0/xk;
	double x2 = 1.0 - 2.0/xk;
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function133(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function134(xk,cov,mean);
	else 
		function134(xk,cov,mean);


}
void
XC::Type2LargestValueRV::function134(double xk, double gm1, double mean)
{
	u = mean/gm1;
	k = xk;
}

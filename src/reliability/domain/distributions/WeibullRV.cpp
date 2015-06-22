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
// $Date: 2003/03/04 00:44:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/WeibullRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/WeibullRV.h>
#include <cmath>
#include <cstring>
#include "GammaRV.h"
#include <classTags.h>


XC::WeibullRV::WeibullRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_weibull)
{
	tag = passedTag;
	setParameters(passedMean,passedStdv);
	startValue = passedStartValue;
}
XC::WeibullRV::WeibullRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_weibull)
{
	tag = passedTag ;
	u = passedParameter1;
	k = passedParameter2;
	startValue = passedStartValue;
}
XC::WeibullRV::WeibullRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_weibull)
{
	tag = passedTag;
	setParameters(passedMean,passedStdv);
	startValue = getMean();
}
XC::WeibullRV::WeibullRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_weibull)
{
	tag = passedTag ;
	u = passedParameter1;
	k = passedParameter2;
	startValue = getMean();
}


void XC::WeibullRV::Print(std::ostream &s, int flag)
  {}


double
XC::WeibullRV::getPDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue) {
		result = k/u * pow(rvValue/u,k-1) * exp(-pow(rvValue/u,k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::WeibullRV::getCDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue) {
		result = 1 - exp(-pow(rvValue/u,k));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::WeibullRV::getInverseCDFvalue(double probValue)
{
	return u * pow((-log(1-probValue)),(1/k));
}


const char *
XC::WeibullRV::getType()
{
	return "WEIBULL";
}


double 
XC::WeibullRV::getMean()
{
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double result = u * aGammaRV->gammaFunction(1.0+1.0/k);
	delete aGammaRV;
	return result;
}



double 
XC::WeibullRV::getStdv()
{
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double a = aGammaRV->gammaFunction(1.0+2.0/k);
	double b = aGammaRV->gammaFunction(1.0+1.0/k);
	delete aGammaRV;
	double result = u*sqrt(a-b*b);
	return result;
}


double 
XC::WeibullRV::getStartValue()
{
	return startValue;
}

double XC::WeibullRV::getParameter1()  {return u;}
double XC::WeibullRV::getParameter2()  {return k;}
double XC::WeibullRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::WeibullRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

void
XC::WeibullRV::setParameters(double mean, double stdv)
{
	double cov = stdv/mean;
	double xk = 0.0;
	function141(xk, cov,mean);
}


void
XC::WeibullRV::function141(double xk, double cov, double mean)
{
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	xk = xk + 1.0;
	double x1 = 1.0 + 1.0/xk;
	double x2 = 1.0 + 2.0/xk;
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function141(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function144(xk,cov,mean);
	else 
		function142(xk,cov,mean);
}
void
XC::WeibullRV::function142(double xk, double cov, double mean)
{
	xk = xk - 0.1;
	double x1 = 1.0 + 1.0/xk;
	double x2 = 1.0 + 2.0/xk;
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function143(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function144(xk,cov,mean);
	else 
		function142(xk,cov,mean);
}
void
XC::WeibullRV::function143(double xk, double cov, double mean)
{
	xk = xk + 0.01;
	double x1 = 1.0 + 1.0/xk;
	double x2 = 1.0 + 2.0/xk;
	GammaRV *aGammaRV = new XC::GammaRV(1, 0.0, 1.0, 0.0);
	double gm1 = aGammaRV->gammaFunction(x1);
	double gm2 = aGammaRV->gammaFunction(x2);
	delete aGammaRV;
	double vy = sqrt(gm2/gm1/gm1 - 1.0);
	if (cov-vy < 0.0) 
		function143(xk,cov,mean);
	else if (cov-vy == 0.0) 
		function144(xk,cov,mean);
	else 
		function144(xk,cov,mean);


}
void
XC::WeibullRV::function144(double xk, double gm1, double mean)
{
	u = mean/gm1;
	k = xk;
}

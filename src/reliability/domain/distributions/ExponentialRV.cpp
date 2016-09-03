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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/03/04 00:44:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/ExponentialRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/ExponentialRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::ExponentialRV::ExponentialRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_exponential)
{
	// Note: this constructor is void.
	std::cerr << "WARNING: This type of random variable is not uniquely defined by mean and stdv." << std::endl;
}
XC::ExponentialRV::ExponentialRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_exponential)
{
	tag = passedTag ;
	lambda = passedParameter1;
	startValue = passedStartValue;
}
XC::ExponentialRV::ExponentialRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_exponential)
{
	// Note: this constructor is void.
	std::cerr << "WARNING: This type of random variable is not uniquely defined by mean and stdv." << std::endl;
}
XC::ExponentialRV::ExponentialRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_exponential)
{
	tag = passedTag ;
	lambda = passedParameter1;
	startValue = getMean();
}


void XC::ExponentialRV::Print(std::ostream &s, int flag)
{
}


double
XC::ExponentialRV::getPDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = lambda * exp(-lambda * rvValue);
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::ExponentialRV::getCDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = 1 - exp(-lambda*rvValue);
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::ExponentialRV::getInverseCDFvalue(double probValue)
{
	return -log(1.0-probValue)/lambda;
}


const char *
XC::ExponentialRV::getType()
{
	return "EXPONENTIAL";
}


double 
XC::ExponentialRV::getMean()
{
	return 1/lambda;
}



double 
XC::ExponentialRV::getStdv()
{
	return 1/lambda;
}


double 
XC::ExponentialRV::getStartValue()
{
	return startValue;
}


double XC::ExponentialRV::getParameter1()  {return lambda;}
double XC::ExponentialRV::getParameter2()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::ExponentialRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::ExponentialRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

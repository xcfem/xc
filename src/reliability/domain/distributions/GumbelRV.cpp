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
                                                                        
// $Revision: 1.7 $
// $Date: 2004/08/27 17:51:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/GumbelRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "GumbelRV.h"
#include <cmath>
#include <cstring>
#include <classTags.h>


XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedMean,
		 double passedStdv,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	double pi = 3.14159265358979;
	double gamma = 0.5772156649;
	u = passedMean - (gamma*sqrt(6.0)*passedStdv)/pi;
	alpha = pi / ( sqrt(6.0) * passedStdv );
	startValue = passedStartValue;
}
XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	u = passedParameter1;
	alpha = passedParameter2;
	startValue = passedStartValue;
}
XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedMean,
		 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	double pi = 3.14159265358979;
	double gamma = 0.5772156649;
	u = passedMean - (gamma*sqrt(6.0)*passedStdv)/pi;
	alpha = pi / ( sqrt(6.0) * passedStdv );
	startValue = getMean();
}
XC::GumbelRV::GumbelRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_gumbel)
{
	tag = passedTag ;
	u = passedParameter1;
	alpha = passedParameter2;
	startValue = getMean();
}


void
XC::GumbelRV::Print(std::ostream &s, int flag)
{
}


double
XC::GumbelRV::getPDFvalue(double rvValue)
{
	return alpha * exp(-alpha*(rvValue-u)-exp(-alpha*(rvValue-u)));
}


double
XC::GumbelRV::getCDFvalue(double rvValue)
{
	return exp(-exp(-alpha*(rvValue-u)));
}


double
XC::GumbelRV::getInverseCDFvalue(double probValue)
{
	return (alpha*u - log(-log(probValue))) / alpha;
}


const char *
XC::GumbelRV::getType()
{
	return "GUMBEL";
}


double 
XC::GumbelRV::getMean()
{
	double gamma = 0.5772156649;
	return u+gamma/alpha;
}



double 
XC::GumbelRV::getStdv()
{
	double pi = 3.14159265358979;
	return pi/(sqrt(6.0)*alpha);
}


double 
XC::GumbelRV::getStartValue()
{
	return startValue;
}

double XC::GumbelRV::getParameter1()  {return u;}
double XC::GumbelRV::getParameter2()  {return alpha;}
double XC::GumbelRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::GumbelRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

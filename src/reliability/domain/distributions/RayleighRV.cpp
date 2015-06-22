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
// $Date: 2003/03/04 00:44:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/RayleighRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/domain/distributions/RayleighRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::RayleighRV::RayleighRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4,
		 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_rayleigh)
{
	tag = passedTag ;
	u = passedParameter1;
	startValue = passedStartValue;
}
XC::RayleighRV::RayleighRV(int passedTag, 
		 double passedParameter1,
		 double passedParameter2,
		 double passedParameter3,
		 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_rayleigh)
{
	tag = passedTag ;
	u = passedParameter1;
	startValue = getMean();
}

void
XC::RayleighRV::Print(std::ostream &s, int flag)
{
}


double
XC::RayleighRV::getPDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = 2.0*rvValue/(u*u) * exp(-pow(rvValue/u,2));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::RayleighRV::getCDFvalue(double rvValue)
{
	double result;
	if ( 0.0 < rvValue ) {
		result = 1-exp(-pow(rvValue/u,2));
	}
	else {
		result = 0.0;
	}
	return result;
}


double
XC::RayleighRV::getInverseCDFvalue(double probValue)
{
	return u*sqrt(-log(1-probValue));
}


const char *
XC::RayleighRV::getType()
{
	return "RAYLEIGH";
}


double 
XC::RayleighRV::getMean()
{
	double pi = 3.14159265358979;
	return 0.5*sqrt(pi) * u;
}



double 
XC::RayleighRV::getStdv()
{
	double pi = 3.14159265358979;
	return 0.5 * sqrt(4.0-pi) * u;
}


double 
XC::RayleighRV::getStartValue()
{
	return startValue;
}


double XC::RayleighRV::getParameter1()  {return u;}
double XC::RayleighRV::getParameter2()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::RayleighRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::RayleighRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}

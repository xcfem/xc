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
// $Date: 2003/10/27 23:04:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/UserDefinedRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/UserDefinedRV.h>
#include <cmath>
#include <string.h>
#include <classTags.h>

#include <utility/matrix/Vector.h>

XC::UserDefinedRV::UserDefinedRV(int passedTag, Vector pxPoints, Vector pPDFpoints, double pStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_userdefined)
{
	xPoints = new XC::Vector(pxPoints);
	PDFpoints = new XC::Vector(pPDFpoints);
	startValue = pStartValue;
}

XC::UserDefinedRV::UserDefinedRV(int passedTag, Vector pxPoints, Vector pPDFpoints)
:RandomVariable(passedTag, RANDOM_VARIABLE_userdefined)
{
	xPoints = new XC::Vector(pxPoints);
	PDFpoints = new XC::Vector(pPDFpoints);
	startValue = getMean();
}


XC::UserDefinedRV::~UserDefinedRV()
{
	if (xPoints != 0) {
		delete xPoints;
	}
	if (PDFpoints != 0) {
		delete PDFpoints;
	}
}


void
XC::UserDefinedRV::Print(std::ostream &s, int flag)
{
}


double
XC::UserDefinedRV::getPDFvalue(double x)
{

	double ok = -1.0;
	if ( x < (*xPoints)(0) ) {
		return 0.0;
	}
	else if ( x > (*xPoints)(xPoints->Size()-1) ) {
		return 0.0;
	}
	else {
		for ( int i=1; i<xPoints->Size(); i++ ) {
			if ( x <= (*xPoints)(i) ) {
				double a = ((*PDFpoints)(i)-(*PDFpoints)(i-1))/((*xPoints)(i)-(*xPoints)(i-1));
				ok = ( a * (x-(*xPoints)(i-1)) + (*PDFpoints)(i-1) );
				break;
			}
		}
		if (ok<0.0) {
			std::cerr << "ERROR: XC::UserDefinedRV::getPDFvalue() -- distribution function seems to be invalid." << std::endl;
		}
		return ok;
	}
}


double
XC::UserDefinedRV::getCDFvalue(double x)
{
	double ok = -1.0;
	if ( x < (*xPoints)(0) ) {
		return 0.0;
	}
	else if ( x > (*xPoints)(xPoints->Size()-1) ) {
		return 1.0;
	}
	else {
		double sum = 0.0;
		for ( int i=1; i<xPoints->Size(); i++ ) {
			if ( x <= (*xPoints)(i) ) {
				
				ok = ( sum + 0.5 * (getPDFvalue(x)+(*PDFpoints)(i-1)) * (x-(*xPoints)(i-1)) );
				break;
			}
			sum += 0.5 * ((*PDFpoints)(i)+(*PDFpoints)(i-1)) * ((*xPoints)(i)-(*xPoints)(i-1));
		}
		if (ok<0.0) {
			std::cerr << "ERROR: XC::UserDefinedRV::getPDFvalue() -- distribution function seems to be invalid." << std::endl;
		}
		return ok;
	}
}


double XC::UserDefinedRV::getInverseCDFvalue(double p)
  {
    if (p < 0.0 || p > 1.0) {
    	std::cerr << "ERROR: Illegal probability value input to XC::UserDefinedRV::getInverseCDFvalue()" << std::endl;
    	return 0.0;
    }
    else if (p == 0.0) {
    	return ((*xPoints)(0));
    }


    double ok= 0.0;
    for(int i=1; i<xPoints->Size(); i++ )
          {
    	if ( getCDFvalue((*xPoints)(i)) > p) {
    		double a = (getPDFvalue((*xPoints)(i))-getPDFvalue((*xPoints)(i-1)))/((*xPoints)(i)-(*xPoints)(i-1));
    		if ( a==0.0 && getPDFvalue((*xPoints)(i-1))==0.0 ) {
    			std::cerr << "ERROR: An inside region of PDF is constant zero..." << std::endl;
    		}
    		else if (a==0.0) {
    			ok = (p-getCDFvalue((*xPoints)(i-1))+getPDFvalue((*xPoints)(i-1))*(*xPoints)(i-1))/getPDFvalue((*xPoints)(i-1));
    		}
    		else {

    			double A = 0.5*a;
    			double B = getPDFvalue((*xPoints)(i-1));
    			double C = getCDFvalue((*xPoints)(i-1)) - p;
    			double x_minus_x_i_1 = (-B+sqrt(B*B-4.0*A*C))/(2.0*A);
    			ok = x_minus_x_i_1 + ((*xPoints)(i-1));
    		}
    		break;
    	}
    }
    return ok;
}


const char *
XC::UserDefinedRV::getType()
{
    return "USERDEFINED";
}


double 
XC::UserDefinedRV::getMean()
{
    double sum = 0.0;
    double a, b;
    for ( int i=1; i<xPoints->Size(); i++ ) {
    	a = ((*PDFpoints)(i)-(*PDFpoints)(i-1))/((*xPoints)(i)-(*xPoints)(i-1));
    	b = (*PDFpoints)(i-1) - a * (*xPoints)(i-1);
    	sum += a*((*xPoints)(i))*((*xPoints)(i))*((*xPoints)(i))/3.0
    		+  0.5*b*((*xPoints)(i))*((*xPoints)(i))
    		-  a*((*xPoints)(i-1))*((*xPoints)(i-1))*((*xPoints)(i-1))/3.0
    		-  0.5*b*((*xPoints)(i-1))*((*xPoints)(i-1));
    }
    return sum;
}



double 
XC::UserDefinedRV::getStdv()
{
    double sum = 0.0;
    double a, b;
    double mu = getMean();
    for ( int i=1; i<xPoints->Size(); i++ ) {
    	a = ((*PDFpoints)(i)-(*PDFpoints)(i-1))/((*xPoints)(i)-(*xPoints)(i-1));
    	b = (*PDFpoints)(i-1) - a * (*xPoints)(i-1);
    	double x1 = (*xPoints)(i-1);
    	double x2 = (*xPoints)(i);
    	sum += 0.25*a*x2*x2*x2*x2
    		-  2.0/3.0*mu*a*x2*x2*x2
    		+ mu*mu*b*x2
    		- mu*b*x2*x2
    		+ b*x2*x2*x2/3.0
    		+ 0.5*mu*mu*a*x2*x2
    		- 0.25*a*x1*x1*x1*x1
    		+ 2.0/3.0*mu*a*x1*x1*x1
    		- mu*mu*b*x1
    		+ mu*b*x1*x1
    		- b*x1*x1*x1/3.0
    		- 0.5*mu*mu*a*x1*x1;
    }
    return sqrt(sum);
}


double XC::UserDefinedRV::getStartValue()
  { return startValue; }

double XC::UserDefinedRV::getParameter1()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::UserDefinedRV::getParameter2()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::UserDefinedRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::UserDefinedRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}


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
// $Date: 2003/03/04 00:44:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/Type2LargestValueRV.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef Type2LargestValueRV_h
#define Type2LargestValueRV_h

#include <reliability/domain/components/RandomVariable.h>

namespace XC {
class Type2LargestValueRV : public RandomVariable
{

public:
	Type2LargestValueRV(int tag, 
			 double mean,
			 double stdv,
			 double startValue);
	Type2LargestValueRV(int tag, 
			 double parameter1,
			 double parameter2,
			 double parameter3,
			 double parameter4,
			 double startValue);
	Type2LargestValueRV(int tag, 
			 double mean,
			 double stdv);
	Type2LargestValueRV(int tag, 
			 double parameter1,
			 double parameter2,
			 double parameter3,
			 double parameter4);
	void Print(std::ostream &s, int flag =0);
	double getPDFvalue(double rvValue);
	double getCDFvalue(double rvValue);
	double getInverseCDFvalue(double probValue);
	const char * getType();
	double getMean();
	double getStdv();
	double getParameter1();
	double getParameter2();
	double getParameter3();
	double getParameter4();
	double getStartValue();

private:
	double u;
	double k;
	void setParameters(double mean, double stdv);
	void function131(double xk, double cov, double mean);
	void function132(double xk, double cov, double mean);
	void function133(double xk, double cov, double mean);
	void function134(double xk, double gm1, double mean);


};
} // fin namespace XC

#endif

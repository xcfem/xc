//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
// $Date: 2003/03/04 00:44:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/WeibullRV.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#ifndef WeibullRV_h
#define WeibullRV_h

#include <reliability/domain/components/RandomVariable.h>

namespace XC {
//! @ingroup ProbDistributions
// 
//! @brief Weibull distribution random variable.
//
//! In probability theory and statistics, the Weibull distribution
//! is a continuous probability distribution. It is named after Swedish
//! mathematician Waloddi Weibull, who described it in detail in 1951,
//! although it was first identified by Fréchet (1927) and first
//! applied by Rosin & Rammler (1933) to describe a particle size distribution.
class WeibullRV: public RandomVariable
  {
  private:
    double u;
    double k;
    void setParameters(double mean, double stdv);
    void function141(double xk, double cov, double mean);
    void function142(double xk, double cov, double mean);
    void function143(double xk, double cov, double mean);
    void function144(double xk, double gm1, double mean);
  public:
    WeibullRV(int tag, 
		     double mean,
		     double stdv,
		     double startValue);
    WeibullRV(int tag, 
		     double parameter1,
		     double parameter2,
		     double parameter3,
		     double parameter4,
		     double startValue);
    WeibullRV(int tag, 
		     double mean,
		     double stdv);
    WeibullRV(int tag, 
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
  };
} // end of XC namespace

#endif

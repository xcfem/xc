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
// $Date: 2003/03/04 00:44:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/NormalRV.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#ifndef NormalRV_h
#define NormalRV_h

#include <reliability/domain/components/RandomVariable.h>

namespace XC {
//! @ingroup ProbDistributions
// 
//! @brief Normal distribution random variable.
//
//! In probability theory, a normal (or Gaussian or Gauss or Laplace–Gauss)
//! distribution is a type of continuous probability distribution for a
//! real-valued random variable.
class NormalRV: public RandomVariable
  {
  private:
    double errorFunction(double x);
    double inverseErrorFunction(double y);
    double mju;
    double sigma;
  public:
    NormalRV(int tag, 
		     double mean,
		     double stdv,
		     double startValue);
    NormalRV(int tag, 
		     double parameter1,
		     double parameter2,
		     double parameter3,
		     double parameter4,
		     double startValue);
    NormalRV(int tag, 
		     double mean,
		     double stdv);
    NormalRV(int tag, 
		     double parameter1,
		     double parameter2,
		     double parameter3,
		     double parameter4);
    void Print(std::ostream &s, int flag =0) const;
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


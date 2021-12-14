// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/10/27 23:04:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/components/RandomVariable.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#ifndef RandomVariable_h
#define RandomVariable_h

#include <reliability/domain/components/ReliabilityDomainComponent.h>
#include <string.h>

namespace XC {
//! @ingroup ReliabilityAnalysis
//!
//! @defgroup ProbDistributions Probability distributions.
//!
//! @ingroup ProbDistributions
//!
//! @brief Base class for random variables.
class RandomVariable: public ReliabilityDomainComponent
  {
  protected:
    int tag;
    double startValue;
  public:
    RandomVariable(int tag, int classTag);

    // utility functions for gamma and beta
    double gammaFunction(double x);
    double incompleteGammaFunction(double a, double x);
    double betaFunction(double passed_q, double passed_r);
    double incompleteBetaFunction(double passed_q, double passed_r, double passed_x);
    
    virtual void Print(std::ostream &s, int flag =0) const=0;
    virtual double getPDFvalue(double rvValue) =0;
    virtual double getCDFvalue(double rvValue) =0;
    virtual double getInverseCDFvalue(double rvValue) =0;    
    virtual const std::string getType(void) =0;
    virtual double getMean() =0;
    virtual double getStdv() =0;
    virtual double getParameter1() =0;
    virtual double getParameter2() =0;
    virtual double getParameter3() =0;
    virtual double getParameter4() =0;
    virtual double getStartValue() =0;

    int setNewTag(int tag);
  };
} // end of XC namespace

#endif
























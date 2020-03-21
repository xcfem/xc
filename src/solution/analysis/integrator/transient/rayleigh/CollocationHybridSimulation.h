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
** (C) Copyright 1999, The Regents of the University of California    **
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
** ****************************************************************** */

// $Revision: 1.1 $
// $Date: 2005/12/19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CollocationHybridSimulation.h,v $

#ifndef CollocationHybridSimulation_h
#define CollocationHybridSimulation_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the class definition for CollocationHybridSimulation.
// CollocationHybridSimulation is an algorithmic class for performing a transient analysis
// using the CollocationHybridSimulation integration scheme.
//
// What: "@(#) CollocationHybridSimulation.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/HHTBase.h"

namespace XC {
class ConvergenceTest;

//! @ingroup RayleighIntegrator
//
//! @brief CollocationHybridSimulation is an algorithmic class
//! for performing a transient analysis using the 
//! CollocationHybridSimulation integration scheme.
class CollocationHybridSimulation: public HHTBase
  {
  private:
    double theta;
    double rFact; //!< displacement increment reduction factor

    ConvergenceTest *theTest; //!< convergence test
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    CollocationHybridSimulation(AnalysisAggregation *);
    CollocationHybridSimulation(AnalysisAggregation *,double theta, ConvergenceTest &);
    CollocationHybridSimulation(AnalysisAggregation *,double theta, ConvergenceTest &,const RayleighDampingFactors &);
    CollocationHybridSimulation(AnalysisAggregation *,double theta, double beta, double gamma, ConvergenceTest &theTest);
    CollocationHybridSimulation(AnalysisAggregation *,double theta, double beta, double gamma, ConvergenceTest &,const RayleighDampingFactors &);
    Integrator *getCopy(void) const;
  public:
    
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        
    
    int domainChanged(void);
    int newStep(double deltaT);
    int revertToLastStep(void);
    int update(const Vector &deltaU);
    int commit(void);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0) const;        
  };
inline Integrator *CollocationHybridSimulation::getCopy(void) const
  { return new CollocationHybridSimulation(*this); }
} // end of XC namespace

#endif

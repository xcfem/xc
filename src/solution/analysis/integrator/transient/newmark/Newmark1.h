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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Newmark1.h,v $
                                                                        
                                                                        
#ifndef Newmark1_h
#define Newmark1_h

// File: ~/analysis/integrator/Newmark1.h
// 
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for Newmark1.
// Newmark1 is an algorithmic class for performing a transient analysis
// using the Newmark1 integration scheme.
//
// What: "@(#) Newmark1.h, revA"

#include <solution/analysis/integrator/transient/newmark/NewmarkBase2.h>

namespace XC {

//! @ingroup NewmarkIntegrator
//
//! @brief Newmark1 is an algorithmic class
//! for performing a transient analysis
//! using the Newmark1 integration scheme.
class Newmark1: public NewmarkBase2
  {
  private:
    double c4;  //!< some constants we need to keep
    ResponseQuantities Up;  //!< predicted response quantities at time t+deltat for 1st iter 
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    Newmark1(AnalysisAggregation *);
    Newmark1(AnalysisAggregation *,double gamma, double beta, bool disp = true);
    Newmark1(AnalysisAggregation *,double gamma, double beta,const RayleighDampingFactors &rF); 
    Integrator *getCopy(void) const;
  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        

    int domainChanged(void);    
    int initialize(void);    
    int newStep(double deltaT);    
    int revertToLastStep(void);            
    int update(const Vector &deltaU);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;  
  };
inline Integrator *Newmark1::getCopy(void) const
  { return new Newmark1(*this); }
} // end of XC namespace

#endif


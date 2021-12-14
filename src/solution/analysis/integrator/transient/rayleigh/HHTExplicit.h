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

// $Revision: 1.2 $
// $Date: 2005/12/21 00:32:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTExplicit.h,v $


#ifndef HHTExplicit_h
#define HHTExplicit_h

// File: ~/analysis/integrator/HHTExplicit.h
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the class definition for HHTExplicit.
// HHTExplicit is an algorithmic class for performing a transient analysis
// using the HHTExplicit integration scheme (beta = 0).
//
// What: "@(#) HHTExplicit.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/HHTRayleighBase.h"

namespace XC {
class DOF_Group;
class FE_Element;
class Vector;

//! @ingroup RayleighIntegrator
//
//! @brief HHTExplicit is an algorithmic class for performing
//! a transient analysis using the HHTExplicit integration scheme (beta = 0).
class HHTExplicit: public HHTRayleighBase
  {
  private:
    int updateCount; //!< method should only have one update per step
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

    friend class SolutionStrategy;
    HHTExplicit(SolutionStrategy *);
    HHTExplicit(SolutionStrategy *,double alpha);
    HHTExplicit(SolutionStrategy *,double alpha,const RayleighDampingFactors &rF);
    HHTExplicit(SolutionStrategy *,double alpha, double gamma);
    HHTExplicit(SolutionStrategy *,double alpha, double gamma,const RayleighDampingFactors &rF);
    Integrator *getCopy(void) const;
  public:
    
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        
    
    int domainChanged(void);
    int newStep(double deltaT);
    int revertToLastStep(void);
    int update(const Vector &aiPlusOne);
    int commit(void);
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag = 0) const;
  };
inline Integrator *HHTExplicit::getCopy(void) const
  { return new HHTExplicit(*this); }
} // end of XC namespace

#endif

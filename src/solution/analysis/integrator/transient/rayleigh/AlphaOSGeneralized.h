// -*-c++-*-
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

// $Revision: 1.2 $
// $Date: 2005/12/21 00:31:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/AlphaOSGeneralized.h,v $

#ifndef AlphaOSGeneralized_h
#define AlphaOSGeneralized_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the class definition for AlphaOSGeneralized.
// AlphaOSGeneralized is an algorithmic class for performing a transient analysis
// using the generalized Alpha-Operator-Splitting integration scheme.
// The parameters alpha correspond to 1+alpha_{HHT}.
//
// What: "@(#) AlphaOSGeneralized.h, revA"

#include <solution/analysis/integrator/transient/rayleigh/AlphaOSBase.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {
class DOF_Group;
class FE_Element;

//! @ingroup RayleighIntegrator
//
//! @brief AlphaOSGeneralized is an algorithmic class for performing a
//! transient analysis using the generalized Alpha-Operator-Splitting
//! integration scheme. The parameters alpha correspond to \f$1+\alpha_{HHT}\f$.
class AlphaOSGeneralized: public AlphaOSBase
  {
  private:
    double alphaI;
    double alphaF;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
    virtual int formElementResidual(void);

    friend class AnalysisAggregation;
    AlphaOSGeneralized(AnalysisAggregation *);
    AlphaOSGeneralized(AnalysisAggregation *,double rhoInf);
    AlphaOSGeneralized(AnalysisAggregation *,double rhoInf,const RayleighDampingFactors &rF);
    AlphaOSGeneralized(AnalysisAggregation *,double alphaI, double alphaF, double beta, double gamma);
    AlphaOSGeneralized(AnalysisAggregation *,double alphaI, double alphaF, double beta, double gamma,const RayleighDampingFactors &rF);
    Integrator *getCopy(void) const;

  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);

    int domainChanged(void);
    int newStep(double deltaT);    
    int update(const Vector &deltaU);
    int commit(void);
    
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag = 0) const;        
  };
inline Integrator *AlphaOSGeneralized::getCopy(void) const
  { return new AlphaOSGeneralized(*this); }
} // end of XC namespace

#endif

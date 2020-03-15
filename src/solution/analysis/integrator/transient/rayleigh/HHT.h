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

// $Revision: 1.7 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHT.h,v $


#ifndef HHT_h
#define HHT_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the class definition for HHT.
// HHT is an algorithmic class for performing a transient analysis
// using the HHT integration scheme.
//
// What: "@(#) HHT.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/HHTBase.h"
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {

//! @ingroup RayleighIntegrator
//
//! @brief HHT is an algorithmic class
//! for performing a transient analysis
//! using the Hilber-Hughes-Taylor integration scheme.
//!
//! HHT is a subclass of TransientIntegrator which implements
//! the Hilber-Hughes-Taylor (HHT) method. In the HHT method, to determine the
//! velocities, accelerations and displacements at time \f$t + \Delta t\f$,
//! by solving the following equilibrium equation.
//!
//! \noindent {\bf Description} 
//! \indent 
//! 
//! \[ R (U_{t + \Delta t}) = P(t + \Delta t) -
//! F_I(Udd_{t+\Delta t}) - F_R(Ud_{t + \alpha \Delta t},U_{t +
//! \alpha \Delta t}) \] 
//! 
//! \noindent where
//! 
//! \[ U_{t + \alpha} = \left( 1 - \alpha \right) U_t + \alpha U_{t +
//! \Delta t} \]
//! 
//! \[ Ud_{t + \alpha} = \left( 1 - \alpha \right) Ud_t + \alpha Ud_{t +
//! \Delta t} \]
//! 
//! \noindent and the velocities and accelerations at time \f$t + \Delta t\f$
//! are determined using the Newmark relations. The HHT method results in
//! the following for determining the response at \f$t + \Delta t\f$
//! 
//! \[ \left[ \frac{1}{\beta \Delta t^2} M + \frac{\alpha \gamma}{\beta
//! \Delta t} C + \alpha K \right] \Delta U_{t + \Delta t}^{(i)} = P(t
//! + \Delta t) - F_I\left(Udd_{t+\Delta  t}^{(i-1)}\right)
//! - F_R\left(Ud_{t + \alpha \Delta t}^{(i-1)},U_{t + \alpha \Delta
//! t}^{(i-1)}\right) \] 
class HHT: public HHTBase
  {
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    HHT(AnalysisAggregation *);
    HHT(AnalysisAggregation *,double alpha);
    HHT(AnalysisAggregation *,double alpha,const RayleighDampingFactors &rF);
    HHT(AnalysisAggregation *,double alpha, double beta, double gamma);
    HHT(AnalysisAggregation *,double alpha, double beta, double gamma,const RayleighDampingFactors &);
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
} // end of XC namespace

#endif

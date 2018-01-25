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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/WilsonTheta.h,v $
                                                                        
                                                                        
#ifndef WilsonTheta_h
#define WilsonTheta_h

// File: ~/analysis/integrator/WilsonTheta.h
// 
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for WilsonTheta.
// WilsonTheta is an algorithmic class for performing a transient analysis
// using the WilsonTheta integration scheme.
//
// What: "@(#) WilsonTheta.h, revA"

#include <solution/analysis/integrator/transient/RayleighBase.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {
class DOF_Group;
class FE_Element;
class Vector;

//! @ingroup RayleighIntegrator
//
//! @brief WilsonTheta is an algorithmic class
//! for performing a transient analysis
//! using the WilsonTheta integration scheme.
//!
//!  WilsonTheta is a subclass of TransientIntegrator which implements
//! the Wilson\f$\Theta\f$ method. In the Wilson\f$\Theta\f$ method, to
//! determine the velocities, accelerations and displacements at time
//! \f$t + \theta \Delta t\f$, \f$\theta \ge 1.37\f$, for \f$U_{t+ \theta
//! \Delta t}\f$ 
//! 
//! \[ R (U_{t + \theta \Delta t}) = P(t + \theta \Delta t) -
//! F_I(Udd_{t+ \theta \Delta t}) 
//! - F_R(Ud_{t + \theta \Delta t},U_{t + \theta \Delta t}) \]
//! 
//! \noindent where we use following functions to relate \f$Ud_{t + \theta
//! \Delta t}\f$ and \f$Udd_{t + \theta \Delta t}\f$ to \f$U_{t + \theta
//! \Delta t}\f$ and the response quantities at time \f$t\f$:
//! 
//! \[
//! \dot U_{t + \theta \Delta t} = \frac{3}{\theta \Delta t} \left(
//! U_{t + \theta \Delta t} - U_t \right)
//!  - 2 \dot U_t + \frac{\theta \Delta t}{2} \ddot U_t 
//! \]
//! 
//! \[
//! \ddot U_{t + \theta \Delta t} = \frac{6}{\theta^2 \Delta t^2}
//! \left( U_{t+\theta \Delta t} - U_t \right)
//!  - \frac{6}{\theta \Delta t} \dot U_t -2 Udd_t
//! \]
//! 
//! \noindent which  results in the following for determining the responses at
//! \f$t + \theta \Delta t\f$ 
//! 
//! \[ \left[ \frac{6}{\theta^2 \Delta t^2} M + \frac{3}{\theta \Delta t}
//! C + K \right] \Delta U_{t + \theta \Delta t}^{(i)} = P(t + \theta
//! \Delta t) - F_I\left(Udd_{t+\theta \Delta  t}^{(i-1)}\right) 
//! - F_R\left(Ud_{t + \theta \Delta t}^{(i-1)},U_{t + \theta \Delta
//! t}^{(i-1)}\right) \]
//! 
//! \noindent The response quantities at time \f$t + \Delta t\f$ are then
//! determined using the following
//! 
//! \[
//! Udd_{t + \Delta t} = Udd_t + \frac{1}{\theta} \left( Udd_{t +
//! \theta \Delta t} - Udd_t \right)
//! \]
//! 
//! \[ Ud_{t + \Delta t} = Ud_t + \frac{\Delta t}{2}\left( Udd_{t +
//! \Delta t} + Udd_t \right) \]
//! 
//! \[ U_{t + \Delta t} = U_t + \Delta tUd_t + \frac{\Delta t^2}{6}\left(
//! Udd_{t + \Delta t} + 2 Udd_t \right) \]
class WilsonTheta: public RayleighBase
  {
  private:
    double theta;
    double c1, c2, c3;  // some constants we need to keep
    ResponseQuantities Ut; //!< response quantities at time t
    ResponseQuantities U; //!< response quantities at time t + delta t
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    WilsonTheta(AnalysisAggregation *);
    WilsonTheta(AnalysisAggregation *,double theta);
    WilsonTheta(AnalysisAggregation *,double theta,const RayleighDampingFactors &rF);    
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

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);        
  };
} // end of XC namespace

#endif


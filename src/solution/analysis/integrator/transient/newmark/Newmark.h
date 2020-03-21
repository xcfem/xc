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

// $Revision: 1.8 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Newmark.h,v $


#ifndef Newmark_h
#define Newmark_h

// Written : fmk 
// Created : 11/98
// Modified: 02/05 ahs
// Revision: A
//
// Description: This file contains the class definition for Newmark.
// Newmark is an algorithmic class for performing a transient analysis
// using the Newmark integration scheme.
//
// What: "@(#) Newmark.h, revA"

#include <solution/analysis/integrator/transient/newmark/NewmarkBase2.h>

namespace XC {

//! @ingroup NewmarkIntegrator
//
//! @brief The two parameter time-stepping method developed by
//! Nathan M. Newmark.
//!
//! Newmark is a subclass of TransientIntegrator which implements
//! the Newmark method. In the Newmark method, to determine the
//! velocities, accelerations and displacements at time \f$t + \Delta t\f$,
//! the equilibrium equation (expressed for the TransientIntegrator) is
//! typically solved at time \f$t + \Delta t\f$ for \f$U_{t+\Delta t}\f$,
//! i.e. solve: 
//! 
//! \[ R (U_{t + \Delta t}) = P(t + \Delta t) - F_I(Udd_{t+\Delta t})
//! - F_R(Ud_{t + \Delta t},U_{t + \Delta t}) \]
//! 
//! \noindent for \f$U_{t+\Delta t}\f$. The following difference relations
//!  are used to relate \f$Ud_{t + \Delta t}\f$ and \f$Udd_{t + \Delta t}\f$ to
//! \f$U_{t + \Delta t}\f$ and the response quantities at time \f$t\f$:
//! 
//! \[
//! \dot U_{t + \Delta t} = \frac{\gamma}{\beta \Delta t}
//! \left( U_{t + \Delta t} - U_t \right)
//!  + \left( 1 - \frac{\gamma}{\beta}\right) \dot U_t + \Delta t \left(1
//! - \frac{\gamma}{2 \beta}\right) \ddot U_t 
//! \]
//! 
//! \[
//! \ddot U_{t + \Delta t} = \frac{1}{\beta {\Delta t}^2}
//! \left( U_{t+\Delta t} - U_t \right)
//!  - \frac{1}{\beta \Delta t} \dot U_t + \left( 1 - \frac{1}{2
//! \beta} \right) \ddot U_t 
//! \]
//! 
//! \noindent which  results in the following 
//! 
//! \[ \left[ \frac{1}{\beta \Delta t^2} M + \frac{\gamma}{\beta \Delta t}
//! C + K \right] \Delta U_{t + \Delta t}^{(i)} = P(t + \Delta t) -
//! F_I\left(Udd_{t+\Delta  t}^{(i-1)}\right)
//! - F_R\left(Ud_{t + \Delta t}^{(i-1)},U_{t + \Delta t}^{(i-1)}\right) \]
//! 
//! \noindent An alternative approach, which does not involve \f$\Delta t\f$
//!  in the denumerator (useful for impulse problems), is to solve for the
//!  accelerations at time \f$t + \Delta t\f$ 
//! 
//! \[ R (Udd_{t + \Delta t}) = P(t + \Delta t) - F_I(Udd_{t+\Delta t})
//! - F_R(Ud_{t + \Delta t},U_{t + \Delta t}) \]
//! 
//! \noindent where we use following functions to relate \f$U_{t + \Delta
//!  t}\f$ and \f$Ud_{t + \Delta t}\f$ to \f$Udd_{t + \Delta t}\f$ and the
//!  response quantities at time \f$t\f$:
//! 
//! \[
//! U_{t + \Delta t} = U_t + \Delta t Ud_t + \left[
//! \left(\frac{1}{2} - \beta\right)Udd_t + \beta Udd_{t + \Delta
//!  t}\right] \Delta t^2
//! \]
//! 
//! \[
//! Ud_{t + \Delta t} = Ud_t + \left[ \left(1 - \gamma\right)Udd_t +
//! \gammaUdd_{t + \Delta t}\right] \Delta t
//! \]
//! 
//! \noindent which results in the following 
//! 
//! \[ \left[ M + \gamma \Delta t C + \beta \Delta t^2 K \right] \Delta
//! Udd_{t + \Delta t}^{(i)} = P(t + \Delta t) - F_I\left(Udd_{t+\Delta 
//!  t}^{(i-1)}\right)
//! - F_R\left(Ud_{t + \Delta t}^{(i-1)},U_{t + \Delta
//!  t}^{(i-1)}\right) \]
class Newmark: public NewmarkBase2
  {
  protected:
    bool displ; //!< a flag indicating whether displ or accel increments.
    ResponseQuantities Ut; //!< response quantities at time t.
    bool determiningMass; //!< flag to check if just want the mass contribution.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    Newmark(AnalysisAggregation *);
    Newmark(AnalysisAggregation *,double gamma, double beta, bool disp = true);
    Newmark(AnalysisAggregation *,double gamma, double beta,const RayleighDampingFactors &rF, bool disp = true);
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
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0) const;        
    
    // AddingSensitivity:BEGIN //////////////////////////////////
    int revertToStart();
    // AddingSensitivity:END ////////////////////////////////////
    
  };
inline Integrator *Newmark::getCopy(void) const
  { return new Newmark(*this); }
} // end of XC namespace

#endif

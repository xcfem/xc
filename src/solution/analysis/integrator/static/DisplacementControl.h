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
// $Date: 2003/02/14 23:00:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/DisplacementControl.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/DisplacementControl.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//

#ifndef DisplacementControl_h
#define DisplacementControl_h

#include <solution/analysis/integrator/static/DispBase.h>

namespace XC {

class Domain;

//! @ingroup StaticIntegrator
//
//! @brief Algorithmic class for performing a static analysis
//! using the displacement control method.
//!
//! DisplacementControl is a subclass of StaticIntegrator, it is
//! used to when performing a static analysis on the FE\_Model using the
//! displacement control method. In the displacement control method the
//! displacement at a specified degree-of-freedom Uc is specified for each
//! iteration. The following constraint equation is added to
//! equation~\ref{staticFormTaylor} of the StaticIntegrator class: 
//! 
//! \[ 
//!  Uc_n^{(i)} - Uc_{n-1} = \delta Uc_n
//! \]
//! 
//! \noindent where \f$\delta Uc_n\f$ depends on \f$\delta Uc_{n-1}\f$,
//! the displacement increment at the previous time step, \f$J_{n-1}\f$,
//! the number of iterations required to achieve convergence in the
//! previous load step, and \f$Jd\f$, the desired number of iteraions. \f$\delta
//! Uc_n\f$ is bounded by \f$\delta Uc_{min}\f$  and \f$\delta Uc_{max}\f$.
//! 
//! 
//! \[ 
//! \delta Ucn = max \left( \delta Uc{min}, min \left(
//! \frac{Jd}{J_{n-1}} \delta Uc{n-1}, \delta Uc{max} \right) \right)
//! \]
class DisplacementControl: public DispBase
  {
  protected:
    int theNodeTag; //!< the node that is being followed
    int theDof; //!< the dof at the node being followed
    int theDofID; //!< the system level id of the dof being followed
    
    double theIncrement; //!< deltaU at step (i)    
    double minIncrement, maxIncrement; //!< min/max values of deltaU at (i)

    const Domain *getDomainPtr(void) const;
    Domain *getDomainPtr(void);

    friend class AnalysisAggregation;
    DisplacementControl(AnalysisAggregation *); 
    DisplacementControl(AnalysisAggregation *,int nodeTag, int dof, double increment,int numIncrStep, double minIncrement, double maxIncrement);
    Integrator *getCopy(void) const;

  public:
    ~DisplacementControl(void);

    int newStep(void);    
    int commit(void);
    int update(const Vector &deltaU);
    int domainChanged(void);

    inline void setNodeTag(const int &tag)
      { theNodeTag= tag; }
    inline int getNodeTag(void) const
      { return theNodeTag; }
    inline void setDof(const int &dof)
      { theDof= dof; }
    inline int getDof(void) const
      { return theDof; }
    void setIncrement(const double &);
    inline double getIncrement(void) const
      { return theIncrement; }
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif


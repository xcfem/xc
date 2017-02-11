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
// Description: This file contains the class definition for DisplacementControl.
// DisplacementControl is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: 
//  i=1        delta U^T delta U + alpha^2 delta lambda^2 = delta s^2
//  i>1        dU^T delta U + alpha^2 dLambda delta lambda = 0
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and DisplacementControl is a control parameter.
//
// What: "@(#) DisplacementControl.h, revA"

#ifndef DisplacementControl_h
#define DisplacementControl_h

#include <solution/analysis/integrator/static/DispBase.h>

namespace XC {

class Domain;

//! @ingroup StaticIntegrator
//
//! @brief Especifica el incremento de desplazamiento a aplicar en un grado
//! de libertad del domain para cada paso of the analysis.
class DisplacementControl: public DispBase
  {
  protected:
    int theNode; //!< the node that is being followed
    int theDof; //!< the dof at the node being followed
    double theIncrement; //!< deltaU at step (i)
    int theDofID; //!< the system level id of the dof being followed
    
    double minIncrement, maxIncrement; // min/max values of deltaU at (i)

    const Domain *getDomainPtr(void) const;
    Domain *getDomainPtr(void);

    friend class SoluMethod;
    DisplacementControl(SoluMethod *); 
    DisplacementControl(SoluMethod *,int node, int dof, double increment,int numIncrStep, double minIncrement, double maxIncrement);
    Integrator *getCopy(void) const;

  public:
    ~DisplacementControl(void);

    int newStep(void);    
    int commit(void);
    int update(const Vector &deltaU);
    int domainChanged(void);

    inline void setNodeTag(const int &tag)
      { theNode= tag; }
    inline int getNodeTag(void) const
      { return theNode; }
    inline void setDof(const int &dof)
      { theDof= dof; }
    inline int getDof(void) const
      { return theDof; }
    void setIncrement(const double &);
    inline double getIncrement(void) const
      { return theIncrement; }
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);

  };
inline Integrator *DisplacementControl::getCopy(void) const
  { return new DisplacementControl(*this); }
} // end of XC namespace

#endif


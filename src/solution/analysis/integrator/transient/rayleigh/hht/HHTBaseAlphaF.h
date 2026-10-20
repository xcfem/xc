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

// $Revision: 1.1 $
// $Date: 2005/12/19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTBaseAlphaF.h,v $

#ifndef HHTBaseAlphaF_h
#define HHTBaseAlphaF_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the class definition for HHTBaseAlphaF.
// HHTBaseAlphaF is an algorithmic class for performing a transient analysis
// using the HHTBaseAlphaF integration scheme.
//
// What: "@(#) HHTBaseAlphaF.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/hht/HHTBase.h"

namespace XC {
class DOF_Group;
class FE_Element;
class Vector;
class ConvergenceTest;

//! @ingroup RayleighIntegrator
//
//! @brief HHTBaseAlphaF is an algorithmic class
//! for performing a transient analysis
//! using the HHTBaseAlphaF integration scheme.
class HHTBaseAlphaF: public HHTBase
  {
  protected:
    double alphaF;

    inline const double &alphaI(void) const
      { return alpha; }
    inline double &alphaI(void)
      { return alpha; }

    int sendData(Communicator &);
    int recvData(const Communicator &);

    friend class SolutionStrategy;
    HHTBaseAlphaF(SolutionStrategy *, int classTag);
    HHTBaseAlphaF(SolutionStrategy *, int classTag,
		  double alphaI, double alphaF);
    HHTBaseAlphaF(SolutionStrategy *, int classTag,
		  double alphaI, double alphaF,
		  const RayleighDampingFactors &rF);
    HHTBaseAlphaF(SolutionStrategy *,int classTag,
		  double alphaI, double alphaF,
		  double beta, double gamma);
    HHTBaseAlphaF(SolutionStrategy *,int classTag,
		  double alphaI, double alphaF,
		  double beta, double gamma,
		  const RayleighDampingFactors &rF);    
  public:
    inline double getAlphaI(void) const
      { return this->alphaI(); } 
    inline void setAlphaI(const double &d)
      { this->alphaI()= d; }
    inline double getAlphaF(void) const
      { return this->alphaF; } 
    inline void setAlphaF(const double &d)
      { this->alphaF= d; }
  
    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif

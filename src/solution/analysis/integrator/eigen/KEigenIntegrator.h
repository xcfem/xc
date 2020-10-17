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
                                                                        
#ifndef KEigenIntegrator_h
#define KEigenIntegrator_h

#include "solution/analysis/integrator/EigenIntegrator.h"

namespace XC {


//! @ingroup EigenIntegrator
//
//! @brief Integrator used to obtain the eigenvalues and
//! eigenvectors of the stiffness matrix.
class KEigenIntegrator: public EigenIntegrator
  {
  protected:
    friend class SolutionStrategy;
    KEigenIntegrator(SolutionStrategy *);
    Integrator *getCopy(void) const;
  public:
     
    // methods to form the M= I (identity) matrix.
    virtual int formM(void);
     
    // methods to instruct the FE_Element and DOF_Group objects
    // how to determining their contribution to M
    virtual int formEleTangM(FE_Element *);
    virtual int formNodTangM(DOF_Group *);

  };
inline Integrator *KEigenIntegrator::getCopy(void) const
  { return new KEigenIntegrator(*this); }
} // end of XC namespace

#endif





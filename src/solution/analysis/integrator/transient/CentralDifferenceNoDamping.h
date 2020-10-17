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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/02/22 22:21:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifferenceNoDamping.h,v $
                                                                        
#ifndef CentralDifferenceNoDamping_h
#define CentralDifferenceNoDamping_h

// Written: fmk 
// Created: 02/05
// Revision: A
//
// Description: This file contains the class definition for CentralDifferenceNoDamping.
// CentralDifferenceNoDamping is an algorithmic class for performing a transient 
// analysis using the Central Difference Scheme as implemented in Dyna
//       An = M(-1) (Pn - Fn)
//       Vn+1/2 = Vn-1/2 + dT * An
//       Dn+1   = Dn + deltaT * Vn+1/2
// which is an explicit direct integration scheme as outlined in the paper:
// Goudreau, G.L. and J.O. Hallquist, "Recent Developments in Large Scale Finite Element Lagrangian 
// Hydrocode Technology", Journal of Computer Methods in Applied Mechanics and Engineering, 30, 1982.
//
// What: "@(#) CentralDifferenceNoDamping.h, revA"

#include <solution/analysis/integrator/transient/CentralDifferenceBase.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief CentralDifferenceNoDamping is an algorithmic class
//! for performing a transient analysis using the Central
//! Difference Scheme as implemented in Dyna.
class CentralDifferenceNoDamping: public CentralDifferenceBase
  {
  private:
    ResponseQuantities U; //!< response quantities at time t + deltaT

    friend class SolutionStrategy;
    friend class FEM_ObjectBroker;
    CentralDifferenceNoDamping(SolutionStrategy *);
    Integrator *getCopy(void) const;
  public:

    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleResidual(FE_Element *theEle);
    int formNodUnbalance(DOF_Group *theDof);    

    int domainChanged(void);    
    int update(const Vector &deltaU);

    int commit(void);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };
inline Integrator *CentralDifferenceNoDamping::getCopy(void) const
  { return new CentralDifferenceNoDamping(*this); }
} // end of XC namespace

#endif


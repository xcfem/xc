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
// $Date: 2005/01/27 04:32:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifferenceAlternative.h,v $
                                                                        
#ifndef CentralDifferenceAlternative_h
#define CentralDifferenceAlternative_h

// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for CentralDifferenceAlternative.
// CentralDifferenceAlternative is an algorithmic class for performing a transient 
// analysis using the alternative form of the Central Differenceintegration scheme, 
// which is an explicit direct integration scheme as outlined in the book 'Concepts
// and Applications of Finite Element Analysis' by Cook, Malkus & Plesha.
//
// What: "@(#) CentralDifferenceAlternative.h, revA"

#include <solution/analysis/integrator/transient/CentralDifferenceBase.h>

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief CentralDifferenceAlternative is an algorithmic class
//! for performing a transient analysis using the alternative
//! form of the Central Differenceintegration scheme, 
//! which is an explicit direct integration scheme as
//! outlined in the book 'Concepts
//! and Applications of Finite Element Analysis' by Cook, Malkus & Plesha.
class CentralDifferenceAlternative : public CentralDifferenceBase
  {
  private:
    Vector Ut, Utp1;  // disp response quantities at time t and t + deltaT

    friend class AnalysisAggregation;
    friend class FEM_ObjectBroker;
    CentralDifferenceAlternative(AnalysisAggregation *);
    Integrator *getCopy(void) const;
  public:

    int domainChanged(void);    
    int update(const Vector &deltaU);

    int commit(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

  };
inline Integrator *CentralDifferenceAlternative::getCopy(void) const
  { return new CentralDifferenceAlternative(*this); }
} // end of XC namespace

#endif


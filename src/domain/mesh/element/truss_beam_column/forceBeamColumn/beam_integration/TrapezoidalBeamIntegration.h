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
// $Date: 2007-10-12 20:57:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/TrapezoidalBeamIntegration.h,v $

#ifndef TrapezoidalBeamIntegration_h
#define TrapezoidalBeamIntegration_h

#include "BeamIntegration.h"

namespace XC {

//! @brief Trapezoidal integration scheme.
//! @ingroup BeamInteg
class TrapezoidalBeamIntegration: public BeamIntegration
  {
  public:
    TrapezoidalBeamIntegration();

    void getSectionLocations(int nIP, double L, double *xi) const;
    void getSectionWeights(int nIP, double L, double *wt) const;

    BeamIntegration *getCopy(void) const;

    // These two methods do nothing
    int sendSelf(Communicator &)
      {return 0;}
    int recvSelf(const Communicator &)
      {return 0;}
    void Print(std::ostream &s, int flag = 0) const;  
  };
} // end of XC namespace

#endif

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
// $Date: 2007-10-12 20:57:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/TrapezoidalBeamIntegration.cpp,v $

#include "TrapezoidalBeamIntegration.h"

XC::TrapezoidalBeamIntegration::TrapezoidalBeamIntegration()
  : BeamIntegration(BEAM_INTEGRATION_TAG_Trapezoidal)
  {
    // Nothing to do
  }

XC::BeamIntegration *XC::TrapezoidalBeamIntegration::getCopy(void) const
  { return new TrapezoidalBeamIntegration(*this); }

void XC::TrapezoidalBeamIntegration::getSectionLocations(int numSections, double L,double *xi) const
  {
    if(numSections > 1)
      {
        xi[0] = -1.0;
        xi[numSections-1] = 1.0;

        double dxi = 2.0/(numSections-1);

        for(int i = 1; i < numSections-1; i++)
          xi[i] = -1.0 + dxi*i;
      }
    for (int i = 0; i < numSections; i++)
      xi[i]  = 0.5*(xi[i] + 1.0);
  }

void XC::TrapezoidalBeamIntegration::getSectionWeights(int numSections, double L, double *wt) const
  {
    if(numSections > 1)
      {
        const double wti= 2.0/(numSections-1);
        for(int i = 1; i < numSections-1; i++)
          wt[i] = wti;

        wt[0] = wt[numSections-1] = 0.5*wti;
      }
    for(int i = 0; i < numSections; i++)
      wt[i] *= 0.5;
  }

void XC::TrapezoidalBeamIntegration::Print(std::ostream &s, int flag) const
  {
    s << "Trapezoidal" << std::endl;
  }

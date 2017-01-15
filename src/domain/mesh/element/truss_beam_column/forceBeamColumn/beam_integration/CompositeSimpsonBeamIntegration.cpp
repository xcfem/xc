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

// $Revision$
// $Date$
// $Source$

#include "CompositeSimpsonBeamIntegration.h"

XC::CompositeSimpsonBeamIntegration::CompositeSimpsonBeamIntegration():
  BeamIntegration(BEAM_INTEGRATION_TAG_CompositeSimpson)
  {
    // Nothing to do
  }

XC::BeamIntegration *XC::CompositeSimpsonBeamIntegration::getCopy(void) const
  { return new CompositeSimpsonBeamIntegration(*this); }

void XC::CompositeSimpsonBeamIntegration::getSectionLocations(int numSections, double L, double *xi) const
  {
    // Check that num sections is odd
    if(numSections % 2 == 1)
      {
        int numIntervals = (numSections+1)/2; // Num intervals is even
        double h = 1.0/numIntervals;
        xi[0] = 0.0;
        xi[numSections-1] = 1.0;
        for(int i = 1; i < numSections-1; i++)
          xi[i] = h*i;    
      }
    else
      {
	std::cerr << "CompositeSimpson, numSections must be odd (" << numSections << " was input)" << std::endl;
      }
  }

void XC::CompositeSimpsonBeamIntegration::getSectionWeights(int numSections, double L, double *wt) const
  {
    // Check that num sections is odd
    if(numSections % 2 == 1)
      {
        const int numIntervals = (numSections+1)/2; // Num intervals is even
        const double h = 1.0/numIntervals;
        wt[0] = h/3.0;
        wt[numSections-1] = h/3.0;
        for(int i = 1; i < numSections; i += 2)
          wt[i] = 4*h/3.0;  
        for(int i = 2; i < numSections-1; i += 2)
          wt[i] = 2*h/3.0;  
      }
    else 
      {
        std::cerr << "CompositeSimpson, numSections must be odd (" << numSections << " was input)" << std::endl;
      }
  }

void XC::CompositeSimpsonBeamIntegration::Print(std::ostream &s, int flag)
  {
    s << "CompositeSimpson" << std::endl;
  }

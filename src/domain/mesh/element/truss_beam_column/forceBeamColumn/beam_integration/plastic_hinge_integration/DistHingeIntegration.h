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
// $Date: 2006/01/18 22:11:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/DistHingeIntegration.h,v $

#ifndef DistHingeIntegration_h
#define DistHingeIntegration_h

#include "PlasticLengthsBeamIntegration.h"

namespace XC {

//! @ingroup PlasticHingeBeamInteg
//
//! @brief Standard Gauss–Lobatto integration rule for distributed plasticity.
class DistHingeIntegration: public PlasticLengthsBeamIntegration
  {
  private:
    BeamIntegration *beamInt;

    void free_mem(void);
    void copy(const BeamIntegration *bi);
  public:
    DistHingeIntegration(double lpI, double lpJ,const BeamIntegration &bi);
    DistHingeIntegration(void);
    DistHingeIntegration(const DistHingeIntegration &);
    DistHingeIntegration &operator=(const DistHingeIntegration &);
    ~DistHingeIntegration();
  
    void getSectionLocations(int numSections, double L, double *xi) const;
    void getSectionWeights(int numSections, double L, double *wt) const;
  
    BeamIntegration *getCopy(void) const;

    void Print(std::ostream &s, int flag = 0) const;

    void getLocationsDeriv(int nIP, double L, double dLdh, double *dptsdh);
    void getWeightsDeriv(int nIP, double L, double dLdh, double *dwtsdh);
  };
} // end of XC namespace

#endif

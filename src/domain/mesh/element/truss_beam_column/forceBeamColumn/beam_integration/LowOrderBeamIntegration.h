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
// $Date: 2007-10-12 21:03:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/LowOrderBeamIntegration.h,v $

#ifndef LowOrderBeamIntegration_h
#define LowOrderBeamIntegration_h

#include "ParameterIDBeamIntegration.h"
namespace XC {

class LowOrderBeamIntegration : public ParameterIDBeamIntegration
  {
  protected:
    int Nc;
    bool computed;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    LowOrderBeamIntegration(int nIP, const Vector &pt, int nc, const Vector &wt);
    LowOrderBeamIntegration();
  
    void getSectionLocations(int numSections, double L, double *xi);
    void getSectionWeights(int numSections, double L, double *wt);

    BeamIntegration *getCopy(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    void Print(std::ostream &s, int flag = 0);  

    void getLocationsDeriv(int nIP, double L, double dLdh, double *dptsdh);
    void getWeightsDeriv(int nIP, double L, double dLdh, double *dwtsdh);
  };
} // end of XC namespace

#endif

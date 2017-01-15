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
// $Date: 2006/01/17 21:32:35 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeMidpointBeamIntegration.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/Information.h>
#include "domain/component/Parameter.h"

XC::HingeMidpointBeamIntegration::HingeMidpointBeamIntegration(double lpi,double lpj)
  : PlasticLengthsBeamIntegration(BEAM_INTEGRATION_TAG_HingeMidpoint,lpi,lpj) {}

XC::HingeMidpointBeamIntegration::HingeMidpointBeamIntegration()
  : PlasticLengthsBeamIntegration(BEAM_INTEGRATION_TAG_HingeMidpoint) {}

void XC::HingeMidpointBeamIntegration::getSectionLocations(int numSections, double L,double *xi) const
{
  double halfOneOverL = 0.5/L;

  xi[0] = lpI*halfOneOverL;
  xi[3] = 1.0-lpJ*halfOneOverL;

  double alpha = 0.5-(lpI+lpJ)*halfOneOverL;
  double beta  = 0.5+(lpI-lpJ)*halfOneOverL;
  xi[1] = alpha*(-1/sqrt(3.0)) + beta;
  xi[2] = alpha*(1/sqrt(3.0)) + beta;

  for (int i = 4; i < numSections; i++)
    xi[i] = 0.0;
}

void XC::HingeMidpointBeamIntegration::getSectionWeights(int numSections, double L,double *wt) const
{
  double oneOverL = 1.0/L;

  wt[0] = lpI*oneOverL;
  wt[3] = lpJ*oneOverL;

  wt[1] = 0.5-0.5*(lpI+lpJ)*oneOverL;
  wt[2] = 0.5-0.5*(lpI+lpJ)*oneOverL;

  for (int i = 4; i < numSections; i++)
    wt[i] = 1.0;
}

XC::BeamIntegration *XC::HingeMidpointBeamIntegration::getCopy(void) const
  { return new HingeMidpointBeamIntegration(*this); }



int XC::HingeMidpointBeamIntegration::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0] == "lpI")
      return param.addObject(1, this);
    else if(argv[0] == "lpJ")
      return param.addObject(2, this);
    else if(argv[0] == "lp")
      return param.addObject(3, this);
    else 
      return -1;
  }

int XC::HingeMidpointBeamIntegration::updateParameter(int parameterID,Information &info)
{
  switch (parameterID) {
  case 1:
    lpI = info.theDouble;
    return 0;
  case 2:
    lpJ = info.theDouble;
    return 0;
  case 3:
    lpI = lpJ = info.theDouble;
    return 0;
  default:
    return -1;
  }
}

int XC::HingeMidpointBeamIntegration::activateParameter(int paramID)
{
  parameterID = paramID;

  // For Terje to do
  return 0;
}

void
XC::HingeMidpointBeamIntegration::Print(std::ostream &s, int flag)
{
  s << "HingeMidpoint" << std::endl;
  s << " lpI = " << lpI;
  s << " lpJ = " << lpJ << std::endl;

  return;
}

void XC::HingeMidpointBeamIntegration::getLocationsDeriv(int numSections, double L,double dLdh, double *dptsdh)
{
  double oneOverL = 1.0/L;
  double halfOneOverL = 0.5*oneOverL;

  for (int i = 0; i < numSections; i++)
    dptsdh[i] = 0.0;

  if (parameterID == 1) { // lpI
    dptsdh[0] = halfOneOverL;
    dptsdh[1] = -0.5*(1.0-1/sqrt(3.0))*oneOverL + oneOverL;
    dptsdh[2] = -0.5*(1.0+1/sqrt(3.0))*oneOverL + oneOverL;
  }
  if (parameterID == 2) { // lpJ
    dptsdh[1] = -0.5*(1.0-1/sqrt(3.0))*oneOverL;
    dptsdh[2] = -0.5*(1.0+1/sqrt(3.0))*oneOverL;
    dptsdh[3] = -halfOneOverL;
  }
  if (parameterID == 3) { // lpI and lpJ
    dptsdh[0] = halfOneOverL;
    dptsdh[1] = -(1.0-1/sqrt(3.0))*oneOverL + oneOverL;
    dptsdh[2] = -(1.0+1/sqrt(3.0))*oneOverL + oneOverL;
    dptsdh[3] = -halfOneOverL;
  }

  if (dLdh != 0.0) {
    // STILL TO DO
    std::cerr << "getPointsDeriv -- to do" << std::endl;
    dptsdh[0] = -0.5*(lpI*dLdh)/(L*L);
    dptsdh[1] = dLdh + 0.5*(lpJ*dLdh)/(L*L);
  }

  return;
}

void XC::HingeMidpointBeamIntegration::getWeightsDeriv(int numSections, double L,double dLdh, double *dwtsdh)
{
  double oneOverL = 1.0/L;

  for (int i = 0; i < numSections; i++)
    dwtsdh[i] = 0.0;

  if (parameterID == 1) { // lpI
    dwtsdh[0] = oneOverL;
    dwtsdh[1] = -0.5*oneOverL;
    dwtsdh[2] = -0.5*oneOverL;
  }
  if (parameterID == 2) { // lpJ
    dwtsdh[1] = -0.5*oneOverL;
    dwtsdh[2] = -0.5*oneOverL;
    dwtsdh[3] = oneOverL;
  }
  if (parameterID == 3) { // lpI and lpJ
    dwtsdh[0] = oneOverL;
    dwtsdh[1] = -oneOverL;
    dwtsdh[2] = -oneOverL;
    dwtsdh[3] = oneOverL;
  }

  if (dLdh != 0.0) {
    dwtsdh[0] = -lpI*dLdh/(L*L);
    dwtsdh[3] = -lpJ*dLdh/(L*L);
    // STILL TO DO
    std::cerr << "getWeightsDeriv -- to do" << std::endl;
  }

  return;
}

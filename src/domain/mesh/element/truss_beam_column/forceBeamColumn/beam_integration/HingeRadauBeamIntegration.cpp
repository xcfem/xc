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
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeRadauBeamIntegration.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::HingeRadauBeamIntegration::HingeRadauBeamIntegration(double lpi,double lpj)
  : PlasticLengthsBeamIntegration(BEAM_INTEGRATION_TAG_HingeRadau,lpi,lpj) {}

XC::HingeRadauBeamIntegration::HingeRadauBeamIntegration(void)
  : PlasticLengthsBeamIntegration(BEAM_INTEGRATION_TAG_HingeRadau) {}

void XC::HingeRadauBeamIntegration::getSectionLocations(int numSections, double L,double *xi) const
  {
    double oneOverL = 1.0/L;

    xi[0] = 0.0;
    xi[1] = 8.0/3*lpI*oneOverL;
    xi[4] = 1.0-8.0/3*lpJ*oneOverL;
    xi[5] = 1.0;

    double alpha = 0.5-2*(lpI+lpJ)*oneOverL;
    double beta  = 0.5+2*(lpI-lpJ)*oneOverL;
    xi[2] = alpha*(-1/sqrt(3.0)) + beta;
    xi[3] = alpha*(1/sqrt(3.0)) + beta;

    for(int i = 6; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::HingeRadauBeamIntegration::getSectionWeights(int numSections, double L,double *wt) const
  {
    double oneOverL = 1.0/L;

    wt[0] = lpI*oneOverL;
    wt[1] = 3*lpI*oneOverL;
    wt[4] = 3*lpJ*oneOverL;
    wt[5] = lpJ*oneOverL;

    wt[2] = 0.5-2*(lpI+lpJ)*oneOverL;
    wt[3] = 0.5-2*(lpI+lpJ)*oneOverL;

    for(int i = 6; i < numSections; i++)
      wt[i] = 1.0;
  }

XC::BeamIntegration *XC::HingeRadauBeamIntegration::getCopy(void) const
  { return new HingeRadauBeamIntegration(*this); }

//! @brief Send object members through the channel defined in cp.
int XC::HingeRadauBeamIntegration::sendData(CommParameters &cp)
  {
    int res= PlasticLengthsBeamIntegration::sendData(cp);
    return res;
  }

//! @brief Receives object members through the channel defined in cp.
int XC::HingeRadauBeamIntegration::recvData(const CommParameters &cp)
  {
    int res= PlasticLengthsBeamIntegration::recvData(cp);
    return res;
  }

//! @brief Sends object through the channel defined in cp.
int XC::HingeRadauBeamIntegration::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel defined in cp.
int XC::HingeRadauBeamIntegration::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::HingeRadauBeamIntegration::Print(std::ostream &s, int flag)
  {
    PlasticLengthsBeamIntegration::Print(s,flag);
    s << "HingeRadau" << std::endl;
    return;
  }

void XC::HingeRadauBeamIntegration::getLocationsDeriv(int numSections,double L, double dLdh,double *dptsdh)
  {
    double oneOverL = 1.0/L;

    for(int i = 0; i < numSections; i++)
      dptsdh[i] = 0.0;

    if(parameterID == 1) { // lpI
    dptsdh[1] = 8.0/3*oneOverL;
    dptsdh[2] = -2*(1.0-1/sqrt(3.0))*oneOverL + 4*oneOverL;
    dptsdh[3] = -2*(1.0+1/sqrt(3.0))*oneOverL + 4*oneOverL;
  }

  if (parameterID == 2) { // lpJ
    dptsdh[2] = -2*(1.0-1/sqrt(3.0))*oneOverL;
    dptsdh[3] = -2*(1.0+1/sqrt(3.0))*oneOverL;
    dptsdh[4] = -8.0/3*oneOverL;
  }

  if (parameterID == 3) { // lpI and lpJ
    dptsdh[1] = 8.0/3*oneOverL;
    dptsdh[2] = -4*(1.0-1/sqrt(3.0))*oneOverL + 4*oneOverL;
    dptsdh[3] = -4*(1.0+1/sqrt(3.0))*oneOverL + 4*oneOverL;
    dptsdh[4] = -8.0/3*oneOverL;
  }

  if (dLdh != 0.0) {
    // STILL TO DO
    std::cerr << "getPointsDeriv -- to do" << std::endl;
  }

  return;
}

void XC::HingeRadauBeamIntegration::getWeightsDeriv(int numSections,double L, double dLdh,double *dwtsdh)
{
  double oneOverL = 1.0/L;

  for (int i = 0; i < numSections; i++)
    dwtsdh[i] = 0.0;

  if (parameterID == 1) { // lpI
    dwtsdh[0] = oneOverL;
    dwtsdh[1] = 3*oneOverL;
    dwtsdh[2] = -2*oneOverL;
    dwtsdh[3] = -2*oneOverL;
  }

  if (parameterID == 2) { // lpJ
    dwtsdh[2] = -2*oneOverL;
    dwtsdh[3] = -2*oneOverL;
    dwtsdh[4] = 3*oneOverL;
    dwtsdh[5] = oneOverL;
  }

  if (parameterID == 3) { // lpI and lpJ
    dwtsdh[0] = oneOverL;
    dwtsdh[1] = 3*oneOverL;
    dwtsdh[2] = -4*oneOverL;
    dwtsdh[3] = -4*oneOverL;
    dwtsdh[4] = 3*oneOverL;
    dwtsdh[5] = oneOverL;
  }

  if (dLdh != 0.0) {
    dwtsdh[0] = -lpI*dLdh/(L*L);
    dwtsdh[5] = -lpJ*dLdh/(L*L);
    // STILL TO DO
    std::cerr << "getWeightsDeriv -- to do" << std::endl;
  }

  return;
}

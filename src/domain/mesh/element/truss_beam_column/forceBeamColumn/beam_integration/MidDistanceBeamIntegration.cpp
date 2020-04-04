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

// $Revision: 1.2 $
// $Date: 2007-10-13 00:45:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/MidDistanceBeamIntegration.cpp,v $

#include "MidDistanceBeamIntegration.h"

XC::MidDistanceBeamIntegration::MidDistanceBeamIntegration(int nIP,const Vector &pt)
  : ParameterIDBeamIntegration(BEAM_INTEGRATION_TAG_MidDistance,pt)
  {

    for(int i = 0; i < nIP; i++)
      {
        int key = i;
        for(int j = i+1; j < nIP; j++)
          {
            if(pts(j) < pts(key))
              {
	        key = j;
	        std::cerr << "MidDistanceBeamIntegration::MidDistanceBeamIntegration -- point are not sorted; sort before calling constructor" << std::endl;
              }
          }
        //double temp = pts(i);
        //pts(i) = pts(key);
        //pts(key) = temp;
      }

    Vector mids(nIP-1);

    for(int i = 0; i < nIP-1; i++)
      {
        mids(i) = 0.5*(pts(i)+pts(i+1));
      }

    wts(0) = mids(0);
    wts(nIP-1) = 1.0-mids(nIP-2);
    for(int i = 1; i < nIP-1; i++)
      { wts(i) = mids(i)-mids(i-1); }
  }

XC::MidDistanceBeamIntegration::MidDistanceBeamIntegration(void)
  : ParameterIDBeamIntegration(BEAM_INTEGRATION_TAG_MidDistance) {}

void XC::MidDistanceBeamIntegration::getSectionLocations(int numSections, double L, double *xi)
  {
    const int nIP = pts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      xi[i] = pts(i);
    for( ; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::MidDistanceBeamIntegration::getSectionWeights(int numSections, double L, double *wt)
  {
    const int nIP = wts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      wt[i] = wts(i);
    for( ; i < numSections; i++)
      wt[i] = 1.0;
  }

XC::BeamIntegration *XC::MidDistanceBeamIntegration::getCopy(void) const
  { return new MidDistanceBeamIntegration(*this); }

//! @brief Sends object through the channel defined in cp.
int XC::MidDistanceBeamIntegration::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel defined in cp.
int XC::MidDistanceBeamIntegration::recvSelf(const Communicator &comm)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

int XC::MidDistanceBeamIntegration::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    return -1;
  }

int XC::MidDistanceBeamIntegration::updateParameter(int parameterID, Information &info)
  {
    // Does nothing for now -- MHS
    return 0;
  }


void XC::MidDistanceBeamIntegration::Print(std::ostream &s, int flag) const
  {
    s << "MidDistance" << std::endl;
    s << " Points: " << pts;
    s << " Weights: " << wts;
  }

void XC::MidDistanceBeamIntegration::getLocationsDeriv(int numSections, double L, double *dptsdh)
  {
    for (int i = 0; i < numSections; i++)
      dptsdh[i] = 0.0;
    return;
  }

void XC::MidDistanceBeamIntegration::getWeightsDeriv(int numSections, double L, double *dwtsdh)
  {
    for (int i = 0; i < numSections; i++)
      dwtsdh[i] = 0.0;
    return;
  }

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

// $Revision: 1.10 $
// $Date: 2004/06/07 23:21:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeRadauBeamIntegration2d.cpp,v $
#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration2d.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::HingeRadauBeamIntegration2d::HingeRadauBeamIntegration2d(double e,
                                                         double a,
                                                         double i,
                                                         double lpi,
                                                         double lpj)
  : HingeBeamIntegration2d(BEAM_INTEGRATION_TAG_HingeRadau2d,e,a,i,lpi,lpj) {}


XC::HingeRadauBeamIntegration2d::HingeRadauBeamIntegration2d()
  : HingeBeamIntegration2d(BEAM_INTEGRATION_TAG_HingeRadau2d)
  {}

void XC::HingeRadauBeamIntegration2d::getSectionLocations(int numSections, double L,double *xi) const
  {
    xi[0] = 0.0;
    xi[1] = 1.0;
    for (int i = 2; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::HingeRadauBeamIntegration2d::getSectionWeights(int numSections, double L, double *wt) const
  {
    double oneOverL = 1.0/L;

    wt[0] = lpI*oneOverL;
    wt[1] = lpJ*oneOverL;
    for(int i = 2; i < numSections; i++)
      wt[i] = 1.0;
  }

int XC::HingeRadauBeamIntegration2d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    double oneOverL = 1.0/L;

    double Le = L-lpI-lpJ;
    fElastic(0,0) += Le/(E*A);

    double x[4];
    double w[4];
  
    static const double eight3 = 8.0/3.0;
    static const double oneOverRoot3 = 1.0/sqrt(3.0);

    double oneOverEI = 1.0/(E*I);
  
    x[0] = eight3*lpI;
    w[0] = 3.0*lpI;

    x[1] = L-eight3*lpJ;
    w[1] = 3.0*lpJ;

    Le = L-4.0*(lpI+lpJ);

    x[2] = 4.0*lpI + 0.5*Le*(1.0-oneOverRoot3);
    w[2] = 0.5*Le;

    x[3] = 4.0*lpI + 0.5*Le*(1.0+oneOverRoot3);
    w[3] = w[2];

    double tmp = 0.0;
    double xL, xL1, wt;
    for(int i = 0; i < 4; i++)
      {
        xL  = x[i]*oneOverL;
        xL1 = xL-1.0;
        wt = w[i]*oneOverEI;
        fElastic(1,1) += xL1*xL1*wt;
        fElastic(2,2) += xL*xL*wt;
        tmp           += xL*xL1*wt;
      }
    fElastic(1,2) += tmp;
    fElastic(2,1) += tmp;
    return -1;
  }

double XC::HingeRadauBeamIntegration2d::getTangentDriftI(double L, double LI,double q2, double q3)
{
  double oneOverL = 1.0/L;

  double betaI = 4*lpI*oneOverL;

  double qq2 = (1-betaI)*q2 - betaI*q3;

  betaI = 8.0/3*lpI*oneOverL;

  double qqq2 = (1-betaI)*q2 - betaI*q3;

  if (LI < lpI)
    return 0.0;
  else
    return (3*lpI)*(LI-8.0/3*lpI)*qqq2/(E*I) +
      (LI-4*lpI)/3*(LI-4*lpI)*qq2/(E*I);
}

double
XC::HingeRadauBeamIntegration2d::getTangentDriftJ(double L, double LI,
                                              double q2, double q3)
{
  double oneOverL = 1.0/L;

  double betaJ = 4*lpJ*oneOverL;

  double qq3 = (1-betaJ)*q3 - betaJ*q2;

  betaJ = 8.0/3*lpJ*oneOverL;

  double qqq3 = (1-betaJ)*q3 - betaJ*q2;

  if (LI > L-lpJ)
    return 0.0;
  else
    return (3*lpJ)*(L-LI-8.0/3*lpJ)*qqq3/(E*I) +
      (L-LI-4*lpJ)/3*(L-LI-4*lpJ)*qq3/(E*I);
}

//! @brief Virtual constructor.
XC::BeamIntegration *XC::HingeRadauBeamIntegration2d::getCopy(void) const
  { return new HingeRadauBeamIntegration2d(*this); }

//! @brief Sends object through the channel being passed as parameter.
int XC::HingeRadauBeamIntegration2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::HingeRadauBeamIntegration2d::recvSelf(const Communicator &comm)
  {
    inicComm(3);
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

int XC::HingeRadauBeamIntegration2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    // E of the beam interior
    if(argv[0] == "E")
      return param.addObject(1, this);
    // A of the beam interior
    if(argv[0] == "A")
      return param.addObject(2, this);
    // I of the beam interior
    if(argv[0] == "I")
      return param.addObject(3, this);
    if(argv[0] == "lpI")
      return param.addObject(4, this);
    else if(argv[0] == "lpJ")
      return param.addObject(5, this);
    else 
      return -1;
  }

int
XC::HingeRadauBeamIntegration2d::updateParameter(int parameterID,
                                             Information &info)
{
  switch (parameterID) {
  case 1:
    E = info.theDouble;
    return 0;
  case 2:
    A = info.theDouble;
    return 0;
  case 3:
    I = info.theDouble;
    return 0;
  case 4:
    lpI = info.theDouble;
    return 0;
  case 5:
    lpJ = info.theDouble;
    return 0;
  default:
    return -1;
  }
}

int XC::HingeRadauBeamIntegration2d::activateParameter(int parameterID)
  {
    // For Terje to do
    return 0;
  }

void XC::HingeRadauBeamIntegration2d::Print(std::ostream &s, int flag) const
  {
    s << "HingeRadau2d" << std::endl;
    HingeBeamIntegration2d::Print(s);
    return;
  }

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

// $Revision: 1.9 $
// $Date: 2004/06/07 23:21:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeMidpointBeamIntegration2d.cpp,v $

#include "HingeMidpointBeamIntegration2d.h"

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::HingeMidpointBeamIntegration2d::HingeMidpointBeamIntegration2d(double e,
                                                               double a,
                                                               double i,
                                                               double lpi,
                                                               double lpj)
  : HingeBeamIntegration2d(BEAM_INTEGRATION_TAG_HingeMidpoint2d,e,a,i,lpi,lpj) {}

XC::HingeMidpointBeamIntegration2d::HingeMidpointBeamIntegration2d()
  : HingeBeamIntegration2d(BEAM_INTEGRATION_TAG_HingeMidpoint2d) {}

void XC::HingeMidpointBeamIntegration2d::getSectionLocations(int numSections, double L,double *xi) const
  {
    double halfOneOverL = 0.5/L;

    xi[0] = lpI*halfOneOverL;
    xi[1] = 1.0-lpJ*halfOneOverL;
    for(int i = 2; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::HingeMidpointBeamIntegration2d::getSectionWeights(int numSections, double L, double *wt) const
  {
    double oneOverL = 1.0/L;

    wt[0] = lpI*oneOverL;
    wt[1] = lpJ*oneOverL;
    for(int i = 2; i < numSections; i++)
      wt[i] = 1.0;
  }

int XC::HingeMidpointBeamIntegration2d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    double oneOverL = 1.0/L;

    // Length of elastic interior
    double Le = L-lpI-lpJ;
    double LoverEA  = Le/(E*A);
    double Lover3EI = Le/(3*E*I);
    double Lover6EI = 0.5*Lover3EI;
  
    // Elastic flexibility of element interior
    static XC::Matrix fe(2,2);
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;
  
    // Equilibrium transformation matrix
    static XC::Matrix B(2,2);
    double betaI = lpI*oneOverL;
    double betaJ = lpJ*oneOverL;
    B(0,0) = 1.0 - betaI;
    B(1,1) = 1.0 - betaJ;
    B(0,1) = -betaI;
    B(1,0) = -betaJ;
  
    // Transform the elastic flexibility of the element
    // interior to the basic system
    static XC::Matrix ftmp(2,2);
    ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);

    fElastic(0,0) += LoverEA;
    fElastic(1,1) += ftmp(0,0);
    fElastic(1,2) += ftmp(0,1);
    fElastic(2,1) += ftmp(1,0);
    fElastic(2,2) += ftmp(1,1);
    return -1;
  }

double XC::HingeMidpointBeamIntegration2d::getTangentDriftI(double L, double LI,double q2, double q3)
  {
    double oneOverL = 1.0/L;
    double betaI = lpI*oneOverL;
    double qq2 = (1-betaI)*q2 - betaI*q3;

    if(LI < lpI)
      return 0.0;
    else
      return (LI-lpI)/3*(LI-lpI)*qq2/(E*I);
  }

double XC::HingeMidpointBeamIntegration2d::getTangentDriftJ(double L, double LI,double q2, double q3)
  {
    double oneOverL = 1.0/L;
    double betaJ = lpJ*oneOverL;
    double qq3 = (1-betaJ)*q3 - betaJ*q2;

    if(LI > L-lpJ)
      return 0.0;
    else
      return (L-LI-lpJ)/3*(L-LI-lpJ)*qq3/(E*I);
  }

//! @brief Virtual constructor.
XC::BeamIntegration *XC::HingeMidpointBeamIntegration2d::getCopy(void) const
  { return new HingeMidpointBeamIntegration2d(*this); }


//! @brief Sends object through the channel being passed as parameter.
int XC::HingeMidpointBeamIntegration2d::sendSelf(Communicator &comm)
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
int XC::HingeMidpointBeamIntegration2d::recvSelf(const Communicator &comm)
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

int XC::HingeMidpointBeamIntegration2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
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

int XC::HingeMidpointBeamIntegration2d::updateParameter(int parameterID,Information &info)
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

int XC::HingeMidpointBeamIntegration2d::activateParameter(int parameterID)
  {
    // For Terje to do
    return 0;
  }

void XC::HingeMidpointBeamIntegration2d::Print(std::ostream &s, int flag) const
  {
    s << "HingeMidpoint2d" << std::endl;
    HingeBeamIntegration2d::Print(s);
    return;
  }

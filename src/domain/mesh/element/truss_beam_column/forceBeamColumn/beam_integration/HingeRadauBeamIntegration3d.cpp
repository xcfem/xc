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
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeRadauBeamIntegration3d.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/Information.h>

XC::HingeRadauBeamIntegration3d::HingeRadauBeamIntegration3d(double e,
							 double a,
							 double iz,
							 double iy,
							 double g,
							 double j,
							 double lpi,
							 double lpj)
  : HingeBeamIntegration3d(BEAM_INTEGRATION_TAG_HingeRadau3d,e,a,iz,iy,g,j,lpi,lpj) {}

XC::HingeRadauBeamIntegration3d::HingeRadauBeamIntegration3d(const CrossSectionProperties3d &cts,const double &lpi,const double &lpj)
  : HingeBeamIntegration3d(BEAM_INTEGRATION_TAG_HingeRadau3d,cts,lpi,lpj) {}

void XC::HingeRadauBeamIntegration3d::getSectionLocations(int numSections, double L, double *xi) const
  {
    xi[0] = 0.0;
    xi[1] = 1.0;
    for (int i = 2; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::HingeRadauBeamIntegration3d::getSectionWeights(int numSections, double L,double *wt) const
  {
    const double oneOverL = 1.0/L;
    wt[0] = lpI*oneOverL;
    wt[1] = lpJ*oneOverL;
    for(int i = 2; i < numSections; i++)
      wt[i] = 1.0;
  }

int XC::HingeRadauBeamIntegration3d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    const double oneOverL = 1.0/L;
    double Le = L-lpI-lpJ;
    fElastic(0,0) += Le/(ctes_scc.EA());
    fElastic(5,5) += Le/(ctes_scc.GJ());

    double x[4];
    double w[4];
  
    static const double eight3 = 8.0/3.0;
    static const double oneOverRoot3 = 1.0/sqrt(3.0);

    const double oneOverEIz = 1.0/(ctes_scc.EIz());
    const double oneOverEIy = 1.0/(ctes_scc.EIy());
  
    x[0] = eight3*lpI;
    w[0] = 3.0*lpI;

    x[1] = L-eight3*lpJ;
    w[1] = 3.0*lpJ;

    Le = L-4.0*(lpI+lpJ);

    x[2] = 4.0*lpI + 0.5*Le*(1.0-oneOverRoot3);
    w[2] = 0.5*Le;

    x[3] = 4.0*lpI + 0.5*Le*(1.0+oneOverRoot3);
    w[3] = w[2];

    double tmpZ = 0.0;
    double tmpY = 0.0;
    double xL, xL1, wt;
    for(int i = 0; i < 4; i++)
      {
        xL  = x[i]*oneOverL;
        xL1 = xL-1.0;
        wt = w[i]*oneOverEIz;
        fElastic(1,1) += xL1*xL1*wt;
        fElastic(2,2) += xL*xL*wt;
        tmpZ          += xL*xL1*wt;
        wt = w[i]*oneOverEIy;
        fElastic(3,3) += xL1*xL1*wt;
        fElastic(4,4) += xL*xL*wt;
        tmpY          += xL*xL1*wt;
      }
    fElastic(1,2) += tmpZ;
    fElastic(2,1) += tmpZ;
    fElastic(3,4) += tmpY;
    fElastic(4,3) += tmpY;
    return -1;
  }

double XC::HingeRadauBeamIntegration3d::getTangentDriftI(double L, double LI,double q2, double q3, bool yAxis)
  {
    double oneOverL = 1.0/L;
    double betaI = 4*lpI*oneOverL;
    double qq2 = (1-betaI)*q2 - betaI*q3;

    betaI = 8.0/3*lpI*oneOverL;

    double qqq2 = (1-betaI)*q2 - betaI*q3;

    if(LI < lpI)
      return 0.0;
    else
      {
        double EI = yAxis ? ctes_scc.EIy() : ctes_scc.EIz();
        return (3*lpI)*(LI-8.0/3*lpI)*qqq2/EI + (LI-4*lpI)/3*(LI-4*lpI)*qq2/EI;
      }
  }

double XC::HingeRadauBeamIntegration3d::getTangentDriftJ(double L, double LI,double q2, double q3, bool yAxis)
  {
    const double oneOverL = 1.0/L;
    double betaJ = 4*lpJ*oneOverL;
    const double qq3 = (1-betaJ)*q3 - betaJ*q2;

    betaJ = 8.0/3*lpJ*oneOverL;

    double qqq3 = (1-betaJ)*q3 - betaJ*q2;

    if(LI > L-lpJ)
      return 0.0;
    else
      {
        const double EI = yAxis ? ctes_scc.EIy() : ctes_scc.EIz();
        return (3*lpJ)*(L-LI-8.0/3*lpJ)*qqq3/(EI) +
          (L-LI-4*lpJ)/3*(L-LI-4*lpJ)*qq3/(EI);
      }
  }

XC::BeamIntegration *XC::HingeRadauBeamIntegration3d::getCopy(void) const
  { return new HingeRadauBeamIntegration3d(*this); }

void XC::HingeRadauBeamIntegration3d::Print(std::ostream &s, int flag)
  {
    s << "HingeRadau3d" << std::endl;
    ctes_scc.Print(s);
    return;
  }

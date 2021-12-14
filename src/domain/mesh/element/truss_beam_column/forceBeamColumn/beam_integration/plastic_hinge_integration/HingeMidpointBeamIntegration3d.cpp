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

// $Revision: 1.8 $
// $Date: 2004/06/07 23:21:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeMidpointBeamIntegration3d.cpp,v $

#include "HingeMidpointBeamIntegration3d.h"

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>

XC::HingeMidpointBeamIntegration3d::HingeMidpointBeamIntegration3d(double e,double a,double iz,double iy,
							           double g,double j,double lpi,double lpj)
  : HingeBeamIntegration3d(BEAM_INTEGRATION_TAG_HingeMidpoint3d,e,a,iz,iy,g,j,lpi,lpj) {}

XC::HingeMidpointBeamIntegration3d::HingeMidpointBeamIntegration3d(const CrossSectionProperties3d &cts,const double &lpi,const double &lpj)
  : HingeBeamIntegration3d(BEAM_INTEGRATION_TAG_HingeMidpoint3d,cts,lpi,lpj) {}

XC::BeamIntegration *XC::HingeMidpointBeamIntegration3d::getCopy(void) const
  { return new HingeMidpointBeamIntegration3d(*this); }

void XC::HingeMidpointBeamIntegration3d::getSectionLocations(int numSections, double L, double *xi) const
  {
    const double halfOneOverL = 0.5/L;
    xi[0] = lpI*halfOneOverL;
    xi[1] = 1.0-lpJ*halfOneOverL;
    for(int i = 2;i<numSections; i++)
      xi[i]= 0.0;
  }

void XC::HingeMidpointBeamIntegration3d::getSectionWeights(int numSections, double L, double *wt) const
  {
    const double oneOverL = 1.0/L;
    wt[0] = lpI*oneOverL;
    wt[1] = lpJ*oneOverL;
    for (int i = 2; i < numSections; i++)
      wt[i]= 1.0;
  }

int XC::HingeMidpointBeamIntegration3d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    const double oneOverL = 1.0/L;

    // Length of elastic interior
    const double Le = L-lpI-lpJ;
    double Lover3EI = Le/(3*ctes_scc.EIz());
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

    fElastic(1,1) += ftmp(0,0);
    fElastic(1,2) += ftmp(0,1);
    fElastic(2,1) += ftmp(1,0);
    fElastic(2,2) += ftmp(1,1);

    Lover3EI = Le/(3*ctes_scc.EIy());
    Lover6EI = 0.5*Lover3EI;
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;
    ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);
    fElastic(3,3) += ftmp(0,0);
    fElastic(3,4) += ftmp(0,1);
    fElastic(4,3) += ftmp(1,0);
    fElastic(4,4) += ftmp(1,1);

    fElastic(0,0) += Le/(ctes_scc.EA());
    fElastic(5,5) += Le/(ctes_scc.GJ());

    return -1;
  }

double XC::HingeMidpointBeamIntegration3d::getTangentDriftI(double L, double LI,double q2, double q3, bool yAxis)
  {
    const double oneOverL = 1.0/L;
    const double betaI = lpI*oneOverL;
    const double qq2 = (1-betaI)*q2 - betaI*q3;
    if (LI < lpI)
      return 0.0;
    else
      {
        const double EI = yAxis ? ctes_scc.EIy() : ctes_scc.EIz();
        return (LI-lpI)/3*(LI-lpI)*qq2/(EI);
      }
  }

double XC::HingeMidpointBeamIntegration3d::getTangentDriftJ(double L, double LI,double q2, double q3, bool yAxis)
  {
    const double oneOverL = 1.0/L;
    const double betaJ = lpJ*oneOverL;
    const double qq3 = (1-betaJ)*q3 - betaJ*q2;

    if(LI > L-lpJ)
      return 0.0;
    else
      {
        const double EI = yAxis ? ctes_scc.EIy() : ctes_scc.EIz();
        return (L-LI-lpJ)/3*(L-LI-lpJ)*qq3/EI;
      }
  }

void XC::HingeMidpointBeamIntegration3d::Print(std::ostream &s, int flag) const
  {
    s << "HingeMidpoint3d" << std::endl;
    ctes_scc.Print(s);
    s << " lpI = " << lpI;
    s << " lpJ = " << lpJ << std::endl;
    return;
  }

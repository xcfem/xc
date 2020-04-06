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

// $Revision: 1.4 $
// $Date: 2003/06/10 00:36:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration2d.cpp,v $

#include "UserDefinedHingeIntegration2d.h"

#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d(int npL,
                                                             const XC::Vector &ptL,
                                                             const XC::Vector &wtL,
                                                             int npR,
                                                             const XC::Vector &ptR,
                                                             const XC::Vector &wtR,
                                                             const double &ee,
                                                             const double &aa,
                                                             const double &ii):
  UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge2d,ptL,wtL,ptR,wtR), ctes_scc(ee,aa,ii)
{}

XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d()
  : UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge2d), ctes_scc(0.0,0.0,0.0) {}

void XC::UserDefinedHingeIntegration2d::getSectionLocations(int numSections,double L, double *xi) const
  {
    int npL = ptsL().Size();
    int npR = ptsR.Size();

    int i, j;
    for (i = 0; i < npL; i++)
      xi[i] = ptsL()(i);
    for (j = 0; j < npR; j++, i++)
      xi[i] = ptsR(j);
    for ( ; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::UserDefinedHingeIntegration2d::getSectionWeights(int numSections,double L, double *wt) const
  {
    int npL = wtsL().Size();
    int npR = wtsR.Size();

    int i, j;
    for (i = 0; i < npL; i++)
      wt[i] = wtsL()(i);
    for (j = 0; j < npR; j++, i++)
      wt[i] = wtsR(j);
    for ( ; i < numSections; i++)
      wt[i] = 1.0;
  }

int XC::UserDefinedHingeIntegration2d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    int npL = wtsL().Size();
    int npR = wtsR.Size();

    double betaI = 0.0;
    double betaJ = 0.0;

    int i;
    for (i = 0; i < npL; i++)
      betaI += wtsL()(i);
    for (i = 0; i < npR; i++)
      betaJ += wtsR(i);

    // Length of elastic interior
    const double Le = L*(1.0-betaI-betaJ);
    const double LoverEA  = Le/(ctes_scc.EA());
    const double Lover3EI = Le/(3*ctes_scc.EI());
    const double Lover6EI = 0.5*Lover3EI;
  
    // Elastic flexibility of element interior
    static Matrix fe(2,2);
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;
  
    // Equilibrium transformation matrix
    static Matrix B(2,2);
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

void XC::UserDefinedHingeIntegration2d::addElasticDeformations(ElementalLoad *theLoad, double loadFactor, double L, double *v0)
  { return; }

XC::BeamIntegration *XC::UserDefinedHingeIntegration2d::getCopy(void) const
  { return new UserDefinedHingeIntegration2d(*this); }

int XC::UserDefinedHingeIntegration2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0] == "E")
      return param.addObject(1, this);
    else if(argv[0] == "A")
      return param.addObject(2, this);
    else if(argv[0] == "I")
      return param.addObject(3, this);
    else 
      return -1;
  }

int XC::UserDefinedHingeIntegration2d::updateParameter(int parameterID, Information &info)
  {
  switch (parameterID) {
  case 1:
    ctes_scc.setE(info.theDouble);
    return 0;
  case 2:
    ctes_scc.setA(info.theDouble);
    return 0;
  case 3:
    ctes_scc.setI(info.theDouble);
    return 0;
  default:
    return -1;
  }
  }

int XC::UserDefinedHingeIntegration2d::activateParameter(int parameterID)
  {
  // For Terje to do
  return 0;
  }

void XC::UserDefinedHingeIntegration2d::Print(std::ostream &s, int flag) const
  {
    s << "UserHinge2d" << std::endl;
    s << " E = " << ctes_scc.E();
    s << " A = " << ctes_scc.A();
    s << " I = " << ctes_scc.I() << std::endl;
    s << " Points left hinge: " << ptsL();
    s << " Weights left hinge: " << wtsL();
    s << " Points right hinge: " << ptsR;
    s << " Weights right hinge: " << wtsR;
  }

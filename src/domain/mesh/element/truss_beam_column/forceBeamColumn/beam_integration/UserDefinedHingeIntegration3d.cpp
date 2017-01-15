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
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration3d.cpp,v $

#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"
#include "UserDefinedHingeIntegration3d.h"
#include <domain/mesh/element/Information.h>

XC::UserDefinedHingeIntegration3d::UserDefinedHingeIntegration3d(int npL,
                                                             const XC::Vector &ptL,
                                                             const XC::Vector &wtL,
                                                             int npR,
                                                             const XC::Vector &ptR,
                                                             const XC::Vector &wtR,
                                                             double ee,
                                                             double aa,
                                                             double iiz,
                                                             double iiy,
                                                             double gg,
                                                             double jj)
  : UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge3d,ptL,wtL,ptR,wtR), ctes_scc(ee,aa,iiz,iiy,gg,jj)
  {}
XC::UserDefinedHingeIntegration3d::UserDefinedHingeIntegration3d(int npL,
                                                             const XC::Vector &ptL,
                                                             const XC::Vector &wtL,
                                                             int npR,
                                                             const XC::Vector &ptR,
                                                             const XC::Vector &wtR,
                                                             const CrossSectionProperties3d &cc)
  : UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge3d,ptL,wtL,ptR,wtR), ctes_scc(cc)
  {}
XC::UserDefinedHingeIntegration3d::UserDefinedHingeIntegration3d(const CrossSectionProperties3d &cts)
  : UserDefinedHingeIntegrationBase(BEAM_INTEGRATION_TAG_UserHinge3d), ctes_scc(cts) {}

void XC::UserDefinedHingeIntegration3d::getSectionLocations(int numSections,double L, double *xi) const
  {
    const int npL= ptsL().Size();
    const int npR= ptsR.Size();

    int i, j;
    for(i= 0; i < npL; i++)
      xi[i]= ptsL()(i);
    for(j= 0; j < npR; j++, i++)
      xi[i]= ptsR(j);
    for( ; i < numSections; i++)
      xi[i]= 0.0;
  }

void XC::UserDefinedHingeIntegration3d::getSectionWeights(int numSections,double L, double *wt) const
  {
    const int npL= wtsL().Size();
    const int npR= wtsR.Size();

    int i, j;
    for(i= 0; i < npL; i++)
      wt[i]= wtsL()(i);
    for(j= 0; j < npR; j++, i++)
      wt[i]= wtsR(j);
    for( ; i < numSections; i++)
      wt[i]= 1.0;
  }

int XC::UserDefinedHingeIntegration3d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    const int npL= wtsL().Size();
    const int npR= wtsR.Size();

    double betaI= 0.0;
    double betaJ= 0.0;

    int i;
   for(i= 0; i < npL; i++)
     betaI+= wtsL()(i);
   for(i= 0; i < npR; i++)
     betaJ+= wtsR(i);

   // Length of elastic interior
   const double Le= L*(1.0-betaI-betaJ);
   double Lover3EI= Le/(3*ctes_scc.EIz());
   double Lover6EI= 0.5*Lover3EI;
  
    // Elastic flexibility of element interior
    static Matrix fe(2,2);
    fe(0,0)= fe(1,1)=  Lover3EI;
    fe(0,1)= fe(1,0)= -Lover6EI;
  
    // Equilibrium transformation matrix
    static XC::Matrix B(2,2);
    B(0,0)= 1.0 - betaI;
    B(1,1)= 1.0 - betaJ;
    B(0,1)= -betaI;
    B(1,0)= -betaJ;
  
    // Transform the elastic flexibility of the element
    // interior to the basic system
    static Matrix ftmp(2,2);
    ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);

    fElastic(1,1)+= ftmp(0,0);
    fElastic(1,2)+= ftmp(0,1);
    fElastic(2,1)+= ftmp(1,0);
    fElastic(2,2)+= ftmp(1,1);

    Lover3EI= Le/(3*ctes_scc.EIy());
    Lover6EI= 0.5*Lover3EI;
    fe(0,0)= fe(1,1)=  Lover3EI;
    fe(0,1)= fe(1,0)= -Lover6EI;
    ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);
    fElastic(3,3)+= ftmp(0,0);
    fElastic(3,4)+= ftmp(0,1);
    fElastic(4,3)+= ftmp(1,0);
    fElastic(4,4)+= ftmp(1,1);

    fElastic(0,0)+= Le/(ctes_scc.EA());
    fElastic(5,5)+= Le/(ctes_scc.GJ());

    return -1;
  }

void XC::UserDefinedHingeIntegration3d::addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L, double *v0)
  { return; }

XC::BeamIntegration *XC::UserDefinedHingeIntegration3d::getCopy(void) const
  { return new UserDefinedHingeIntegration3d(*this); }

int XC::UserDefinedHingeIntegration3d::sendSelf(CommParameters &cp)
  { return -1; }

int XC::UserDefinedHingeIntegration3d::recvSelf(const CommParameters &cp)
  { return -1; }

int XC::UserDefinedHingeIntegration3d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    std::cerr << "Se pasa un puntero nulo." << std::endl;
    return ctes_scc.setParameter(argv,param,nullptr);
  }

int XC::UserDefinedHingeIntegration3d::updateParameter(int parameterID,
                                               Information &info)
  { return ctes_scc.updateParameter(parameterID,info); }

int XC::UserDefinedHingeIntegration3d::activateParameter(int parameterID)
  {
    // For Terje to do
    return 0;
  }

void XC::UserDefinedHingeIntegration3d::Print(std::ostream &s, int flag)
  {
    s << "UserHinge3d" << std::endl;
    ctes_scc.Print(s);
    s << " Points left hinge: " << ptsL();
    s << " Weights left hinge: " << wtsL();
    s << " Points right hinge: " << ptsR;
    s << " Weights right hinge: " << wtsR;
  }

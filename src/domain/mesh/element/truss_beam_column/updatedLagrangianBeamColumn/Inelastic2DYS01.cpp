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
// Inelastic2DYS01.cpp
//////////////////////////////////////////////////////////////////////

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS01.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Inelastic2DYS01::Inelastic2DYS01(int tag, double a, double e, double iz,
                                 int Nd1, int Nd2,
				                 YieldSurface_BC *ysEnd1,
				                 YieldSurface_BC *ysEnd2,
				                 int rf_algo, bool islinear, double rho
				                 )
			    :InelasticYS2DGNL (tag, Nd1, Nd2, ysEnd1, ysEnd2,
			                   rf_algo, islinear, rho),
			     A(a), Iz(iz), E(e),
			     damageFactorEnd1(0.0), damageFactorEnd2(0.0), fpeak(0.0)
{
  massDof = A*L*rho;
  massDof = massDof/2;

}

//! @brief Virtual constructor.
XC::Element* XC::Inelastic2DYS01::getCopy(void) const
  { return new XC::Inelastic2DYS01(*this); }

// very simple element
// provides the elastic stiffness based on the average degraded
// Iz at each ends
void XC::Inelastic2DYS01::getLocalStiff(Matrix &K) const
{

//	double i1Factor = (1 - damageFactorEnd1); // this->getDegradeFactor(1);
//	double i2Factor = (1 - damageFactorEnd2); // this->getDegradeFactor(2);
//
//	double iFactor = (i1Factor + i2Factor)/2;
//	iFactor = 1.0;
	double iz = Iz; //*iFactor;
	
    double	EIbyL = E*iz/L;

    K(0, 1) = K(0, 2) = K(0, 4) = K(0, 5)=0;
    K(1, 0) = K(1, 3) =0;
    K(2, 0) = K(2, 3) =0;
    K(3, 1) = K(3, 2) = K(3, 4) = K(3, 5)=0;
    K(4, 0) = K(4, 3) =0;
    K(5, 0) = K(5, 3) =0;

	K(0,0) = K(3,3) = (A/iz)*(EIbyL);
	K(0,3) = K(3,0) = (-A/iz)*(EIbyL);
	K(1,1) = K(4,4) = (12/(L*L))*(EIbyL);
	K(1,4) = K(4,1) = (-12/(L*L))*(EIbyL);
	K(1,2) = K(2,1) = K(1,5) = K(5,1) = (6/L)*(EIbyL);
	K(2,4) = K(4,2) = K(4,5) = K(5,4) = (-6/L)*(EIbyL);
	K(2,2) = K(5,5) = 4*(EIbyL);
	K(2,5) = K(5,2) = 2*(EIbyL);

}//getLocalStiff




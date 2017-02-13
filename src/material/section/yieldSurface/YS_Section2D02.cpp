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
// @ rkaul@stanford.edu
//   ggd@stanford.edu

#include "YS_Section2D02.h"
#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include <cmath>
#define SEC_TAG_YS2D02 -1


XC::YS_Section2D02::YS_Section2D02(int tag)
  :YieldSurfaceSection2d(tag, SEC_TAG_YS2D02, 0, true), E(0), A(0), I(0), maxPlstkRot(0.0), peakPlstkRot(0.0), iFactor(1.0)
  {}

XC::YS_Section2D02::YS_Section2D02(void)
:YieldSurfaceSection2d(0, SEC_TAG_YS2D02, 0, true),
 E(0), A(0), I(0), maxPlstkRot(0.0), peakPlstkRot(0.0), iFactor(1.0)
{
}

XC::YS_Section2D02::YS_Section2D02
(int tag, double E_in, double A_in, double I_in, double theta_p_max, YieldSurface_BC *ptrys, bool use_kr)
:YieldSurfaceSection2d(tag, SEC_TAG_YS2D02, ptrys, use_kr),
 E(E_in), A(A_in), I(I_in), maxPlstkRot(theta_p_max),
 peakPlstkRot(0.0), iFactor(1.0)
{
    if (E <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input E <= 0.0 ... setting E to 1.0\n";
      E = 1.0;
    }

    if (A <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input A <= 0.0 ... setting A to 1.0\n";
      A = 1.0;
    }

    if (I <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input I <= 0.0 ... setting I to 1.0\n";
      I = 1.0;
    }

    if(maxPlstkRot <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input maxPlstkRot <= 0.0 ... setting to 0.0\n";
      maxPlstkRot = 0.0;
    }

}


int XC::YS_Section2D02::commitState(void)
  {
    double pRot = fabs(ys->hModel->getTrialPlasticStrains(0));
    if(pRot > peakPlstkRot)
		peakPlstkRot = pRot;

	if(fabs(maxPlstkRot) <= 1e-10)
		iFactor = 1.0;
	else
		iFactor =  1 - (peakPlstkRot/maxPlstkRot);

	if(iFactor < 0.02)
		iFactor = 0.02;

	std::cerr << peakPlstkRot << "\t" << iFactor << std::endl;
	return this->XC::YieldSurfaceSection2d::commitState();
  }

void XC::YS_Section2D02::getSectionStiffness(XC::Matrix &Ks)
  {
    Ks(0,0) = E*A; Ks(0,1) = 0.0;
    Ks(1,0) = 0.0; Ks(1,1) = E*I*iFactor;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::YS_Section2D02::getInitialTangent(void) const
  {
    ks(0,0) = E*A; ks(0,1) = 0.0;
    ks(1,0) = 0.0; ks(1,1) = E*I;
    return ks;
  }


XC::SectionForceDeformation* XC::YS_Section2D02::getCopy(void) const
  {
    // Make a copy of the hinge
    YS_Section2D02 *theCopy =
    new XC::YS_Section2D02 (this->getTag(), E, A, I, maxPlstkRot, ys, use_Kr_orig);
    theCopy->eCommit = eCommit;
    theCopy->sCommit = sCommit;
    theCopy->peakPlstkRot =  peakPlstkRot;

    return theCopy;
  }

void XC::YS_Section2D02::Print (std::ostream &s, int flag)
{
    s << "YS_Section2D02, tag: " << this->getTag() << std::endl;
	s << "\tE: " << E << std::endl;
	s << "\tA: " << A << std::endl;
	s << "\tI: " << I << std::endl;
	s << "\tDegradation Factor      :" << iFactor << std::endl;
	s << "\tPeak plastic-rotation   :" << peakPlstkRot << std::endl;
	s << "\tMaximum plastic-rotation:" << maxPlstkRot << std::endl;
	this->XC::YieldSurfaceSection2d::Print(s, flag);
}

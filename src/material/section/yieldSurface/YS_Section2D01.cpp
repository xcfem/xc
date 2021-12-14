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
// @ rkaul@stanford.edu
// @ ggd@stanford.edu

#include "YS_Section2D01.h"
#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
#define SEC_TAG_YS2D01 -1


XC::YS_Section2D01::YS_Section2D01(int tag)
  :YieldSurfaceSection2d(tag, SEC_TAG_YS2D01, 0, true), E(0), A(0), I(0)
{}

XC::YS_Section2D01::YS_Section2D01(void)
  :YieldSurfaceSection2d(0, SEC_TAG_YS2D01, 0, true), E(0), A(0), I(0)
{
}

XC::YS_Section2D01::YS_Section2D01(int tag, double E_in, double A_in, double I_in, YieldSurface_BC *ptrys, bool use_kr)
  :YieldSurfaceSection2d(tag, SEC_TAG_YS2D01, ptrys, use_kr), E(E_in), A(A_in), I(I_in)
  {
    if(E <= 0.0)
      {
        std::cerr << "XC::YS_Section2D01::YS_Section2D01 -- Input E <= 0.0 ... setting E to 1.0\n";
        E = 1.0;
      }
    if(A <= 0.0)
      {
        std::cerr << "XC::YS_Section2D01::YS_Section2D01 -- Input A <= 0.0 ... setting A to 1.0\n";
        A = 1.0;
      }
    if(I <= 0.0)
      {
        std::cerr << "XC::YS_Section2D01::YS_Section2D01 -- Input I <= 0.0 ... setting I to 1.0",
        I = 1.0;
      }
  }

void XC::YS_Section2D01::getSectionStiffness(XC::Matrix &Ks)
  {
    Ks(0,0) = E*A; Ks(0,1) = 0.0;
    Ks(1,0) = 0.0; Ks(1,1) = E*I;
  }

const XC::Matrix &XC::YS_Section2D01::getInitialTangent(void) const
  {
    ks(0,0) = E*A; ks(0,1) = 0.0;
    ks(1,0) = 0.0; ks(1,1) = E*I;
    return ks;
  }

XC::SectionForceDeformation* XC::YS_Section2D01::getCopy(void) const
  { return new YS_Section2D01(*this); }

void XC::YS_Section2D01::Print(std::ostream &s, int flag) const
  {
    s << "YS_Section2D01, tag: " << this->getTag() << std::endl;
    s << "\tE: " << E << std::endl;
    s << "\tA: " << A << std::endl;
    s << "\tI: " << I << std::endl;
    XC::YieldSurfaceSection2d::Print(s, flag);
  }

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
// @ ggd@stanford.edu

#include "YieldSurfaceSection2d.h"

#ifndef YS_Section2D01_h
#define YS_Section2D01_h

namespace XC {
//! \ingroup MATSCCYS
//
//! @brief Sección transversal con superficie de fluencia
//! en un problema bidimensional (tres esfuerzos por sección).
class YS_Section2D01: public YieldSurfaceSection2d
  {
  private:
    double E, A, I;
  protected:
    void getSectionStiffness(Matrix &Ks);
  public:
    YS_Section2D01(int tag, double E, double A, double I,YieldSurface_BC *ptrys, bool use_kr=true);
    YS_Section2D01(int tag);
    YS_Section2D01(void);
	
    const Matrix &getInitialTangent(void) const;
    SectionForceDeformation *getCopy(void) const;
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif

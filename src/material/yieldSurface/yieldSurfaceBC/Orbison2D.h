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
// Orbison2D.h: interface for the Orbison2D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ORBISON2D_H
#define ORBISON2D_H

#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC2D.h"
#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! \ingroup MATYS
//! @brief Superficie de fluencia de tipo Orbison 2D.
class Orbison2D : public YieldSurface_BC2D
  {
  public:
    Orbison2D(int tag, double xmax, double ymax, YS_Evolution &model);
    virtual YieldSurface_BC *getCopy(void);
    virtual void Print(std::ostream &s, int flag =0);

//protected:
//  For the following 2 methods, x, y already non-dimensionalized
    virtual void getGradient(double &gx, double &gy, double x, double y);
    virtual double getSurfaceDrift(double x, double y);
    virtual void setExtent();
  };
} // end of XC namespace

#endif

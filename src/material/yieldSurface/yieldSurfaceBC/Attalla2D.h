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
// Attalla2D.h: interface for the Attalla2D class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ATTALLA2D_H
#define ATTALLA2D_H

#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC2D.h"

namespace XC {
//! @ingroup MATYS
//! @brief Atalla 2D yield surface.
class Attalla2D: public YieldSurface_BC2D
  {
  protected:
    double a1, a2, a3, a4, a5, a6;
    int driftAlgo;
  public:
    Attalla2D(int tag, double xmax, double ymax, YS_Evolution &model,
              double a01=0.19,  double a02=0.54, double a03=-1.4,
              double a04=-1.64, double a05=2.21, double a06=2.10);

    virtual YieldSurface_BC *getCopy(void);
    virtual void Print(std::ostream &s, int flag =0) const;

//protected:
//  For the following 2 methods, x, y already non-dimensionalized
    virtual void getGradient(double &gx, double &gy, double x, double y);
    virtual double getSurfaceDrift(double x, double y);
    virtual void setExtent();
	virtual void	customizeInterpolate(double &xi, double &yi, double &xj, double &yj);
  };
} // end of XC namespace

#endif

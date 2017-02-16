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
// ElTawil2DUnSym.h: interface for the ElTawil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELTAWIL2DUNSYM_H
#define ELTAWIL2DUNSYM_H

#include "YieldSurface_BC2D.h"
/*
             ytPos
         xt2 _  xt1
            /  \
           /    \
          /      )
       <------------>
          \     /
       xt3 \___/ xt4
            ytNeg



*/
namespace XC {
//! \ingroup MATYS
//! @brief Unsymmetric 2D El Tawil yield surface.
class ElTawil2DUnSym : public YieldSurface_BC2D
  {
  protected:

	double xBal, yBal;
	double xPosBal, yPosBal;
	double xNegBal, yNegBal;
	double yPosCap, yNegCap;
	double yPosCap_orig, yNegCap_orig;
	double czPos, tyPos;
	double czNeg, tyNeg;
	double ytPos, ytNeg;
	double xt1, xt2, xt3, xt4;
	double qy;
  public:
    ElTawil2DUnSym(int tag, double xPosBal, double yPosBal,
				double xNegBal, double yNegBal,
				double ypos, double yneg,
				YS_Evolution &model,
				double czPos=1.6, double tyPos=1.9,
				double czNeg=1.6, double tyNeg=1.9);


	virtual YieldSurface_BC *getCopy(void);
	virtual void	Print(std::ostream &s, int flag =0);

//protected:
//  For the following 2 methods, x, y already non-dimensionalized
    virtual void getGradient(double &gx, double &gy, double x, double y);
    virtual double getSurfaceDrift(double x, double y);
    virtual void setExtent();
    virtual void customizeInterpolate(double &xi, double &yi, double &xj, double &yj);
  };
} // end of XC namespace

#endif



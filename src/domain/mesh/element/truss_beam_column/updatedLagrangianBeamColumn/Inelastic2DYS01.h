// -*-c++-*-
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
#ifndef Inelastic2DYS01_H
#define Inelastic2DYS01_H

#include "InelasticYS2DGNL.h"

namespace XC {
//! @ingroup UpdatedLagrangianBeams
//
//! @brief Very simple Updated Lagrangian inelastic element; provides
//! the elastic stiffness based on the average degraded Iz at each ends.
class Inelastic2DYS01: public InelasticYS2DGNL
  {
  private:
    double A;
    double Iz;
    double E;
    double damageFactorEnd1;
    double damageFactorEnd2;
    double fpeak;
  public:
    Inelastic2DYS01(int tag, double A, double E, double Iz,
                    int Nd1, int Nd2,
                    YieldSurface_BC *ysEnd1,  YieldSurface_BC *ysEnd2,
                    int rf_algo=-1, bool islinear=false, double rho=0.0);
    Element *getCopy(void) const;
  protected:
    virtual void getLocalStiff(Matrix &K) const;
  };
} // end of XC namespace

#endif


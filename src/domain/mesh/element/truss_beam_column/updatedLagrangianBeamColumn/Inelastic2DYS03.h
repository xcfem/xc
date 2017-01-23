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
#ifndef Inelastic2DYS03_H
#define Inelastic2DYS03_H

#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/InelasticYS2DGNL.h"

namespace XC {
//! \ingroup OneDimensionalElem
//
//! @brief ??.
class Inelastic2DYS03 : public InelasticYS2DGNL
  {
  private:
    double Atens, Acomp, E;
    double IzPos, IzNeg;
    mutable Vector ndisp;
    Vector ndisp_hist;
  public:
    Inelastic2DYS03(int tag, double a_ten, double a_com, double e,
                double iz_pos, double iz_neg, int Nd1, int Nd2,
                   YieldSurface_BC *ysEnd1,  YieldSurface_BC *ysEnd2,
                    int rf_algo=-1, bool islinear=false, double rho=0);
    virtual int commitState();
    Element *getCopy(void) const;

  protected:
    virtual void getLocalStiff(Matrix &K) const;
  };
} // end of XC namespace

#endif


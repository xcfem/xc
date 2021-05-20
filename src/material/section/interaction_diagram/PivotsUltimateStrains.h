// -*-c++-*-
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
//PivotsUltimateStrains

#ifndef PIVOTSULTIMATESTRAINS_H
#define PIVOTSULTIMATESTRAINS_H

#include "utility/kernel/CommandEntity.h"

namespace XC {

//! @ingroup MATSCCDiagInt
//
//! @brief Definition of ultimate strains for the pivots
//! of a reinforced concrete sections.
class PivotsUltimateStrains: public CommandEntity
  {
    double eps_agot_A; //!< A pivot ultimate strain.
    double eps_agot_B; //!< B pivot ultimate strain.
    double eps_agot_C; //!< C pivot ultimate strain.
  public:
    PivotsUltimateStrains(const double &eA= 0.01,const double &eB= -0.0035,const double &eC= -0.002);

    const double &getUltimateStrainAPivot(void) const;
    const double &getUltimateStrainBPivot(void) const;
    const double &getUltimateStrainCPivot(void) const;
    void setUltimateStrainAPivot(const double &);
    void setUltimateStrainBPivot(const double &);
    void setUltimateStrainCPivot(const double &);
    const double getIncEpsAB(const double &ndiv= 25) const;

  };

} // end of XC namespace

#endif

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
//NormalStressStrengthParameters

#ifndef NORMALSTRESSSTRENGTHPARAMETERS_H
#define NORMALSTRESSSTRENGTHPARAMETERS_H

namespace XC {

class PivotsUltimateStrains;

//! @ingroup MATSCCDiagInt
//
//! @brief Parameter that define the ultimate limit states of a RC
//! section under normal stresses.
class NormalStressStrengthParameters
  {
    const PivotsUltimateStrains &agot_pivots; //!< Ultimate strain at pivots.
    double eps_c_max; //!< concrete max strain (most tensioned fiber).
    double eps_c_min; //!< concrete min strain (most compressed fiber).
    double eps_c_pC; //!< concrte strain at C pivot.

  private:
    double fc_tracc(void) const;
    double fc_comp(void) const;
    double fc_pC(void) const;
    double fc_min(void) const;
  public:
    NormalStressStrengthParameters(const PivotsUltimateStrains &ap,const double &emx= 0.0,const double &emn= 0.0,const double &ec= 0.0);
    
    bool OK(void) const;
    bool KO(void) const;
  };

} // end of XC namespace

#endif

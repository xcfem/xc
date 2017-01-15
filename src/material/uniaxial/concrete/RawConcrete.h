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
//RawConcrete.h
                                              
#ifndef RawConcrete_h
#define RawConcrete_h


#include "material/uniaxial/UniaxialMaterial.h"
#include "material/uniaxial/UniaxialStateVars.h"
#include "material/uniaxial/UniaxialHistoryVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Clase base para los materiales de tipo hormigón.
class RawConcrete: public UniaxialMaterial
  {
  protected:
    double fpc; //!< Resistencia a compresión
    double epsc0; //!< Deformación cuando se alcanza la resistencia a compresión
    double epscu; //!< Strain at crushing strength

    virtual void setup_parameters(void)= 0;

  public:
    RawConcrete(int tag, int classTag, double fpc, double eco, double ecu);
    RawConcrete(int tag, int classTag);

    void setFpc(const double &);
    double getFpc(void) const;
    void setEpsc0(const double &);
    double getEpsc0(void) const;
    void setEpscu(const double &);
    double getEpscu(void) const;
  };

} // end of XC namespace


#endif



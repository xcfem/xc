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
//FedeasConcrMaterial.h

#ifndef FedeasConcrMaterial_h
#define FedeasConcrMaterial_h

#include <material/uniaxial/FedeasMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasConcrMaterial wraps the FEDEAS
//! 1d material subroutine Concr_1.
class FedeasConcrMaterial: public FedeasMaterial
  {
  public:
    FedeasConcrMaterial(int tag, int classTag, int numHV, int numData, double fc, double ec, double fu, double eu);
    FedeasConcrMaterial(int tag, int classTag, int numHV, int numData, const Vector &data);
    FedeasConcrMaterial(int tag, int classTag, int numHV, int numData);

    double getInitialTangent(void) const;
  };
} // end of XC namespace


#endif


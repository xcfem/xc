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
//SectionFDPhysicalProperties.h

#ifndef Joint3DPhysicalProperties_h
#define Joint3DPhysicalProperties_h

#include "UniaxialMatPhysicalProperties.h"
#include "material/damage/DamageModelVector.h"

namespace XC {

//! @ingroup PhysicalProperties
//
//! @brief Physical properties for shells.
class Joint3DPhysicalProperties : public UniaxialMatPhysicalProperties
  {
  public:
    Joint3DPhysicalProperties(const size_t &nMat= 0,const UniaxialMaterial *ptr_mat= nullptr); 
    Joint3DPhysicalProperties(const UniaxialMaterial &springx, const UniaxialMaterial &springy, const UniaxialMaterial &springz);

    int update(const Vector &,const int &);
    Vector getTangent(void) const;
    Vector getStress(void) const;
    Vector getStrain(void) const;
    Vector getStrainStress(void) const;
    Vector getResponse7(void) const;
  }; 

} // end of XC namespace
#endif

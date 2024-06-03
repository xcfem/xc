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

#include <material/uniaxial/soil_structure_interaction/EyBasic.h>
#include <utility/matrix/Vector.h>
#include <cmath>
#include <cfloat>
#include "utility/utils/misc_utils/colormod.h"


//! @brief Sets the less negative yield stress value (tension).
void XC::EyBasic::set_fyp(const double &f)
  {
    fyp= f;
    if(fyp > 0) // Must be negative (no tension in soil)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; fyp > 0, setting < 0"
	          << Color::def << std::endl;
        fyp*= -1.;
      }
  }

//! @brief Set the yield stress a compression value.
void XC::EyBasic::set_fyn(const double &f)
  {
    fyn= f;
    if(fyn > 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; fyn > 0, setting < 0"
	          << Color::def << std::endl;
        fyn*= -1.;
      }  
  }

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp positive yield strain value (tension).
XC::EyBasic::EyBasic(int tag, double e, double eyp)
  : ElasticPPMaterialBase(tag, MAT_TAG_EyBasic, e,eyp) {}

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp positive yield strain value (tension).
//! @param[in] eyn negative yield strain value (compression).
XC::EyBasic::EyBasic(int tag, double e, double eyp,double eyn, double ez )
  : ElasticPPMaterialBase(tag, MAT_TAG_EyBasic, e,eyp, eyn, ez) {}

//! @brief Constructor.
XC::EyBasic::EyBasic(int tag)
  :ElasticPPMaterialBase(tag, MAT_TAG_EyBasic){}

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::EyBasic::getCopy(void) const
  { return new EyBasic(*this); }


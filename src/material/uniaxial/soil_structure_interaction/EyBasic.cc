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
    this->split_stress= (fyp+fyn)/2.0;
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
    this->split_stress= (fyp+fyn)/2.0;
  }

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp upper yield strain value (decompression).
//! @param[in] eyn lower yield strain value (compression).
//! @param[in] ez initial strain.
XC::EyBasic::EyBasic(int tag, double e, double eyp,double eyn, double ez )
  : ElasticPPMaterialBase(tag, MAT_TAG_EyBasic, e,eyp, eyn, ez),
    split_stress(0.0) {}

//! @brief Constructor.
XC::EyBasic::EyBasic(int tag)
  :ElasticPPMaterialBase(tag, MAT_TAG_EyBasic),
   split_stress(0.0) {}

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::EyBasic::getCopy(void) const
  { return new EyBasic(*this); }

//! @brief Set the values of the parameters that define the material response.
void XC::EyBasic::setParameters(const double &E, const double &lowerYS, const double &upperYS)
  {
    this->setTangent(E); // deformation modulus.
    this->setLowerYieldStress(lowerYS); // lower yield stress (compression).
    this->setUpperYieldStress(upperYS); // upper yield stress (decompression).
    this->revertToStart(); // recompute internal parameters.
  }

//! @brief Sets trial strain.
int XC::EyBasic::setTrialStrain(double strain, double strainRate)
  {
  /*
    if (fabs(trialStrain - strain) < DBL_EPSILON)
      return 0;
  */
    trialStrain= strain;

    // compute trial stress
    const double sigtrial= E * get_total_strain(); // trial stress
    const double f= yield_function(sigtrial); //yield function
    const double fYieldSurface= - E * DBL_EPSILON;
    if(f<=fYieldSurface) // elastic.
      {
        // elastic
        trialStress= sigtrial;
        trialTangent= E;
      }
    else // plastic
      {
        if(sigtrial>split_stress)
          { trialStress = fyp; }
        else
          { trialStress = fyn; }
        trialTangent = 0.0;
      }
    return 0;
  }

//! @brief Commit material state.
int XC::EyBasic::commitState(void)
  {
    // compute trial stress
    const double sigtrial= E * get_total_strain(); // trial stress

    const double f= yield_function(sigtrial); //yield function

    const double fYieldSurface= - E * DBL_EPSILON;
    if(f>fYieldSurface)
      {
        // plastic
        if(sigtrial>0.0)
          { ep+= f / E; }
        else
          { ep-= f / E; }
      }
    //added by SAJalali for energy recorder
    EnergyP+= 0.5*(commitStress + trialStress)*(trialStrain - commitStrain);
    
    EPPBaseMaterial::commitState();
    commitStress= trialStress;
    return 0;
  }        

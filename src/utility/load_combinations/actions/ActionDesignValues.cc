//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ActionDesignValues.cxx

#include "utility/load_combinations/actions/ActionDesignValues.h"
#include "utility/load_combinations/actions/ActionDesignValuesList.h"
#include "utility/load_combinations/actions/factors/PartialSafetyFactors.h"
#include "utility/load_combinations/actions/factors/CombinationFactorsMap.h"
#include "utility/load_combinations/actions/factors/PartialSafetyFactorsMap.h"
#include "utility/load_combinations/comb_analysis/Variations.h"
#include "LeadingActionInfo.h"



//! @brief Default constructor.
cmb_acc::ActionDesignValues::ActionDesignValues(const std::string &n, const std::string &descrip,ActionDesignValuesList *fam)
  : ActionRepresentativeValues(n,descrip, fam),
    partial_safety_factors(nullptr)
   {}

//! @brief Default constructor.
cmb_acc::ActionDesignValues::ActionDesignValues(const Action &a,ActionDesignValuesList *fam,const std::string &nmb_combination_factors, const std::string &nmb_partial_safety_factors)
  : ActionRepresentativeValues(a, fam, nmb_combination_factors),
    partial_safety_factors(nullptr)
  {
    setPartialSafetyFactors(nmb_partial_safety_factors);
  }

//! @brief Return the partual safety factors.
const cmb_acc::PartialSafetyFactors *cmb_acc::ActionDesignValues::getPartialSafetyFactors(void) const
  {
    if(!partial_safety_factors)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; ERROR: action " << getName()
	        << ": partial safety factors not set."
	        << std::endl;
    return partial_safety_factors;
  }

//! @brief Set the partial safety factors.
void cmb_acc::ActionDesignValues::setPartialSafetyFactors(const std::string &nmb_factors)
  {
    if(!nmb_factors.empty())
      {
        const PartialSafetyFactors *tmp=nullptr;
        if(acc_familia)
          tmp= acc_familia->getPtrPartialSafetyFactors()->getPtrCoefs(nmb_factors);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; ERROR: pointer to actions family not set."
		    << std::endl;
        if(tmp)
           partial_safety_factors= tmp;
	else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; ERROR: combination factors with name: '"
		    << nmb_factors
	            << "' not found." << std::endl;
      }
  }

//! @brief Return the index of the action in its family.
int cmb_acc::ActionDesignValues::getIndex(void) const
  {
    int retval= -1;
    if(acc_familia)
      retval= acc_familia->getIndex(this);
    return retval;
  }

//! @brief Compute the variations that can be formed with this action.
//!
//! @param uls: True if it's an ultimate limit state.
//! @param sit_accidental: true if it's an accidental or seismic situation.
cmb_acc::Variations cmb_acc::ActionDesignValues::getVariations(const bool &uls,const bool &sit_accidental) const
  {
    Variation v= getPartialSafetyFactors()->getVariation(uls,sit_accidental);
    return Variations::first_combination(v);
  }

//! @brief Return the combination value for the action.
//! @param leadingActioInfo: Information about the leading action.
//! @param psf: partial safety factor.
cmb_acc::Action cmb_acc::ActionDesignValues::getCombinationValue(const LeadingActionInfo &lai, const double &psf) const
  { return psf*getRepresentativeValue(lai); }

//! @brief Print stuff.
void cmb_acc::ActionDesignValues::Print(std::ostream &os) const
  {
    ActionRepresentativeValues::Print(os);
    if(combination_factors)
      os << "; " << *combination_factors;
  }

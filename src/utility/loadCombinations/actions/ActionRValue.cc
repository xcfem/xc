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
//ActionRValue.cxx

#include "utility/loadCombinations/actions/ActionRValue.h"
#include "utility/loadCombinations/actions/ActionRValueList.h"
#include "utility/loadCombinations/factors/PartialSafetyFactors.h"
#include "utility/loadCombinations/factors/CombinationFactorsMap.h"
#include "utility/loadCombinations/factors/PartialSafetyFactorsMap.h"
#include "utility/loadCombinations/comb_analysis/Variations.h"
#include "LeadingActionInfo.h"



//! @brief Default constructor.
cmb_acc::ActionRValue::ActionRValue(const std::string &n, const std::string &descrip,ActionRValueList *fam)
  : Action(n,descrip), acc_familia(fam),
    partial_safety_factors(nullptr),
    combination_factors(nullptr)
   {}

//! @brief Default constructor.
cmb_acc::ActionRValue::ActionRValue(const Action &a,ActionRValueList *fam,const std::string &nmb_combination_factors, const std::string &nmb_partial_safety_factors)
  : Action(a), acc_familia(fam),
    partial_safety_factors(nullptr),
    combination_factors(nullptr)
  {
    setPartialSafetyFactors(nmb_partial_safety_factors);
    setCombinationFactors(nmb_combination_factors);
  }

//! @brief Asigna los coeficientes de simultaneidad de la acción.
void cmb_acc::ActionRValue::setCombinationFactors(const std::string &nmb_factors)
  {
    if(!nmb_factors.empty())
      {
        const CombinationFactors *tmp=nullptr;
        if(acc_familia)
          tmp= acc_familia->getPtrCombinationFactors()->getPtrCoefs(nmb_factors);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; ERROR: pointer to actions family not set." << std::endl;
        if(tmp)
           combination_factors= tmp;
	else
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; ERROR: combination factors with name: '"
		    << nmb_factors
	            << "' not found." << std::endl;	  
      }
  }


//! @brief Return the r-th combination factor.
double cmb_acc::ActionRValue::getCombinationFactor(short int r) const
  {
    double retval= 1.0;
    if(combination_factors)
      retval= combination_factors->getCombinationFactor(r);
    else
      if(verbosity>1)
	std::clog << getClassName() << "::" << __FUNCTION__
		  << "; combination factors not set. Returning 1.0."
		  << std::endl;
    return retval;
  }

//! @brief Return the r-th combination factor.
const cmb_acc::PartialSafetyFactors *cmb_acc::ActionRValue::getPartialSafetyFactors(void) const
  {
    if(!partial_safety_factors)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; ERROR: action " << getName()
	        << ": partial safety factors not set."
	        << std::endl;
    return partial_safety_factors;
  }

//! @brief Set the partial safety factors.
void cmb_acc::ActionRValue::setPartialSafetyFactors(const std::string &nmb_factors)
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

//! @brief Return the representative value of the action.
cmb_acc::Action cmb_acc::ActionRValue::getValue(short int r) const
  {
    Action retval(*this);
    switch(r)
      {
      case(-1):
        break; //characteristic value.
      case(0):
        retval*=getCombinationFactor(0); //combination value.
        break;
      case(1):
        retval*=getCombinationFactor(1); //frequent value.
        break;
      case(2):
        retval*=getCombinationFactor(2); //quasi-permanent value.
        break;
      default:
        break;
      }
    return retval;
  }

//! @brief Return the index of the action in its family.
int cmb_acc::ActionRValue::getIndex(void) const
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
cmb_acc::Variations cmb_acc::ActionRValue::getVariations(const bool &uls,const bool &sit_accidental) const
  {
    Variation v= getPartialSafetyFactors()->getVariation(uls,sit_accidental);
    return Variations::first_combination(v);
  }

//! @brief Return the representative value for the action.
//! @param leadingActioInfo: Information about the leading action.
cmb_acc::Action cmb_acc::ActionRValue::getRepresentativeValue(const LeadingActionInfo &lai) const
  {
    Action retval= getValue(lai.getGeneralRepresentativeValueIndex());
    if(lai.leadingActionExists())
      {
	const int j= getIndex();
        if(lai.isLeadingActionIndex(j)) //j is the leading action index.
	  retval= getValue(lai.getLeadingRepresentativeValueIndex()); //Representative value for the leading action.
      }
    return retval;
  }

//! @brief Return the combination value for the action.
//! @param leadingActioInfo: Information about the leading action.
//! @param psf: partial safety factor.
cmb_acc::Action cmb_acc::ActionRValue::getCombinationValue(const LeadingActionInfo &lai, const double &psf) const
  { return psf*getRepresentativeValue(lai); }

//! @brief Print stuff.
void cmb_acc::ActionRValue::Print(std::ostream &os) const
  {
    Action::Print(os);
    if(partial_safety_factors)
      os << "; " << *partial_safety_factors;
    if(combination_factors)
      os << "; " << *combination_factors;
  }

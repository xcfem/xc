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
//ActionRepresentativeValues.cxx

#include "utility/load_combinations/actions/ActionRepresentativeValues.h"
#include "utility/load_combinations/actions/ActionDesignValuesList.h"
#include "utility/load_combinations/factors/PartialSafetyFactors.h"
#include "utility/load_combinations/factors/CombinationFactorsMap.h"
#include "utility/load_combinations/factors/PartialSafetyFactorsMap.h"
#include "utility/load_combinations/comb_analysis/Variations.h"
#include "LeadingActionInfo.h"



//! @brief Default constructor.
cmb_acc::ActionRepresentativeValues::ActionRepresentativeValues(const std::string &n, const std::string &descrip,ActionDesignValuesList *fam)
  : Action(n,descrip), acc_familia(fam),
    combination_factors(nullptr)
   {}

//! @brief Default constructor.
cmb_acc::ActionRepresentativeValues::ActionRepresentativeValues(const Action &a,ActionDesignValuesList *fam,const std::string &nmb_combination_factors)
  : Action(a), acc_familia(fam),
    combination_factors(nullptr)
  {
    setCombinationFactors(nmb_combination_factors);
  }

//! @brief Asigna los coeficientes de simultaneidad de la acción.
void cmb_acc::ActionRepresentativeValues::setCombinationFactors(const std::string &nmb_factors)
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
double cmb_acc::ActionRepresentativeValues::getCombinationFactor(short int r) const
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

//! @brief Return the representative value of the action.
cmb_acc::Action cmb_acc::ActionRepresentativeValues::getValue(short int r) const
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

//! @brief Return the representative value for the action.
//! @param leadingActioInfo: Information about the leading action.
cmb_acc::Action cmb_acc::ActionRepresentativeValues::getRepresentativeValue(const LeadingActionInfo &lai) const
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

//! @brief Print stuff.
void cmb_acc::ActionRepresentativeValues::Print(std::ostream &os) const
  {
    Action::Print(os);
    if(combination_factors)
      os << "; " << *combination_factors;
  }

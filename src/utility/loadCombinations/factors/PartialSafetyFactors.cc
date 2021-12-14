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
//PartialSafetyFactors.cc
//Partial safety factors de acciones.

#include "PartialSafetyFactors.h"
#include "utility/loadCombinations/comb_analysis/Variation.h"
#include "utility/loadCombinations/comb_analysis/Variations.h"

#include "utility/loadCombinations/actions/ActionRValueList.h"

cmb_acc::SLSPartialSafetyFactors::SLSPartialSafetyFactors(const float &fav,const float &desfav)
  : partial_safety_factors_fav(fav), partial_safety_factors_unfav(desfav) {}


//! @brief Return the partial safety factors for serviceability limit state.
cmb_acc::Variation cmb_acc::SLSPartialSafetyFactors::Coefs(void) const
  {
    if(partial_safety_factors_fav == partial_safety_factors_unfav) return Variation(1,partial_safety_factors_fav); //If they are equal return only one.
    Variation retval(2,0.0); //Partial safety factors.
    retval[0]= partial_safety_factors_fav; //Favorable.
    retval[1]= partial_safety_factors_unfav; //Desfavorable.
    return retval;
  }

void cmb_acc::SLSPartialSafetyFactors::Print(std::ostream &os) const
  { os << "favourable: " << partial_safety_factors_fav << ", unfavourable: " << partial_safety_factors_unfav; }

cmb_acc::ULSPartialSafetyFactors::ULSPartialSafetyFactors(const float &fav,const float &desfav,const float &fav_acc,const float &desfav_acc)
  : SLSPartialSafetyFactors(fav,desfav), partial_safety_factors_fav_acc(fav_acc), partial_safety_factors_unfav_acc(desfav_acc) {}

//! @brief Return the partial safety factors correspondientes 
//! a situaciones persistentes o transitorias.
cmb_acc::Variation cmb_acc::ULSPartialSafetyFactors::CoefsPT(void) const
  { return SLSPartialSafetyFactors::Coefs(); }

//! @brief Return the partial safety factors correspondientes 
//! a situaciones accidentales o sísmicas.
cmb_acc::Variation cmb_acc::ULSPartialSafetyFactors::CoefsAcc(void) const
  {
    if(partial_safety_factors_fav_acc == partial_safety_factors_unfav_acc) return Variation(1,partial_safety_factors_fav_acc); //If they are equal return only one.
    Variation retval(2,0.0); //Partial safety factors.
    retval[0]= partial_safety_factors_fav_acc; //Favorable.
    retval[1]= partial_safety_factors_unfav_acc; //Desfavorable.
    return retval;
  }

//! @brief Print stuff.
void cmb_acc::ULSPartialSafetyFactors::Print(std::ostream &os) const
  {
    SLSPartialSafetyFactors::Print(os);
    os << ", favourable (accidental): " << partial_safety_factors_fav_acc
       << ", unfavourable (accidental): " << partial_safety_factors_unfav_acc;
  }

//! @brief Constructor
cmb_acc::PartialSafetyFactors::PartialSafetyFactors(const ULSPartialSafetyFactors &gf_uls, const SLSPartialSafetyFactors &gf_els)
  : CommandEntity(), uls_partial_safety_factors(gf_uls), sls_partial_safety_factors(gf_els) {}

//! @brief Return the partial safety factors for serviceability limit states.
cmb_acc::Variation cmb_acc::PartialSafetyFactors::CoefsEls(void) const
  { return sls_partial_safety_factors.Coefs(); }

//! @brief Return the variation that correspond to the arguments.
//!
//! @param uls: Verdadero si se trata de un ultimate limit state.
//! @param sit_accidental: Verdadero si se trata de una situación accidental o sísmica.
cmb_acc::Variation cmb_acc::PartialSafetyFactors::getVariation(const bool &uls,const bool &sit_accidental) const
  {
    Variation retval;
    if(uls)
      {
	if(sit_accidental)
	  retval= uls_partial_safety_factors.CoefsAcc();
	else
	  retval= uls_partial_safety_factors.CoefsPT();
      }
    else
      retval= sls_partial_safety_factors.Coefs();
    return retval;
  }

//! @brief Print stuff.
void cmb_acc::PartialSafetyFactors::Print(std::ostream &os) const
  {
    os << " SLS: " << sls_partial_safety_factors
       << " ULS: " << uls_partial_safety_factors;
  }

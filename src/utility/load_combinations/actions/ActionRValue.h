// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//ActionRValue.hxx
//Representative values of an action.

#ifndef ACTIONRVALUE_H
#define ACTIONRVALUE_H

#include "Action.h"

namespace cmb_acc{

class PartialSafetyFactors;
class CombinationFactors;
class Variations;
class LeadingActionInfo;

//! @ingroup CMBACC
//
//! @brief Representative values of an action.
class ActionRValue: public Action
  {
  private:
    ActionRValueList *acc_familia; //!< Container for the actions of the family.
    const PartialSafetyFactors *partial_safety_factors; //!< Partial safety factors for this action.
    const CombinationFactors *combination_factors; //!< Combination factors.

    inline double getCombinationFactor(short int r) const;

  protected:
    friend class ActionRValueList;
    //! @brief Default constructor.
    ActionRValue(const std::string &n="", const std::string &descrip="",ActionRValueList *fam= nullptr);
    ActionRValue(const Action &a,ActionRValueList *fam= nullptr,const std::string &nmb_comb_factors= "",const std::string &nmb_partial_safety_factors= "");
  public:
    Action getValue(short int r) const;
    void setCombinationFactors(const std::string &);
    const PartialSafetyFactors *getPartialSafetyFactors(void) const;
    void setPartialSafetyFactors(const std::string &);
    int getIndex(void) const; 
    Variations getVariations(const bool &,const bool &) const;
    Action getRepresentativeValue(const LeadingActionInfo &) const;    
    Action getCombinationValue(const LeadingActionInfo &, const double &) const;    
    void Print(std::ostream &os) const;    
  };

} //fin namespace nmb_acc.

#endif

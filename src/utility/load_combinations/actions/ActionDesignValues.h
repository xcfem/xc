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
//ActionDesignValues.hxx
//Representative values of an action.

#ifndef ACTIONRVALUE_H
#define ACTIONRVALUE_H

#include "ActionRepresentativeValues.h"

namespace cmb_acc{

//! @ingroup CMBACC
//
//! @brief Design values of an action.
class ActionDesignValues: public ActionRepresentativeValues
  {
  private:
    const PartialSafetyFactors *partial_safety_factors; //!< Partial safety factors for this action.

  protected:
    friend class ActionDesignValuesList;
    //! @brief Default constructor.
    ActionDesignValues(const std::string &n="", const std::string &descrip="",ActionDesignValuesList *fam= nullptr);
    ActionDesignValues(const Action &a,ActionDesignValuesList *fam= nullptr,const std::string &nmb_comb_factors= "",const std::string &nmb_partial_safety_factors= "");
  public:
    const PartialSafetyFactors *getPartialSafetyFactors(void) const;
    void setPartialSafetyFactors(const std::string &);
    int getIndex(void) const; 
    Variations getVariations(const bool &,const bool &) const;
    Action getCombinationValue(const LeadingActionInfo &, const double &) const;    
    void Print(std::ostream &os) const;    
  };

} //fin namespace nmb_acc.

#endif

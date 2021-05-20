// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//LeadingActionInfo.hxx
//Information about the leading action.

#ifndef LEADINGACTIONINFO_H
#define LEADINGACTIONINFO_H

#include "utility/kernel/CommandEntity.h"

namespace cmb_acc{
class Action;

//! @ingroup CMBACC
//
//! @brief Information about the leading action.
class LeadingActionInfo: public CommandEntity
  {
  private:
    short int index; //!< Index of leading variable action (-1 if none).

// representative_value_index= -1 -> characteristic value.
// representative_value_index= 0 -> combination value.
// representative_value_index= 1 -> frequent value.
// representative_value_index= 2 -> quasi-permanent value.

    short int leading_representative_value_index; //!< Index of the representative value for the leading action.
    short int general_representative_value_index; //!< Index of the representative value for the non leading actions.
  public:
    //! @brief Default constructor.
    LeadingActionInfo(const short int &i= -1, const short int &lrv= -1, const short int &grv= 0);

    //! @brief Return true if there is a leading action.
    inline bool leadingActionExists(void) const
      { return index>-1; }
    //! @brief Return true if the argument is the leading action index.
    inline bool isLeadingActionIndex(const short int &j) const
      { return (index==j); }
    inline short int getLeadingActionIndex(void) const
      { return index; }
    inline short int getLeadingRepresentativeValueIndex(void) const
      { return leading_representative_value_index; }
    inline short int getGeneralRepresentativeValueIndex(void) const
      { return general_representative_value_index; }
    void Print(std::ostream &os) const;
  };

 inline std::ostream &operator<<(std::ostream &os, const LeadingActionInfo &lai)
  {
    lai.Print(os);
    return os;
  }

} //fin namespace nmb_acc.

#endif

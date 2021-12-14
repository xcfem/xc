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
//NamedEntity.h
//Named entity.

#ifndef ENTCONNMB_H
#define ENTCONNMB_H

#include <string>
#include "CommandEntity.h"

//! @ingroup CMDINTRP
//
//! @defgroup BASE Auxiliary classes.
//
//! @ingroup BASE
//
//!  @brief Object identified by a name.
class NamedEntity: public CommandEntity
  {
  private:
    std::string name; //!< @brief Object name.
  public:
    //! @brief Constructor.
  NamedEntity(const std::string &Name="",CommandEntity *owr= nullptr)
      : CommandEntity(owr), name(Name) {}
    //! @brief Return the object name.
    const std::string &getName(void) const
      { return name; }
    //! @brief Set the  object name.
    void setName(const std::string &s)
      { name= s; }
    //! @brief Return a reference to the object name.
    std::string &Name(void)
      { return name; }
    //! @brief Comparison operator.
    inline virtual bool operator==(const NamedEntity &other) const
      { return (name==other.name); }
    //! @brief Less than operator.
    inline bool operator<(const NamedEntity &other) const
      { return (name<other.name); }
    NamedEntity &operator+=(const NamedEntity &);
    NamedEntity &operator-=(const NamedEntity &);
    NamedEntity &operator*=(const NamedEntity &);
  };

NamedEntity operator+(const NamedEntity &,const NamedEntity &);
NamedEntity operator-(const NamedEntity &,const NamedEntity &);
NamedEntity operator*(const NamedEntity &,const NamedEntity &);

#endif

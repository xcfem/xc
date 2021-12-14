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
//EntityWithProperties.h

#ifndef ENTPROP_H
#define ENTPROP_H

#include <string>

//! @ingroup NUCLEO
//
//! @brief Object that can return properties as Python objects.
class EntityWithProperties
  {
  private:
    EntityWithProperties *owner; //! Object that owns THIS ONE.
  protected:
    static int verbosity; //!< Verbosity level.
   public:
    EntityWithProperties(EntityWithProperties *owr= nullptr);
    EntityWithProperties(const EntityWithProperties &);
    EntityWithProperties &operator=(const EntityWithProperties &);
    virtual bool operator==(const EntityWithProperties &) const;
    //! @brief Virtual destructor
    virtual ~EntityWithProperties(void) {}
    static int getVerbosityLevel(void);
    static void setVerbosityLevel(const int &);
    virtual std::string getClassName(void) const;
    void set_owner(EntityWithProperties *owr);
    inline EntityWithProperties *Owner(void)
      { return owner; }
    inline const EntityWithProperties *Owner(void) const
      { return owner; }
  };

#endif

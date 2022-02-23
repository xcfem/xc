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
//EntityWithOwner.h
//Entidad con punetero a su propietaria.

#ifndef ENTITYWITHOWNER_H
#define ENTITYWITHOWNER_H

#include <string>

//! @ingroup BASE
//
//!  @brief Entity with a pointer to its owner.
class EntityWithOwner
  {
  private:
    EntityWithOwner *owner; //! Object that owns THIS ONE.
  protected:
    static int verbosity; //!< Verbosity level.
   public:
    EntityWithOwner(EntityWithOwner *owr= nullptr);
    EntityWithOwner(const EntityWithOwner &);
    EntityWithOwner &operator=(const EntityWithOwner &);
    virtual bool operator==(const EntityWithOwner &) const;
    //! @brief Virtual destructor
    virtual ~EntityWithOwner(void) {}
    
    static int getVerbosityLevel(void);
    static void setVerbosityLevel(const int &);
    virtual std::string getClassName(void) const;
    
    void set_owner(EntityWithOwner *owr);
    inline EntityWithOwner *Owner(void)
      { return owner; }
    inline const EntityWithOwner *Owner(void) const
      { return owner; }    
  };

#endif

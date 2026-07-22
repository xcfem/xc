// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//FrictionModelHandler.h

#ifndef FrictionModelHandler_h
#define FrictionModelHandler_h

#include "PrepHandler.h"
#include <map>

namespace XC {
class FrictionModel;

//! @ingroup Ldrs
//! 
//! @brief Friction models handler.
class FrictionModelHandler: public PrepHandler
  {
  public:
    typedef std::map<std::string,FrictionModel *> map_friction_models;
    typedef map_friction_models::const_iterator const_iterator;
    typedef map_friction_models::iterator iterator;
  private:
    int friction_model_tag; //!< Default friction model tag.
    map_friction_models friction_models;
    void free_mem(void);
    FrictionModelHandler(const FrictionModelHandler &);
    FrictionModelHandler &operator=(const FrictionModelHandler &);
  protected:
    friend class ElementHandler;
  public:
    FrictionModelHandler(Preprocessor *owr);
    const map_friction_models &Map(void) const;
    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);
    
    std::string getName(const int &tag) const;
    const_iterator find(const std::string &str) const;
    iterator find(const std::string &str);
    FrictionModel *find_ptr(const std::string &str);
    const FrictionModel *find_ptr(const std::string &nmb) const;

    inline size_t size(void) const
      { return friction_models.size(); }

    bool exists(const std::string &nmb) const;
    FrictionModel *newFM(const std::string &,const std::string &);
    FrictionModel &get(const std::string &);

    ~FrictionModelHandler(void);
    inline void clearAll(void)
      { free_mem(); }
  };

} // end of XC namespace

#endif

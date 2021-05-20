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
//ActionWeightingMap.h
//Container for actions and corresponding factors.

#ifndef ACTIONWEIGHTINGMAP_H
#define ACTIONWEIGHTINGMAP_H

#include "utility/kernel/CommandEntity.h"
#include "utility/loadCombinations/factors/Factors.h"
#include <map>


namespace cmb_acc {

class Action;
class ActionRValue;
class ActionsAndFactors;
class LoadCombinations;

//! @ingroup CMBACC
//
//! @brief Container for ActionAndFactors objects.
class ActionWeightingMap: public CommandEntity
  {
  public:
    typedef std::map<std::string,ActionsAndFactors *> map_actions_and_factors;
    typedef map_actions_and_factors::iterator iterator;
    typedef map_actions_and_factors::const_iterator const_iterator;

  private:
    map_actions_and_factors actions_and_factors; //!< actions with its factors.

    bool exists(const std::string &nmb) const;
    ActionsAndFactors *findOrCreate(const std::string &nmb,const Factors &coefs= Factors());
    void clear(void);
    void copy(const map_actions_and_factors &pond);
  protected:
    friend class ActionsAndFactors;
  public:
    ActionWeightingMap(void);
    ActionWeightingMap(const ActionWeightingMap &);
    ActionWeightingMap &operator=(const ActionWeightingMap &);
    virtual ~ActionWeightingMap(void);
    cmb_acc::ActionsAndFactors *create(const std::string &,const Factors &coefs= Factors());

    ActionRValue &insert(const std::string &pond,const std::string &,const Action &,const std::string &combination_factors_name="",const std::string &partial_safety_factors_name="");

    size_t size(void) const;
    bool empty(void) const;
    iterator begin(void);
    const_iterator begin(void) const;
    iterator end(void);
    const_iterator end(void) const;
    boost::python::list getKeys(void) const;    
    ActionsAndFactors *findByName(const std::string &nmb);
    const ActionsAndFactors *findByName(const std::string &nmb) const;

    LoadCombinations getLoadCombinations(void);
  };

} // fin namespace cmb_acc

#endif

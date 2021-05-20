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
//ConjVariables.h

#ifndef CONJVARIABLES_H
#define CONJVARIABLES_H

#include "TablaVariables.h"
#include <set>
#include <vector>

class ConjVariables : public std::set<const Variable *>
  {
  public:
    typedef std::set<const Variable *> pset_CV;

    bool In(const Variable *var) const;
    bool In(const Variable &var) const;
    std::vector<std::string> getNames(void) const;
  };

std::ostream &operator << (std::ostream &stream,const ConjVariables &cv);


#endif

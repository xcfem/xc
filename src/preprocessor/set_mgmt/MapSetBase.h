// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//MapSetBase.h
//Sets container.

#ifndef MAPSETBASE_H
#define MAPSETBASE_H

#include "preprocessor/PreprocessorContainer.h"
#include "utility/actor/actor/MovableObject.h"
#include <map>
#include <deque>


namespace XC {

class SetBase;
class Set;
class SetEstruct;
class EntMdlr;
class Pnt;
class Edge;
class Face;
class Body;
class UniformGrid;

//!  @ingroup Set
//! 
//!  @brief Sets container.
//!  
class MapSetBase: public std::map<std::string,SetBase *> 
  {
  public:
    typedef std::map<std::string,SetBase *> map_sets;
  protected:
    static std::deque<std::string> setsClassNames; //! sets class names.
    friend class EntMdlr;

    SetEstruct *create_set_estruct(const SetEstruct &);

    SetBase *find(const std::string &nmb) const;
    SetEstruct *find_set_estruct(const std::string &nmb);
    void clearSets(void);
    friend class Set;
      
    MapSetBase(const MapSetBase &);
    MapSetBase &operator=(const MapSetBase &);
    const std::deque<std::string> &getSetsClassNames(void) const;
    const std::deque<std::string> &getSetsNames(void) const;
    const std::deque<std::string> &getKeys(void) const;
    DbTagData &getDbTagData(void) const;
    const ID &getSetsDBTags(Communicator &comm);

    friend class Preprocessor;
  public:
    MapSetBase(void)
      :map_sets() {}
    virtual ~MapSetBase(void);

    bool exists(const std::string &nmb) const;
    void remove(const std::string &);
    void rename(const std::string &, const std::string &);
    
    boost::python::list getSetsNamesPy(void) const;
    boost::python::list getKeysPy(void) const;

    std::set<SetBase *> get_sets(const Node *);
    std::set<SetBase *> get_sets(const Element *);
    std::set<SetBase *> get_sets(const Pnt *);
    std::set<SetBase *> get_sets(const Edge *);
    std::set<SetBase *> get_sets(const Face *);
    std::set<SetBase *> get_sets(const Body *);
    std::set<SetBase *> get_sets(const UniformGrid *);

  };
} // end of XC namespace

#endif

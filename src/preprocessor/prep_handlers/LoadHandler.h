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
//LoadHandler.h

#ifndef LOADLOADER_H
#define LOADLOADER_H

#include "PrepHandler.h"
#include "domain/load/pattern/MapLoadPatterns.h"
#include "domain/load/pattern/LoadCombinationGroup.h"

namespace XC {
class GroundMotion;
class LoadCombination;

//!  @ingroup Ldrs
//! 
//! @brief Load definition manager.
class LoadHandler: public PrepHandler
  {
    typedef std::map<std::string,GroundMotion *> map_ground_motions;
    map_ground_motions ground_motions; //!< Ground movement.

    MapLoadPatterns lpatterns; //!< Load patterns container.
    int tag_lp; //!< Default identifier for load patterns or load combinations.

    LoadCombinationGroup combinations; //!< Load combination container.

  protected:
    friend class LoadCombination;
    friend class Domain;
 
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    LoadHandler(Preprocessor *owr);
    ~LoadHandler(void);

    void addToDomain(const std::string &);
    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    inline int &getTagLP(void)
      { return tag_lp; }
    inline const int &getTagLP(void) const
      { return tag_lp; }
    inline void setTagLP(const int &tlp)
      { tag_lp= tlp; } 
    MapLoadPatterns &getLoadPatterns(void)
      { return lpatterns; }
    const MapLoadPatterns &getLoadPatterns(void) const
      { return lpatterns; }
    LoadCombinationGroup &getLoadCombinations(void)
      { return combinations; }
    const LoadCombinationGroup &getLoadCombinations(void) const
      { return combinations; }

    void clearAll(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

  };

} // end of XC namespace

#endif

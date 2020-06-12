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
//MapSet.h
//Sets container.

#ifndef MAPSET_H
#define MAPSET_H

#include "preprocessor/PreprocessorContainer.h"
#include "utility/actor/actor/MovableObject.h"
#include "MapSetBase.h"
#include <map>
#include <deque>


namespace XC {

class Domain;

//!  @ingroup Set
//! 
//!  @brief Sets container.
//!  
class MapSet: public PreprocessorContainer, public MovableObject, public MapSetBase
  {
    static ID setsDbTags;//! dbTags para the sets.
  public:
    typedef std::map<std::string,EntMdlr *> map_ent_mdlr;
  private:
    map_ent_mdlr entities; //! Geometric entities (points, lines, surfaces,...).
    Set *total; //!< Pointer to total set (Created in constructor).
    map_sets open_sets; //!< Opened sets (those for which each new entity will be added to).

    friend class EntMdlr;

    bool is_open(const std::string &) const;
    Set *create_set(const std::string &);
    Set *alloc_set(const Set &);
    SetBase *broke_set(const std::string &,const std::string &);

    SetBase *find_set(const std::string &nmb);
    SetEstruct *find_struct_set(const std::string &nmb);
    void setup_total(void);
    void clearSets(void);
    void clearAll(void);
    void reset(void);
    friend class MultiBlockTopology;
    friend class SetMeshComp;
    friend class SetEntities;
    friend class Set;
    void abre_set(const std::string &nmb);
    void cierra_set(const std::string &nmb);
      
    MapSet(const MapSet &);
    MapSet &operator=(const MapSet &);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendSetsDbTags(int posDbTag,Communicator &comm);
    int sendSetsClassNames(int posDbTag,Communicator &comm);
    int receiveSetsDbTags(int posDbTag,int size,const Communicator &comm);
    int receiveSetsClassNames(int pDbTg,int sz,const Communicator &comm);
    int sendSets(int posDbTag1, int posDbTag2, int posDbTag3,Communicator &comm);
    int receiveSets(int posDbTag1, int posDbTag2, int posDbTag3,const int &,const Communicator &comm);
    int sendOpenSets(int posDbTag1, int posDbTag2,Communicator &comm);
    int receiveOpenSets(int posDbTag1, int posDbTag2,const Communicator &comm);

    int sendData(Communicator &);
    int recvData(const Communicator &);

    friend class Preprocessor;
    MapSet(Preprocessor *preprocessor= nullptr);
  public:

    virtual ~MapSet(void);

    EntMdlr *insert_ent_mdlr(EntMdlr *ent_mdlr);

    Set *get_set_total(void)
      { return total; }
    const Set *get_set_total(void) const
      { return total; }
    inline const map_sets &get_open_sets(void) const
      { return open_sets; }
    inline map_sets &get_open_sets(void)
      { return open_sets; }
    Set *defSet(const std::string &);
    void removeSet(const std::string &);

    const SetBase *find_set(const std::string &nmb) const;
    SetBase &getSet(const std::string &nmb);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif

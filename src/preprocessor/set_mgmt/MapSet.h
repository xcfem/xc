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

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include <map>
#include <deque>


namespace XC {

class Domain;
class SetBase;
class Set;
class SetEstruct;
class EntMdlr;
class Preprocessor;
class Pnt;
class Edge;
class Face;
class Body;
class UniformGrid;

//!  @ingroup Set
//! 
//!  @brief Sets container.
//!  
class MapSet: public EntCmd, public MovableObject
  {
    static ID setsDbTags;//! dbTags para the sets.
    static std::deque<std::string> setsClassNames; //! sets class names.
  public:
    typedef std::map<std::string,SetBase *> map_sets;
    typedef map_sets::iterator iterator;
    typedef map_sets::const_iterator const_iterator;
    typedef std::map<std::string,EntMdlr *> map_ent_mdlr;
  private:
    Preprocessor *preprocessor; //!< Pointer to preprocessor.

    map_sets sets; //!< Sets of entidades.
    map_ent_mdlr entidades; //! Geometric entities (points, lines, surfaces,...).
    Set *total; //!< Pointer to total set (Created in constructor).
    map_sets abiertos; //!< Opened sets (aquellos a los que se añade cada nueva entidad creada en Cad).

    bool existe(const std::string &nmb) const;
    friend class EntMdlr;

    Set *crea_set(const std::string &);
    SetEstruct *crea_set_estruct(const SetEstruct &);
    SetBase *broke_set(const std::string &,const std::string &);

    SetBase *busca_set(const std::string &nmb);
    SetEstruct *busca_set_estruct(const std::string &nmb);
    void setup_total(void);
    void clearSets(void);
    void clearAll(void);
    void reset(void);
    friend class Cad;
    friend class SetMeshComp;
    friend class Set;
    void abre_set(const std::string &nmb);
    void cierra_set(const std::string &nmb);
    iterator begin(void);
    iterator end(void);
      
    MapSet(const MapSet &otro);
    MapSet &operator=(const MapSet &otro);
  protected:
    const ID &getSetsDBTags(CommParameters &cp);
    const std::deque<std::string> &getSetsClassNames(void);
    DbTagData &getDbTagData(void) const;
    int sendSetsDbTags(int posDbTag,CommParameters &cp);
    int sendSetsClassNames(int posDbTag,CommParameters &cp);
    int receiveSetsDbTags(int posDbTag,int size,const CommParameters &cp);
    int receiveSetsClassNames(int pDbTg,int sz,const CommParameters &cp);
    int sendSets(int posDbTag1, int posDbTag2, int posDbTag3,CommParameters &cp);
    int receiveSets(int posDbTag1, int posDbTag2, int posDbTag3,const int &,const CommParameters &cp);
    int sendAbiertos(int posDbTag1, int posDbTag2,CommParameters &cp);
    int receiveAbiertos(int posDbTag1, int posDbTag2,const CommParameters &cp);

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class Preprocessor;
    MapSet(Preprocessor *preprocessor= NULL);
  public:

    virtual ~MapSet(void);

    EntMdlr *inserta_ent_mdlr(EntMdlr *ent_mdlr);

    Set *get_set_total(void)
      { return total; }
    const Set *get_set_total(void) const
      { return total; }
    inline const map_sets &get_sets_abiertos(void) const
      { return abiertos; }
    inline map_sets &get_sets_abiertos(void)
      { return abiertos; }
    Set *defSet(const std::string &);
    void removeSet(const std::string &);

    const SetBase *busca_set(const std::string &nmb) const;
    SetBase &getSet(const std::string &nmb);
    const_iterator begin(void) const;
    const_iterator end(void) const;
    std::set<SetBase *> get_sets(const Node *);
    std::set<SetBase *> get_sets(const Element *);
    std::set<SetBase *> get_sets(const Pnt *);
    std::set<SetBase *> get_sets(const Edge *);
    std::set<SetBase *> get_sets(const Face *);
    std::set<SetBase *> get_sets(const Body *);
    std::set<SetBase *> get_sets(const UniformGrid *);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

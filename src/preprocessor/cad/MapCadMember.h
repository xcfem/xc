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
//MapCadMember.h

#ifndef MAPCADMEMBER_H
#define MAPCADMEMBER_H

#include "preprocessor/cad/MapCadMemberBase.h"
#include <map>
#include "boost/python/list.hpp"

namespace XC {

//! @ingroup Cad
//
//! @brief Container for model entities.
template <class T>
 class MapCadMember: public MapCadMemberBase, public std::map<MapCadMemberBase::Indice,T *>
  {
  public:
    typedef typename std::map<Indice,T *> map_base;
    typedef typename std::pair<Indice,T *> pair;
    typedef typename map_base::iterator iterator;
    typedef typename map_base::const_iterator const_iterator;

    MapCadMember(Cad *cad= NULL);

    T * busca(const Indice &);
    const T * busca(const Indice &) const;
    bool existe(const Indice &) const;
    T * get(const Indice &);
    boost::python::list getKeys(void) const;

    void clearAll(void);
    virtual ~MapCadMember(void);


  };

//! @brief Constructor.
template <class T>
MapCadMember<T>::MapCadMember(Cad *cad)
  : MapCadMemberBase(cad) {}

//! @brief Returns a pointer to the la entidad cuyo identificador being passed as parameter.
template <class T>
T * MapCadMember<T>::busca(const Indice &id)
  {    T * retval= NULL;
    iterator i= this->find(id);
    if(i!= this->end()) //La entidad existe.
      retval= (*i).second;
    return retval;
  }

//! @brief Returns a pointer to the la entidad cuyo nombre being passed as parameter.
template <class T>
const T * MapCadMember<T>::busca(const Indice &id) const
  {
    const T * retval= NULL;
    const_iterator i= this->find(id);
    if(i!= this->end()) //La entidad existe.
      retval= (*i).second;
    return retval;
  }

//! @brief Returns true ifla entidad ya está definida.
template <class T>
bool MapCadMember<T>::existe(const Indice &id) const
  { return (busca(id)!=NULL); }


//! @brief Returns the entity with tag iEnt.
template <class T>
T * MapCadMember<T>::get(const size_t &iEnt)
  {
    T *retval= busca(iEnt);
    if(!retval)
      std::cerr << "MapCadMember::get; no se encontró la entidad: '" 
                << iEnt << "'.\n";
    return retval;   
  }

//! @brief Borra todas las entidades.
template <class T>
void MapCadMember<T>::clearAll(void)
  {
    for(iterator i=this->begin();i!=this->end();i++)
      {
        T * tmp= (*i).second;
        if(tmp)
          delete tmp;
        tmp= NULL;
      }
    this->clear();
  }

//! @brief Returns container's keys.
template <class T>
boost::python::list MapCadMember<T>::getKeys(void) const
  {
    boost::python::list retval;
    for(const_iterator i=this->begin();i!=this->end();i++)
      retval.append((*i).first);
    return retval;
  }

//! @brief Destructor.
template <class T>
MapCadMember<T>::~MapCadMember(void)
  { clearAll(); }

} //end of XC namespace
#endif

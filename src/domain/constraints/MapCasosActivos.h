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
//MapCasosActivos.h
                                                                        
                                                                        
#ifndef MapCasosActivos_h
#define MapCasosActivos_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include <map>


namespace XC {
template<class T>
class MapCasosActivos: public CommandEntity, public std::map<int,T *>
  {
  public:
    typedef std::map<int, T *> t_map;
    typedef typename t_map::iterator iterator;
    typedef typename t_map::const_iterator const_iterator;
    typedef typename t_map::reference reference;
    typedef typename t_map::const_reference const_reference;
    typedef typename t_map::value_type value_type;
    MapCasosActivos(CommandEntity *);
    void Print(std::ostream &s, int flag =0);
  };

//! @brief Constructor
template<class T>
MapCasosActivos<T>::MapCasosActivos(CommandEntity *owr)

  : CommandEntity(owr), t_map(*this) {}

template<class T>
void MapCasosActivos<T>::Print(std::ostream &s, int flag)
  {
    // go through the array invoking Print on non-zero entries
    const_iterator p = this->begin();
    while(p!=this->end())
      {
	((*p).second)->Print(s, flag);
	p++;
      }
  }

} // end of XC namespace

#endif




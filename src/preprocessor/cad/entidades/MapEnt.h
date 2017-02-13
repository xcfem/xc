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
//MapEnt.h

#ifndef MAPENT_H
#define MAPENT_H

#include "preprocessor/cad/MapCadMember.h"
#include "EntMdlr.h"
#include <map>

namespace XC {

//! @ingroup Cad
//
//! @brief Contenedor de entidades de CAD (puntos, líneas, superficies,...).
template <class T>
class MapEnt: public MapCadMember<T>
  {
  protected:

  public:
    typedef typename MapCadMember<T>::iterator iterator;
    typedef typename MapCadMember<T>::const_iterator const_iterator;

    MapEnt(Cad *cad= NULL);

    T *getNearest(const Pos3d &p);
    const T *getNearest(const Pos3d &p) const;
    void numera(void);
  };

//! @brief Constructor.
template <class T>
MapEnt<T>::MapEnt(Cad *cad)
  : MapCadMember<T>(cad) {}

//! @brief Returns the objeto más cercano a la posición being passed as parameter.
template <class T>
T *MapEnt<T>::getNearest(const Pos3d &p)
  {
    //El código comentado da problemas (recursión infinita) si
    //se llama a la función desde boost.python
    /* MapEnt<T> *this_no_const= const_cast<MapEnt<T> *>(this); */
    /* return const_cast<T *>(this_no_const->getNearest(p)); */
    T *retval= NULL;
    if(!this->empty())
      {
	iterator i= this->begin();
        double d2= (*i).second->DistanciaA2(p);
        retval= (*i).second; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i).second->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= (*i).second;
              }
          }
      }
    return retval;
  }

//! @brief Returns the objeto más cercano a la posición being passed as parameter.
template <class T>
const T *MapEnt<T>::getNearest(const Pos3d &p) const
  {
    const T *retval= NULL;
    if(!this->empty())
      {
	const_iterator i= this->begin();
        double d2= (*i).second->DistanciaA2(p);
        retval= (*i).second; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i).second->DistanciaA2(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= (*i).second;
              }
          }
      }
    return retval;
  }

//!  @brief Asigna índices a los objetos de la lista para poder emplearlos en VTK.
template <class T>
void MapEnt<T>::numera(void)
  {
    size_t idx= 0;
    for(iterator i= this->begin();i!=this->end();i++,idx++)
      {
        EntMdlr *ptr= (*i).second;
        ptr->set_indice(idx);
      }
  }

} //end of XC namespace
#endif

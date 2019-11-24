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
//EntityMap.h

#ifndef MAPENT_H
#define MAPENT_H

#include "preprocessor/multi_block_topology/ModelComponentContainer.h"
#include "preprocessor/multi_block_topology/entities/EntMdlr.h"
#include <map>

namespace XC {

//! @ingroup MultiBlockTopology
//
//! @brief Geometric entities container (points, lines, surfaces,...).
template <class Entity>
class EntityMap: public ModelComponentContainer<Entity>
  {
  public:
    typedef typename ModelComponentContainer<Entity>::iterator iterator;
    typedef typename ModelComponentContainer<Entity>::const_iterator const_iterator;

    EntityMap(MultiBlockTopology *mbt= nullptr);

    Entity *getNearest(const Pos3d &p);
    const Entity *getNearest(const Pos3d &p) const;
    void numera(void);
  };

//! @brief Constructor.
template <class Entity>
EntityMap<Entity>::EntityMap(MultiBlockTopology *mbt)
  : ModelComponentContainer<Entity>(mbt) {}

//! @brief Returns the object closest to the position being passed as parameter.
template <class Entity>
Entity *EntityMap<Entity>::getNearest(const Pos3d &p)
  {
    //Commented code is buggy (infinite recursion) if
    //the function is called from Python
    /* EntityMap<Entity> *this_no_const= const_cast<EntityMap<Entity> *>(this); */
    /* return const_cast<Entity *>(this_no_const->getNearest(p)); */
    Entity *retval= nullptr;
    if(!this->empty())
      {
	iterator i= this->begin();
        double d2= (*i).second->getSquaredDistanceTo(p);
        retval= (*i).second; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i).second->getSquaredDistanceTo(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= (*i).second;
              }
          }
      }
    return retval;
  }

//! @brief Returns the object closest to the position being passed as parameter.
template <class Entity>
const Entity *EntityMap<Entity>::getNearest(const Pos3d &p) const
  {
    const Entity *retval= nullptr;
    if(!this->empty())
      {
	const_iterator i= this->begin();
        double d2= (*i).second->getSquaredDistanceTo(p);
        retval= (*i).second; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i).second->getSquaredDistanceTo(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= (*i).second;
              }
          }
      }
    return retval;
  }

//!  @brief Set indices to the objects to allow its use in VTK.
template <class Entity>
void EntityMap<Entity>::numera(void)
  {
    size_t idx= 0;
    for(iterator i= this->begin();i!=this->end();i++,idx++)
      {
        EntMdlr *ptr= (*i).second;
        ptr->set_index(idx);
      }
  }

} //end of XC namespace
#endif

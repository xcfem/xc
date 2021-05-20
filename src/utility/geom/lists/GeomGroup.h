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
//GeomGroup.h

#ifndef GEOMGROUP_H
#define GEOMGROUP_H

#include "utility/utils/stl/pdeque.h"


//! @ingroup GEOM
//
//! @brief Base class for geometric entities groups.
template <typename GO>
class GeomGroup : public GO
  {
    static const GO *downcast(const GeomObj *p)
      { return dynamic_cast<const GO *>(p); }
    static GO *downcast(GeomObj *p)
      { return dynamic_cast<GO *>(p); }
  protected:
    typedef pdeque<GO> pdeque_geom_obj;
    pdeque_geom_obj objetos; //!< Objects of the group.
    bool equal_dimension(void) const;
  private:
    void copia_objetos(const pdeque_geom_obj &objs);
  public:
    typedef typename pdeque_geom_obj::const_iterator const_iterator;
    typedef typename pdeque_geom_obj::iterator iterator;

    inline bool empty(void) const
      { return objetos.empty(); }

    inline const_iterator begin(void) const
      { return objetos.begin(); }
    inline const_iterator end(void) const
      { return objetos.end(); }
    inline bool Vacio(void) const
      { return objetos.empty(); }

    GeomGroup(void) : GO(), objetos() {}
    GeomGroup(const GeomGroup &other) : GO(other)
      { copia_objetos(other.objetos); }
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    virtual unsigned short int Dimension(void) const;
    size_t size(void) const
      { return objetos.size(); }
    void push_back(const GO &obj)
      { objetos.push_back(downcast(obj.clon())); }
    virtual GEOM_FT getLength(void) const;
    virtual GEOM_FT getArea(void) const;
    virtual GEOM_FT getVolume(void) const;
    void Print(std::ostream &os) const;
  };

template <typename GO>
void GeomGroup<GO>::copia_objetos(const pdeque_geom_obj &objs)
  {
    for(const_iterator i= objs.begin();i!=objs.end();i++)
      objetos.push_back(downcast((*i)->clon()));
  }

//! @brief Return true if all objects have the same dimension.
template <typename GO>
bool GeomGroup<GO>::equal_dimension(void) const
  {
    if(objetos.empty()) return true;
    const_iterator i(objetos.begin());
    const unsigned short int d((*i)->Dimension());
    i++;
    for(;i!=objetos.end();i++)
      if(d!=(*i)->Dimension()) return false;
    return true;
  }

//! @brief Return the dimension of the object 0, 1, 2 or 3.
template <typename GO>
unsigned short int GeomGroup<GO>::Dimension(void) const
  {
    if(objetos.empty()) return 0;
    const_iterator i(objetos.begin());
    unsigned short int d((*i)->Dimension());
    i++;
    for(;i!=objetos.end();i++)
      d= std::max(d,(*i)->Dimension());
    return d;
  }

template <typename GO>
GEOM_FT GeomGroup<GO>::GetMax(unsigned short int i) const
  {
    if(objetos.empty()) return 0.0;
    const_iterator j=objetos.begin();
    GEOM_FT mx= (*j)->GetMax(i);
    j++;
    for(;j != objetos.end();j++)
      mx= std::max((*j)->GetMax(i),mx);
    return mx;
  }
template <typename GO>
GEOM_FT GeomGroup<GO>::GetMin(unsigned short int i) const
  {
    if(objetos.empty()) return 0.0;
    const_iterator j=objetos.begin();
    GEOM_FT mn= (*j)->GetMin(i);
    j++;
    for(;j != objetos.end();j++)
      mn= std::min((*j)->GetMin(i),mn);
    return mn;
  }

//! @brief Return the length of the object.
template <typename GO>
GEOM_FT GeomGroup<GO>::getLength(void) const
  {
    if(objetos.empty()) return 0.0;
	const_iterator i(objetos.begin());
    GEOM_FT retval((*i)->getLength());
    i++;
    for(;i!=objetos.end();i++)
      retval+= (*i)->getLength();
    return retval;
  }

//! @brief Return the object area.
template <typename GO>
GEOM_FT GeomGroup<GO>::getArea(void) const
  {
    if(objetos.empty()) return 0.0;
	const_iterator i(objetos.begin());
    GEOM_FT retval((*i)->getArea());
    i++;
    for(;i!=objetos.end();i++)
      retval+= (*i)->getArea();
    return retval;
  }

//! @brief Return the volume of the object.
template <typename GO>
GEOM_FT GeomGroup<GO>::getVolume(void) const
  {
    if(objetos.empty()) return 0.0;
	const_iterator i(objetos.begin());
    GEOM_FT retval((*i)->getVolume());
    i++;
    for(;i!=objetos.end();i++)
      retval+= (*i)->getVolume();
    return retval;
  }

template <typename GO>
void GeomGroup<GO>::Print(std::ostream &os) const
  {
    if(Vacio()) return;
    const_iterator i= begin();
    os << **i; i++;
    for(;i!=end();i++)
      os << std::endl << **i;
  }

#endif

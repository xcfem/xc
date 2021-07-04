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
//GeomObj.h

#ifndef GEOMOBJ_H
#define GEOMOBJ_H

#include <iostream>
#include "ProtoGeom.h"
#include "lists/PolyPos.h"

class Pos2d;
class Pos3d;
class Vector2d;
class Trf2d;
class Plotter;


//! @ingroup GEOM
//
//! @brief Clase base para las entidades geométricas.
class GeomObj: public ProtoGeom
  {
  public:
    typedef PolyPos<Pos3d> list_Pos3d;
    typedef PolyPos<Pos2d> list_Pos2d;

    GeomObj(void);
    virtual ~GeomObj(void) {}
    virtual GeomObj *getCopy(void) const=0;
    //! @brief Return the dimension of the object 0, 1, 2 or 3.
    virtual unsigned short int Dimension(void) const= 0;
    //! @brief Return the length of the object.
    virtual GEOM_FT getLength(void) const= 0;
    //! @brief Return the area of the object.
    virtual GEOM_FT getArea(void) const= 0;
    //! @brief Return the volume of the object.
    virtual GEOM_FT getVolume(void) const= 0;
    //! @brief Return true if the object has a center of mass.
    inline virtual bool hasCenterOfMass(void) const
      { return false;}
    GEOM_FT getCenterOfMassArea(void) const;

    virtual void Print(std::ostream &os) const= 0;
    virtual void Plot(Plotter &) const {}
  };

std::ostream &operator<<(std::ostream &, const GeomObj &);


//! @brief Return the sum of the areas.
template <class input_iterator>
GEOM_FT area(input_iterator begin,input_iterator end)
  {
    GEOM_FT retval= 0.0;
    for(input_iterator i=begin;i!=end;i++)
      retval+= i->getArea();
    return retval;
  }

//! @brief Return the sum of the volumes.
template <class input_iterator>
GEOM_FT volume(input_iterator begin,input_iterator end)
  {
    GEOM_FT retval= 0.0;
    for(input_iterator i=begin;i!=end;i++)
      retval+= i->getVolume();
    return retval;
  }

//! @brief Return the sum of the lengths.
template <class input_iterator>
GEOM_FT length(input_iterator begin,input_iterator end)
  {
    GEOM_FT retval= 0.0;
    for(input_iterator i=begin;i!=end;i++)
      retval+= i->getLength();
    return retval;
  }

//! @brief Imprime los objetos.
template <class input_iterator>
void print(std::ostream &os,input_iterator begin,input_iterator end)
  {
    if(begin!=end)
      {
        input_iterator i= begin;
        os << *i; i++;
        for(;i!=end;i++)
          os << ", " << *i;
      }
  }
#endif

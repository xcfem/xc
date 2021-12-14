// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//center_of_mass.h

#ifndef CENTER_OF_MASS_H
#define CENTER_OF_MASS_H

//! @ingroup GEOM
//! 
//! @brief Return the center of mass of the objects.
template<class InputIter>
const Pos2d center_of_mass(InputIter begin, InputIter end)
  {
    Pos2d retval;
    if(begin!=end)
      {
        InputIter i=begin;
        GEOM_FT area= (*i).getArea();
        Vector2d vpos= (*i).getCenterOfMass().VectorPos()*(*i).getArea();
        i++;
        for(;i!=end;i++)
          {
            area+= (*i).getArea();
            vpos= vpos + (*i).getCenterOfMass().VectorPos()*(*i).getArea();
          }
        vpos= vpos/area;
        retval= Pos2d(vpos[0],vpos[1]);
      }
    return retval;
  }


#endif

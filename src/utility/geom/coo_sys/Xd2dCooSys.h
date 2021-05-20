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
//CooSys.h
//Coordinate system (abstract).

#ifndef XD2D_COO_SYS_H
#define XD2D_COO_SYS_H

#include "CooSys.h"

class Vector2d;
class Pos2d;
class Dir2d;


//! @ingroup CooSys
//
//! @brief Coordinate system of dimension X defined in a
//! two-dimensional space.
class Xd2dCooSys: public CooSys
  {
  public:
    typedef Vector2d VGlobal; //!< Vector in the 2D space.
    typedef Dir2d DGlobal; //!< Direction in the 2D space.
    typedef Pos2d PGlobal; //!< Point the 2D space.
  protected:
    Xd2dCooSys(const size_t &);
    Xd2dCooSys(const size_t &,const VGlobal &);
    Xd2dCooSys(const size_t &,const PGlobal &,const PGlobal &);

    void putRow(const size_t &axis,const VGlobal &v);
    void XAxisVector(const VGlobal &vX);

  public:
    virtual VGlobal getAxisVDir(const size_t &axis) const;
    //Return el direction vector del axis que se pasa como parámetro.
    virtual DGlobal getAxisDir(const size_t &axis) const;
    //Return the dirección of the axis que se pasa como parámetro.

    VGlobal GetCooGlobales(const FT_matrix &v) const;
    FT_matrix GetCooLocales(const VGlobal &v) const;
  };

#endif

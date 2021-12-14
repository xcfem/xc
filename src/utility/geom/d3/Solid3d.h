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
//Solid3d.h

#ifndef SOLID3D_H
#define SOLID3D_H

#include "GeomObj3d.h"

//! @ingroup GEOM
//
//! @brief Clase base para los sólidos en tres dimensiones.
class Solid3d : public GeomObj3d
  {
  public:
    Solid3d(void);
    virtual GEOM_FT dist(const Pos3d &p) const;
    virtual inline GEOM_FT IArea(void) const
      { return getVolume(); }
    virtual unsigned short int Dimension(void) const
      { return 3; }
    virtual void Print(std::ostream &os) const;
  };
#endif

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
//ProtoGeom.h

#ifndef PROTOGEOM_H
#define PROTOGEOM_H

#include "utility/kernel/CommandEntity.h"

//! @defgroup GEOM Entidades geométricas.
//
//! @ingroup GEOM
//
//! @brief Clase base para redefinir el método printprop
class ProtoGeom: public CommandEntity
  {
  private:
    bool exts; //! False when solution is not possible: intersection of parallel planes.
  public:
    ProtoGeom(void);
    virtual void Print(std::ostream &os) const;
    virtual ~ProtoGeom(void) {}
    inline void setExists(bool e)
      { exts= e; }
    bool exists(void) const
      { return exts; }
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
  };

#endif

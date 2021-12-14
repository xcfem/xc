// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//MeshEdges.h
                                                                        
#ifndef MeshEdges_h
#define MeshEdges_h

#include "utility/kernel/CommandEntity.h"
#include "domain/mesh/MeshEdge.h"

class Polyline3d;


namespace XC {
class Element;
class Domain;
class ID;
 

//! @ingroup Elem
//
//! @brief Element edge container.
class MeshEdges: public CommandEntity, public std::deque<MeshEdge>
  {
  public:
    MeshEdges(void);

    std::deque<const MeshEdge *> getLoop(const MeshEdge *) const;
    MeshEdges getEdgesNotInLoop(const std::deque<const MeshEdge *> &) const;
    std::deque<Polyline3d> getContours(const double &factor) const;
    void print(std::ostream &) const;
  };

std::deque<Polyline3d> getContours(MeshEdges edges,const double &factor);

//! @brief Printing stuff.
inline std::ostream &operator<<(std::ostream &os, const MeshEdges &me)
  {
    me.print(os);
    return os;
  }
} // end of XC namespace

#endif


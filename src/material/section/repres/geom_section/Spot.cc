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
//Spot.cc

#include "Spot.h"
#include "Axis.h"
#include "SectionReferenceFrame.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/d2/BND2d.h"



XC::Spot::Spot(const Pos2d &pto)
  : EntGeomSection(), p(pto) {}

//! Inserts the line being passed as parameter in the list
//! of the lines that begin or end in this point.
void XC::Spot::insert_linea(Axis *l)
  { pt_axes.insert(l); }

//! Erases the line being passed as parameter from the list
//! of the lines that begin or end in this point.
void XC::Spot::borra_linea(Axis *l)
  {
    std::set<const Axis *>::iterator i= find(pt_axes.begin(),pt_axes.end(),l);
    if(i!= pt_axes.end()) //La ha encontrado.
      pt_axes.erase(i);
  }
//! Returns position vector of the point.
Vector2d XC::Spot::VectorPos(void) const
  { return p.VectorPos();  }

//! @brief Update topology.
void XC::Spot::update_topology(void)
  {}

//! @brief Returns the object BND.
BND2d XC::Spot::Bnd(void) const
  { return BND2d(p,p);  }

//! @brief Returns true if the point is an end of the line.
bool XC::Spot::Toca(const Axis &l) const
  {
    std::set<const Axis *>::const_iterator i= pt_axes.find(&l);
//     for(std::set<const Axis *>::const_iterator i= pt_axes.begin(); i!=pt_axes.end();i++)
//       { if(&l == (*i)) return true; }
    return (i != pt_axes.end());
  }


//! @brief Return the distance to the position being passed as parameter.
double XC::Spot::DistanciaA(const Pos2d &pt) const
  { return dist(p,pt);  }



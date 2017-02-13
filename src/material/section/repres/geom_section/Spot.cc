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
#include "Eje.h"
#include "SisRefScc.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/d2/BND2d.h"



XC::Spot::Spot(const Pos2d &pto)
  : EntGeomSection(), p(pto) {}

//! Inserta la línea being passed as parameter en la lista
//! de líneas que tocan al punto.
void XC::Spot::inserta_linea(Eje *l)
  { ejes_pt.insert(l); }
//! Borra la línea being passed as parameter de la lista
//! de líneas que tocan al punto.
void XC::Spot::borra_linea(Eje *l)
  {
    std::set<const Eje *>::iterator i= find(ejes_pt.begin(),ejes_pt.end(),l);
    if(i!= ejes_pt.end()) //La ha encontrado.
      ejes_pt.erase(i);
  }
//! Returns the vector de posición del punto.
Vector2d XC::Spot::VectorPos(void) const
  { return p.VectorPos();  }

//! @brief Actualiza la topología.
void XC::Spot::actualiza_topologia(void)
  {}

//! @brief Returns the BND del objeto.
BND2d XC::Spot::Bnd(void) const
  { return BND2d(p,p);  }

//! @brief Returns true ifel punto toca a la línea.
bool XC::Spot::Toca(const Eje &l) const
  {
    std::set<const Eje *>::const_iterator i= ejes_pt.find(&l);
//     for(std::set<const Eje *>::const_iterator i= ejes_pt.begin(); i!=ejes_pt.end();i++)
//       { if(&l == (*i)) return true; }
    return (i != ejes_pt.end());
  }


//! @brief Return the distancia a la posición being passed as parameter.
double XC::Spot::DistanciaA(const Pos2d &pt) const
  { return dist(p,pt);  }



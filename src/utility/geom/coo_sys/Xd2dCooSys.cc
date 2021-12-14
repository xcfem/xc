//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//Xd2dCooSys.cc

#include "Xd2dCooSys.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Dir2d.h"
#include "../pos_vec/Pos2d.h"


//! @brief Define a coordinate system of dimension ne
//! in a two-dimensional space.
//! The axis are those of the global system.
Xd2dCooSys::Xd2dCooSys(const size_t &ne)
  : CooSys(ne,2) { identity(); }
//! @brief Define a coordinate system of dimension ne
//! in a two-dimensional space. See Xd2dCooSys::XAxisVector.
Xd2dCooSys::Xd2dCooSys(const size_t &ne,const VGlobal &vX)
  : CooSys(ne,2) //Axis 1 parallel to Vx.
  { XAxisVector(vX); }
//! @brief Define a coordinate system of dimension ne
//! in a two-dimensional space. See Xd2dCooSys::XAxisVector.
Xd2dCooSys::Xd2dCooSys(const size_t &ne,const PGlobal &o,const PGlobal &p)
  : CooSys(ne,2)
  { XAxisVector(p-o); }

void Xd2dCooSys::putRow(const size_t &axis,const VGlobal &v)
  { CooSys::putRow(axis,traspuesta(v.getMatrix())); }
Xd2dCooSys::DGlobal Xd2dCooSys::getAxisDir(const size_t &axis) const
  { return DGlobal(getAxisVDir(1)); }
//! @brief Return the direction vector of the axis being passed as parameter.
Xd2dCooSys::VGlobal Xd2dCooSys::getAxisVDir(const size_t &axis) const
  { 
    const FT_matrix row= getRow(axis);
    return VGlobal(row(1,1),row(1,2));
  }

//! @brief Return the global coordinates of the vector.
//!
//! @param v: Local coordinates of the vector.
Xd2dCooSys::VGlobal Xd2dCooSys::getGlobalCoordinates(const FT_matrix &v) const
  {
    const FT_matrix tmp= CooSys::getGlobalCoordinates(v);
    return VGlobal(tmp(1),tmp(2)); 
  }

//! @brief Return the local coordinates of the vector.
//!
//! @param v: Global coordinates of the vector.
FT_matrix Xd2dCooSys::getLocalCoordinates(const Xd2dCooSys::VGlobal &v) const
  { return CooSys::getLocalCoordinates(v.getMatrix()); }

//! @brief Hace que el vector básico 1 tenga la dirección
//! y sentido del being passed as parameter. If the system has
//! dimension 2 the basic vector is normal to the first one
//! in couterclockwise sense.
void Xd2dCooSys::XAxisVector(const VGlobal &vX)
  {
    if(vX.Nulo())
      {
	std::cerr << "Xd2dCooSys::XAxisVector: the vector: " 
             << vX << " is null. No changes were made."
             << std::endl;
        return;
      }
    const VGlobal i_= vX.getNormalized();
    putRow(1,i_);
    if(numberOfAxis()>1)
      putRow(2,i_.Perpendicular(CGAL::COUNTERCLOCKWISE));
  }

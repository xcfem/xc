//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//PolygonMap.cc

#include "PolygonMap.h"

PolygonVertexStorage::PolygonVertexStorage(const size_t &sz,const size_t &V)
  : ind_vertices(sz,0)
  {}
void PolygonVertexStorage::Print(std::ostream &os) const
  {
    const size_t nv= ind_vertices.size();
    if(nv<1) return;
    os << "Cara: " << ind_vertices[0];
    for(size_t i=1;i<nv;i++)
       os << ", " << ind_vertices[i];
  }




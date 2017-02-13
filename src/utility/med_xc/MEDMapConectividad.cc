//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDMapConectividad

#include "MEDMapConectividad.h"
#include "MEDMapIndices.h"
#include "utility/matrix/ID.h"
#include <iostream>

//! @brief Agrega la conectividad de una celda.
void XC::MEDMapConectividad::nueva_celda(const MED_EN::medGeometryElement &tipo,const ID &conex,const MEDMapIndices &map_indices_vertices)
  {
    std::vector<int> &c= getConnectivity(tipo);
    const int sz= conex.Size();
    for(int i= 0;i<sz;i++)
      c.push_back(map_indices_vertices.getMEDIndice(conex(i)));
  }

//! @brief Returns the vector con la conectividad de los elementos
//! del tipo being passed as parameter.
std::vector<int> &XC::MEDMapConectividad::getConnectivity(const int &tipoElem)
  {
    iterator i= med_cell_connectivity.find(tipoElem);
    if(i!= med_cell_connectivity.end())
      return i->second;
    else
      return med_cell_connectivity[tipoElem];
  }

//! @brief Returns the vector con la conectividad de los elementos
//! del tipo being passed as parameter.
const std::vector<int> &XC::MEDMapConectividad::getConnectivity(const int &tipoElem) const
  { return med_cell_connectivity.find(tipoElem)->second; }


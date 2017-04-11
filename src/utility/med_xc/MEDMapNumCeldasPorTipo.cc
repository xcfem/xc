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
//MEDMapNumCeldasPorTipo

#include "MEDMapNumCeldasPorTipo.h"
#include <iostream>

//! @brief Adds a cell of the type being passed as parameter.
void XC::MEDMapNumCeldasPorTipo::add_cell(const MED_EN::medGeometryElement &tipo)
  {
    iterator i= tipos.find(tipo);
    if(i!=tipos.end())
      i->second++;
    else //new type.
      tipos[tipo]= 1;
  }

//! Erases los indices.
void XC::MEDMapNumCeldasPorTipo::clear(void)
  { tipos.clear(); }

//! @brief Returns the number of cell types.
size_t XC::MEDMapNumCeldasPorTipo::getNumberOfTypes(void) const
  { return tipos.size(); }

//! @brief Returns the number of cells.
size_t XC::MEDMapNumCeldasPorTipo::getNumCeldas(void) const
  {
    size_t retval= 0;
    for(const_iterator i=tipos.begin();i!=tipos.end();i++)
      retval+= i->second;
    return retval;
  }
//! @brief Returns the number of elements for each cell type.
std::vector<int> XC::MEDMapNumCeldasPorTipo::getNumCeldasPorTipo(void) const
  {
    const int numTipos= getNumberOfTypes();
    std::vector<int> retval(numTipos);
    size_t conta= 0;
    for(const_iterator i= tipos.begin();i!= tipos.end();i++,conta++)
      retval[conta]= i->second;
    return retval; 
  }

//! @brief Returns elements types of the mesh.
XC::MEDMapNumCeldasPorTipo::vector_tipos XC::MEDMapNumCeldasPorTipo::getTipos(void) const
  {
    const size_t sz= getNumberOfTypes();
    vector_tipos retval(sz,0);
    size_t conta= 0;
    for(const_iterator i= tipos.begin();i!= tipos.end();i++,conta++)
      retval[conta]= i->first;
    return retval;
  }

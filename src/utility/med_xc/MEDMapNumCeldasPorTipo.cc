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

//! @brief Agrega una celda del tipo being passed as parameter.
void XC::MEDMapNumCeldasPorTipo::agrega_celda(const MED_EN::medGeometryElement &tipo)
  {
    iterator i= tipos.find(tipo);
    if(i!=tipos.end())
      i->second++;
    else //nuevo tipo.
      tipos[tipo]= 1;
  }

//! Erases los indices.
void XC::MEDMapNumCeldasPorTipo::clear(void)
  { tipos.clear(); }

//! @brief Returns the número de tipos de celda.
size_t XC::MEDMapNumCeldasPorTipo::getNumberOfTypes(void) const
  { return tipos.size(); }

//! @brief Returns the número total de elementos.
size_t XC::MEDMapNumCeldasPorTipo::getNumCeldas(void) const
  {
    size_t retval= 0;
    for(const_iterator i=tipos.begin();i!=tipos.end();i++)
      retval+= i->second;
    return retval;
  }
//! @brief Returns the número de elementos por cada tipo de celda.
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

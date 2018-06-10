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
//MEDMapNumCellsByType

#include "MEDMapNumCellsByType.h"
#include <iostream>

//! @brief Adds a cell of the type being passed as parameter.
void XC::MEDMapNumCellsByType::add_cell(const MED_EN::medGeometryElement &type)
  {
    iterator i= types.find(type);
    if(i!=types.end())
      i->second++;
    else //new type.
      types[type]= 1;
  }

//! Erases los indices.
void XC::MEDMapNumCellsByType::clear(void)
  { types.clear(); }

//! @brief Returns the number of cell types.
size_t XC::MEDMapNumCellsByType::getNumberOfTypes(void) const
  { return types.size(); }

//! @brief Returns the number of cells.
size_t XC::MEDMapNumCellsByType::getNumCeldas(void) const
  {
    size_t retval= 0;
    for(const_iterator i=types.begin();i!=types.end();i++)
      retval+= i->second;
    return retval;
  }
//! @brief Returns the number of elements for each cell type.
std::vector<int> XC::MEDMapNumCellsByType::getNumberOfCellsByType(void) const
  {
    const int numberOfTypes= getNumberOfTypes();
    std::vector<int> retval(numberOfTypes);
    size_t conta= 0;
    for(const_iterator i= types.begin();i!= types.end();i++,conta++)
      retval[conta]= i->second;
    return retval; 
  }

//! @brief Returns elements types of the mesh.
XC::MEDMapNumCellsByType::type_vector XC::MEDMapNumCellsByType::getTypes(void) const
  {
    const size_t sz= getNumberOfTypes();
    type_vector retval(sz,0);
    size_t conta= 0;
    for(const_iterator i= types.begin();i!= types.end();i++,conta++)
      retval[conta]= i->first;
    return retval;
  }

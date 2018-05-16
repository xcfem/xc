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
//MEDCellBaseInfo

#include "MEDCellBaseInfo.h"


//! @brief Returns the number of elements.
size_t XC::MEDCellBaseInfo::getNumberOfElements(void) const
  { return cell_types.getNumCeldas(); }

//! @brief Adds cell connectivity.
void XC::MEDCellBaseInfo::new_cell(size_t tag,const MED_EN::medGeometryElement &type)
  {
    cell_types.add_cell(type);
    MEDBaseInfo::new_tag(tag);
  }

const XC::MEDMapNumCellsByType &XC::MEDCellBaseInfo::getMapCellTypes(void) const
  { return cell_types; }

//! @brief Dump the cells into a SUPPORT object of MEDMEM.
void XC::MEDCellBaseInfo::to_support_med(MEDMEM::SUPPORT &supp) const
  {
    const int NumberOfTypes= cell_types.getNumberOfTypes();
    supp.setNumberOfGeometricType(NumberOfTypes);
    XC::MEDMapNumCellsByType::type_vector types= cell_types.getTypes();
    supp.setGeometricType(&types[0]);
    const std::vector<int> numberOfElements= cell_types.getNumberOfCellsByType();
    supp.setNumberOfElements(&numberOfElements[0]);
  }

//! @brief Dumps information on MED mesh.
void XC::MEDCellBaseInfo::to_med_mesh(MEDMEM::MESHING &mesh) const
  {
    const int NumberOfTypes= cell_types.getNumberOfTypes();
    mesh.setNumberOfTypes(NumberOfTypes,MED_EN::MED_CELL);
    XC::MEDMapNumCellsByType::type_vector types= cell_types.getTypes();
    mesh.setTypes(&types[0],MED_EN::MED_CELL);
    const std::vector<int> numberOfElements= cell_types.getNumberOfCellsByType();
    mesh.setNumberOfElements(&numberOfElements[0],MED_EN::MED_CELL);
  }

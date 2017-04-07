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
//MEDCellBaseInfo.h

#ifndef MEDCELLBASEINFO_H
#define MEDCELLBASEINFO_H

#include "MEDBaseInfo.h"
#include "xc_basic/src/med_xc/MEDMEM_Meshing.hxx"
#include "xc_basic/src/med_xc/MEDMEM_Support.hxx"
#include "MEDMapNumCeldasPorTipo.h"
#include <map>

namespace XC {
class ID;

//! @ingroup MED
//
//!  @brief Basic information about mesh cells.
class MEDCellBaseInfo: public MEDBaseInfo
  {
  protected:
    MEDMapNumCeldasPorTipo cell_types; //!< Type and number of elements for each of those types.
  public:
    size_t getNumeroElementos(void) const;
    void new_cell(size_t i,const MED_EN::medGeometryElement &);

    const MEDMapNumCeldasPorTipo &getMapCellTypes(void) const;
    void to_support_med(MEDMEM::SUPPORT &supp) const;
    void to_med_mesh(MEDMEM::MESHING &) const;
  };
} // end of XC namespace
#endif

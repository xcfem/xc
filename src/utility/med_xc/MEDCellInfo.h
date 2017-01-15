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
//MEDCellInfo.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

#ifndef MEDCELLINFO_H
#define MEDCELLINFO_H

#include "MEDCellBaseInfo.h"
#include "MEDMapConectividad.h"

namespace XC {
class Mesh;
class ID;

//! @ingroup MED
//
//!  @brief Información sobre las celdas de la malla (conectividad,...).
class MEDCellInfo: public MEDCellBaseInfo
  {
  private:
    MEDMapConectividad med_cell_connectivity; 
  public:
    MEDCellInfo(void);
    MEDCellInfo(const Mesh &,const MEDMapIndices &);

    void nueva_celda(size_t i,const MED_EN::medGeometryElement &,const ID &,const MEDMapIndices &);

    void to_med(MEDMEM::MESHING &) const;
  };
} // end of XC namespace
#endif

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
//MEDCellInfo

#include "MEDCellInfo.h"

#include "domain/mesh/Mesh.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include "MEDMapIndices.h"

//! @brief Constructor.
XC::MEDCellInfo::MEDCellInfo(void)
  {}

//! @brief Constructor.
XC::MEDCellInfo::MEDCellInfo(const Mesh &mesh,const MEDMapIndices &map_indices)
  {
    const Element *theElement= nullptr;
    Mesh &mesh_no_const= const_cast<Mesh &>(mesh);
    ElementIter &theElements = mesh_no_const.getElements();
    while((theElement = theElements()) != nullptr)
      {
        MED_EN::medGeometryElement tipo= theElement->getMEDCellType();
        const ID conex= theElement->getNodePtrs().getExternalNodes();
        nueva_celda(theElement->getTag(),tipo,conex,map_indices);
      }
  }

//! @brief Adds cell connectivity.
void XC::MEDCellInfo::nueva_celda(size_t tag,const MED_EN::medGeometryElement &tipo,const ID &conex,const MEDMapIndices &map_indices_vertices)
  {
    MEDCellBaseInfo::new_cell(tag,tipo);
    med_cell_connectivity.nueva_celda(tipo,conex,map_indices_vertices);
  }

//! @brief Dumps cells definition on MED mesh.
void XC::MEDCellInfo::to_med(MEDMEM::MESHING &mesh) const
  {
    MEDCellBaseInfo::to_med_mesh(mesh);
    for(MEDMapNumCeldasPorTipo::const_iterator i= cell_types.begin();i!=cell_types.end();i++)
      {
        const std::vector<int> &conn= med_cell_connectivity.getConnectivity(i->first);
        mesh.setConnectivity(MED_EN::MED_CELL,i->first,&conn[0]);
      }
  }

//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MEDCellInfo

#include "MEDCellInfo.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/Mesh.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/element/NodePtrsWithIDs.h"
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

//! @brief Agrega la conectividad de una celda.
void XC::MEDCellInfo::nueva_celda(size_t tag,const MED_EN::medGeometryElement &tipo,const ID &conex,const MEDMapIndices &map_indices_vertices)
  {
    MEDCellBaseInfo::new_cell(tag,tipo);
    med_cell_connectivity.nueva_celda(tipo,conex,map_indices_vertices);
  }

//! @brief Vuelca la definición de las celdas en la
//! malla MED
void XC::MEDCellInfo::to_med(MEDMEM::MESHING &malla) const
  {
    MEDCellBaseInfo::to_malla_med(malla);
    for(MEDMapNumCeldasPorTipo::const_iterator i= cell_types.begin();i!=cell_types.end();i++)
      {
        const std::vector<int> &conn= med_cell_connectivity.getConnectivity(i->first);
        malla.setConnectivity(MED_EN::MED_CELL,i->first,&conn[0]);
      }
  }

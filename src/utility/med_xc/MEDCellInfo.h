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

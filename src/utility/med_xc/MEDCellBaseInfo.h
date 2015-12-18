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
//MEDCellBaseInfo.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

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
//!  @brief Información básica sobre las celdas de la malla.
class MEDCellBaseInfo: public MEDBaseInfo
  {
  protected:
    MEDMapNumCeldasPorTipo cell_types; //!< Tipo y número de elementos de ese tipo.
  public:
    size_t getNumeroElementos(void) const;
    void new_cell(size_t i,const MED_EN::medGeometryElement &);

    const MEDMapNumCeldasPorTipo &getMapCellTypes(void) const;
    void to_support_med(MEDMEM::SUPPORT &supp) const;
    void to_malla_med(MEDMEM::MESHING &) const;

    any_const_ptr GetProp(const std::string &) const;
  };
} // end of XC namespace
#endif

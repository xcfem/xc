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
//MEDCellBaseInfo

#include "MEDCellBaseInfo.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Devuelve el número de elementos.
size_t XC::MEDCellBaseInfo::getNumeroElementos(void) const
  { return cell_types.getNumCeldas(); }

//! @brief Agrega la conectividad de una celda.
void XC::MEDCellBaseInfo::new_cell(size_t tag,const MED_EN::medGeometryElement &tipo)
  {
    cell_types.agrega_celda(tipo);
    MEDBaseInfo::nuevo_tag(tag);
  }

const XC::MEDMapNumCeldasPorTipo &XC::MEDCellBaseInfo::getMapCellTypes(void) const
  { return cell_types; }

//! @brief Vuelca la las celdas en un objeto SUPPORT de MEDMEM.
void XC::MEDCellBaseInfo::to_support_med(MEDMEM::SUPPORT &supp) const
  {
    const int NumberOfTypes= cell_types.getNumberOfTypes();
    supp.setNumberOfGeometricType(NumberOfTypes);
    XC::MEDMapNumCeldasPorTipo::vector_tipos tipos= cell_types.getTipos();
    supp.setGeometricType(&tipos[0]);
    const std::vector<int> numberOfElements= cell_types.getNumCeldasPorTipo();
    supp.setNumberOfElements(&numberOfElements[0]);
  }

//! @brief Vuelca la las celdas en la malla MEDMEM.
void XC::MEDCellBaseInfo::to_malla_med(MEDMEM::MESHING &malla) const
  {
    const int NumberOfTypes= cell_types.getNumberOfTypes();
    malla.setNumberOfTypes(NumberOfTypes,MED_EN::MED_CELL);
    XC::MEDMapNumCeldasPorTipo::vector_tipos tipos= cell_types.getTipos();
    malla.setTypes(&tipos[0],MED_EN::MED_CELL);
    const std::vector<int> numberOfElements= cell_types.getNumCeldasPorTipo();
    malla.setNumberOfElements(&numberOfElements[0],MED_EN::MED_CELL);
  }

any_const_ptr XC::MEDCellBaseInfo::GetProp(const std::string &cod) const
  {
    return MEDBaseInfo::GetProp(cod);
  }

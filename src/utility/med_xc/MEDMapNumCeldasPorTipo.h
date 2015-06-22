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
//MEDMapNumCeldasPorTipo.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDMAPNUMCELDASPORTIPO_H
#define MEDMAPNUMCELDASPORTIPO_H

#include <map>
#include <vector>
#include "xc_basic/src/med_xc/MEDMEM_define.hxx"

namespace XC {
//! @ingroup MED
//
//!  @brief Número de celdas de cada tipo.
class MEDMapNumCeldasPorTipo
  {
    std::map<MED_EN::medGeometryElement,int> tipos;
  public:
    typedef std::map<MED_EN::medGeometryElement,int>::const_iterator const_iterator;
    typedef std::map<MED_EN::medGeometryElement,int>::iterator iterator;
    typedef std::vector<MED_EN::medGeometryElement> vector_tipos;

    void agrega_celda(const MED_EN::medGeometryElement &);

    size_t getNumberOfTypes(void) const;
    size_t getNumCeldas(void) const;
    std::vector<int> getNumCeldasPorTipo(void) const;
    vector_tipos getTipos(void) const;
    inline const_iterator begin(void) const
      { return tipos.begin(); } 
    inline const_iterator end(void) const
      { return tipos.end(); } 
    inline size_t size(void) const
      { return tipos.size(); }

    void clear(void);

  };
} // fin namespace XC
#endif

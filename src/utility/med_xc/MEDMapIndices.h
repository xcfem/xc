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
//MEDMapIndices.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDMAPINDICES_H
#define MEDMAPINDICES_H

#include <map>
#include <vector>
#include <iostream>

namespace XC {
//! @ingroup MED
//
//!  @brief Correspondencia de índices entre XC y MEDMEM.
class MEDMapIndices
  {
    std::map<int,int> indices; //!< Índices.
    typedef std::map<int,int>::const_iterator const_iterator;
    typedef std::map<int,int>::iterator iterator;

  public:
    void nuevo_tag(size_t i);

    int getMEDIndice(const size_t &i) const;
    std::vector<int> getMEDIndices(const std::vector<int> &) const;

    void clear(void);

    void print(std::ostream &os) const;
  };

 std::ostream &operator<<(std::ostream &,const MEDMapIndices &);
} // end of XC namespace
#endif

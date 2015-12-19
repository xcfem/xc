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
//MEDMapConectividad.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

#ifndef MEDMAPCONECTIVIDAD_H
#define MEDMAPCONECTIVIDAD_H

#include <map>
#include <vector>
#include "xc_basic/src/med_xc/MEDMEM_define.hxx"

namespace XC {
class Mesh;
class ID;
class MEDMapIndices;

//! @ingroup MED
//
//!  @brief Información sobre la conectividad de las celdas de la malla.
class MEDMapConectividad
  {
  public:
    typedef std::map<int,std::vector<int> > map_connectivity;
    typedef map_connectivity::iterator iterator;
    typedef map_connectivity::const_iterator const_iterator;
  private:
    map_connectivity med_cell_connectivity; 
    std::vector<int> &getConnectivity(const int &);
  public:

    void nueva_celda(const MED_EN::medGeometryElement &,const ID &,const MEDMapIndices &);

    const std::vector<int> &getConnectivity(const int &) const;
  };
} // end of XC namespace
#endif

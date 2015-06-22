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
//MEDMapConectividad

#include "MEDMapConectividad.h"
#include "MEDMapIndices.h"
#include "utility/matrix/ID.h"
#include <iostream>

//! @brief Agrega la conectividad de una celda.
void XC::MEDMapConectividad::nueva_celda(const MED_EN::medGeometryElement &tipo,const ID &conex,const MEDMapIndices &map_indices_vertices)
  {
    std::vector<int> &c= getConnectivity(tipo);
    const int sz= conex.Size();
    for(int i= 0;i<sz;i++)
      c.push_back(map_indices_vertices.getMEDIndice(conex(i)));
  }

//! @brief Devuelve el vector con la conectividad de los elementos
//! del tipo que se pasa como parámetro.
std::vector<int> &XC::MEDMapConectividad::getConnectivity(const int &tipoElem)
  {
    iterator i= med_cell_connectivity.find(tipoElem);
    if(i!= med_cell_connectivity.end())
      return i->second;
    else
      return med_cell_connectivity[tipoElem];
  }

//! @brief Devuelve el vector con la conectividad de los elementos
//! del tipo que se pasa como parámetro.
const std::vector<int> &XC::MEDMapConectividad::getConnectivity(const int &tipoElem) const
  { return med_cell_connectivity.find(tipoElem)->second; }


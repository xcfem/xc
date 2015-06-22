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
//MEDMapIndices

#include "MEDMapIndices.h"
#include <iostream>

//! Borra los indices.
void XC::MEDMapIndices::clear(void)
  { indices.clear(); }

void XC::MEDMapIndices::nuevo_tag(size_t i)
  {
    const int numVertices= indices.size()+1;
    indices[i]= numVertices;
  }

//! @brief Devuelve el índice correspondiente al que
//! se pasa como parámetro.
int XC::MEDMapIndices::getMEDIndice(const size_t &i) const
  {
    const_iterator iter= indices.find(i);
    if(iter!=indices.end())
      return iter->second;
    else
      {
        std::cerr << "MEDMapIndices::getMEDIndice; no se encontró el índice"
                  << " correspondiente al " << i << std::endl;
        return -1;
      }
  }

//! @brief Devuelve los índices correspondientes a los que
//! se pasan como parámetro.
std::vector<int> XC::MEDMapIndices::getMEDIndices(const std::vector<int> &idx) const
  {
    const size_t sz= idx.size();
    std::vector<int> retval(sz,0);
    for(size_t i= 0;i<sz;i++)
      retval[i]= getMEDIndice(idx[i]);
    return retval;
  }

//! @brief Imprime el mapa de índices.
void XC::MEDMapIndices::print(std::ostream &os) const
  {
    if(!indices.empty())
      {
        const_iterator i= indices.begin();
        os << i->first << "->" << i->second;
        i++;
        for(;i!=indices.end();i++)
          os << "," << i->first << "->" << i->second;
      }
  }

std::ostream &XC::operator<<(std::ostream &os,const MEDMapIndices &mi)
  {
    mi.print(os);
    return os;
  }


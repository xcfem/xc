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
//MEDMapNumCeldasPorTipo

#include "MEDMapNumCeldasPorTipo.h"
#include <iostream>

//! @brief Agrega una celda del tipo being passed as parameter.
void XC::MEDMapNumCeldasPorTipo::agrega_celda(const MED_EN::medGeometryElement &tipo)
  {
    iterator i= tipos.find(tipo);
    if(i!=tipos.end())
      i->second++;
    else //nuevo tipo.
      tipos[tipo]= 1;
  }

//! Borra los indices.
void XC::MEDMapNumCeldasPorTipo::clear(void)
  { tipos.clear(); }

//! @brief Devuelve el número de tipos de celda.
size_t XC::MEDMapNumCeldasPorTipo::getNumberOfTypes(void) const
  { return tipos.size(); }

//! @brief Devuelve el número total de elementos.
size_t XC::MEDMapNumCeldasPorTipo::getNumCeldas(void) const
  {
    size_t retval= 0;
    for(const_iterator i=tipos.begin();i!=tipos.end();i++)
      retval+= i->second;
    return retval;
  }
//! @brief Devuelve el número de elementos por cada tipo de celda.
std::vector<int> XC::MEDMapNumCeldasPorTipo::getNumCeldasPorTipo(void) const
  {
    const int numTipos= getNumberOfTypes();
    std::vector<int> retval(numTipos);
    size_t conta= 0;
    for(const_iterator i= tipos.begin();i!= tipos.end();i++,conta++)
      retval[conta]= i->second;
    return retval; 
  }

//! @brief Devuelve los tipos de elemento de la malla.
XC::MEDMapNumCeldasPorTipo::vector_tipos XC::MEDMapNumCeldasPorTipo::getTipos(void) const
  {
    const size_t sz= getNumberOfTypes();
    vector_tipos retval(sz,0);
    size_t conta= 0;
    for(const_iterator i= tipos.begin();i!= tipos.end();i++,conta++)
      retval[conta]= i->first;
    return retval;
  }

//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//TrasfCooLoader.cc

#include "TransfCooLoader.h"
#include "domain/mesh/element/coordTransformation/LinearCrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/LinearCrdTransf3d.h"
#include "domain/mesh/element/coordTransformation/PDeltaCrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/PDeltaCrdTransf3d.h"
#include "domain/mesh/element/coordTransformation/CorotCrdTransf2d.h"
#include "domain/mesh/element/coordTransformation/CorotCrdTransf3d.h"
#include "boost/any.hpp"

//! @brief Libera la memoria ocupada por las transformaciones de coordenadas.
void XC::TransfCooLoader::libera(void)
  {
    for(iterator i= begin();i!=end();i++)
      delete (*i).second;
    transfcoo.erase(begin(),end());
  }

//! @brief Constructor.
XC::TransfCooLoader::TransfCooLoader(Preprocessor *owr)
  : Loader(owr), tag_trf(0) {}

//! @brief Constructor de copia.
XC::TransfCooLoader::TransfCooLoader(const TransfCooLoader &otro)
 : Loader(otro)
  { std::cerr << "No se debe copiar este objeto." << std::endl; }

//! @brief Operador asignación.
XC::TransfCooLoader &XC::TransfCooLoader::operator=(const TransfCooLoader &otro)
  {
    Loader::operator=(otro);
    std::cerr << "No se debe asignar este objeto." << std::endl;
    return *this;
  }

//! @brief Destructor.
XC::TransfCooLoader::~TransfCooLoader(void)
  { libera(); }

XC::LinearCrdTransf2d *XC::TransfCooLoader::newLinearCrdTransf2d(const std::string &cod_trf)
  {
    LinearCrdTransf2d *retval= new LinearCrdTransf2d(tag_trf++);
    assert(retval);
    transfcoo[cod_trf]= retval;
    return retval;    
  } 

XC::LinearCrdTransf3d *XC::TransfCooLoader::newLinearCrdTransf3d(const std::string &cod_trf)
  { 
    LinearCrdTransf3d *retval= new LinearCrdTransf3d(tag_trf++);
    assert(retval);
    transfcoo[cod_trf]= retval;
    return retval;    
  } 

XC::PDeltaCrdTransf2d *XC::TransfCooLoader::newPDeltaCrdTransf2d(const std::string &cod_trf)
  {
    PDeltaCrdTransf2d *retval= new PDeltaCrdTransf2d(tag_trf++);
    assert(retval);
    transfcoo[cod_trf]= retval;
    return retval;
  } 

XC::PDeltaCrdTransf3d *XC::TransfCooLoader::newPDeltaCrdTransf3d(const std::string &cod_trf)
  {
    PDeltaCrdTransf3d *retval= new PDeltaCrdTransf3d(tag_trf++);
    assert(retval);
    transfcoo[cod_trf]= retval;
    return retval;
  }

XC::CorotCrdTransf2d *XC::TransfCooLoader::newCorotCrdTransf2d(const std::string &cod_trf)
  {
    CorotCrdTransf2d *retval= new CorotCrdTransf2d(tag_trf++);
    assert(retval);
    transfcoo[cod_trf]= retval;
    return retval;
  } 

XC::CorotCrdTransf3d *XC::TransfCooLoader::newCorotCrdTransf3d(const std::string &cod_trf)
  {
    CorotCrdTransf3d *retval= new CorotCrdTransf3d(tag_trf++);
    assert(retval);
    transfcoo[cod_trf]= retval;
    return retval;
  }

//! @brief Devuelve una referencia al mapa de transformaciones de coordenadas.
const XC::TransfCooLoader::map_transfcoo &XC::TransfCooLoader::Map(void) const
  { return transfcoo; }

//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::TransfCooLoader::const_iterator XC::TransfCooLoader::begin(void) const
  { return transfcoo.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::TransfCooLoader::const_iterator XC::TransfCooLoader::end(void) const
  { return transfcoo.end(); }
//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::TransfCooLoader::iterator XC::TransfCooLoader::begin(void)
  { return transfcoo.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::TransfCooLoader::iterator XC::TransfCooLoader::end(void)
  { return transfcoo.end(); }

//! @brief Si encuentra la transformacion cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::TransfCooLoader::const_iterator XC::TransfCooLoader::find(const std::string &nmb) const
  { return transfcoo.find(nmb); }

//! @brief Si encuentra la transformacion cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::TransfCooLoader::iterator XC::TransfCooLoader::find(const std::string &nmb)
  { return transfcoo.find(nmb); }

//! @brief Si encuentra la transformacion cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::CrdTransf *XC::TransfCooLoader::find_ptr(const std::string &nmb)
  {
    XC::TransfCooLoader::iterator i= find(nmb);
    if(i!= transfcoo.end())
      return (*i).second;
    else
      return nullptr; 
  }

void XC::TransfCooLoader::clearAll(void)
  {
    libera();
    tag_trf= 0;
  }

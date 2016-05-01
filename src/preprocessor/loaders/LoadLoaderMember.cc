//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
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
//LoadLoaderMember.cc

#include "LoadLoaderMember.h"
#include "preprocessor/loaders/LoadLoader.h"


//! @brief Constructor por defecto.
XC::LoadLoaderMember::LoadLoaderMember(LoadLoader *owr)
  : EntCmd(owr), MovableObject(0) {}


//! @brief Devuelve el cargador propietario.
const XC::LoadLoader *XC::LoadLoaderMember::getLoadLoader(void) const
  {
    const LoadLoader *loader= dynamic_cast<const LoadLoader *>(Owner());
    if(!loader)
      std::cerr << "(LoadLoaderMember) necesito un propietario." << std::endl;
    return loader;
  }

//! @brief Devuelve el cargador propietario.
XC::LoadLoader *XC::LoadLoaderMember::getLoadLoader(void)
  {
    LoadLoader *loader= dynamic_cast<LoadLoader *>(Owner());
    if(!loader)
      std::cerr << "(LoadLoaderMember) necesito un propietario." << std::endl;
    return loader;
  }

//! @brief Devuelve el dominio en el que se cargan los elementos.
XC::Domain *XC::LoadLoaderMember::getDomain(void) const
  {
    Domain *retval= nullptr;
    const LoadLoader *loader= getLoadLoader();
    if(loader)
      retval= loader->getDomain();
    return retval;
  }

//! @brief Devuelve el tag para el siguiente caso de carga.
int &XC::LoadLoaderMember::getTagLP(void)
  { return getLoadLoader()->getTagLP(); }

//! @brief Devuelve el tag para el siguiente caso de carga.
const int &XC::LoadLoaderMember::getTagLP(void) const
  { return getLoadLoader()->getTagLP(); }


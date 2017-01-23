//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDObject.h
//Envoltorio para objetos de MED (para exportar archivos a «salome»).

#ifndef MEDOBJECT_H
#define MEDOBJECT_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/utils/ObjMap.h"

namespace XC {
//! @defgroup MED-Fichier interface.
//
//! @ingroup MED
//
//!  @brief Clase base para objetos MED.
class MEDObject: public EntCmd
  {
  public:
    typedef ObjMap<MEDObject> contenedor;
    typedef contenedor::iterator iterator;
    typedef contenedor::const_iterator const_iterator;
  public:
    MEDObject(void);

  };
} // end of XC namespace
#endif

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
//Wrapper for MED objects (to export models to «Salome»).

#ifndef MEDOBJECT_H
#define MEDOBJECT_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/utils/ObjMap.h"

namespace XC {
//! @ingroup Utils
//!
//! @defgroup MED MED-Fichier interface.
//
//! @ingroup MED
//
//!  @brief Base class for MED objects.
class MEDObject: public EntCmd
  {
  public:
    typedef ObjMap<MEDObject> container;
    typedef container::iterator iterator;
    typedef container::const_iterator const_iterator;
  public:
    MEDObject(void);

  };
} // end of XC namespace
#endif

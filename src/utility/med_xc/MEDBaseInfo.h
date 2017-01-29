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
//MEDBaseInfo.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDBASEINFO_H
#define MEDBASEINFO_H

#include "MEDObject.h"
#include "MEDMapIndices.h"

namespace XC {

class Mesh;
//! @ingroup MED
//
//!  @brief Base class for infomation objects.
class MEDBaseInfo: public MEDObject
  {
  protected:
    MEDMapIndices indices; //!< Correspondencia de índices XC y MED.
    void nuevo_tag(size_t tag);

  public:
    const MEDMapIndices &getMapIndices(void) const;
    virtual void clear(void);


  };
} // end of XC namespace
#endif

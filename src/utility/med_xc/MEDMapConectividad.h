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
//MEDMapConectividad.h

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
//!  @brief Information about mesh cells connectivity.
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

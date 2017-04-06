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
//MEDMapIndices.h
//Envoltorio para el objeto MESHING de MED (para exportar archivos a «salome»).

#ifndef MEDMAPINDICES_H
#define MEDMAPINDICES_H

#include <map>
#include <vector>
#include <iostream>

namespace XC {
//! @ingroup MED
//
//!  @brief Map between the indexes in XC and in MEDMEM.
class MEDMapIndices
  {
    std::map<int,int> indices; //!< Indexes.
    typedef std::map<int,int>::const_iterator const_iterator;
    typedef std::map<int,int>::iterator iterator;

  public:
    void nuevo_tag(size_t i);

    int getMEDIndice(const size_t &i) const;
    std::vector<int> getMEDIndices(const std::vector<int> &) const;

    void clear(void);

    void print(std::ostream &os) const;
  };

 std::ostream &operator<<(std::ostream &,const MEDMapIndices &);
} // end of XC namespace
#endif

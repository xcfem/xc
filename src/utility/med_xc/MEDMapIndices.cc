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

//! @brief Returns the index that corresponds to the argument.
int XC::MEDMapIndices::getMEDIndice(const size_t &i) const
  {
    const_iterator iter= indices.find(i);
    if(iter!=indices.end())
      return iter->second;
    else
      {
        std::cerr << "MEDMapIndices" << "::" << __FUNCTION__
	          << "; index that corresponds with " << i
                  << " not found." << std::endl;
        return -1;
      }
  }

//! @brief Returns the indexes that correspond to those
//! being passed as parameter.
std::vector<int> XC::MEDMapIndices::getMEDIndices(const std::vector<int> &idx) const
  {
    const size_t sz= idx.size();
    std::vector<int> retval(sz,0);
    for(size_t i= 0;i<sz;i++)
      retval[i]= getMEDIndice(idx[i]);
    return retval;
  }

//! @brief Prints indexes map.
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


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
//MEDIntFieldInfo.h

#ifndef MEDINTFIELDINFO_H
#define MEDINTFIELDINFO_H

#include "MEDTFieldInfo.h"

namespace XC {
class Set;
class MEDGroupInfo;

//! @ingroup MED
//
//! @brief Information about field defined over a mesh subset.
class MEDIntFieldInfo: public MEDTFieldInfo<int>
  {
  protected:
    friend class MEDMeshing;
    MEDIntFieldInfo(const FieldInfo &,MEDGroupInfo *);


    void populateOnNodes(const Set &,const FieldInfo &);
    void populateOnElements(const Set &,const FieldInfo &);
    void populateOnGaussPoints(const Set &,const FieldInfo &);
  };
} // end of XC namespace
#endif

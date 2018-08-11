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
//MEDGroupInfo.h

#ifndef MEDGROUPINFO_H
#define MEDGROUPINFO_H

#include "MEDCellBaseInfo.h"

namespace XC {
class Set;
class MEDMeshing;

//! @ingroup MED
//
//!  @brief Information about node and element sets.
class MEDGroupInfo: public MEDCellBaseInfo
  {
  public:
    typedef std::map<int,std::vector<int> > map_type_indexes;
  private:
    std::string grp_name; //!< name of the group.
    map_type_indexes type_indexes; //!< indexes of the elements by type. 
    MED_EN::medEntityMesh entity_type; //!< type of the entity stored by the set (node,cell,face,edge,...).
    mutable MEDMEM::GROUP *med_group; //!< Group definition in MEDMEM.
  protected:
    friend class MEDMeshing;
    friend class MEDFieldInfo;
    MEDGroupInfo(MEDMeshing *mesh,const std::string &);
    MEDGroupInfo(MEDMeshing *mesh,const Set &);

    std::vector<int> &getElementOfTypeIndices(const MED_EN::medGeometryElement &);
    std::vector<int> getTypeIndexesVector(void) const;
    std::vector<int> getElementIndices(void) const;

    const MEDMeshing &getMesh(void) const;
    MEDMEM::GROUP *getMEDGroup(void) const;
  public:
    ~MEDGroupInfo(void);
    const std::string &getName(void) const;

    void new_vertice(size_t i,const MEDMapIndices &);
    void nueva_celda(size_t i,const MED_EN::medGeometryElement &);
    void to_med(void) const;
  };
} // end of XC namespace
#endif

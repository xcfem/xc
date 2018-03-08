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
//MEDMeshing.h
//Wrapper for the MESHING object of MED (to export files to "salome").

#ifndef MEDMESHING_H
#define MEDMESHING_H

#include "MEDObject.h"
#include "xc_basic/src/med_xc/MEDMEM_Meshing.hxx"
#include "MEDCellInfo.h"
#include "MEDVertexInfo.h"
#include "MEDGroupInfo.h"
#include "MEDFieldInfo.h"
#include <deque>

namespace XC {
class MapSet;
class MapFields;
class MEDDblFieldInfo;
class FEProblem;

//! @ingroup MED
//
//!  @brief Wrapper for the MESHING object of MED.
class MEDMeshing: public MEDObject
  {
    const MapSet &sets;
    const MapFields &fields;
    MEDVertexInfo vertices;
    MEDCellInfo cells;
    mutable std::deque<MEDGroupInfo> med_groups;
    mutable std::deque<MEDFieldInfo *> med_fields;

    static const std::string str_node_group;
    static const std::string str_element_group;

    mutable MEDMEM::MESHING mesh;
    MEDMeshing(const MEDMeshing &);
    MEDMeshing &operator=(const MEDMeshing &);
  protected:
    void new_node(size_t i,const std::vector<double> &coo);

    friend class MEDGroupInfo;
    MEDMEM::MESHING &getMEDMesh(void) const;
    MEDGroupInfo *getGroupInfo(const std::string &) const;
    MEDGroupInfo *getGroupInfo(const Set &,const FieldInfo &) const;

    void defineMEDGroups(void);
    void defineMEDGaussModels(const Set &,MEDFieldInfo &) const;
    void defineMEDDblField(const Set &,const FieldInfo &,MEDGroupInfo *) const;
    void defineMEDIntField(const Set &,const FieldInfo &,MEDGroupInfo *) const;
    void defineMEDFields(void) const;
    
  public:
    MEDMeshing(const Mesh &,const MapSet &, const MapFields &);
    MEDMeshing(const FEProblem &);
    ~MEDMeshing(void);

    void clear(void);
    void write(const std::string &);

    inline const std::string getMeshName(void) const
      { return mesh.getName(); }
    inline void setMeshName(const std::string &str) const
      { return mesh.setName(str); }
    inline MEDVertexInfo &getVertices(void)
      { return vertices; }
    inline void setVertices(const MEDVertexInfo &vtx)
      { vertices= vtx; }
    inline MEDCellInfo &getCells(void)
      { return cells; }
    inline void setCells(const MEDCellInfo &cls)
      { cells= cls; }  

    const MEDMapIndices &getMapIndicesVertices(void) const;
    const MEDMapIndices &getMapIndicesCeldas(void) const;


    void to_med(void);
  };
} // end of XC namespace
#endif

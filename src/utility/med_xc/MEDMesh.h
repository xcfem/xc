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
//MEDMesh.h
//Wrapper for the MESHING object of MED (to export files to "salome").

#ifndef MEDMESH_H
#define MEDMESH_H

#include "MEDObject.h"
#include "xc_basic/src/med_xc/MEDMEM_Mesh.hxx"

namespace XC {
  class Matrix;
//! @ingroup MED
//
//!  @brief Wrapper for the MESHING object of MED.
class MEDMesh: public MEDObject
  {
    std::string meshName;
    mutable MEDMEM::MESH *mesh;
    void free_mem(void) const;
    void alloc(const std::string &) const;
    void alloc(const MEDMesh &) const;
    MEDMesh(const MEDMesh &);
    MEDMesh &operator=(const MEDMesh &);
  protected:

  public:
    MEDMesh(void);
    ~MEDMesh(void);
    inline std::string getMeshName(void) const
      { return meshName; }
    inline void setMeshName(const std::string &s)
      { meshName= s; }

    size_t getSpaceDimension(void);
    size_t getMeshDimension(void);
    size_t getNumberOfNodes(void);
    boost::python::list getCoordinatesNames(void);
    boost::python::list getCoordinatesUnits(void);
    Matrix getCoordinates(void);
    size_t getNumberOfCellTypes();
    boost::python::list getCellTypes(void);
    boost::python::list getCellTypeNames(void);
    size_t getNumCellsOfType(int );
    Matrix getConnectivityCellsOfType(int );
    size_t getNumberOfGroups(int entityType= MED_EN::MED_ALL_ENTITIES);
    size_t getNumberOfFamilies(int entityType= MED_EN::MED_ALL_ENTITIES);

    void read(const std::string &);
  };
} // end of XC namespace
#endif

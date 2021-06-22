// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
//PolygonalFace.h

#ifndef POLYGONALFACE_H
#define POLYGONALFACE_H

#include "Face.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray.h"
#include "utility/geom/ref_sys/Ref2d3d.h"

namespace XC {

class Paver; // wrapper of the paving algorithm.
  
//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Polygonal face defined by an arbitrary number of vertices.
class PolygonalFace: public Face
  {
  protected:
    Ref2d3d ref; //!< local reference system.
    int create_elements_from_quads(const std::deque<std::vector<int> > &);
    void create_nodes_from_paving(Paver &paver);
    int create_elements_from_paving(const Paver &paver);
    void gen_mesh_paving(meshing_dir dm);
    void create_gmsh_points(const double &) const;
    void create_gmsh_lines(void) const;
    std::map<int, const Node *> create_nodes_from_gmsh(void);
    int create_elements_from_gmsh(const std::map<int, const Node *> &);
    void gen_mesh_gmsh(meshing_dir dm);
  public:
    PolygonalFace(Preprocessor *m);
    virtual SetEstruct *getCopy(void) const;

    // Surface geometry.
    void setPoints(const ID &);
    

    // Surface orientation.
    Vector3d getIVector(void) const;
    Vector3d getJVector(void) const;

    // Mesh generation.
    void genMesh(meshing_dir dm, bool paving= false);
  };

} //end of XC namespace
#endif

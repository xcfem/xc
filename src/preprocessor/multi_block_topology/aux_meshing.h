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
//aux_meshing.h
                                                                        
#ifndef aux_meshing_h
#define aux_meshing_h

#include "preprocessor/MeshingParams.h"

namespace XC {

class Element;
class NodePtrArray3d;
class ElemPtrArray3d;

//void mesh_quad4N_bidimensional(const Element &e,const NodePtrArray3d &nodes,ElemPtrArray3d &);
ElemPtrArray3d put_quad4N_on_mesh(const Element &e,const NodePtrArray3d &,meshing_dir dm);
ElemPtrArray3d put_quad9N_on_mesh(const Element &e,const NodePtrArray3d &,meshing_dir dm);

} // end of XC namespace
#endif

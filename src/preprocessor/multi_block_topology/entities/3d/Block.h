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
//Block.h

#ifndef BLOCK_H
#define BLOCK_H

#include "Body.h"

namespace XC {

//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Six-faced body.
class Block: public Body
  {
    BodyFace sups[6];
    size_t index(Face *s) const;
    void put(const size_t &i,Face *s);
  protected:
    void insert(const size_t &);
    //XXX void add_points(const std::vector<size_t> &);
    void append_faces(const std::vector<size_t> &);
    void create_face_nodes(void);
    Pos3dArray3d get_positions(void) const;

    virtual BodyFace *getFace(const size_t &i);
  public:
    Block(Preprocessor *m,const std::string &name= "");
    virtual bool operator==(const Block &) const;
    SetEstruct *getCopy(void) const;
    //! @brief Return the number of lines of the object.
    virtual size_t getNumberOfLines(void) const;
    virtual size_t getNumberOfVertices(void) const;
    virtual size_t getNumberOfFaces(void) const;
    size_t getNumberOfDefinedFaces(void) const;
    void update_topology(void);
    virtual std::set<const Face *> getSurfaces(void);
    const BodyFace *getFace(const size_t &i) const;
    Face *newFace(const size_t &i,Pnt *,Pnt *,Pnt *,Pnt *);
    CmbEdge::Side *getEdge(const size_t &i);
    const CmbEdge::Side *getEdge(const size_t &i) const;
    Pnt *getVertex(const size_t &i);
    const Pnt *getVertex(const size_t &i) const;
    int getVtkCellType(void) const;

    void addPoints(const ID &);
    void setPoints(const ID &);
    
    size_t NDivI(void) const;
    void setNDivI(const size_t &nDiv);
    size_t NDivJ(void) const;
    void setNDivJ(const size_t &nDiv);
    size_t NDivK(void) const;
    void setNDivK(const size_t &nDiv);
    bool checkNDivs(void) const;
    void create_nodes(void);
    void genMesh(meshing_dir dm);
  };
} //end of XC namespace

#endif

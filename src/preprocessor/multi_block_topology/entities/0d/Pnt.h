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
//Pnt.h
//Point entities (similar a los K points de ANSYS)

#ifndef PNT_H
#define PNT_H

#include "preprocessor/multi_block_topology/entities/EntMdlr.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {
class Edge;
class Face;
class Body;
class TrfGeom;

//! @ingroup MultiBlockTopology
//!
//! @defgroup MultiBlockTopologyEnt Geometric entities.
//
//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Point (KPoint).
class Pnt: public EntMdlr
  {
    friend class Edge;
  private:
    Pos3d p; //!< Position of the point.
    mutable std::set<const Edge *> lines_pt; //!< Lines that begin or end in this point (topology).
  protected:

    virtual void update_topology(void);
    void create_nodes(void);
  public:
    Pnt(Preprocessor *m,const Pos3d &pto= Pos3d());
    Pnt(const std::string &name= "",Preprocessor *m= nullptr,const Pos3d &pto= Pos3d());
    virtual bool operator==(const Pnt &) const;
    SetEstruct *getCopy(void) const;
    //! @brief Return the object dimension (0, 1, 2 or 3).
    inline virtual unsigned short int GetDimension(void) const
      { return 0; }
    BND3d Bnd(void) const;
    bool hasNode(void) const;
    int getTagNode(void) const;
    Node *getNode(void);
    const Node *getNode(void) const;
    virtual void genMesh(meshing_dir dm);

    //! @brief Returns the object position.
    const Pos3d &GetPos(void) const
      { return p; }
    //! @brief Returns the object position.
    Pos3d &Pos(void)
      { return p; }
    //! @brief Sets the object position.
    void setPos(const Pos3d &pos)
      { p= pos; }
    bool In(const GeomObj3d &, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &, const double &tol= 0.0) const;

    Vector3d VectorPos(void) const;

    void insert_line(Edge *l) const;
    void erase_line(Edge *l) const;

    //! @brief Return the list of the lines that begin or end at the point.
    const std::set<const Edge *> &getConnectedEdges(void) const
      { return lines_pt; }
    boost::python::list getConnectedEdgesTags(void) const;

    //! @brief Return the number of connected edges.
    const size_t getNLines(void) const
      { return getConnectedEdges().size(); }
    std::set<const Edge *> getEdgesThatEndOnThisPoint(void) const;

    const std::string &getConnectedEdgesNames(void) const;
    bool isConnectedTo(const Edge &l) const;
    bool isEndPoint(const Edge &l) const;
    bool isConnectedTo(const Face &s) const;
    bool isConnectedTo(const Body &b) const;
    double getSquaredDistanceTo(const Pos3d &pt) const;

    const std::set<const Face *> getConnectedSurfaces(void) const;
    
    void Move(const Vector3d &);
    void Transform(const TrfGeom &trf);
    void Transform(const size_t &indice_trf);

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);


  };

Vector &operator-(const Pnt &b,const Pnt &a);
const Edge *find_connected_edge_const_ptr(const Pnt &,const Pnt &);
const Edge *find_connected_edge_const_ptr(const Pnt &,const Pnt &,const Pnt &);
Edge *find_connected_edge_ptr(const Pnt &,const Pnt &);
Edge *find_connected_edge_ptr(const Pnt &,const Pnt &,const Pnt &);

Edge *find_edge_ptr_by_endpoints(const Pnt &pA,const Pnt &pB);
Edge *find_edge_ptr_by_endpoints(const Pnt &,const Pnt &,const Pnt &);
const Edge *find_edge_const_ptr_by_endpoints(const Pnt &pA,const Pnt &pB);
const Edge *find_edge_const_ptr_by_endpoints(const Pnt &,const Pnt &,const Pnt &);

Face *find_face_ptr_by_vertices(const Pnt &pA,const Pnt &pB,const Pnt &pC,const Pnt &pD);
const Face *find_face_const_ptr_by_vertices(const Pnt &pA,const Pnt &pB,const Pnt &pC,const Pnt &pD);

} //end of XC namespace

#endif

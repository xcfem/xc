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
//Edge.h

#ifndef EDGE_H
#define EDGE_H

#include "EntMdlr.h"


class BND3d;
class MatrizPos3d;

namespace XC {
class Pnt;
class Face;
class Body;

//! \ingroup MultiBlockTopologyEnt
//!
//! @brief Base class for one-dimensional geometry objects.
class Edge: public EntMdlr
  {
    friend class CmbEdge;
    friend class Face;
  private:
    size_t ndiv; //!< number of divisions
    std::set<const Face *> surfaces_line; //!< Surface neighbors (topology).
  protected:

    void inserta_surf(Face *s);

    virtual Pnt *P1(void);
    virtual Pnt *P2(void);
    void create_nodes_en_extremos(void);
  public:
    Edge(Preprocessor *m,const size_t &nd= 4);
    Edge(const std::string &nombre= "",Preprocessor *m= nullptr,const size_t &nd= 4);
    //! @brief Return the object dimension (0, 1, 2 or 3).
    inline virtual unsigned short int GetDimension(void) const
      { return 1; }

    virtual const Pnt *P1(void) const;
    virtual const Pnt *P2(void) const;
    bool In(const GeomObj3d &, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &, const double &tol= 0.0) const;
    bool ExtremosEn(const Pnt *,const Pnt *) const;
    //! @brief Return the number of vertices.
    virtual size_t getNumberOfVertices(void) const= 0;

    virtual double getLongitud(void) const= 0;
    virtual Pos3d getCentroid(void) const= 0;

    virtual const Pnt *getVertex(const size_t &i) const= 0;
    virtual void SetVertice(const size_t &,Pnt *)= 0;
    std::vector<int> getIndicesVertices(void) const;
    virtual ID getKPoints(void) const;

    virtual void SetNDiv(const size_t &);
    inline virtual size_t NDiv(void) const
      { return ndiv; }
    void SetElemSize(const double &sz);
    std::set<const XC::Edge *> getHomologousSides(const std::set<const XC::Edge *> &) const;
    void actualiza_topologia(void);

    //! @brief Return the surfaces that touch the line.
    const std::set<const Face *> &SupsTocan(void) const
      { return surfaces_line; }
    //! @brief Return the surface names that touch the line.
    const std::string &NombresSupsTocan(void) const;
    bool Toca(const Face &s) const;
    bool Toca(const Body &b) const;
    bool Extremo(const Pnt &) const;
    virtual BND3d Bnd(void) const= 0;
    virtual void create_nodes(void);
    virtual void genMesh(meshing_dir dm);

    virtual MatrizPos3d get_posiciones(void) const= 0;
    virtual MatrizPos3d get_pos_nodes(void) const;

    virtual Node *getNode(const size_t &i1,const size_t &j,const size_t &k=1);
    virtual const Node *getNode(const size_t &i,const size_t &j,const size_t &k=1) const;
    virtual Node *getNode(const size_t &i);
    virtual const Node *getNode(const size_t &i) const;
    Node *getNodeDir(const size_t &i);
    const Node *getNodeDir(const size_t &i) const;
    Node *getNodeInv(const size_t &i);
    const Node *getNodeInv(const size_t &i) const;
    Node *getFirstNode(void);
    const Node *getFirstNode(void) const;
    Node *getLastNode(void);
    const Node *getLastNode(void) const;

    std::vector<int> getTagsNodesDir(void) const;
    std::vector<int> getTagsNodesInv(void) const;
    MatrizPos3d getNodePosDir(void) const;
    MatrizPos3d getNodePosInv(void) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual const Vector &getTang(const double &) const;
    void divide(void);

  };

std::set<const Edge *> getLinesThatTouch(const Pnt &p);
size_t calcula_ndiv_lados(const std::set<const XC::Edge *> &);

} //end of XC namespace
#endif

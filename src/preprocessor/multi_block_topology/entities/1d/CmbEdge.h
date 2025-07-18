// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//CmbEdge.h
//Compound line.

#ifndef CMBEDGE_H
#define CMBEDGE_H

#include <deque>
#include "preprocessor/multi_block_topology/entities/1d/Edge.h"

class Polyline3d;

namespace XC {

class LineMap;
  
//! @brief Compound line.
//! @ingroup MultiBlockTopologyEnt
class CmbEdge: public Edge
  {
  public:
    //! @ingroup Geom
    //!
    //! @brief Component of a compound line.
    //!
    //! A compound line is a sequence of pointers to lines (Line) (we
    //! call them lados here), such that the end of one of them is the origin
    //! of the edge that follows. Since the orientation of the line defined
    //! in the model, can not coincide with the orientation needed here, each
    //! pointer to line is accompanied by a boolean that indicates if the 
    //! line must be taken with its reversed direction or not.
    class Side: public CommandEntity
      {
        Edge *edge; //!< Pointer to line.
        bool forward; //!< true: forward (P1->P2), false: reversed (P2->P1)
      public:
        Side(Edge *ptr= nullptr,const bool &s=true);
        bool equal(const Side &) const;
        Edge *getEdge(void);
        const Edge *getEdge(void) const;
        void SetEdge(Edge *l);
        const Pnt *P1(void) const;
        const Pnt *P2(void) const;
        Pnt *P1(void);
        Pnt *P2(void);
        bool areEndPoints(const Pnt *,const Pnt *) const;
        const std::string &getName(void) const;
        inline bool isDirect(void) const
          { return forward; }
        inline void reverse(void)
	  { forward= !forward; }
        size_t getTag(void) const;

	// Number of divisions.
        void setNDivHomologousEdges(const size_t &);
        void setNDiv(const size_t &nd);
        size_t NDiv(void) const;
        double getElemSize(void) const;
	

	// Node positions.
        Pos3dArray get_positions(void) const;
        std::vector<Node *> getNodesForward(void);
        std::vector<Node *> getNodesReverse(void);
        std::vector<const Node *> getNodesForward(void) const;
        std::vector<const Node *> getNodesReverse(void) const;
        std::vector<int> getTagsNodesForward(void) const;
        std::vector<int> getTagsNodesReverse(void) const;
        Pos3dArray getNodePosForward(void) const;
        Pos3dArray getNodePosReverse(void) const;

	// Geometry
        double getLength(void) const;
        Pos3d getCentroid(void) const;	
        bool In(const GeomObj3d &, const double &tol= 0.0) const;
        bool Out(const GeomObj3d &, const double &tol= 0.0) const;
        const Vector &getTang(const double &) const;
        double getDist(const Pos3d &) const;

	// Mesh
        Node *getNodeForward(const size_t &i);
        Node *getNodeReverse(const size_t &i);
        Node *getNode(const size_t &i);
        const Node *getNode(const size_t &i) const;
        void genMesh(meshing_dir dm);
        friend bool operator==(const Side &il1,const Side &il2);
    
        boost::python::dict getPyDict(void) const;
        void setPyDict(LineMap &, const boost::python::dict &);
      };
  protected:
    std::deque<Side> lines; //!< Lines that compose the object.
    Pos3dArray get_positions(void) const;

    Side *first_line(void);
    const Side *first_line(void) const;
    Side *last_line(void);
    const Side *last_line(void) const;


    const Pnt *first_point(void) const;
    const Pnt *last_point(void) const;

    virtual void create_line_nodes(void);
    void line_meshing(meshing_dir dm);
    void close(void);
    void revolve(const int &);
  public:
    CmbEdge(void);
    CmbEdge(Preprocessor *m,const size_t &nd= 4);
    CmbEdge(const std::string &name,Preprocessor *m,const size_t &nd);
    virtual bool operator==(const CmbEdge &) const;
    virtual SetEstruct *getCopy(void) const;
    virtual void insert(Edge *l);
    void insert(const size_t &i);
    Edge *newLine(Pnt *,Pnt *);
    Edge *newLine(Pnt *,Pnt *,Pnt *);
    void addPoints(const ID &);
    void addLines(const ID &);

    // Number of divisions.
    size_t NDiv(void) const;
    void setNDiv(const size_t &nd);
    double getAvgElemSize(void) const;
    double getMaxElemSize(void) const;
    double getMinElemSize(void) const;

    //! @brief Return the number of edges.
    size_t getNumberOfEdges(void) const
      { return lines.size(); }
    inline std::deque<Side> &getSides(void)
      { return lines; }
    inline const std::deque<Side> &getSides(void) const
      { return lines; }
    CmbEdge getReversed(void) const;
    virtual void reverse(void);
    bool In(const GeomObj3d &, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &, const double &tol= 0.0) const;

    double getLength(void) const;
    Pos3d getCentroid(void) const;	
    virtual double getDist(const Pos3d &) const;

    //! @brief Return the number of vertices.
    virtual size_t getNumberOfVertices(void) const
      { return getNumberOfEdges()+1; }

    Pnt *P1(void);
    Pnt *P2(void);
    const Pnt *P1(void) const;
    const Pnt *P2(void) const;

    bool hasEdge(const Edge *) const;
    size_t IndiceEdge(const Edge *) const;
    const Side *getSide(const size_t &i) const;
    Side *getSide(const size_t &i);
    const Side *getSideByPoints(const Pnt *,const Pnt *) const;
    Side *getSideByPoints(const Pnt *,const Pnt *);
    const Side *getSideByPoints(const size_t &,const size_t &) const;
    Side *getSideByPoints(const size_t &,const size_t &);
    std::deque<Edge *> getEdges(void);
    std::deque<const Edge *> getEdges(void) const;
    boost::python::list getEdgesPy(void) const;
    std::deque<Side *> findSides(const Pos3d &);
    
    virtual const Pnt *getVertex(const size_t &i) const;
    virtual Pnt *getVertex(const size_t &i);
    std::deque<Pnt *> getVertices(void);
    std::deque<const Pnt *> getVertices(void) const;
    std::set<const Pnt *> getVertexSet(void) const;
    virtual void SetVertice(const size_t &,Pnt *);
    virtual ID getKPoints(void) const;
    Pnt *findVertex(const Pos3d &);

    Polyline3d getPolyline(void) const;

    bool isConnectedTo(const Pnt &) const;
    bool isConnectedTo(const CmbEdge &) const;
    std::set<const Face *> getConnectedSurfaces(void) const;
    boost::python::list getConnectedSurfacesPy(void) const;
    std::set<const Face *> getConnectedSurfaces(const SetBase *) const;
    boost::python::list getConnectedSurfacesPy(const SetBase *) const;    

    
    BND3d Bnd(void) const;
    void genMesh(meshing_dir dm);

    friend bool operator==(const Side &il1,const Side &il2);
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
 };

//! @brief Comparison operator.
inline bool operator==(const CmbEdge::Side &il1,const CmbEdge::Side &il2)
  { return il1.equal(il2); }

} //end of XC namespace

#endif

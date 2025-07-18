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
//NodePtrs.h
                                                                        
#ifndef NodePtrs_h
#define NodePtrs_h

#include "utility/kernel/CommandEntity.h"
#include "domain/mesh/node/Node.h"
#include <list>
#include <vector>

class Pos3d;
class HalfSpace3d;
class Plane;
class GeomObj2d;
class GeomObj3d;
class BND3d;


namespace XC {
class Element;
class Domain;
class ID;
class Matrix;
class Vector;
class SetBase;

//! @ingroup Elem
//!
//! @defgroup FEMisc Utilities for finite element calculations.
//
//! @ingroup FEMisc
//
//! @brief Element pointers to nodes.
class NodePtrs: public CommandEntity, public std::vector<Node *>
  {
  public:
    typedef std::vector<Node *> vector_ptr_nodes;
    typedef vector_ptr_nodes::const_reference const_reference;
    typedef vector_ptr_nodes::reference reference;
    typedef vector_ptr_nodes::iterator iterator;
    typedef vector_ptr_nodes::const_iterator const_iterator;
    typedef Node::ElementPtrSet ElementPtrSet; //!< Container of element pointers.
    typedef Node::ElementConstPtrSet ElementConstPtrSet; //!< Container of const element pointers.

  protected:
    void disconnect(void);
    void setPtrs(Domain *theDomain, const ID &theNodeTags);

  public:
    NodePtrs(Element *owr,const size_t &sz);
    NodePtrs(const NodePtrs &);
    NodePtrs &operator=(const NodePtrs &);
    ~NodePtrs(void);

    void inic(void);

    iterator find(const int &);
    const_iterator find(const int &) const;
    int find(const Node *) const;

    Node *getNodePtr(const size_t &);
    const_reference operator()(const size_t &i) const;
    void set_node(const size_t &i,Node *);

    bool hasNull(void) const;

    ID getNumDOFs(void) const;
    int getTotalDOFs(void) const;
    Vector getDistributionFactor(int mode) const;
    Matrix getNodeVectors(const Vector &) const;
    double MaxCooNod(int i) const;
    double MinCooNod(int i) const;
    std::vector<int> getIdx(void) const;
    bool checkDimension(const size_t &dim) const;
    size_t getDimension(void) const;
    BND3d Bnd(const double &) const;    
    const Matrix &getCoordinates(void) const;
    Pos3d getPosNode(const size_t &i,bool initialGeometry= true) const;
    std::deque<Pos3d> getPositions(bool initialGeometry= true) const;
    Pos3d getCenterOfMassPosition(bool initialGeometry= true) const;
    Node *getNearestNode(const Pos3d &p,bool initialGeometry= true);
    const Node *getNearestNode(const Pos3d &p,bool initialGeometry= true) const;
    bool In(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool In(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Out(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Corta(const Plane &,bool initialGeometry= true) const;
    bool Crosses(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Crosses(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;

    void resetTributaries(void) const;
    void dumpTributaries(const std::vector<double> &) const;

    const std::vector<int> &getTags(void) const;
    int getNodeIndex(const Node *) const;
    int getNodeIndex(const int &tag) const;

    void reverse(void);

    ElementConstPtrSet getConnectedElements(void) const;
    ElementPtrSet getConnectedElements(void);
    boost::python::list getConnectedElementsPy(void);    
    boost::python::list getConnectedElementTags(void) const;
    
    ElementConstPtrSet getConnectedElements(const SetBase *) const;
    ElementPtrSet getConnectedElements(const SetBase *);
    boost::python::list getConnectedElementsPy(const SetBase *);    
    boost::python::list getConnectedElementTags(const SetBase *) const;   
  };
} // end of XC namespace

#endif


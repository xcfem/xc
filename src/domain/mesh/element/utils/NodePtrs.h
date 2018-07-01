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
//NodePtrs.h
                                                                        
#ifndef NodePtrs_h
#define NodePtrs_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include <list>

class Pos3d;
class SemiEspacio3d;
class Plane;
class GeomObj2d;
class GeomObj3d;


namespace XC {
class Element;
class Node;
class Domain;
class ID;
class Matrix;
class Vector; 

//! @ingroup Elem
//!
//! @defgroup FEMisc Utilities for finite element calculations.
//
//! @ingroup FEMisc
//
//! @brief Pointers to nodes.
class NodePtrs: public EntCmd, public std::vector<Node *>
  {
  public:
    typedef std::vector<Node *> vector_ptr_nodes;
    typedef vector_ptr_nodes::const_reference const_reference;
    typedef vector_ptr_nodes::reference reference;
    typedef vector_ptr_nodes::iterator iterator;
    typedef vector_ptr_nodes::const_iterator const_iterator;

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
    const Matrix &getCoordinates(void) const;
    Pos3d getPosNode(const size_t &i,bool initialGeometry= true) const;
    std::list<Pos3d> getPositions(bool initialGeometry= true) const;
    Pos3d getCenterOfMassPosition(bool initialGeometry= true) const;
    Node *getNearestNode(const Pos3d &p,bool initialGeometry= true);
    const Node *getNearestNode(const Pos3d &p,bool initialGeometry= true) const;
    bool In(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool In(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Out(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Corta(const Plane &,bool initialGeometry= true) const;

    void resetTributaries(void) const;
    void dumpTributaries(const std::vector<double> &) const;

    const std::vector<int> &getTags(void) const;
    int getNodeIndex(const Node *) const;
  };
} // end of XC namespace

#endif


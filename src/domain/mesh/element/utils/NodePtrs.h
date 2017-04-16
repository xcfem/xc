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
class Plano3d;

namespace XC {
class Element;
class Node;
class Domain;
class ID;
class Matrix;
class Vector; 

//! @ingroup Elem
//
//! @defgroup FEMisc Utilities for finite element calculations.
//
//! @ingroup FEMisc
//
//! @brief Pointers to nodes.
class NodePtrs: public EntCmd
  {
  public:
    typedef std::vector<Node *> vector_ptr_nodes;
    typedef vector_ptr_nodes::const_reference const_reference;
    typedef vector_ptr_nodes::reference reference;
    typedef vector_ptr_nodes::iterator iterator;
    typedef vector_ptr_nodes::const_iterator const_iterator;

  private:
    vector_ptr_nodes theNodes; //!< node set.
  protected:
    void disconnect(void);
    void setPtrs(Domain *theDomain, const ID &theNodeTags);

  public:
    NodePtrs(Element *owr,const size_t &sz);
    NodePtrs(const NodePtrs &otro);
    NodePtrs &operator=(const NodePtrs &otro);
    ~NodePtrs(void);

    void inic(void);

    inline size_t size(void) const
      { return theNodes.size(); }
    inline const_reference front() const
      { return theNodes.front(); }
    inline reference front()
      { return theNodes.front(); }
    inline const_reference back() const
      { return theNodes.back(); }
    inline reference back()
      { return theNodes.back(); }
    inline const_iterator begin() const
      { return theNodes.begin(); }
    inline iterator begin()
      { return theNodes.begin(); }
    inline const_iterator end() const
      { return theNodes.end(); }
    inline iterator end()
      { return theNodes.end(); }
    iterator find(const int &);
    const_iterator find(const int &) const;
    int find(const Node *) const;

    Node *getNodePtr(const size_t &);
    const_reference operator()(const size_t &i) const;
    const_reference operator[](const size_t &i) const;
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
    Pos3d getPosNodo(const size_t &i,bool initialGeometry= true) const;
    std::list<Pos3d> getPosiciones(bool initialGeometry= true) const;
    Pos3d getPosCdg(bool initialGeometry= true) const;
    Node *getNearestNode(const Pos3d &p,bool initialGeometry= true);
    const Node *getNearestNode(const Pos3d &p,bool initialGeometry= true) const;
    bool In(const SemiEspacio3d &semiEsp,const double &tol= 0.0,bool initialGeometry= true) const;
    bool Out(const SemiEspacio3d &semiEsp,const double &tol= 0.0,bool initialGeometry= true) const;
    bool Corta(const Plano3d &,bool initialGeometry= true) const;

    void resetTributarias(void) const;
    void vuelcaTributarias(const std::vector<double> &) const;

    const std::vector<int> &getTags(void) const;
    int getIndiceNodo(const Node *) const;


  };
} // end of XC namespace

#endif


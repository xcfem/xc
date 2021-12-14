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
//ClosedTriangleMesh.h

#ifndef CLOSEDTRIANGLEMESH_H
#define CLOSEDTRIANGLEMESH_H

#include "utility/geom/d2/Trihedron.h"
#include <set>
#include <deque>
#include "utility/actor/actor/MovableObject.h"

class Triang3dMesh;
class TriangleMap;

namespace XC {

class Vector;
class FiberSectionBase;

//! @ingroup MATSCCDiagInt
//
//! @brief Closed triangle mesh used to represent
//! the interaction diagram (N,Mx,My) of a section.
class ClosedTriangleMesh: public GeomObj3d, public MovableObject
  {
  protected:
    typedef std::vector<Trihedron> v_trihedrons;
    typedef v_trihedrons::iterator iterator;
    typedef v_trihedrons::const_iterator const_iterator;
    
    v_trihedrons trihedrons;
    double tol;
    double rMax; //! Radius of the convex-hull circunscribed sphere.
    double rMin; //! Radius of the sphere that passes through the nearest vertex.

    GeomObj::list_Pos3d get_intersection(const Pos3d &p) const;
  protected:
    void getPositionsMatrix(Matrix &);
    virtual void setPositionsMatrix(const Matrix &);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    ClosedTriangleMesh(void);
    ClosedTriangleMesh(const Pos3d &org,const Triang3dMesh &mll);
    ClosedTriangleMesh(const ClosedTriangleMesh &);
    ClosedTriangleMesh &operator=(const ClosedTriangleMesh &);
    virtual ClosedTriangleMesh *getCopy(void) const;

    virtual double GetMax(short unsigned int i) const;
    virtual double GetMin(short unsigned int i) const;
    virtual Pos3d getCenterOfMass(void) const;
    virtual double getLength(void) const;
    virtual double getArea(void) const;
    virtual double getVolume(void) const;
    virtual double Ix(void) const; 
    virtual double Iy(void) const;
    virtual double Iz(void) const;
    short unsigned int Dimension() const
      { return 3; }

    size_t GetNumFacetas(void) const;

    TriangleMap getTriangleMap(void) const;

    iterator begin(void);
    iterator end(void);
    const_iterator begin() const;
    const_iterator end() const;
    size_t size(void) const;
    
    const_iterator findTrihedron(const Pos3d &p) const;
    const Trihedron *findTrihedronPtr(const Pos3d &p) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void write(std::ofstream &);
    void read(std::ifstream &);
    void writeTo(const std::string &);
    void readFrom(const std::string &);
    void Print(std::ostream &os) const;
  };

} // end of XC namespace

#endif

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
//ClosedTriangleMesh.h

#ifndef CLOSEDTRIANGLEMESH_H
#define CLOSEDTRIANGLEMESH_H

#include "xc_utils/src/geom/d2/Triedro3d.h"
#include <set>
#include <deque>
#include "utility/actor/actor/MovableObject.h"

class MallaTriang3d;
class TriangleMap;

namespace XC {

class Vector;
class FiberSectionBase;

//! \@ingroup MATSCCDiagInt
//
//! @brief Diagrama de interacción (N,Mx,My) de una sección.
class ClosedTriangleMesh: public GeomObj3d, public MovableObject
  {
  protected:
    typedef std::vector<Triedro3d> v_triedros;
    typedef v_triedros::iterator iterator;
    typedef v_triedros::const_iterator const_iterator;
    
    v_triedros triedros;
    double tol;
    double rMax; //! Radio de una esfera circunscrita al convex-hull.
    double rMin; //! Radio de una esfera que toca al vértice más cercano.

    GeomObj::list_Pos3d get_interseccion(const Pos3d &p) const;
  protected:
    void getMatrizPosiciones(Matrix &);
    virtual void setMatrizPosiciones(const Matrix &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ClosedTriangleMesh(void);
    ClosedTriangleMesh(const Pos3d &org,const MallaTriang3d &mll);
    ClosedTriangleMesh(const ClosedTriangleMesh &otro);
    ClosedTriangleMesh &operator=(const ClosedTriangleMesh &otro);
    virtual ClosedTriangleMesh *clon(void) const;

    virtual double GetMax(short unsigned int i) const;
    virtual double GetMin(short unsigned int i) const;
    virtual Pos3d Cdg(void) const;
    virtual double Longitud(void) const;
    virtual double Area(void) const;
    virtual double Volumen(void) const;
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
    
    const_iterator BuscaTriedro(const Pos3d &p) const;
    const Triedro3d *BuscaPtrTriedro(const Pos3d &p) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void write(std::ofstream &);
    void read(std::ifstream &);
    void writeTo(const std::string &);
    void readFrom(const std::string &);
    void Print(std::ostream &os) const;
  };

} // end of XC namespace

#endif

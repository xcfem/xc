// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Planepolyline3d.h

#ifndef PLANEPOLYLINE3D_H
#define PLANEPOLYLINE3D_H

#include "utility/geom/d2/D2to3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d1/Polyline2d.h"
#include "../cgal_types.h"
#include "utility/geom/d2/Plane.h"

class VectorPos3d;

//! @ingroup GEOM
//
//! @brief Plane polygon in a 3D space.
class PlanePolyline3d: public D2to3d
  {
  public:
    typedef Polyline2d::iterator iterator;
    typedef Polyline2d::const_iterator const_iterator;    
  protected:
    Polyline2d pline2d;
  public:
    PlanePolyline3d(void): D2to3d(),pline2d() {}
    template <typename InputIterator>
    explicit PlanePolyline3d(InputIterator first,InputIterator last);
    PlanePolyline3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3);
    PlanePolyline3d(const Ref2d3d &,const Polyline2d &p= Polyline2d());
    PlanePolyline3d(const Ref3d3d &,const Polyline2d &p= Polyline2d());
    explicit PlanePolyline3d(const GeomObj::list_Pos3d &);
    explicit PlanePolyline3d(const boost::python::list &);

    virtual GeomObj *getCopy(void) const
      { return new PlanePolyline3d(*this); }

    inline void push_back(const Pos3d &p)
      { pline2d.push_back(to_2d(p)); }
    
    Pos3d getFromPoint(void) const
      { return to_3d(this->pline2d.getFromPoint()); }
    
    Pos3d getToPoint(void) const
      { return to_3d(this->pline2d.getToPoint()); }

    bool empty(void) const;
    GEOM_FT GetXMax(void) const;
    GEOM_FT GetYMax(void) const;
    GEOM_FT GetZMax(void) const;
    GEOM_FT GetXMin(void) const;
    GEOM_FT GetYMin(void) const;
    GEOM_FT GetZMin(void) const;

    inline virtual unsigned int getNumVertices(void) const
      { return pline2d.getNumVertices(); }
    inline virtual unsigned int getNumSegments(void) const
      { return pline2d.getNumSegments(); }
    //! @brief Return the length of the object.
    inline GEOM_FT getLength(void) const
      { return pline2d.getLength(); }
    bool isClosed(const GEOM_FT &tol= 1e-6) const;
    const_iterator getSegmentAtLength(const GEOM_FT &s) const;
    int getIndexOfSegmentAtLength(const GEOM_FT &s) const;
    int getIndexOfSegmentAtParam(const GEOM_FT &lambda) const;
    const Pos3d appendVertex(const Pos3d &);
    const Pos3d appendVertexLeft(const Pos3d &);
    void insertVertex(const Pos3d &, const GEOM_FT &tol= 1e-6);
    
    //! @brief Return the position of the i-th vertex.
    inline Pos3d Vertice(unsigned int i) const
      { return to_3d(pline2d.Vertice(i)); }
    //! @brief Return the position of the i-th vertex
    //! (0 based: j=0..getNumVertices()-1)
    inline Pos3d Vertice0(unsigned int j) const
      { return to_3d(pline2d.Vertice0(j)); }
    GeomObj::list_Pos3d getVertexList(void) const;
    boost::python::list getVertexListPy(void) const;
    Segment3d getSegment0(unsigned int i) const;
    Segment3d getSegment(unsigned int i) const;
    
    Pos3d getPointAtLength(const GEOM_FT &) const;
    Vector3d getIVectorAtLength(const GEOM_FT &) const;
    Vector3d getJVectorAtLength(const GEOM_FT &) const;
    Vector3d getKVectorAtLength(const GEOM_FT &) const;

    VectorPos3d Divide(int num_partes) const;
    boost::python::list DividePy(int num_partes) const;
    VectorPos3d Divide(const std::vector<double> &) const;
    boost::python::list DividePy(const boost::python::list &) const;
    
    void swap(void);

    Plane getPlaneFromSide0(unsigned int i) const;
    Plane getPlaneFromSide(unsigned int i) const;

    Pos3d getCenterOfMass(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Iz(void) const;

    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    bool In(const Pos3d &p,const double &tol) const;
    bool TocaCuadrante(const int &) const;

    // GEOM_FT distSigno(const Pos3d &p) const;
    // GEOM_FT dist(const Pos3d &p) const;
    // GEOM_FT distSigno2(const Pos3d &p) const;
    // GEOM_FT dist2(const Pos3d &p) const;
    
    bool intersects(const Line3d &) const;
    GeomObj3d::list_Pos3d getIntersection(const Line3d &) const;
    bool intersects(const Plane &) const;
    GeomObj3d::list_Pos3d getIntersection(const Plane &) const;
    
    void simplify(GEOM_FT epsilon, iterator it1, iterator it2);
    void simplify(GEOM_FT epsilon);
    PlanePolyline3d getChunk(const Pos3d &,const short int &, const GEOM_FT &) const;
    PlanePolyline3d getLeftChunk(const Pos3d &, const GEOM_FT &tol) const;
    PlanePolyline3d getRightChunk(const Pos3d &, const GEOM_FT &tol) const;
    boost::python::list split(const Pos3d &) const;

    void Print(std::ostream &os) const;
  };

template <typename InputIterator>
PlanePolyline3d::PlanePolyline3d(InputIterator begin,InputIterator end)
  : D2to3d(), pline2d()
  {
    Plane p(begin, end);
    auto i= begin;
    const Pos3d A= *i; i++;
    const Pos3d B= *i; i++;
    const Pos3d C= *i;
    const Ref2d3d ref= Ref2d3d(*begin,p.Base1(), p.Base2());
    PlanePolyline3d tmp(ref);
    for(InputIterator i= begin; i!= end; i++)
      tmp.push_back(*i);
    (*this)= tmp;

  }

#endif

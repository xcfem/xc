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
//Quadrilateral3d.h

#ifndef QUADRILATERAL3D_H
#define QUADRILATERAL3D_H

#include "utility/geom/d2/D2to3d.h"
#include "utility/geom/d2/2d_polygons/Quadrilateral2d.h"

class Polygon3d;
class Pos3dArray;

//! @ingroup GEOM
//
//! @brief Plane quadrilateral in a 3D space.
class Quadrilateral3d: public D2to3d
  {
    Quadrilateral2d quad2d;
  public:
    Quadrilateral3d(void): D2to3d(), quad2d() {}
    Quadrilateral3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3, const Pos3d &p4);
    Quadrilateral3d(const Ref2d3d &,const Quadrilateral2d &p= Quadrilateral2d());
    Quadrilateral3d(const Ref3d3d &,const Quadrilateral2d &p= Quadrilateral2d());
    Quadrilateral3d(const GeomObj::list_Pos3d &);
    Quadrilateral3d(const boost::python::list &);
    virtual GeomObj *getCopy(void) const
      { return new Quadrilateral3d(*this); }

    Polygon3d getPolygon3d(void) const;

    inline virtual unsigned int getNumVertices(void) const
      { return quad2d.getNumVertices(); }
    inline virtual unsigned int getNumEdges(void) const
      { return quad2d.getNumEdges(); }
    //! @brief Return the length of the object.
    inline GEOM_FT getLength(void) const
      { return quad2d.getLength(); }
    //! @brief Return the area of the object.
    inline GEOM_FT getArea(void) const
      { return quad2d.getArea(); }
    inline std::vector<double> getTributaryAreas(void) const
      { return quad2d.getTributaryAreas(); }
    //! @brief Return the position of the i-th vertex.
    inline Pos3d Vertice(unsigned int i) const
      { return to_3d(quad2d.Vertice(i)); }
    //! @brief Return the position of the i-th vertex
    //! (0 based: j=0..getNumVertices()-1)
    inline Pos3d Vertice0(unsigned int j) const
      { return to_3d(quad2d.Vertice0(j)); }
    GeomObj::list_Pos3d getVertexList(void) const;
    boost::python::list getVertexListPy(void) const;
    Segment3d Lado0(unsigned int i) const;
    Segment3d Lado(unsigned int i) const;
    int getIndexOfDistalEdge(const Pos3d &) const;
    int getIndexOfProximalEdge(const Pos3d &) const;
    int getIndexOfDistalVertex(const Pos3d &) const;
    int getIndexOfProximalVertex(const Pos3d &) const;

    bool clockwise(const Pos3d &) const;
    bool counterclockwise(const Pos3d &) const;
    void swap(void);
    std::string orientation(const Pos3d &) const;

    Plane getPlaneFromSide0(unsigned int i) const;
    Plane getPlaneFromSide(unsigned int i) const;

    Vector3d getNormal(void) const;
    
    Pos3d getCenterOfMass(void) const;
    Pos3d Centroid(void) const;
    GEOM_FT Ix(void) const;
    GEOM_FT Iy(void) const;
    GEOM_FT Iz(void) const;

    GEOM_FT GetMax(unsigned short int i) const;
    GEOM_FT GetMin(unsigned short int i) const;
    bool In(const Pos3d &p,const double &tol) const;
    bool TocaCuadrante(const int &) const;

    GEOM_FT distSigno(const Pos3d &p) const;
    GEOM_FT dist(const Pos3d &p) const;
    GEOM_FT distSigno2(const Pos3d &p) const;
    GEOM_FT dist2(const Pos3d &p) const;
    
    
    std::vector<double> getNaturalCoordinates(const Pos3d &) const;
    std::vector<std::vector<double> > getNaturalCoordinates(const std::list<Pos3d> &) const;
    boost::python::list getNaturalCoordinatesPy(const boost::python::list &) const;
    std::vector<double> Ni(const Pos3d &) const;
    std::vector<std::vector<double> > Ni(const std::list<Pos3d> &) const;
    boost::python::list NiPy(const Pos3d &) const;
    boost::python::list NiPy(const boost::python::list &) const;
    Pos3d getMidpoint(void) const;
    
    Segment3d Clip(const Line3d &) const;
    Segment3d Clip(const Ray3d &) const;
    Segment3d Clip(const Segment3d &) const;

    bool intersects(const Line3d &) const;
    Segment3d getIntersection(const Line3d &) const;
    bool intersects(const Plane &) const;
    Segment3d getIntersection(const Plane &) const;

    Pos3dArray genMesh(int n1,int n2) const;
    Pos3dArray genBilinMesh(const size_t &n1,const size_t &n2) const;
    
    void Print(std::ostream &os) const;
  };

#endif

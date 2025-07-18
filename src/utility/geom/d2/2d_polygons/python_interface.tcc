//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//python_interface.cxx


Segment2d (PolygonalSurface2d::*getSide0Segment)(unsigned int i) const= &PolygonalSurface2d::Lado0;
GEOM_FT (PolygonalSurface2d::*getCoverA)(const Pos2d &) const= &PolygonalSurface2d::getCover;
GEOM_FT  (PolygonalSurface2d::*getCoverB)(const Pos2d &, const Vector2d &) const= &PolygonalSurface2d::getCover;
Segment2d (PolygonalSurface2d::*clipLine)(const Line2d &) const=&PolygonalSurface2d::Clip;
Segment2d (PolygonalSurface2d::*clipRay)(const Ray2d &) const=&PolygonalSurface2d::Clip;
Segment2d (PolygonalSurface2d::*clipSegment)(const Segment2d &) const=&PolygonalSurface2d::Clip;
class_<PolygonalSurface2d, bases<Surface2d>, boost::noncopyable >("PolygonalSurface2d", no_init)
.def("getPerimeter",&PolygonalSurface2d::getPerimeter, " Return the object perimeter.")
  .def("getLength",&PolygonalSurface2d::getLength, " Return the object perimeter.")
  .def("getCenterOfMass",&PolygonalSurface2d::getCenterOfMass)
  .def("getIx",&PolygonalSurface2d::Ix)
  .def("getIy",&PolygonalSurface2d::Iy)
  .def("getPxy",&PolygonalSurface2d::Pxy)
  .def("getVertice",&PolygonalSurface2d::Vertice0, "***DEPRECATED*** Return the i-th vertex.")
  .def("getLado",getSide0Segment, "***DEPRECATED*** Return the i-th edge.")
  .def("getVertex",&PolygonalSurface2d::Vertice0, "Return the i-th vertex.")
  .def("getEdge",getSide0Segment, "Return the i-th edge.")
  .def("getNormalLado",&PolygonalSurface2d::getLado0Normal)
  .def("getNormalVertice",&PolygonalSurface2d::getVertex0Normal)
  .def("getIndexOfDistalEdge", &PolygonalSurface2d::getIndexOfDistalEdge,"Return the index of the distal edge with respect to the point argument.")
  .def("getIndexOfProximalEdge", &PolygonalSurface2d::getIndexOfProximalEdge,"Return the index of the proximal edge with respect to the point argument.")
  .def("getIndexOfDistalVertex", &PolygonalSurface2d::getIndexOfDistalVertex,"Return the index of the distal vertex with respect to the point argument.")
  .def("getIndexOfProximalVertex", &PolygonalSurface2d::getIndexOfProximalVertex,"Return the index of the proximal vertex with respect to the point argument.")
  .def("getNearestEdge", &PolygonalSurface2d::getNearestEdge,"Return the closest edge to the given point.")
  .def("getProjection", &PolygonalSurface2d::Projection,"Return the projection of the given point onto the polygolal surface contour.")
  .def("getLengthUpTo", &PolygonalSurface2d::getLengthUpTo, "Return the length along the perimeter until the point p is reached.")
  .def("getLambda", &PolygonalSurface2d::getLambda, "Return the length along the perimeter until the point p is reached.")
  .def("getPolyline",&PolygonalSurface2d::getPolyline)
  .def("getVertices",&PolygonalSurface2d::getVertices,"Return the vertices list.")
  .def("getApproxTangentPositions",&PolygonalSurface2d::getApproxTangentPositions,"Return the points for which the polygon is almost tangent to the direction argument.")
  .def("getRecubrimiento",getCoverA,"TO DEPRECATE. Return the cover of the position inside the surface.")
  .def("getCover",getCoverA,"Return the cover of the position inside the surface.")
  .def("getCover",getCoverB,"Return the distance from the point to the nearest of the intersections of the ray defined by the point and the vector with the nearest edge.")
  .def("getArea",&PolygonalSurface2d::getArea,"Return the object area.")
  .def("clip",clipLine, "Clips the line by the polygonal surface.")
  .def("clip",clipRay, "Clips the ray by the polygonal surface.")
  .def("clip",clipSegment, "Clips the segment by the polygonal surface.")
  .def("dist", &PolygonalSurface2d::Dist, "Return the distance from the point to the polygon.")
  .def("signedDist", &PolygonalSurface2d::DistSigno, " return the signed distance fromt the point to the polygon (positive if the point is outside the polygon).")
  ;

typedef std::list<Polygon2d> polygon_2D_list;
class_<polygon_2D_list >("polygon_2D_list")
//.def(vector_indexing_suite<std::list<Polygon2d> >())
  .def("__iter__", iterator<polygon_2D_list >())
  .add_property("size", &polygon_2D_list::size)
  .def("__len__", &polygon_2D_list::size)
  .def("empty", &polygon_2D_list::empty)
  .def("append", static_cast<void (polygon_2D_list::*)(const Polygon2d&)>(&polygon_2D_list::push_back))
  ;

Segment2d (Polygon2d::*Plg2dIntersLine2d)(const Line2d &) const= &Polygon2d::getIntersection;
Segment2d (Polygon2d::*Plg2dIntersRay2d)(const Ray2d &) const= &Polygon2d::getIntersection;
Segment2d (Polygon2d::*Plg2dIntersSegment2d)(const Segment2d &) const= &Polygon2d::getIntersection;
void (Polygon2d::*unePolygon2d)(const Polygon2d &) =&Polygon2d::une;
boost::python::list (Polygon2d::*clipPolygon)(const Polygon2d &) const= &Polygon2d::ClipPy;
boost::python::list (Polygon2d::*clipHalfPlane2d)(const HalfPlane2d &) const= &Polygon2d::ClipPy;
boost::python::list (Polygon2d::*Plg2dIntersHalfPlane2d)(const HalfPlane2d &) const= &Polygon2d::getIntersectionPy;
boost::python::list (Polygon2d::*Plg2dIntersPlg2d)(const Polygon2d &) const= &Polygon2d::getIntersectionPy;
class_<Polygon2d, Polygon2d *, bases<PolygonalSurface2d> >("Polygon2d")
  .def(init<>())
//.def(init<GeomObj::list_Pos2d>()) # Apparently this doesn't works.
  .def(init<Polyline2d>())
  .def(init<Polygon2d>())
  .def(init<boost::python::list>())
  .def("getNumVertices",&Polygon2d::getNumVertices)
   //.def("insert",&Polygon2d::insert, "insert(vertex_iterator i,InputIterator first,InputIterator last) Insert the vertices between [first,last) before the vertex pointed by i.")
  .def("__getitem__",&Polygon2d::Vertice0,"return the i-th vertex.")
  .def("__iter__",range(&Polygon2d::vertices_begin, &Polygon2d::vertices_end))
  .def("offset",&Polygon2d::offset, "Return a polygon parallel to this one at the given distance. The new polygon will be exterior if the distance is positive.")
  .def("getBuffer",&Polygon2d::buffer,"Returns the buffer (a polygon being the spatial point set collection within a specified maximum distance from this polygon) of this polygon.")
  .def("getTributaryAreas",&Polygon2d::getTributaryAreas,"Return the tributary areas.")
  .def("appendVertex",&Polygon2d::push_back)
  .def("unePolygon2d",unePolygon2d,"Return the union of this polygon with the argument.")
  .def("clipUsingPolygon",&Polygon2d::clipBy)
  .def("getBayazitDecomposition",&Polygon2d::getBayazitDecomposition)
  .def("getVertexList",&Polygon2d::getVertexListPy,"Return a Python list containing the positions of the polygon vertices.")
  .def("getIntersection",Plg2dIntersLine2d, "return the intersection with the given line.")
  .def("getIntersection",Plg2dIntersRay2d, "return the intersection with the given ray.")
  .def("getIntersection",Plg2dIntersSegment2d, "return the intersection with the given segment.")
  .def("getIntersection",Plg2dIntersHalfPlane2d, "return the intersection with the given half plane.")
  .def("getIntersection",Plg2dIntersPlg2d, "return the intersection with the given polygon.")
  .def("clip",clipLine, "Clips the line.")
  .def("clip",clipRay, "Clips the ray.")
  .def("clip",clipSegment, "Clips the segment.")
  .def("clip",clipHalfPlane2d, "Return the given half plane clipped by this one.")
  .def("clip",clipPolygon, "Return the given polygon clipped by this one.")
  .def("swap", &Polygon2d::swap,"Reverse the orientation of the polygon.")
  ;

Polygon2d (*remove_duplicated_vertices_polygon_2d)(const Polygon2d &, const GEOM_FT &)= &remove_duplicated_vertices;
def("remove_duplicated_vertices",remove_duplicated_vertices_polygon_2d, "Remove the consecutive vertices that are at distance less than than tol.");

std::vector<double> (Quadrilateral2d::*get2DPointNaturalCoordinates)(const Pos2d &) const= &Quadrilateral2d::getNaturalCoordinates;
boost::python::list (Quadrilateral2d::*get2dPointNi)(const Pos2d &) const= &Quadrilateral2d::NiPy;
boost::python::list (Quadrilateral2d::*get2dPointsNi)(const boost::python::list &) const= &Quadrilateral2d::NiPy;
class_<Quadrilateral2d, bases<Polygon2d> >("Quadrilateral2d")
  .def(init<>())
  .def(init<Pos2d, Pos2d, Pos2d, Pos2d>())
  .def("Ni",get2dPointNi,"Return the values of the shape functions a the point argument (expressed in regular cartesian coordinates).")
  .def("Ni",get2dPointsNi,"Return the values of the shape functions a the points contained in the argument list (expressed in regular cartesian coordinates).")
  .add_property("midpoint", &Quadrilateral2d::getMidpoint,"Return the intersection between the segments that joint the midpoints of its sides")
  .def("getNaturalCoordinates", get2DPointNaturalCoordinates,"Returns the natural coordinates of a 2D point.")
  .def("getNaturalCoordinates", &Quadrilateral2d::getNaturalCoordinatesPy,"Returns the natural coordinates of a list of 2D points.")
  .def("getGrid",&Quadrilateral2d::genMesh, "Return a grid of points uniformly distributed on the quadrilateral surface.")
  .def("getBilinearGrid",&Quadrilateral2d::genBilinMesh, "Return a grid of points generated using bililiear coordinate interpolation.")
  ;

Quadrilateral2d (*get_quad_oriented_bounding_box_ref)(const PointCloud2d &, const Ref2d2d &)= &get_oriented_bounding_box;
Quadrilateral2d (*get_quad_oriented_bounding_box)(const PointCloud2d &)= &get_oriented_bounding_box;
def("get_oriented_bounding_box", get_quad_oriented_bounding_box_ref, "Return the oriented bounding box that contains the given point cloud and whose edges are parallel to the given reference.");
def("get_oriented_bounding_box", get_quad_oriented_bounding_box, "Return the oriented bounding box that contains the given point cloud.");

class_<Square2d, bases<Quadrilateral2d> >("Square2d")
  .def(init<>())
  ;

class_<Triangle2d, bases<PolygonalSurface2d> >("Triangle2d")
  .def(init<>())
  .def(init<Pos2d, Pos2d, Pos2d>())
  ;

class_<LineSegmentArrangement, bases<Surface2d> >("LineSegmentArrangement")
  .def(init<>())
  .def(init<std::list<Segment2d> >())
  .def("getNumVertices",&LineSegmentArrangement::getNumberOfVertices)
  .def("getNumEdges",&LineSegmentArrangement::getNumberOfEdges)
  .def("getNumFaces",&LineSegmentArrangement::getNumberOfFaces)
  ;

class_<PolygonWithHoles2d, bases<Surface2d> >("PolygonWithHoles2d")
  .def(init<>())
  .def(init<Polygon2d>())
  .def("contour",&PolygonWithHoles2d::contour)
  .def("addHole",&PolygonWithHoles2d::add_hole)
  ;

def("particiona",python_particiona);
def("clip",python_clip);


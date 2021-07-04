//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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

class_<Linear2d, bases<GeomObj2d>, boost::noncopyable  >("Linear2d", no_init)
  ;

Line2d (Line2d::*OffsetVector)(const Vector2d &v) const= &Line2d::Offset;
Line2d (Line2d::*OffsetDouble)(const GEOM_FT &) const= &Line2d::Offset;
GeomObj::list_Pos2d (Line2d::*intersectionWithR2D)(const Line2d &) const= &Line2d::getIntersection;
Pos2d (Line2d::*Pos2dProj)(const Pos2d &) const= &Line2d::Projection;
Vector2d (Line2d::*Vector2dProj)(const Vector2d &) const= &Line2d::Projection;
class_<Line2d, bases<Linear2d> >("Line2d")
  .def(init<Pos2d, Pos2d>()) //Constructs the line from two points.
  .def(init<Pos2d, Dir2d>()) //Constructs the line from a point and a direction.
  .def(init<Pos2d, Vector2d>()) //Constructs the line from a point and a vector.
  .def(init<Line2d>()) //Copy constructor.
  .def("OffsetVector",OffsetVector,"returns a parallel line obtained by adding the vector to the points that define this line.")
  .def("OffsetDouble",OffsetDouble,"returns a parallel line.")
  .def("getParamA",&Line2d::GetParamA,"returns line slope; 'a' parameter from equation (y= a*x+b).")
  .def("getParamB",&Line2d::GetParamB,"returns line y-intercept; 'b' parameter from equation (y= a*x+b).")
  .def("getIntersectionWithLine", intersectionWithR2D, "Return the intersection with the line argument.")
  .def("getPos2dProj",Pos2dProj,"return the projection of a point onto the line.")
  .def("getVector2dProj",Vector2dProj,"return the projection of a vector onto the line.")
  .def("getPoint",&Line2d::PtoParametricas,"return a point on the line.")
  .def("getVDir",&Line2d::VDir,"return the line direction vector.")
  .def("getSlope", &Line2d::getSlope)
  .def("dist", &Line2d::dist,"return the distance to the point.")
  .def("dist2", &Line2d::dist2,"return the squared distance to the point.")
  .def("swap", &Line2d::swap,"changes the orientation of the line.")
  .add_property("getIVector", &Line2d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Line2d::getJVector,"Return the local y vector.")
  ;

class_<Ray2d, bases<Linear2d> >("Ray2d")
  .def(init<Ray2d>())
  .def(init<Pos2d, Pos2d>())
  .def(init<Pos2d, Vector2d>())
  .def("dist", &Ray2d::dist,"return the distance to the point.")
  .def("dist2", &Ray2d::dist2,"return the squared distance to the point.")
  .def("swap", &Ray2d::swap,"changes the orientation of the ray.")
  .def("getSlope", &Ray2d::getSlope,"return the ray slope.")
  .def("upwards", &Ray2d::upwards,"return true if the ray goes up.")
  .def("downwards", &Ray2d::downwards,"return true if the ray goes down.")
  .add_property("getIVector", &Ray2d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Ray2d::getJVector,"Return the local y vector.")
  ;

GEOM_FT (Segment2d::*AngleVector)(const Vector2d &v) const= &Segment2d::getAngle;
GEOM_FT (Segment2d::*AngleSegment)(const Segment2d &v) const= &Segment2d::getAngle;

Segment2d (Segment2d::*OffsetSegmentVector)(const Vector2d &v) const= &Segment2d::Offset;
Segment2d (Segment2d::*OffsetSegmentDouble)(const GEOM_FT &d) const= &Segment2d::Offset;
GeomObj::list_Pos2d (Segment2d::*segment2dIntersectionWithLine)(const Line2d &) const= &Segment2d::getIntersection;
GeomObj::list_Pos2d (Segment2d::*segment2dIntersectionWithRay)(const Ray2d &) const= &Segment2d::getIntersection;
class_<Segment2d, bases<Linear2d> >("Segment2d")
  .def(init<>())
  .def(init<Pos2d,Pos2d>())
  .def(init<Segment2d>())
  .def("getFromPoint", &Segment2d::getFromPoint,"return the back endpoint of the segment.")
  .def("getToPoint", &Segment2d::getToPoint,"return the front endpoint of the segment.")
  .def("getSlope", &Segment2d::getSlope,"return the segment slope.")
  .def("upwards", &Segment2d::upwards,"return true if the segment goes up.")
  .def("downwards", &Segment2d::downwards,"return true if the segment goes down.")
  .def("getXAxisAngle", &Segment2d::XAxisAngle,"return the segment angle with respect to the X axis.")
  .def("getYAxisAngle", &Segment2d::YAxisAngle,"return the segment angle with respect to the Y axis.")
  .def("getNormal", &Segment2d::Normal,"return a vector perpendicular to the segment.")
  .def("dist", &Segment2d::dist,"return the distance to the point.")
  .def("dist2", &Segment2d::dist2,"return the squared distance to the point.")
  .def("getLength", &Segment2d::getLength,"Return the length of the segment.")
  .def("getCenterOfMass", &Segment2d::getCenterOfMass, "Return the position of the center of mass.")
  .def("angleVector",AngleVector)
  .def("angleSegment",AngleSegment)
  .def("offsetVector",OffsetSegmentVector)
  .def("offsetDouble",OffsetSegmentDouble)
  .def("getIntersectionWithLine", segment2dIntersectionWithLine, "Return the intersection with the line argument.")
  .def("getIntersectionWithRay", segment2dIntersectionWithRay, "Return the intersection with the ray argument.")
  .def("Divide", &Segment2d::DividePy,"Divide(numparts); returns the points that divide the segment.")
  .def("swap", &Segment2d::swap,"changes the orientation of the segment.")
  ;

class_<Linear3d, bases<GeomObj3d>, boost::noncopyable  >("Linear3d", no_init);

Pos3d (Line3d::*Pos3dProj)(const Pos3d &) const= &Line3d::Projection;
Vector3d (Line3d::*Vector3dProj)(const Vector3d &) const= &Line3d::Projection;

class_<Line3d, bases<Linear3d> >("Line3d")
  .def(init<Pos3d, Pos3d>()) //Constructs the line from two points.
  .def(init<Pos3d, Dir3d>()) //Constructs the line from a point and a direction.
  .def(init<Pos3d, Vector3d>()) //Constructs the line from a point and a vector.
  .def(init<Line3d>()) //Copy constructor.
  .def("getPos3dProj",Pos3dProj,"return the projection of a point onto the line.")
  .def("getVector3dProj",Vector3dProj,"return the projection of a vector onto the line.")
  .def("getVDir",&Line3d::VDir,"return the line direction vector.")
  .def("getPoint",&Line3d::PtoParametricas)
  .def("getXY2DProjection",&Line3d::XY2DProjection,"Return the projection of the line onto the XY plane as a 2D line.")
  .def("getXZ2DProjection",&Line3d::XZ2DProjection,"Return the projection of the line onto the XZ plane as a 2D line.")
  .def("getYZ2DProjection",&Line3d::YZ2DProjection,"Return the projection of the line onto the YZ plane as a 2D line.")
  .def("getXY3DProjection",&Line3d::XY3DProjection,"Return the projection of the line onto the XY plane as a 3D line.")
  .def("getXZ3DProjection",&Line3d::XZ3DProjection,"Return the projection of the line onto the XZ plane as a 3D line.")
  .def("getYZ3DProjection",&Line3d::YZ3DProjection,"Return the projection of the line onto the YZ plane as a 3D line.")
  .def("dist", &Line3d::dist,"return the distance to the point.")
  .def("dist2", &Line3d::dist2,"return the squared distance to the point.")
  .def("linearLeastSquaresFitting", &Line3d::linearLeastSquaresFitting,"compute the line that best suits the point cloud.")
  .add_property("getIVector", &Line3d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Line3d::getJVector,"Return the local y vector.")
  .add_property("getKVector", &Line3d::getKVector,"Return the local z vector.")
  .def("swap", &Line3d::swap,"changes the orientation of the line.")
 ;

class_<Ray3d, bases<Linear3d> >("Ray3d")
  .def(init<Ray3d>())
  .def(init<Pos3d, Pos3d>())
  .def(init<Pos3d, Vector3d>())
  .add_property("getIVector", &Ray3d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Ray3d::getJVector,"Return the local y vector.")
  .add_property("getKVector", &Ray3d::getKVector,"Return the local z vector.")
  .def("swap", &Ray3d::swap,"changes the orientation of the ray.")
  .def("dist", &Ray3d::dist,"return the distance to the point.")
  .def("dist2", &Ray3d::dist2,"return the squared distance to the point.")
  .def("getSlope", &Ray3d::getSlope,"return the ray slope.")
  .def("upwards", &Ray3d::upwards,"return true if the ray goes up.")
  .def("downwards", &Ray3d::downwards,"return true if the ray goes down.")
  ;

GEOM_FT (Segment3d::*AngleVector3D)(const Vector3d &v) const= &Segment3d::getAngle;
GEOM_FT (Segment3d::*AngleSegment3D)(const Segment3d &v) const= &Segment3d::getAngle;
GeomObj::list_Pos3d (Segment3d::*segment3dIntersectionWithLine)(const Line3d &) const= &Segment3d::getIntersection;
GeomObj::list_Pos3d (Segment3d::*segment3dIntersectionWithRay)(const Ray3d &) const= &Segment3d::getIntersection;
GeomObj::list_Pos3d (Segment3d::*segment3dIntersectionWithSegment)(const Segment3d &) const= &Segment3d::getIntersection;
class_<Segment3d, bases<Linear3d> >("Segment3d")
  .def(init<>())
  .def(init<Pos3d,Pos3d>())
  .def("getFromPoint", &Segment3d::getFromPoint,"return the back endpoint of the segment.")
  .def("getToPoint", &Segment3d::getToPoint,"return the front endpoint of the segment.")
  .def("getSlope", &Segment3d::getSlope)
  .def("upwards", &Segment3d::upwards,"return true if the segment goes up.")
  .def("downwards", &Segment3d::downwards,"return true if the segment goes down.")
  .def("dist", &Segment3d::dist,"return the distance to the point.")
  .def("dist2", &Segment3d::dist2,"return the squared distance to the point.")
  .def("getLength", &Segment3d::getLength,"Return the length of the segment.")
  .def("getCenterOfMass", &Segment3d::getCenterOfMass)
  .def("getPoint",&Segment3d::PtoParametricas)
  .def("getAngle",AngleVector3D,"Returns the angle between the line segment and the vector.")
  .def("getAngle",AngleSegment3D,"Returns the angle between both line segments.")
  .def("getIntersectionWithLine", segment3dIntersectionWithLine, "Return the intersection with the line argument.")
  .def("getIntersectionWithRay", segment3dIntersectionWithRay, "Return the intersection with the ray argument.")
  .def("getIntersectionWithSegment", segment3dIntersectionWithSegment, "Return the intersection with the segment argument.")
  .add_property("getIVector", &Segment3d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Segment3d::getJVector,"Return the local y vector.")
  .add_property("getKVector", &Segment3d::getKVector,"Return the local z vector.")
  .def("getVDir",&Segment3d::VDir,"return the direction vector of the segment.")
  .def("Divide", &Segment3d::DividePy,"Divide(numparts); returns the points that divide the segment.")
  .def("swap", &Segment3d::swap,"changes the orientation of the segment.")
  ;

GeomObj::list_Pos2d (Polyline2d::*intersectionWithLine)(const Line2d &) const= &Polyline2d::getIntersection;
GeomObj::list_Pos2d (Polyline2d::*intersectionWithRay)(const Ray2d &) const= &Polyline2d::getIntersection;
GeomObj::list_Pos2d (Polyline2d::*intersectionWithSegment)(const Segment2d &) const= &Polyline2d::getIntersection;
void (Polyline2d::*simplify2DPoly)(GEOM_FT epsilon)= &Polyline2d::simplify;
Segment2d (Polyline2d::*get2DSegment)(const size_t &) const= &Polyline2d::getSegment;
class_<Polyline2d, bases<Linear2d, polyPos2d> >("Polyline2d")
  .def(init<>())
  .def(init<Pos2dList>())
  .def(init<Polyline2d>())
  .def("appendVertex", &Polyline2d::AgregaVertice,return_internal_reference<>() )
  .def("getNumVertices", &Polyline2d::getNumVertices)
  .def("getNumSegments", &Polyline2d::getNumSegments)
  .def("getFromPoint", &Polyline2d::getFromPoint,"return the first point of the polyline.")
  .def("getToPoint", &Polyline2d::getToPoint,"return the last point of the polyline.")
  .def("getVertexList",&Polyline2d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getIx", &Polyline2d::Ix)
  .def("getIy", &Polyline2d::Iy)
  .def("getIz", &Polyline2d::Iz)
  .def("getPxy", &Polyline2d::Pxy)
  .def("getLength", &Polyline2d::getLength,"Return the length of the polyline.")
  .def("offset", &Polyline2d::Offset)
  .def("getIntersectionWithLine", intersectionWithLine, "Return the intersection with the line argument.")
  .def("getIntersectionWithRay", intersectionWithRay, "Return the intersection with the ray argument.")
  .def("getIntersectionWithSegment", intersectionWithSegment, "Return the intersection with the segment argument.")
  .def("isClosed",&Polyline2d::isClosed,"returns true if the last vertex is coincident with the first one -dist(first,last)<tol*length-.")
  .def("simplify", simplify2DPoly,"simplification of the polyline (Douglas-Peucker algorithm).")
  .def("getSegment", get2DSegment, "return the i-th segment.")  
  .add_property("getIVector", &Segment2d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Segment2d::getJVector,"Return the local y vector.")
  ;



void (Polyline3d::*simplify3DPoly)(GEOM_FT epsilon)= &Polyline3d::simplify;
Segment3d (Polyline3d::*get3DSegment)(const size_t &) const= &Polyline3d::getSegment;
class_<Polyline3d, bases<Linear3d, polyPos3d> >("Polyline3d")
  .def(init<>())
//.def(init<Pos3dList>())
  .def(init<Polyline3d>())
  .def("appendVertex", &Polyline3d::AgregaVertice,return_internal_reference<>() )
  .def("getNumVertices", &Polyline3d::getNumVertices)
  .def("getNumSegments", &Polyline3d::getNumSegments)
  .def("getLength", &Polyline3d::getLength,"Return the length of the polyline.")
  .def("getFromPoint", &Polyline3d::getFromPoint,"return the first point of the polyline.")
  .def("getToPoint", &Polyline3d::getToPoint,"return the last point of the polyline.")
  .def("getVertexList",&Polyline3d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getIntersection", &Polyline3d::getIntersection, "Return the intersection with the plane argument.")
  .def("isClosed",&Polyline3d::isClosed,"returns true if the last vertex is coincident with the first one -dist(first,last)<tol*length-.")
  .def("simplify", simplify3DPoly,"simplification of the polyline (Douglas-Peucker algorithm).")
  .def("getCenterOfMass", &Polyline3d::getCenterOfMass)
  .def("getSegment", get3DSegment, "return the i-th segment.")
  ;

typedef std::deque<Polyline3d> dq_polyline3D;
class_<dq_polyline3D>("dq_polyline3D")
  .def(vector_indexing_suite<dq_polyline3D>() )
  ;

typedef std::deque<Segment3d> dq_segment3D;
class_<dq_segment3D>("dq_segment3D")
  .def(vector_indexing_suite<dq_segment3D>() )
  ;

class_<int_pair>("IntPair")
  .def_readwrite("first", &std::pair<int, int>::first)
  .def_readwrite("second", &std::pair<int, int>::second)
  ;
class_<int_pair_deque>("int_pair_deque")
  .def(vector_indexing_suite<int_pair_deque>() )
  ;

def("getSegmentIntersections",getIntersections, "Return the intersection between the segments.");
def("get_2d_polylines",py_get_2d_polylines, "Return the polylines obtained by connection the 2D segments.");
def("get_3d_polylines",py_get_3d_polylines, "Return the polylines obtained by connection the 3D segments.");

#include "function_from_points/python_interface.tcc"

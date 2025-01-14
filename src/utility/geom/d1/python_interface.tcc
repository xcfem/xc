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

class_<Linear2d, bases<GeomObj2d>, boost::noncopyable  >("Linear2d", no_init)
  .def("getArea", &Linear2d::getArea, "return the object area.")
  .def("getVolume", &Linear2d::getVolume, "return the object volume.")
  .def("getLambda", &Linear2d::getLambda, "Return the length along the line until the given point is reached.")
  .def("sortPointsAlong", &Linear2d::sortPointsAlongPy, "Return the given points sorted by its parameter along the line (assuming they are on the line) in the form of a list of (position, lambda) pairs.")
  ;

Line2d (Line2d::*OffsetVector)(const Vector2d &v) const= &Line2d::offset;
Line2d (Line2d::*OffsetDouble)(const GEOM_FT &) const= &Line2d::offset;
GeomObj::list_Pos2d (Line2d::*intersectionWithR2D)(const Line2d &) const= &Line2d::getIntersection;
GeomObj::list_Pos2d (Line2d::*intersectionWithPline2D)(const Polyline2d &) const= &Line2d::getIntersection;
Pos2d (Line2d::*Pos2dProj)(const Pos2d &) const= &Line2d::Projection;
Vector2d (Line2d::*Vector2dProj)(const Vector2d &) const= &Line2d::Projection;
class_<Line2d, bases<Linear2d> >("Line2d")
  .def(init<Pos2d, Pos2d>()) // Constructs the line from two points.
  .def(init<Pos2d, Dir2d>()) // Constructs the line from a point and a direction.
  .def(init<Pos2d, Vector2d>()) // Constructs the line from a point and a vector.
  .def(init<Line2d>()) //Copy constructor.
  .def(init<const boost::python::list &>()) // Least squares constructor. 
  .def("offset",OffsetVector,"returns a parallel segment obtained by adding the vector to the points that define this line.")
  .def("offset",OffsetDouble,"returns a parallel segment.")
  .def("getParamA",&Line2d::GetParamA,"returns line slope; 'a' parameter from equation (y= a*x+b).")
  .def("getParamB",&Line2d::GetParamB,"returns line y-intercept; 'b' parameter from equation (y= a*x+b).")
  .def("getIntersection", intersectionWithR2D, "Return the intersection with the line argument.")
  .def("getIntersection", intersectionWithPline2D, "Return the intersection with the given polyline.")
  .def("getPos2dProj",Pos2dProj,"return the projection of a point onto the line.")
  .def("getVector2dProj",Vector2dProj,"return the projection of a vector onto the line.")
  .def("getPoint",&Line2d::PtoParametricas,"return a point on the line.")
  .def("getVDir",&Line2d::VDir,"return the line direction vector.")
  .def("getNormal", &Line2d::Normal,"return a vector perpendicular to the segment.")
  .def("getSlope", &Line2d::getSlope)
  .def("dist", &Line2d::dist,"return the distance to the point.")
  .def("dist2", &Line2d::dist2,"return the squared distance to the point.")
  .def("swap", &Line2d::swap,"changes the orientation of the line.")
  .add_property("getIVector", &Line2d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Line2d::getJVector,"Return the local y vector.")
  ;

GeomObj::list_Pos2d (Ray2d::*ray_intersectionWithR2D)(const Line2d &) const= &Ray2d::getIntersection;
GeomObj::list_Pos2d (Ray2d::*ray_intersectionWithRay2D)(const Line2d &) const= &Ray2d::getIntersection;
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
  .def("getVDir",&Ray2d::VDir,"return the ray direction vector.")
  .def("getNormal", &Ray2d::Normal,"return a vector perpendicular to the segment.")
  .def("getPoint",&Ray2d::PtoParametricas,"return a point on the line.")
  .def("getIntersection", ray_intersectionWithR2D, "Return the intersection with the line argument.")
  .def("getIntersection", ray_intersectionWithRay2D, "Return the intersection with the ray argument.")
  .def("getSupportLine", &Ray2d::getSupportLine, "Return the supporting line.")
  ;

GEOM_FT (Segment2d::*AngleVector2D)(const Vector2d &v) const= &Segment2d::getAngle;
GEOM_FT (Segment3d::*AngleLine2D)(const Line3d &) const= &Segment3d::getAngle;
GEOM_FT (Segment3d::*AngleRay2D)(const Ray3d &) const= &Segment3d::getAngle;
GEOM_FT (Segment2d::*AngleSegment2D)(const Segment2d &v) const= &Segment2d::getAngle;

Segment2d (Segment2d::*OffsetSegmentVector)(const Vector2d &v) const= &Segment2d::offset;
Segment2d (Segment2d::*OffsetSegmentDouble)(const GEOM_FT &d) const= &Segment2d::offset;
GeomObj::list_Pos2d (Segment2d::*segment2dIntersectionWithLine)(const Line2d &) const= &Segment2d::getIntersection;
GeomObj::list_Pos2d (Segment2d::*segment2dIntersectionWithRay)(const Ray2d &) const= &Segment2d::getIntersection;
boost::python::list (Segment2d::*dividePyEq2d)(int) const= &Segment2d::DividePy;
boost::python::list (Segment2d::*dividePyProp2d)(const boost::python::list &) const= &Segment2d::DividePy;
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
  .def("getVDir",&Segment2d::VDir,"return the segment direction vector.")
  .def("getNormal", &Segment2d::Normal,"return a vector perpendicular to the segment.")
  .def("dist", &Segment2d::dist,"return the distance to the point.")
  .def("dist2", &Segment2d::dist2,"return the squared distance to the point.")
  .def("getLength", &Segment2d::getLength,"Return the length of the segment.")
  .def("getCenterOfMass", &Segment2d::getCenterOfMass, "Return the position of the center of mass.")
  .def("getPoint",&Segment2d::PtoParametricas,"return a point on the segment.")
  .def("getMidPoint",&Segment2d::getMidPoint,"return the mid point on the segment.")
  .def("getAngle",AngleVector2D,"Returns the angle between the line segment and the vector.")
  .def("getAngle",AngleLine2D,"Returns the angle between the line segment and the line.")
  .def("getAngle",AngleRay2D,"Returns the angle between the line segment and the ray.")
  .def("getAngle",AngleSegment2D,"Returns the angle between both line segments.")
  .def("angleVector",AngleVector2D,"DEPRECATED use getAngle; returns the angle between the line segment and the vector.")
  .def("angleSegment",AngleSegment2D,"DEPRECATED use getAngle; returns the angle between the line segments.")
  .def("offset",OffsetSegmentVector)
  .def("offset",OffsetSegmentDouble)
  .def("getIntersection", segment2dIntersectionWithLine, "Return the intersection with the line argument.")
  .def("getIntersection", segment2dIntersectionWithRay, "Return the intersection with the ray argument.")
  .def("getSupportLine", &Segment2d::getSupportLine, "Return the supporting line.")
  .def("Divide", dividePyEq2d,"Divide(numparts); returns the points that divide the segment in numparts equal parts.")
  .def("Divide", dividePyProp2d,"Divide(proportions); returns the points that divide the segment in the proportions of the list.")
  .def("split", &Segment2d::split,"split(point) returns the result of splitting the segment by the point argument.")
  .def("swap", &Segment2d::swap,"changes the orientation of the segment.")
  .add_property("getIVector", &Segment2d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Segment2d::getJVector,"Return the local y vector.")
  .def("getBufferPolygon", &Segment2d::getBufferPolygon, "Return a buffer polygon around the segment.")
  ;

class_<Linear3d, bases<GeomObj3d>, boost::noncopyable  >("Linear3d", no_init)
  .def("getLambda", &Linear3d::getLambda, "Return the length along the line until the given point is reached.")
  .def("sortPointsAlong", &Linear3d::sortPointsAlongPy, "Return the given points sorted by its parameter along the line (assuming they are on the line) in the form of a list of (position, lambda) pairs.")
  ;

Pos3d (Line3d::*LinePos3dProj)(const Pos3d &) const= &Line3d::Projection;
Vector3d (Line3d::*Vector3dProj)(const Vector3d &) const= &Line3d::Projection;
class_<Line3d, bases<Linear3d> >("Line3d")
  .def(init<Pos3d, Pos3d>()) //Constructs the line from two points.
  .def(init<Pos3d, Dir3d>()) //Constructs the line from a point and a direction.
  .def(init<Pos3d, Vector3d>()) //Constructs the line from a point and a vector.
  .def(init<Line3d>()) //Copy constructor.
  .def(init<const boost::python::list &>()) // Least squares constructor. 
  .def("getPos3dProj",LinePos3dProj,"TO DEPRECATE return the projection of a point onto the line.")
  .def("getProjection",LinePos3dProj,"return the projection of a point onto the line.")
  .def("getVector3dProj",Vector3dProj,"return the projection of a vector onto the line.")
  .def("getVDir",&Line3d::VDir,"return the line direction vector.")
  .def("getPoint",&Line3d::PtoParametricas,"return a point on the line.")
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

Pos3d (Ray3d::*Ray3dPos3dProj)(const Pos3d &) const= &Ray3d::Projection;
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
  .def("getProjection",Ray3dPos3dProj,"return the projection of a point onto the line.")
  .def("getSupportLine", &Ray3d::getSupportLine, "Return the supporting line.")
  .def("getPoint",&Ray3d::PtoParametricas,"return a point on the line.")
  ;

GEOM_FT (Segment3d::*AngleVector3D)(const Vector3d &v) const= &Segment3d::getAngle;
GEOM_FT (Segment3d::*AngleLine3D)(const Line3d &) const= &Segment3d::getAngle;
GEOM_FT (Segment3d::*AngleRay3D)(const Ray3d &) const= &Segment3d::getAngle;
GEOM_FT (Segment3d::*AngleSegment3D)(const Segment3d &v) const= &Segment3d::getAngle;
GeomObj::list_Pos3d (Segment3d::*segment3dIntersectionWithLine)(const Line3d &) const= &Segment3d::getIntersection;
GeomObj::list_Pos3d (Segment3d::*segment3dIntersectionWithRay)(const Ray3d &) const= &Segment3d::getIntersection;
GeomObj::list_Pos3d (Segment3d::*segment3dIntersectionWithSegment)(const Segment3d &) const= &Segment3d::getIntersection;
boost::python::list (Segment3d::*dividePyEq3d)(int) const= &Segment3d::DividePy;
boost::python::list (Segment3d::*dividePyProp3d)(const boost::python::list &) const= &Segment3d::DividePy;
Pos3d (Segment3d::*Segment3dPos3dProj)(const Pos3d &) const= &Segment3d::Projection;
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
  .def("getPoint",&Segment3d::PtoParametricas,"return a point on the segment.")
  .def("getMidPoint",&Segment3d::getMidPoint,"return the mid point on the segment.")
  .def("getAngle",AngleVector3D,"Returns the angle between the line segment and the vector.")
  .def("getAngle",AngleLine3D,"Returns the angle between the line segment and the line.")
  .def("getAngle",AngleRay3D,"Returns the angle between the line segment and the ray.")
  .def("getAngle",AngleSegment3D,"Returns the angle between both line segments.")
  .def("getProjection",Segment3dPos3dProj,"return the projection of a point onto the line.")
  .def("getIntersection", segment3dIntersectionWithLine, "Return the intersection with the line argument.")
  .def("getIntersection", segment3dIntersectionWithRay, "Return the intersection with the ray argument.")
  .def("getIntersection", segment3dIntersectionWithSegment, "Return the intersection with the segment argument.")
  .def("getSupportLine", &Segment3d::getSupportLine, "Return the supporting line.")
  .add_property("getIVector", &Segment3d::getIVector,"Return the local x vector.")
  .add_property("getJVector", &Segment3d::getJVector,"Return the local y vector.")
  .add_property("getKVector", &Segment3d::getKVector,"Return the local z vector.")
  .def("getVDir",&Segment3d::VDir,"return the direction vector of the segment.")
  .def("Divide", dividePyEq3d,"Divide(numparts); returns the points that divide the segment in numparts equal parts.")
  .def("Divide", dividePyProp3d,"Divide(proportions); returns the points that divide the segment in the proportions of the list.")
  .def("split", &Segment3d::split,"split(point) returns the result of splitting the segment by the point argument.")
  .def("swap", &Segment3d::swap,"changes the orientation of the segment.")
  ;

GeomObj::list_Pos2d (Polyline2d::*intersectionWithLine)(const Line2d &) const= &Polyline2d::getIntersection;
GeomObj::list_Pos2d (Polyline2d::*intersectionWithRay)(const Ray2d &) const= &Polyline2d::getIntersection;
GeomObj::list_Pos2d (Polyline2d::*intersectionWithSegment)(const Segment2d &) const= &Polyline2d::getIntersection;
void (Polyline2d::*simplify2DPoly)(GEOM_FT epsilon)= &Polyline2d::simplify;
Segment2d (Polyline2d::*get2DSegment)(const size_t &) const= &Polyline2d::getSegment;
boost::python::list (Polyline2d::*pline2dDividePyEq2d)(int) const= &Polyline2d::DividePy;
boost::python::list (Polyline2d::*pline2dDividePyProp2d)(const boost::python::list &) const= &Polyline2d::DividePy;
class_<Polyline2d, bases<Linear2d, polyPos2d> >("Polyline2d")
  .def(init<>())
  .def(init<Pos2dList>())
  .def(init<Polyline2d>())
  .def(init<boost::python::list>())
  .def("appendVertex", &Polyline2d::appendVertex,return_internal_reference<>() )
  .def("appendVertexLeft", &Polyline2d::appendVertexLeft,return_internal_reference<>() )
  .def("getNumVertices", &Polyline2d::getNumVertices)
  .def("getNumSegments", &Polyline2d::getNumSegments)
  .def("getFromPoint", &Polyline2d::getFromPoint,"return the first point of the polyline.")
  .def("getToPoint", &Polyline2d::getToPoint,"return the last point of the polyline.")
  .def("getVertexList",&Polyline2d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getVertices",&Polyline2d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getIx", &Polyline2d::Ix)
  .def("getIy", &Polyline2d::Iy)
  .def("getIz", &Polyline2d::Iz)
  .def("getPxy", &Polyline2d::Pxy)
  .def("getLength", &Polyline2d::getLength,"Return the length of the polyline.")
  .def("getLengths", &Polyline2d::getLengthsPy,"Return the lengths of the polyline in each of its vertices.")
  .def("offset", &Polyline2d::offset)
  .def("getBufferPolygon", &Polyline2d::getBufferPolygon, "Return a buffer polygon around the polyline.")
  .def("getIntersection", intersectionWithLine, "Return the intersection with the line argument.")
  .def("getIntersection", intersectionWithRay, "Return the intersection with the ray argument.")
  .def("getIntersection", intersectionWithSegment, "Return the intersection with the segment argument.")
  .def("getNearestSegment", &Polyline2d::getNearestSegment, "Return the nearest segment to the given point.")
  .def("getNearestSegmentIndex", &Polyline2d::getNearestSegmentIndex, "Return the index of the nearest segment to the given point.")
  .def("getProjection", &Polyline2d::Projection, "Return the projection of the given point onto the polyline.")
  .def("getLengthUpTo", &Polyline2d::getLengthUpTo, "Return the length along the polyline until the point p is reached.")
  .def("getLambda", &Polyline2d::getLambda, "Return the length along the polyline until the point p is reached.")
  .def("isClosed",&Polyline2d::isClosed,"returns true if the last vertex is coincident with the first one -dist(first,last)<tol*length-.")
  .def("simplify", simplify2DPoly,"simplification of the polyline (Douglas-Peucker algorithm).")
  .def("insertVertex", &Polyline2d::insertVertex,"Insert the point argurment as vertex by splitting the nearest segment.")
  .def("getChunk", &Polyline2d::getChunk,"getChunk(point, sgn, tol) returns the chunk of the polyline that goes from the begining to the argument point if sgn<0 or from the argument point to the polyline end if sgn>=0. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("getLeftChunk", &Polyline2d::getLeftChunk,"getChunk(point, tol) returns the chunk of the polyline that goes from its beginning to the point argument. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("getRightChunk", &Polyline2d::getRightChunk,"getChunk(point, tol) returns the chunk of the polyline that goes from the point argument to its end. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("split", &Polyline2d::split,"split(point) returns the result of splitting the polyline by the point argument.")
  .def("clip", &Polyline2d::clipPy, "Return the polyline chunks that result from clipping this polyline with the given half plane.")
  .def("getSegment", get2DSegment, "return the i-th segment.")  
  .def("getIndexOfSegmentAtLength", &Polyline2d::getIndexOfSegmentAtLength,"getIndexOfSegmentAtLength(s): return the index of the segment that lies at the point at a distance \"s\" measured along the polyline from its origin.")
  .def("getIndexOfSegmentAtParam", &Polyline2d::getIndexOfSegmentAtParam,"getIndexOfSegmentAtParam(s): return the index of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getPointAtLength", &Polyline2d::getPointAtLength, "getPointAtLength(s): return the point that lies at a distance \"s\" measured along the polyline from its origin.")
  .def("getIVectorAtLength", &Polyline2d::getIVectorAtLength, "getIVectorAtLength(s): return the I vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getJVectorAtLength", &Polyline2d::getJVectorAtLength, "getJVectorAtLength(s): return the J vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getTangentVectorAtVertices", &Polyline2d::getTangentVectorAtVerticesPy, "Return a Python list containing the tangent vectors at each of the polyline vertices.")
  .def("getNormalVectorAtVertices", &Polyline2d::getNormalVectorAtVerticesPy, "Return a Python list containing the normal vectors at each of the polyline vertices.")
  .def("Divide", pline2dDividePyEq2d,"Divide(numparts); returns the points that divide the polyline in numparts equal parts.")
  .def("Divide", pline2dDividePyProp2d,"Divide(proportions); returns the points that divide the polyline in the proportions of the list.")
  .def("dist", &Polyline2d::dist,"return the distance to the point.")
  .def("dist2", &Polyline2d::dist2,"return the squared distance to the point.")
  .def("removeRepeatedVertexes",  &Polyline2d::removeRepeated,"remove the vertices that are closer than the specified tolerance.")
  .def("removeBackwardSegments",  &Polyline2d::removeBackwardSegments,"remove the vertices that generate backwards segments (the tolerance is the minimum accepted value for the dot product (<0) of consecutive direction vectors).")
  ;

void (PlanePolyline3d::*simplifyPP3DPoly)(GEOM_FT epsilon)= &PlanePolyline3d::simplify;
Segment3d (PlanePolyline3d::*getPP3DSegment)(const unsigned int ) const= &PlanePolyline3d::getSegment;
bool (PlanePolyline3d::*getPPPlaneIntersects)(const Plane &) const= &PlanePolyline3d::intersects;
bool (PlanePolyline3d::*getPPLineIntersects)(const Line3d &) const= &PlanePolyline3d::intersects;
GeomObj3d::list_Pos3d (PlanePolyline3d::*getPPPlaneIntersection)(const Plane &) const= &PlanePolyline3d::getIntersection;
GeomObj3d::list_Pos3d (PlanePolyline3d::*getPPLineIntersection)(const Line3d &) const= &PlanePolyline3d::getIntersection;
Vector3d (D2to3d::*ppto_3dv)(const Vector2d &) const= &D2to3d::to_3d;
Pos3d (D2to3d::*ppto_3dp)(const Pos2d &) const= &D2to3d::to_3d;
Line3d (D2to3d::*ppto_3dln)(const Line2d &) const= &D2to3d::to_3d;
Segment3d (D2to3d::*ppto_3dsg)(const Segment2d &) const= &D2to3d::to_3d;
Vector2d (D2to3d::*ppto_2dv)(const Vector3d &) const= &D2to3d::to_2d;
Pos2d (D2to3d::*ppto_2dp)(const Pos3d &) const= &D2to3d::to_2d;
Line2d (D2to3d::*ppto_2dln)(const Line3d &) const= &D2to3d::to_2d;
Segment2d (D2to3d::*ppto_2dsg)(const Segment3d &) const= &D2to3d::to_2d;
BND3d (D2to3d::*bnd_3d)(void) const= &D2to3d::Bnd;
boost::python::list (PlanePolyline3d::*planePline3dDividePyEq3d)(int) const= &PlanePolyline3d::DividePy;
boost::python::list (PlanePolyline3d::*planePline3dDividePyProp3d)(const boost::python::list &) const= &PlanePolyline3d::DividePy;
class_<PlanePolyline3d, bases<Linear3d> >("PlanePolyline3d")
  .def(init<>())
  .def(init<PlanePolyline3d>())
  .def(init<Ref2d3d,Polyline2d>())
  .def(init<Ref3d3d,Polyline2d>())
  .def(init<boost::python::list>())
  .add_property("getXMax",&PlanePolyline3d::GetXMax, "Return maximum value for x coordinate.")
  .add_property("getYMax",&PlanePolyline3d::GetYMax, "Return maximum value for y coordinate.")
  .add_property("getZMax",&PlanePolyline3d::GetZMax, "Return maximum value for z coordinate.")
  .add_property("getXMin",&PlanePolyline3d::GetXMin, "Return minimum value for x coordinate.")
  .add_property("getYMin",&PlanePolyline3d::GetYMin, "Return minimum value for y coordinate.")
  .add_property("getZMin",&PlanePolyline3d::GetZMin, "Return minimum value for z coordinate.")
  .def("getPlane",&PlanePolyline3d::getPlane, "Return the plane that contains the polyline.")
  .def("to_3d",ppto_3dv, "Return the corresponding 3D vector.")
  .def("to_3d",ppto_3dp, "Return the corresponding 3D point.")
  .def("to_3d",ppto_3dln, "Return the corresponding 3D line.")
  .def("to_3d",ppto_3dsg, "Return the corresponding 3D segment.")
  .def("to_2d",ppto_2dv, "Return the corresponding 2D vector.")
  .def("to_2d",ppto_2dp, "Return the corresponding 2D point.")
  .def("to_2d",ppto_2dln, "Return the corresponding 2D line.")
  .def("to_2d",ppto_2dsg, "Return the corresponding 2D segment.")
  .def("getBnd",bnd_3d, "Return the object boundary box.")
  .def("appendVertex", &PlanePolyline3d::appendVertex, "Append a vertex to the end of the polyline.")
  .def("appendVertexLeft", &PlanePolyline3d::appendVertexLeft, "Append a vertex to the beginning of the polyline.")
  .def("getNumVertices", &PlanePolyline3d::getNumVertices)
  .def("getNumSegments", &PlanePolyline3d::getNumSegments)
  .def("getLength", &PlanePolyline3d::getLength,"Return the length of the polyline.")
  .def("getLengths", &PlanePolyline3d::getLengthsPy,"Return the lengths of the polyline in each of its vertices.")
  .def("getFromPoint", &PlanePolyline3d::getFromPoint,"return the first point of the polyline.")
  .def("getToPoint", &PlanePolyline3d::getToPoint,"return the last point of the polyline.")
  .def("getVertexList",&PlanePolyline3d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getVertices",&PlanePolyline3d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getPolyline3d", &PlanePolyline3d::getPolyline3d,"Return the corresponding 3D polyline.")
  .def("intersects", getPPLineIntersects, "Return true if this object intersects with the line argument.")
  .def("getIntersection", getPPPlaneIntersection, "Return the intersection with the plane argument.")
  .def("intersects", getPPPlaneIntersects, "Return true if this object intersects with the plane argument.")
  .def("getIntersection", getPPLineIntersection, "Return the intersection with the line argument.")
  .def("getNearestSegment", &PlanePolyline3d::getNearestSegment, "Return the nearest segment to the given point.")
  .def("getProjection", &PlanePolyline3d::Projection, "Return the projection of the given point onto the polyline.")
  .def("getLengthUpTo", &PlanePolyline3d::getLengthUpTo, "Return the length along the polyline until the point p is reached.")
  .def("isClosed",&PlanePolyline3d::isClosed,"returns true if the last vertex is coincident with the first one -dist(first,last)<tol*length-.")
  .def("simplify", simplifyPP3DPoly,"simplification of the polyline (Douglas-Peucker algorithm).")
  .def("getCenterOfMass", &PlanePolyline3d::getCenterOfMass)
  .def("getSegment", getPP3DSegment, "return the i-th segment.")
  .def("getIndexOfSegmentAtParam", &PlanePolyline3d::getIndexOfSegmentAtParam,"Return the index of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getIndexOfSegmentAtLength", &PlanePolyline3d::getIndexOfSegmentAtLength,"getIndexOfSegmentAtLength(s): return the index of the segment that lies at the point at a distance \"s\" measured along the polyline from its origin.")
  .def("getPointAtLength", &PlanePolyline3d::getPointAtLength, "getPointAtLength(s): return the point that lies at a distance \"s\" measured along the polyline from its origin.")
  .def("getIVectorAtLength", &PlanePolyline3d::getIVectorAtLength, "getIVectorAtLength(s): return the I vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getJVectorAtLength", &PlanePolyline3d::getJVectorAtLength, "getJVectorAtLength(s): return the J vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getKVectorAtLength", &PlanePolyline3d::getKVectorAtLength, "getKVectorAtLength(s): return the K vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("Divide", planePline3dDividePyEq3d,"Divide(numparts); returns the points that divide the polyline in numparts equal parts.")
  .def("Divide", planePline3dDividePyProp3d,"Divide(proportions); returns the points that divide the polyline in the proportions of the list.")
  .def("insertVertex", &PlanePolyline3d::insertVertex,"Insert the point argurment as vertex by splitting the nearest segment.")
  .def("getChunk", &PlanePolyline3d::getChunk,"getChunk(point, sgn, tol) returns the chunk of the polyline that goes from the begining to the argument point if sgn<0 or from the argument point to the polyline end if sgn>=0. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("getLeftChunk", &PlanePolyline3d::getLeftChunk,"getChunk(point, tol) returns the chunk of the polyline that goes from its beginning to the point argument. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("getRightChunk", &PlanePolyline3d::getRightChunk,"getChunk(point, tol) returns the chunk of the polyline that goes from the point argument to its end. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("split", &PlanePolyline3d::split,"split(point) returns the result of splitting the polyline by the point argument.")
  .def("offset", &PlanePolyline3d::offset)
  .def("getBufferPolygon", &PlanePolyline3d::getBufferPolygon, "Return a buffer polygon around the polyline.")
  .def("removeRepeatedVertexes", &PlanePolyline3d::removeRepeated,"remove the vertices that are closer than the specified tolerance.")
  .def("removeBackwardSegments",  &PlanePolyline3d::removeBackwardSegments,"remove the vertices that generate backward segments (the tolerance is the minimum accepted value for the dot product (<0) of consecutive direction vectors).")
  ;


void (Polyline3d::*simplify3DPoly)(GEOM_FT epsilon)= &Polyline3d::simplify;
Segment3d (Polyline3d::*get3DSegment)(const size_t &) const= &Polyline3d::getSegment;
boost::python::list (Polyline3d::*pline3dDividePyEq3d)(int) const= &Polyline3d::DividePy;
boost::python::list (Polyline3d::*pline3dDividePyProp3d)(const boost::python::list &) const= &Polyline3d::DividePy;
class_<Polyline3d, bases<Linear3d, polyPos3d> >("Polyline3d")
  .def(init<>())
//.def(init<Pos3dList>())
  .def(init<Polyline3d>())
  .def(init<boost::python::list>())
  .def("appendVertex", &Polyline3d::appendVertex,return_internal_reference<>() )
  .def("appendVertexLeft", &Polyline3d::appendVertexLeft,return_internal_reference<>() )
  .def("getNumVertices", &Polyline3d::getNumVertices)
  .def("getNumSegments", &Polyline3d::getNumSegments)
  .def("getLength", &Polyline3d::getLength,"Return the length of the polyline.")
  .def("getLengths", &Polyline3d::getLengthsPy,"Return the lengths of the polyline in each of its vertices.")
  .def("getFromPoint", &Polyline3d::getFromPoint,"return the first point of the polyline.")
  .def("getToPoint", &Polyline3d::getToPoint,"return the last point of the polyline.")
  .def("getVertexList",&Polyline3d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getVertices",&Polyline3d::getVertexListPy,"Return a Python list containing the positions of the polyline vertices.")
  .def("getIntersection", &Polyline3d::getIntersection, "Return the intersection with the plane argument.")
  .def("getNearestSegmentIndex", &Polyline3d::getNearestSegmentIndex, "Return the index of the nearest segment to the given point.")
  .def("getNearestSegment", &Polyline3d::getNearestSegment, "Return the nearest segment to the given point.")
  .def("getProjection", &Polyline3d::Projection, "Return the projection of the given point onto the polyline.")
  .def("getLengthUpTo", &Polyline3d::getLengthUpTo, "Return the length along the polyline until the point p is reached.")
  .def("isClosed",&Polyline3d::isClosed,"returns true if the last vertex is coincident with the first one -dist(first,last)<tol*length-.")
  .def("simplify", simplify3DPoly,"simplification of the polyline (Douglas-Peucker algorithm).")
  .def("getCenterOfMass", &Polyline3d::getCenterOfMass)
  .def("getSegment", get3DSegment, "return the i-th segment.")
  .def("getIndexOfSegmentAtParam", &Polyline3d::getIndexOfSegmentAtParam,"Return the index of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getIndexOfSegmentAtLength", &Polyline3d::getIndexOfSegmentAtLength,"getIndexOfSegmentAtLength(s): return the index of the segment that lies at the point at a distance \"s\" measured along the polyline from its origin.")
  .def("getPointAtLength", &Polyline3d::getPointAtLength, "getPointAtLength(s): return the point that lies at a distance \"s\" measured along the polyline from its origin.")
  .def("getIVectorAtLength", &Polyline3d::getIVectorAtLength, "getIVectorAtLength(s): return the I vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getJVectorAtLength", &Polyline3d::getJVectorAtLength, "getJVectorAtLength(s): return the J vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getKVectorAtLength", &Polyline3d::getKVectorAtLength, "getKVectorAtLength(s): return the K vector of the segment that lies at the point at a distance \"lambda*L\" measured along the polyline from its origin.")
  .def("getTangentVectorAtVertices", &Polyline3d::getTangentVectorAtVerticesPy, "Return a Python list containing the tangent vectors at each of the polyline vertices.")
  .def("getNormalVectorAtVertices", &Polyline3d::getNormalVectorAtVerticesPy, "Return a Python list containing the normal vectors at each of the polyline vertices.")
  .def("Divide", pline3dDividePyEq3d,"Divide(numparts); returns the points that divide the polyline in numparts equal parts.")
  .def("Divide", pline3dDividePyProp3d,"Divide(proportions); returns the points that divide the polyline in the proportions of the list.")
  .def("insertVertex", &Polyline3d::insertVertex,"Insert the point argurment as vertex by splitting the nearest segment.")
  .def("getChunk", &Polyline3d::getChunk,"getChunk(point, sgn, tol) returns the chunk of the polyline that goes from the begining to the argument point if sgn<0 or from the argument point to the polyline end if sgn>=0. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("getLeftChunk", &Polyline3d::getLeftChunk,"getChunk(point, tol) returns the chunk of the polyline that goes from its beginning to the point argument. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("getRightChunk", &Polyline3d::getRightChunk,"getChunk(point, tol) returns the chunk of the polyline that goes from the point argument to its end. If distance from the point to the nearest vertex is greater than tol, append p to the resulting polyline.")
  .def("split", &Polyline3d::split,"split(point) returns the result of splitting the polyline by the point argument.")
  .def("clip", &Polyline3d::clipPy, "Return the polyline chunks that result from clipping this polyline with the given half space.")
  .def("dist", &Polyline3d::dist,"return the distance to the point.")
  .def("dist2", &Polyline3d::dist2,"return the squared distance to the point.")
  .def("removeRepeatedVertexes", &Polyline3d::removeRepeated,"remove the vertices that are closer than the specified tolerance.")
  .def("removeBackwardSegments",  &Polyline3d::removeBackwardSegments,"remove the vertices that generate backwards segments (the tolerance is the minimum accepted value for the dot product (<0) of consecutive direction vectors).")
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

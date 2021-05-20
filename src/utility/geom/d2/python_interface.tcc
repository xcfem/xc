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

class_<Surface2d, bases<GeomObj2d>, boost::noncopyable  >("Surface2d", no_init);

class_<HalfPlane2d, bases<Surface2d> >("HalfPlane2d")
  .def(init<>())
  .def(init<Line2d>())
  ;

class_<BND2d, bases<GeomObj2d> >("BND2d")
  .def(init<>())
  .def(init<Pos2d, Pos2d>())
  .def(init<BND2d>())
  .add_property("width", &BND2d::getWidth, "returns object width.")
  .add_property("height", &BND2d::getHeight, "returns object height.")
  .add_property("length", &BND2d::getLength, "returns object length.")
  .add_property("diagonal", &BND2d::Diagonal, "returns a vector from PMin to PMax.")
  .add_property("area", &BND2d::getArea, "returns object area.")
  .add_property("volume", &BND2d::getVolume, "returns object volume.")


  .def("update", &BND2d::Update)
  .add_property("pMax", &BND2d::GetPMax, &BND2d::PutPMax,"upper right corner.")
  .add_property("pMin", &BND2d::GetPMin, &BND2d::PutPMin,"lower left corner.")

  .def("getIx", &BND2d::Ix)
  .def("getIy", &BND2d::Iy)
  .def("getPxy", &BND2d::Pxy)
  .def("getCenterOfMass", &BND2d::getCenterOfMass, " return the center of mass.")
  .def("getMax", &BND2d::GetMax,"return maximum value of i coordinate.")
  .def("getMin", &BND2d::GetMin,"return minimum value of i coordinate.")
  ;

class_<Circle2d, bases<Surface2d> >("Circle2d")
  .def(init<>())
  .def(init<Pos2d, Pos2d, Pos2d>())
  .def(init<Pos2d,double>())
  .def(init<Circle2d>())
  .def("getCenter",&Circle2d::Centro)
  .def("getRadius",&Circle2d::getRadius,"Return the object radius.")
  .def("getDiameter",&Circle2d::getDiameter, "Return the object diameter.")
  .def("getPerimeter",&Circle2d::getPerimeter, "Return the object perimeter.")
  .def("getInscribedPolygon",&Circle2d::getInscribedPolygon,"getInscribedPolygon(n,theta_inic) return an inscribed regular polygon with n sides starting in the angle argument")
  .def("getArea", &Circle2d::getArea, "Return the object area.")
  .def("getIx", &Circle2d::Ix)
  .def("getIy", &Circle2d::Iy)
  .def("getPxy", &Circle2d::Pxy)
  .def("getCenterOfMass", &Circle2d::getCenterOfMass, " return the center of mass.")
  ;

#include "2d_polygons/python_interface.tcc"

def("get_convex_hull2d",get_convex_hull2d);
def("get_basic_alpha_shape2d",get_basic_alpha_shape2d);

class_<GeomGroup2d, bases<GeomObj2d> >("GeomGroup2d")
  .def("append",&GeomGroup2d::push_back,"Append the argument to the group.")
  .def("getArea", &GeomGroup2d::getArea, "Return the object area.")
  .def("getIx", &GeomGroup2d::Ix)
  .def("getIy", &GeomGroup2d::Iy)
  .def("getPxy", &GeomGroup2d::Pxy)
  .def("getCenterOfMass", &GeomGroup2d::getCenterOfMass, " return the center of mass.")
  .def("empty", &GeomGroup2d::empty, " return true if the container is empty.")
  .def("size", &GeomGroup2d::size, " return the number of object in the container.")
  ;


class_<Surface3d, bases<GeomObj3d>, boost::noncopyable  >("Surface3d", no_init)
  .def("dist",&Surface3d::dist,"Return the distance from point to this surface.")
  ;

class_<D2to3d, bases<Surface3d>, boost::noncopyable  >("D2to3d", no_init)
  .def("getPlane",&D2to3d::getPlane)
  .def("getIVector",&D2to3d::getIVector, "Return unary vector i.")
  .def("getJVector",&D2to3d::getJVector, "Return unary vector j.")
  .def("getKVector",&D2to3d::getKVector, "Return unary vector k.")  
  ;

class_<GeneralEquationOfPlane, boost::noncopyable>("GeneralEquationOfPlane", no_init)
  .def(init<double,double,double,double>());

double (Plane::*PlaneAngle)(const Plane &p) const= &Plane::getAngle;
double (Plane::*getAngleWithVector3d)(const Vector3d &) const= &Plane::getAngle;
Pos3d (Plane::*Pos3dProjection)(const Pos3d &) const= &Plane::Projection;
Vector3d (Plane::*Vector3dProjection)(const Vector3d &) const= &Plane::Projection;
Line3d (Plane::*Line3dProjection)(const Line3d &) const= &Plane::Projection;
Line3d (Plane::*IntersPlane)(const Plane &) const= &Plane::getIntersection;
Pos3d (Plane::*IntersLine3d)(const Line3d &) const= &Plane::getIntersection;
Pos3d (Plane::*IntersRay3d)(const Ray3d &) const= &Plane::getIntersection;
Pos3d (Plane::*IntersSegment3d)(const Segment3d &) const= &Plane::getIntersection;
double (Plane::*linearLeastSquaresFittingPtr)(const GeomObj3d::list_Pos3d &)= &Plane::linearLeastSquaresFitting;
class_<Plane, bases<Surface3d> >("Plane3d")
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def(init<Pos3d,Vector3d>())
  .def(init<Pos3d,Vector3d,Vector3d>())
  .def(init<Line3d,Pos3d>()) 
  .def(init<GeneralEquationOfPlane>()) 
  .def(init<Plane>())
  .def("getPlaneAngle",PlaneAngle)
  .def("getAngleWithVector3d",getAngleWithVector3d)
  .def("getProjection",Pos3dProjection, "Return the projection of the point.")
  .def("getProjection",Vector3dProjection, "Return the projection of the vector.")
  .def("getProjection",Line3dProjection, "Return the projection of the line.")
  .def("getXYTrace",&Plane::XYTrace,"return the trace on the XY plane.")
  .def("getXZTrace",&Plane::XZTrace,"return the trace on the XZ plane.")
  .def("getYZTrace",&Plane::YZTrace,"return the trace on the YZ plane.")
  // Boost.Python supports function overloading
  .def("getIntersPlane",IntersPlane,"TO DEPRECATE return the intersection with the plane argument.")
  .def("getIntersLine3d",IntersLine3d,"TO DEPRECATE return the intersection with the line argument.")
  .def("getIntersRay3d",IntersRay3d,"TO DEPRECATE return the intersection with the ray argument.")
  .def("getIntersSegment3d",IntersSegment3d,"TO DEPRECATE return the intersection with the segment argument.")
  // Overloaded methods.
  .def("getIntersection",IntersPlane, "return the intersection with the plane argument.")
  .def("getIntersection",IntersLine3d,"return the intersection with the line argument.")
  .def("getIntersection",IntersRay3d,"return the intersection with the ray argument.")
  .def("getIntersection",IntersSegment3d,"return the intersection with the segment argument.")
  .def("getNormal", &Plane::Normal,"return the plane normal.")
  .def("getBase1", &Plane::Base1)
  .def("getBase2", &Plane::Base2)
  .def("linearLeastSquaresFitting", linearLeastSquaresFittingPtr,"compute the plane that best suits the point cloud.")
  .def("positiveSide", &Plane::positiveSide, " return true if the point is on the positive side of the plane (local coordinate z>0).")
  .def("negativeSide", &Plane::negativeSide, " return true if the point is on the negative side of the plane (local coordinate z<0).")
  ;

class_<Polygon3d, bases<D2to3d> >("Polygon3d")
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def(init<const Ref2d3d &,const Polygon2d &>())
  .def(init<const Ref3d3d &,const Polygon2d &>())
  .def(init<const GeomObj::list_Pos3d &>())
  .def(init<boost::python::list>())
  .def("orientation", &Polygon3d::orientation, "orientation(vPoint) return the orientation of the polygon with respect to the point of view argument.")
  .def("clockwise", &Polygon3d::clockwise, "clockwise(vPoint) return true if polygon is clockwise oriented with respect to the point of view argument.")
  .def("counterclockwise", &Polygon3d::counterclockwise, "counterclockwise(vPoint) return true if the polygon is counterclockwise oriented with respect to the point of view argument.")
  .def("appendVertex",&Polygon3d::push_back,"Append the vertex to this polygon.")
  .def("getArea",&Polygon3d::getArea, "Return the object area.")
  .def("getPerimeter",&Polygon3d::getPerimeter, "Return the object perimeter.")
  .def("getVertexList",&Polygon3d::getVertexListPy,"Return a Python list containing the positions of the polygon vertices.")
  .def("dist",&Polygon3d::dist,"Return the distance from point to this polygon.")
  .def("getCenterOfMass", &Polygon3d::getCenterOfMass, " return the center of mass.")
  ;

class_<Circle3d, bases<D2to3d> >("Circle3d")
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def(init<Ref2d3d,Circle2d>())
  .def(init<Ref3d3d,Circle2d>())
  .def("getCenter",&Circle3d::Centro)
  .def("getRadius",&Circle3d::getRadius,"Return the object radius.")
  .def("getDiameter",&Circle3d::getDiameter, "Return the object diameter.")
  .def("getPerimeter",&Circle3d::getPerimeter, "Return the object perimeter.")
  .def("getInscribedPolygon",&Circle3d::getInscribedPolygon,"getInscribedPolygon(n,theta_inic) return an inscribed regular polygon with n sides starting in the angle argument")
  .def("getArea", &Circle3d::getArea, "Return the object area.")
  .def("getIx", &Circle3d::Ix)
  .def("getIy", &Circle3d::Iy)
  .def("getPxy", &Circle3d::Pxy)
  .def("getCenterOfMass", &Circle3d::getCenterOfMass, " return the center of mass.")
  ;

class_<Triangle3d, bases<Polygon3d>  >("Triangle3d")
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def(init<Triangle3d>())
  ;

class_<Grid2d, bases<Surface2d> >("Grid2d")
  .def(init<>())
  .def("getMax",&Grid2d::GetMax)
  .def("getMin",&Grid2d::GetMin)
  .def("getArea",&Grid2d::getArea, "Return the object area.")
  .def("Ix",&Grid2d::Ix)
  .def("Iy",&Grid2d::Iy)
  .def("Pxy",&Grid2d::Pxy)
  .def("getCenterOfMass",&Grid2d::getCenterOfMass, " return the center of mass.")
  .def("getPoint",&Grid2d::getPoint)
  .def("getQuad",&Grid2d::GetQuad)
  .def("In",&Grid2d::In)
  .def("getPoints",make_function(&Grid2d::getPoints,return_internal_reference<>()))
   ;



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

double (HalfSpace3d::*getAngleWithHalfSpace3d)(const HalfSpace3d &) const= &HalfSpace3d::getAngle;
double (HalfSpace3d::*getAngleWithPlane)(const Plane &) const= &HalfSpace3d::getAngle;
double (HalfSpace3d::*getAngleWithVector3d)(const Vector3d &) const= &HalfSpace3d::getAngle;
Ray3d (HalfSpace3d::*clipLine)(const Line3d &) const= &HalfSpace3d::clip;
Ray3d (HalfSpace3d::*clipRay)(const Ray3d &) const= &HalfSpace3d::clip;
Segment3d (HalfSpace3d::*clipSegment)(const Segment3d &) const= &HalfSpace3d::clip;
class_<HalfSpace3d, bases<GeomObj3d> >("HalfSpace3d")
  .def(init<Plane>())
  .def(init<Plane, Pos3d>())
  .def(init<HalfSpace3d>())
  .def("getAngle", getAngleWithHalfSpace3d, "Return the angle between the two half-spaces.")
  .def("getAngle", getAngleWithPlane, "Return the angle between the half-space and the plane.")
  .def("getAngle", getAngleWithVector3d, "Return the angle between the half-space and the vector.")
  .def("getLima", &HalfSpace3d::getLima)
  .def("getNormalExterior", &HalfSpace3d::NormalExterior)
  .def("getNormalInterior", &HalfSpace3d::NormalInterior)
  .def("getBoundaryPlane", &HalfSpace3d::getBoundaryPlane,return_internal_reference<>() )
  .def("clip", clipLine, "Returns the part of the object that is inside the half-space.")
  .def("clip", clipRay, "Returns the part of the object that is inside the half-space.")
  .def("clip", clipSegment, "Returns the part of the object that is inside the half-space.")
 ;

class_<BND3d, bases<GeomObj3d> >("BND3d")
  .def(init<>())
  .def(init<Pos3d, Pos3d>())
  .def(init<BND3d>())
  .add_property("width", &BND3d::getWidth, "returns object width.")
  .add_property("height", &BND3d::getHeight, "returns object height.")
  .add_property("length", &BND3d::getLength, "returns object length.")
  .add_property("diagonal", &BND3d::Diagonal, "returns a vector from PMin to PMax.")
  .add_property("area", &BND3d::getArea, "return object area.")
  .add_property("volume", &BND3d::getVolume, "returns object volume.")


  .def("update", &BND3d::Update)
  .add_property("pMax", &BND3d::GetPMax, &BND3d::PutPMax,"upper right corner.")
  .add_property("pMin", &BND3d::GetPMin, &BND3d::PutPMin,"lower left corner.")

  .def("getIx", &BND3d::Ix)
  .def("getIy", &BND3d::Iy)
  .def("getPxy", &BND3d::Pxy)
  .def("getCenterOfMass", &BND3d::getCenterOfMass)
  .def("getMax", &BND3d::GetMax,"returns maximum value of i coordinate.")
  .def("getMin", &BND3d::GetMin,"returns minimum value of i coordinate.")
  ;

#include "utility/geom/d3/3d_polyhedrons/python_interface.tcc"

class_<GeomGroup3d, bases<GeomObj3d> >("GeomGroup3d")
  .def("append",&GeomGroup3d::push_back,"Append the argument to the group.")
  .def("getArea", &GeomGroup3d::getArea, "Return the object area.")
  .def("getIx", &GeomGroup3d::Ix)
  .def("getIy", &GeomGroup3d::Iy)
  .def("getPxy", &GeomGroup3d::Pxy)
  .def("getCenterOfMass", &GeomGroup3d::getCenterOfMass, " return the center of mass.")
  .def("empty", &GeomGroup3d::empty, " return true if the container is empty.")
  .def("size", &GeomGroup3d::size, " return the number of object in the container.")
  ;

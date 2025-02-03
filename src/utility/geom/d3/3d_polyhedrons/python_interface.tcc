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

class_<PolyhedronBase, bases<GeomObj3d>, boost::noncopyable >("PolyhedronBase", no_init)
  .add_property("area", &PolyhedronBase::getArea, "Return object area.")
  ;

class_<Polyhedron3d, bases<PolyhedronBase> >("Polyhedron3d")
  .def("getVertices", &Polyhedron3d::getVerticesPy, "Return the vertices of the polyhedron")
  .def("pseudoDist", &Polyhedron3d::PseudoDist, "Return the signed distance from the given point to the this polyhedron")
  ;

class_<Tetrahedron3d, bases<PolyhedronBase> >("Polyhedron3d")
  ;

class_<Hexahedron3d, bases<Polyhedron3d> >("Hexahedron3d")
  .def(init<>())
  .def(init<Pos3d, Pos3d, Pos3d, Pos3d, Pos3d, Pos3d, Pos3d, Pos3d>())  
  .add_property("signedVolume", &Hexahedron3d::getVolumeWithSign, "returns object volume with sign.")
  .add_property("volume", &Hexahedron3d::getVolume, "returns object volume.")
  ;

Hexahedron3d (*get_cube_oriented_bounding_box_ref)(const PointCloud3d &, const Ref3d3d &)= &get_oriented_bounding_box;
Hexahedron3d (*get_cube_oriented_bounding_box)(const PointCloud3d &)= &get_oriented_bounding_box;
def("get_oriented_bounding_box", get_cube_oriented_bounding_box_ref, "Return the oriented bounding box that contains the given point cloud and whose edges are parallel to the given reference.");
def("get_oriented_bounding_box", get_cube_oriented_bounding_box, "Return the oriented bounding box that contains the given point cloud.");

//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
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
//python_interface.tcc


XC::Edge *(XC::Face::*getFaceOppositeEdge)(const XC::Edge &)= &XC::Face::getOppositeEdge;
boost::python::list (XC::Face::*get_connected_surfaces_face_py)(void) const= &XC::Face::getConnectedSurfacesPy;
boost::python::list (XC::Face::*get_connected_surfaces_face_set_py)(const XC::SetBase *) const= &XC::Face::getConnectedSurfacesPy;
boost::python::list (XC::Face::*get_connected_bodies_face_py)(void) const= &XC::Face::getConnectedBodiesPy;
boost::python::list (XC::Face::*get_connected_bodies_face_set_py)(const XC::SetBase *) const= &XC::Face::getConnectedBodiesPy;
class_<XC::Face, XC::Face *,bases<XC::CmbEdge>, boost::noncopyable >("Face", no_init)
  .add_property("nDivI", &XC::Face::NDivI, &XC::Face::setNDivI,"get/set the number of divisions of the even sides of the surface.")
  .add_property("nDivJ", &XC::Face::NDivJ, &XC::Face::setNDivJ,"get/set the number of divisions of the odd sides of the surface.")
  .def("orientation", &XC::Face::orientation, "Return the orientation of the face.")
  .def("setKOrientation", &XC::Face::setKOrientation, "Set the orientation of the face normal according to the vector argument.")
  .def("setIOrientation", &XC::Face::setIOrientation, "Set the face i vector as close as possible to the given vector.")
  .def("revolve", &XC::Face::revolve,"Revolve the vertex sequence.")
  .def("getContour",&XC::Face::getContour,"Return the face contour as a 3D polyline.")
  .def("getArea",&XC::Face::getArea,"Return the face area.")
  .def("setElemSizeI",&XC::Face::SetElemSizeI,"setElementSizeI(sz) set the element size on the even sides of the surface.")
  .def("setElemSizeJ",&XC::Face::SetElemSizeJ,"setElementSizeJ(sz) set the element size on the odd sides of the surface.")
  .def("setElemSizeIJ",&XC::Face::SetElemSizeIJ,"setElementSizeIJ(szI, szJ) set the element size on the even and odd sides of the surface.")
  .def("setElemSize",&XC::Face::SetElemSize,"setElementSize(sz, mustBeEven) set the element size on the sides of the surface; if mustBeEven is true the number of divisions in each side is forced to the next even number.")
  .def("getPlane",&XC::Face::getPlane,"Return the plane containing the face.")
  .def("isFlat", &XC::Face::isFlat,"Return true if the face is flat.")
  .def("isWarped", &XC::Face::isWarped,"Return true if the face is not flat.")
  .def("getPolygon",&XC::Face::getPolygon,"Return the contour as a 3D polygon.")
  .def("addHole", &XC::Face::addHole, "Add a hole to the face.")
  .def("getHoles", &XC::Face::getPyHoles, "Return a list with the face holes in it.")
  .def("getOppositeEdge", make_function(getFaceOppositeEdge, return_internal_reference<>()), "Return the opposite side with respect to the one being passed as parameter.")
  .def("getCommonEdges", &XC::Face::getCommonEdgesPy, "Return a list with the edges that are common with the given surface.")

  .add_property("connectedSurfaces", get_connected_surfaces_face_py, "Returns the surfaces connected to this surface.")
  .def("getConnectedSurfaces", get_connected_surfaces_face_set_py, "Returns the surfaces of the given set that are connected to this one.")
  .add_property("connectedBodies", get_connected_bodies_face_py, "Returns the bodies connected to this surface.")
  .def("getConnectedBodies", get_connected_bodies_face_set_py, "Returns the bodies of the given set that are connected to this surface.")

   ;

class_<XC::QuadSurface, XC::QuadSurface *, bases<XC::Face>, boost::noncopyable >("QuadSurface", no_init)
  .def("defGridPoints",&XC::QuadSurface::defGridPoints)
  .add_property("getIVector", &XC::QuadSurface::getIVector,"Return the local x vector.")
  .add_property("getJVector", &XC::QuadSurface::getJVector,"Return the local x vector.")
  .add_property("getKVector", &XC::QuadSurface::getKVector,"Return the local x vector.")
  .def("getLocalAxes",&XC::QuadSurface::getLocalAxes,"Return the surface local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
   ;

class_<XC::PolygonalFace, XC::PolygonalFace *, bases<XC::Face>, boost::noncopyable >("PolygonalFace", no_init)
  .def("genMesh", &XC::PolygonalFace::genMesh, "Generate mesh.")
   ;




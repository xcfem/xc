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

size_t (XC::Pnt::*get_num_connected_edges_pnt)(void) const= &XC::Pnt::getNumConnectedEdges;
size_t (XC::Pnt::*get_num_connected_edges_pnt_set)(const XC::SetBase *) const= &XC::Pnt::getNumConnectedEdges;
boost::python::list (XC::Pnt::*get_connected_edges_pnt_py)(void) const= &XC::Pnt::getConnectedEdgesPy;
boost::python::list (XC::Pnt::*get_connected_edges_pnt_set_py)(const XC::SetBase *) const= &XC::Pnt::getConnectedEdgesPy;
boost::python::list (XC::Pnt::*get_connected_surfaces_pnt_py)(void) const= &XC::Pnt::getConnectedSurfacesPy;
boost::python::list (XC::Pnt::*get_connected_surfaces_pnt_set_py)(const XC::SetBase *) const= &XC::Pnt::getConnectedSurfacesPy;
XC::Node *(XC::Pnt::*getNodePnt)(void)= &XC::Pnt::getNode;
class_<XC::Pnt, XC::Pnt *, bases<XC::EntMdlr>, boost::noncopyable >("Pnt", no_init)
  .add_property("getPos", make_function(&XC::Pnt::getPos, return_internal_reference<>() ),&XC::Pnt::setPos,"TO DEPRECATE: return the position vector of the point.")
  .add_property("pos", make_function(&XC::Pnt::getPos, return_internal_reference<>() ),&XC::Pnt::setPos,"return the position of the point.")
  .def("setPos", &XC::Pnt::setPos,"set the position vector of the point.")
  .def("getVectorPos", &XC::Pnt::VectorPos,"Return the position vector of the point.")
  .add_property("hasNode",&XC::Pnt::hasNode,"True if the point has a node.")
  .add_property("getTagNode",&XC::Pnt::getTagNode,"Returns node's tag.")
  .def("getNode",make_function(getNodePnt, return_internal_reference<>() ),"Returns point's node.")
  .def("setNode", make_function(&XC::Pnt::setNode, return_internal_reference<>() ),"Assigns the given node to this point.")
  .add_property("getNLines", get_num_connected_edges_pnt, "TO DEPRECATE, returns number of lines that touch the point.")
  .add_property("numConnectedEdges", get_num_connected_edges_pnt, "Returns number of edges that touch this point.")
  .def("getNumConnectedEdges", get_num_connected_edges_pnt_set,"Returns number of edges from the given set that touch this point.")
  .add_property("connectedEdgesTags",&XC::Pnt::getConnectedEdgesTags,"Returns the tags of the lines connected to the point.")
  .add_property("connectedEdges", get_connected_edges_pnt_py,"Returns the lines connected to this point.")
  .def("getConnectedEdges", get_connected_edges_pnt_set_py,"Returns the lines from the given set connected to this point.")
  .add_property("connectedSurfaces", get_connected_surfaces_pnt_py, "Returns the surfaces connected to the point.")
  .def("getConnectedSurfaces", get_connected_surfaces_pnt_set_py, "Returns the surfaces of the given set connected to the point.")
  .def("getAverageElementSize", &XC::Pnt::getAverageElementSize,"Returns the centroid of the object.")
   ;

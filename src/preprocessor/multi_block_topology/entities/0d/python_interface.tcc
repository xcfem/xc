//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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

XC::Node *(XC::Pnt::*getNodePnt)(void)= &XC::Pnt::getNode;
class_<XC::Pnt, XC::Pnt *, bases<XC::EntMdlr>, boost::noncopyable >("Pnt", no_init)
  .add_property("getPos", make_function(&XC::Pnt::GetPos, return_internal_reference<>() ),&XC::Pnt::setPos)
  .def("getVectorPos", &XC::Pnt::VectorPos,"Return the position vector of the point.")
  .add_property("hasNode",&XC::Pnt::hasNode,"True if the point has a node.")
  .add_property("getTagNode",&XC::Pnt::getTagNode,"Returns node's tag.")
  .def("getNode",make_function(getNodePnt, return_internal_reference<>() ),"Returns point's node.")
  .add_property("getNLines",&XC::Pnt::getNLines,"Returns number of lines that touch the point.")
  .add_property("getConnectedEdgesTags",&XC::Pnt::getConnectedEdgesTags,"Returns the lines connected to the point.")
   ;

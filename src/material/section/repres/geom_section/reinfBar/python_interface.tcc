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

class_<XC::ReinfBar, XC::ReinfBar *, bases<XC::DiscretBase>, boost::noncopyable >("ReinfBar", no_init)

  .add_property("diameter",&XC::ReinfBar::getDiameter,&XC::ReinfBar::setDiameter,"Bar diameter.")
  .add_property("area",&XC::ReinfBar::getArea,&XC::ReinfBar::setArea,"Bar area.")
  .add_property("pos",make_function(&XC::ReinfBar::getPosition,return_internal_reference<>()),&XC::ReinfBar::setPosition,"Position vector of bar centroid.")
  .add_property("getPos2d",&XC::ReinfBar::getPos2d,"Pos2d of bar centroid.")

  .add_property("getIx",&XC::ReinfBar::Ix,"Moment of inertia about x axis.")
  .add_property("getIy",&XC::ReinfBar::Iy,"Moment of inertia about y axis.")
  .add_property("getIz",&XC::ReinfBar::Iz,"Moment of inertia about z axis.")
  .add_property("getPyz",&XC::ReinfBar::Pyz,"Moment of inertia about z axis.")
  .add_property("getThetaP",&XC::ReinfBar::Theta_p,"Angle of principal inertia axis with reference axis.")
  .add_property("getIAxisDirA",make_function(&XC::ReinfBar::DirEjeI_a,return_internal_reference<>()),"Principal inertia axis direction A.")
  .add_property("getIAxisDirB",make_function(&XC::ReinfBar::DirEjeI_b,return_internal_reference<>()),"Principal inertia axis direction B.")
  .add_property("getI1",&XC::ReinfBar::getI1,"First principal moment of inertia.")
  .add_property("getI2",&XC::ReinfBar::getI2,"Second principal moment of inertia.")
  //.def("getI",&XC::ReinfBar::getI,"(i,j) component of inertia tensor.")
  //.def("getI",&XC::ReinfBar::getI,"(i,j,o) component of inertia tensor about point o.")
  //.def("getI",&XC::ReinfBar::getI,"(O,e) inertia .")
  .def("getIO",&XC::ReinfBar::getIO,"(o) inertia .")
  //.def("getI",&XC::ReinfBar::getI,"inertia tensor.")
  //.def("getI",&XC::ReinfBar::getI,"(o) inertia tensor about o.")
  ;

typedef std::vector<XC::ReinfBar *> vector_ptr_reinf_bar;
class_<vector_ptr_reinf_bar, boost::noncopyable>("vector_ptr_reinf_bar")
  .def(vector_indexing_suite<vector_ptr_reinf_bar>() )
  ;

class_<XC::VectorReinfBar, bases<XC::SectionMassProperties,vector_ptr_reinf_bar>, boost::noncopyable >("VectorReinfBar", no_init);

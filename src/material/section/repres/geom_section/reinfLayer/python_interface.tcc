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
XC::VectorReinfBar &(XC::ReinfLayer::*getReinfBarsRef)(void)= &XC::ReinfLayer::getReinfBars;
class_<XC::ReinfLayer,XC::ReinfLayer *, bases<XC::DiscretBase>, boost::noncopyable >("ReinfLayer", no_init)
  .add_property("numReinfBars",&XC::ReinfLayer::getNumReinfBars,&XC::ReinfLayer::setNumReinfBars,"Number of bars along layer guideline.")
  .add_property("barDiameter",make_function(&XC::ReinfLayer::getReinfBarDiameter, return_value_policy<return_by_value>()),&XC::ReinfLayer::setReinfBarDiameter,"Diameter of bars.")
  .add_property("barArea",make_function(&XC::ReinfLayer::getReinfBarArea, return_value_policy<return_by_value>()),&XC::ReinfLayer::setReinfBarArea,"Area of bars.")
  .add_property("getReinfBars",make_function(getReinfBarsRef,return_internal_reference<>()),"Returns reinforcement bars.")
  .def("getCover",&XC::ReinfLayer::getCover,"returns concrete cover of the bars.")
  .def("getCenterOfMass",&XC::ReinfLayer::getCenterOfMass,"Return the position of the centroid.")
  .def("getArea",&XC::ReinfLayer::getArea,"Return the reinforcement area.")
  ;

class_<XC::SingleBar , bases<XC::ReinfLayer>, boost::noncopyable >("SingleBar", no_init);

class_<XC::CircReinfLayer , bases<XC::ReinfLayer>, boost::noncopyable >("CircReinfLayer", no_init)
  .add_property("initAngle",&XC::CircReinfLayer::getInitAngle,&XC::CircReinfLayer::setInitAngle,"Initial angle.")
  .add_property("finalAngle",&XC::CircReinfLayer::getFinalAngle,&XC::CircReinfLayer::setFinalAngle,"Final angle.")
  .add_property("radius",&XC::CircReinfLayer::getArcRadius,&XC::CircReinfLayer::setArcRadius,"Final angle.")
  .add_property("center",&XC::CircReinfLayer::getCenterPosition,&XC::CircReinfLayer::setCenterPosition,"Center position.")
  ;

class_<XC::StraightReinfLayer , bases<XC::ReinfLayer>, boost::noncopyable >("StraightReinfLayer", no_init)
  .add_property("p1",&XC::StraightReinfLayer::getInitialPos,&XC::StraightReinfLayer::setInitialPos,"Initial position.")
  .add_property("p2",&XC::StraightReinfLayer::getFinalPos,&XC::StraightReinfLayer::setFinalPos,"Final position.")
  .def("getSpacement",&XC::StraightReinfLayer::getSpacement,"Returns the spacement of the bars.")
  ;

typedef std::list<XC::ReinfLayer *> list_ptr_reinf_layer;
class_<list_ptr_reinf_layer, boost::noncopyable>("list_ptr_reinf_layer")
//.def(vector_indexing_suite<list_ptr_reinf_layer>() )
  .def("__iter__", boost::python::iterator<list_ptr_reinf_layer>())
  .def("__len__", &list_ptr_reinf_layer::size)
  .def("clear", &list_ptr_reinf_layer::clear)
  // APPEND NOT ALLOWED .def("append", &std_item<list_ptr_reinf_layer>::add,
  //       with_custodian_and_ward<1,2>()) // to let container keep value
  //.def("__getitem__", &std_item<list_ptr_reinf_layer>::get,
  //      return_value_policy<copy_non_const_reference>())
  // ASSIGN NOT ALLOWED .def("__setitem__", &std_item<list_ptr_reinf_layer>::set,
  //       with_custodian_and_ward<1,2>()) // to let container keep value
  // REMOVE NOT ALLOWED .def("__delitem__", &std_item<list_ptr_reinf_layer>::del)
  ;

class_<XC::ListReinfLayer, bases<XC::SectionMassProperties,list_ptr_reinf_layer>, boost::noncopyable >("ListReinfLayer", no_init)
  .def("newStraightReinfLayer",make_function(&XC::ListReinfLayer::newStraightReinfLayer,return_internal_reference<>()))
  .def("newCircReinfLayer",make_function(&XC::ListReinfLayer::newCircReinfLayer,return_internal_reference<>()))
  .def("newReinfBar",make_function(&XC::ListReinfLayer::newReinfBar,return_internal_reference<>()))
  .add_property("getNumReinfBars",&XC::ListReinfLayer::getNumReinfBars,"Number of bars.")
  .add_property("getSectionGeometry",make_function(&XC::ListReinfLayer::getSectionGeometry,return_internal_reference<>()),"Returns the GeomSection object that owns this bars.")
  .def("getCover",&XC::ListReinfLayer::getCover,"returns concrete cover of the bars.")
  ;

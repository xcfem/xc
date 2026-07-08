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
XC::VectorReinfBar &(XC::ReinfLayer::*getReinfBarsRef)(void)= &XC::ReinfLayer::getReinfBars;
class_<XC::ReinfLayer,XC::ReinfLayer *, bases<XC::DiscretBase>, boost::noncopyable >("ReinfLayer", no_init)
  .add_property("numReinfBars",&XC::ReinfLayer::getNumReinfBars,&XC::ReinfLayer::setNumReinfBars,"Number of bars along layer guideline.")
  .add_property("barDiameter",make_function(&XC::ReinfLayer::getReinfBarDiameter, return_value_policy<return_by_value>()),&XC::ReinfLayer::setReinfBarDiameter,"Diameter of bars.")
  .add_property("barArea",make_function(&XC::ReinfLayer::getReinfBarArea, return_value_policy<return_by_value>()),&XC::ReinfLayer::setReinfBarArea,"Area of bars.")
  .add_property("getReinfBars",make_function(getReinfBarsRef,return_internal_reference<>()),"Returns reinforcement bars.")
  .def("getCover",&XC::ReinfLayer::getCover,"returns concrete cover of the bars.")
  .def("getCenterOfMass",&XC::ReinfLayer::getCenterOfMass,"Return the position of the centroid.")
  .def("getArea",&XC::ReinfLayer::getArea,"Return the reinforcement area.")
  .def("getBnd",&XC::ReinfLayer::getBnd, "Returns reinforcement layer boundary.")
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
  .def("setP1P2", &XC::StraightReinfLayer::setP1P2, "Set the initial and final positions.")
  .def("setSegment", &XC::StraightReinfLayer::setLineSegment, "Set the initial and final positions using the given segment.")
  .def("getSegment", &XC::StraightReinfLayer::getLineSegment, "Get the segment that lies the initial and final positions.")
  .def("getSpacement", &XC::StraightReinfLayer::getSpacement,"Returns the spacement of the bars.")
  ;

class_<XC::PolylineReinfLayer , bases<XC::ReinfLayer>, boost::noncopyable >("PolylineReinfLayer", no_init)
  .def("setPolyline",&XC::PolylineReinfLayer::setPolyline, "Set the polyline where the rebars are located along.")
  .add_property("getPolyline",&XC::PolylineReinfLayer::getPolyline,"Get the polyline where the rebars are located along.")
  .def("getSpacement",&XC::PolylineReinfLayer::getSpacement,"Returns the spacement of the bars.")
  ;

typedef std::list<XC::ReinfLayer *> list_ptr_reinf_layer;
class_<list_ptr_reinf_layer, boost::noncopyable>("list_ptr_reinf_layer")
//.def(vector_indexing_suite<list_ptr_reinf_layer>() )
  .def("__iter__", boost::python::iterator<list_ptr_reinf_layer>())
  .def("__len__", &list_ptr_reinf_layer::size)
  .def("clear", &list_ptr_reinf_layer::clear,"Clear the container.")
  // APPEND NOT ALLOWED .def("append", &std_item<list_ptr_reinf_layer>::add,
  //       with_custodian_and_ward<1,2>()) // to let container keep value
  //.def("__getitem__", &std_item<list_ptr_reinf_layer>::get,
  //      return_value_policy<copy_non_const_reference>())
  // ASSIGN NOT ALLOWED .def("__setitem__", &std_item<list_ptr_reinf_layer>::set,
  //       with_custodian_and_ward<1,2>()) // to let container keep value
  // REMOVE NOT ALLOWED .def("__delitem__", &std_item<list_ptr_reinf_layer>::del)
  ;

XC::StraightReinfLayer *(XC::ListReinfLayer::*reinforce_mid_points_straight_reinf_layer)(const XC::StraightReinfLayer &, const double &)= &XC::ListReinfLayer::reinforceMidPoints;
XC::PolylineReinfLayer *(XC::ListReinfLayer::*reinforce_mid_points_polyline_reinf_layer)(const XC::PolylineReinfLayer &, const double &)= &XC::ListReinfLayer::reinforceMidPoints;
class_<XC::ListReinfLayer, bases<XC::SectionMassProperties,list_ptr_reinf_layer>, boost::noncopyable >("ListReinfLayer", no_init)
  .def("newStraightReinfLayer",make_function(&XC::ListReinfLayer::newStraightReinfLayer,return_internal_reference<>()), "Create a reinforcement layer along a line segment.")
  .def("newPolylineReinfLayer",make_function(&XC::ListReinfLayer::newPolylineReinfLayer,return_internal_reference<>()), "Create a reinforcement layer along a polyline.")
  .def("newCircReinfLayer",make_function(&XC::ListReinfLayer::newCircReinfLayer,return_internal_reference<>()), "Create a circular reinforcement layer.")
  .def("newReinfBar", make_function(&XC::ListReinfLayer::newReinfBar,return_internal_reference<>()), "Create a single rebar.")
  .def("reinforceSegment", make_function(&XC::ListReinfLayer::reinforceSegment,return_internal_reference<>()), "Create the StraightReinfLayer object defining the reinforcement along the given segment.")
  .def("reinforcePolyline", make_function(&XC::ListReinfLayer::reinforcePolyline,return_internal_reference<>()), "Create the PolylineReinfLayer object defining the reinforcement along the given polyline.")
  .def("reinforceMidPoints", make_function(reinforce_mid_points_straight_reinf_layer, return_internal_reference<>()), ".reinforceMidPoints(StraightReinfLayer, rebarDiameter): create the StraightReinfLayer object defining the reinforcement at the mid points of the given one.")
  .def("reinforceMidPoints", make_function(reinforce_mid_points_polyline_reinf_layer, return_internal_reference<>()), ".reinforceMidPoints(PolylineReinfLayer, rebarDiameter): create the PolylineReinfLayer object defining the reinforcement at the mid points of the given one.")
  .add_property("getNumReinfBars",&XC::ListReinfLayer::getNumReinfBars,"Number of bars.")
  .add_property("getSectionGeometry",make_function(&XC::ListReinfLayer::getSectionGeometry,return_internal_reference<>()),"Returns the SectionGeometry object that owns this bars.")
  .def("getCover",&XC::ListReinfLayer::getCover,"returns concrete cover of the bars.")
  .def("getMaterials",&XC::ListReinfLayer::getMaterialsPy, "Return a Python list containing the different materials of the reinforcement.")
  .def("getBnd",&XC::ListReinfLayer::getBnd, "Returns reinforcement layer boundary.")
  ;

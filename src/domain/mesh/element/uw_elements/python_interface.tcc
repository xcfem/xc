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

//Python exposition of the utilities.

//class_<XC::ElemPos, bases<KDTreePos>, boost::noncopyable >("ElemPos", no_init);
typedef XC::ElementBase<4> beam_contact_element_base_4n;
class_<beam_contact_element_base_4n, bases<XC::Element>, boost::noncopyable >("beam_contact_element_base_4n", no_init);

typedef XC::ElemWithMaterial<4,XC::Contact2D> beam_contact_2d_base_4n;
XC::Contact2D &(beam_contact_2d_base_4n::*getContact2DPhysicalProp)(void)= &beam_contact_2d_base_4n::getPhysicalProperties;
class_<beam_contact_2d_base_4n, bases<beam_contact_element_base_4n>, boost::noncopyable >("<beam_contact_2d_base_4n", no_init)
  .add_property("physicalProperties", make_function(getContact2DPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  ;

class_<XC::BeamContact2D, bases<beam_contact_2d_base_4n>, boost::noncopyable >("BeamContact2D", no_init)
  .add_property("width", &XC::BeamContact2D::getWidth, &XC::BeamContact2D::setWidth, "Get/set the width of the wall represented by the beam element in plane strain.")
  .add_property("gapTolerance", &XC::BeamContact2D::getGapTolerance, &XC::BeamContact2D::setGapTolerance, "Get/set the gap tolerance.")
  .add_property("forceTolerance", &XC::BeamContact2D::getForceTolerance, &XC::BeamContact2D::setForceTolerance, "Get/set the force tolerance.")
  .add_property("initialContactFlag", &XC::BeamContact2D::getInitialContactFlag, &XC::BeamContact2D::setInitialContactFlag, "Get/set the initial contact flag (= 0 contact between bodies is initially assumed, =1 NO contact between bodies is initially assumed.")
   ;

typedef XC::ElemWithMaterial<4,XC::Contact3D> beam_contact_3d_base_4n;
XC::Contact3D &(beam_contact_3d_base_4n::*getContact3DPhysicalProp)(void)= &beam_contact_3d_base_4n::getPhysicalProperties;
class_<beam_contact_3d_base_4n, bases<beam_contact_element_base_4n>, boost::noncopyable >("<beam_contact_3d_base_4n", no_init)
  .add_property("physicalProperties", make_function(getContact3DPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  ;

class_<XC::BeamContact3D, bases<beam_contact_3d_base_4n>, boost::noncopyable >("BeamContact3D", no_init)
//.add_property("width", &XC::BeamContact3D::getWidth, &XC::BeamContact3D::setWidth, "Get/set the width of the wall represented by the beam element in plane strain.")
   ;

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

class_<XC::Load, bases<XC::DomainComponent>, boost::noncopyable >("Load", no_init);

class_<XC::NodalLoad, bases<XC::Load>, boost::noncopyable >("NodalLoad", no_init)
  .add_property("getNodeTag", &XC::NodalLoad::getNodeTag,"Returns loaded node tag.")
  .add_property("getNode", make_function(&XC::NodalLoad::getNode,return_internal_reference<>()),"Returns loaded node.")
  .add_property("getForce", make_function(&XC::NodalLoad::getForce,return_internal_reference<>()),"Returns force components.")
  .add_property("getMoment", make_function(&XC::NodalLoad::getMoment,return_internal_reference<>()),"Returns moment components.")
  ;

class_<XC::NodalLoadIter, boost::noncopyable >("NodalLoadIter", no_init)
  .def("next", &XC::NodalLoadIter::operator(), return_internal_reference<>(),"Returns next load.")
   ;

class_<XC::ElementalLoad, bases<XC::Load>, boost::noncopyable >("ElementalLoad", no_init)
  .add_property("numElements", &XC::ElementalLoad::numElements, "Returns the number of loaded elements.")
  .add_property("elementTags", make_function(&XC::ElementalLoad::getElementTags,return_internal_reference<>()),&XC::ElementalLoad::setElementTags)
  ;

class_<XC::ElementalLoadIter, boost::noncopyable >("ElementalLoadIter", no_init)
  .def("next", &XC::ElementalLoadIter::operator(), return_internal_reference<>(),"Returns next load.")
   ;

class_<XC::ElementBodyLoad, bases<XC::ElementalLoad>, boost::noncopyable >("ElementBodyLoad", no_init);

#include "beam_loads/python_interface.tcc"
#include "plane/python_interface.tcc"
#include "volumetric/python_interface.tcc"

class_<XC::ForceReprComponent, bases<XC::DomainComponent>, boost::noncopyable >("ForceReprComponent", no_init);

class_<XC::LoadHandlerMember, bases<CommandEntity,XC::MovableObject>, boost::noncopyable >("LoadHandlerMember", no_init);

#include "pattern/python_interface.tcc"
#include "groundMotion/python_interface.tcc"


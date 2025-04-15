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
class_<XC::Material, bases<XC::MovableObject,XC::TaggedObject>, boost::noncopyable >("Material", no_init)
  .def("commitState", &XC::Material::commitState,"Commit material's state.")
  .def("revertToLastCommit", &XC::Material::revertToLastCommit,"Return the material to the last committed state.")
  .def("revertToStart", &XC::Material::revertToStart,"Return the material to its initial state.")
  .def("getName",&XC::Material::getName,"Return the name of the material.")
  .add_property("name",&XC::Material::getName,"Return the name of the material.")
  .def("getGeneralizedStress", make_function(&XC::Material::getGeneralizedStress, return_internal_reference<>() ),"Return the stress in this material point.")
  .def("getGeneralizedStrain", make_function(&XC::Material::getGeneralizedStrain, return_internal_reference<>() ),"Return the strain in this material point.")
  .add_property("getResponseType",make_function(&XC::Material::getResponseType, return_internal_reference<>()),"Returns the type of response of the material.")
   ;

boost::python::list (XC::ResponseId::*getComponentIndexesFromCodePy_string)(const std::string &) const= &XC::ResponseId::getComponentIndexesFromCodePy;
boost::python::list (XC::ResponseId::*getComponentIndexesFromCodePy_int)(const int &) const= &XC::ResponseId::getComponentIndexesFromCodePy;
class_<XC::ResponseId, bases<XC::ID> >("ResponseId")
  .def(init<boost::python::list &>())
  .def("hasResponse", &XC::ResponseId::hasResponse, "Return true if the given response identifier is found in this object.")
  .def("getString", &XC::ResponseId::getString, "Return the response identifiers in text form as comma separated values in a string.")
  .def("getStringIdentifiers", &XC::ResponseId::getStringIdentifiers, "Return a Python list with the response identifiers in text form.")
  .def("getIdentifiers", &XC::ResponseId::getIdentifiers, "Return a Python list with the response identifiers.")
  .def("getComponentIdFromString", &XC::ResponseId::getComponentIdFromString, "Return the response component identifier that corresponds to the given string.")
  .def("getStringFromComponentId", &XC::ResponseId::getStringFromComponentId, "Return the string label that corresponds to the given response component identifier.")
  .def("getComponentIndexesFromCode", getComponentIndexesFromCodePy_string, "Return the indexes of the response components corresponding to the the given string.")
  .def("getComponentIndexesFromCode", getComponentIndexesFromCodePy_int, "Return the indexes of the response components corresponding to the the given integer.")
  ;

class_<XC::RespPMz, bases<XC::ResponseId>, boost::noncopyable >("RespPMz", no_init);

class_<XC::RespPMzV, bases<XC::ResponseId>, boost::noncopyable >("RespPMzV", no_init);

class_<XC::RespVyP, bases<XC::ResponseId>, boost::noncopyable >("RespVyP", no_init);

class_<XC::RespPMzMy, bases<XC::ResponseId>, boost::noncopyable >("RespPMzMy", no_init);

class_<XC::RespPMzMyT, bases<XC::ResponseId>, boost::noncopyable >("RespPMzMyT", no_init);

class_<XC::RespPMzVyMyVzT, bases<XC::ResponseId>, boost::noncopyable >("RespPMzVyMyVzT", no_init);

class_<XC::RespFiberSectionShear2d, bases<XC::ResponseId>, boost::noncopyable >("RespFiberSectionShear2d", no_init);

class_<XC::RespFiberSectionShear3d, bases<XC::ResponseId>, boost::noncopyable >("RespFiberSectionShear3d", no_init);

class_<XC::RespPVyMz, bases<XC::ResponseId>, boost::noncopyable >("RespPVyMz", no_init);

class_<XC::RespMembraneMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespMembraneMaterial", no_init);

class_<XC::RespPlateMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespPlateMaterial", no_init);

class_<XC::RespShellMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespShellMaterial", no_init);

class_<XC::RespSolidMecanics1DMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespSolidMecanics1DMaterial", no_init);

class_<XC::RespSolidMecanics2DMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespSolidMecanics2DMaterial", no_init);

class_<XC::RespSolidMecanics3DMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespSolidMecanics3DMaterial", no_init);


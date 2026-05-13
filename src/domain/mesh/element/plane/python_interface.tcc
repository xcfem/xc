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

//Elasticity.

XC::SolidMech2D &(ElemWithMaterial4N_Mech2D::*getMech2DPhysicalProp)(void)= &ElemWithMaterial4N_Mech2D::getPhysicalProperties;
void (ElemWithMaterial4N_Mech2D::*ewm4n_mech2d_set_material_ref)(const XC::Material &)= &ElemWithMaterial4N_Mech2D::setMaterial;
void (ElemWithMaterial4N_Mech2D::*ewm4n_mech2d_set_material_name)(const std::string &)= &ElemWithMaterial4N_Mech2D::setMaterial;
class_<ElemWithMaterial4N_Mech2D, bases<ElementBase4N >, boost::noncopyable >("ElemWithMaterial4N_Mech2D", no_init)
  .add_property("physicalProperties", make_function(getMech2DPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
  .add_property("getPhysicalProperties", make_function(getMech2DPhysicalProp, return_internal_reference<>() ),"TO DEPRECATE: use physicalProperties. Returns materials at integration points (gauss points).")
  .add_property("extrapolationMatrix",make_function(&ElemWithMaterial4N_Mech2D::getExtrapolationMatrix,return_internal_reference<>() ),"Returns the element extrapolation matrix.")
  .def("getExtrapolatedValues", &ElemWithMaterial4N_Mech2D::getExtrapolatedValues,"Return the values at nodes from the values at the Gauss points.")
  .def("setMaterial", ewm4n_mech2d_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", ewm4n_mech2d_set_material_name, "Assigns the material with the given name to the element.")
  .def("copyMaterialFrom", &ElemWithMaterial4N_Mech2D::copyMaterialFrom, "Copy the material from the given element.")
   ;

void (PlaneElement4N_Mech2D::*plne4n_mech2d_set_material_ref)(const XC::Material &)= &PlaneElement4N_Mech2D::setMaterial;
void (PlaneElement4N_Mech2D::*plne4n_mech2d_set_material_name)(const std::string &)= &PlaneElement4N_Mech2D::setMaterial;
class_<PlaneElement4N_Mech2D, bases<ElemWithMaterial4N_Mech2D>, boost::noncopyable >("PlaneElement4N_Mech2D", no_init)
  .def("getPerimeter", &PlaneElement4N_Mech2D::getPerimeter, "Returns element's perimeter.")
  .def("getSide", &PlaneElement4N_Mech2D::getSide, "getSide(i, initialGeometry): the i-th side of the element.")
  .def("getMaximumCornerAngle", &PlaneElement4N_Mech2D::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
  .def("orientation", &PlaneElement4N_Mech2D::orientation, "orientation(initialGeometry) return the orientation of the element.")
  .def("setMaterial", plne4n_mech2d_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", plne4n_mech2d_set_material_name, "Assigns the material with the given name to the element.")
   ;

class_<QuadBase4N_Mech2D, bases<PlaneElement4N_Mech2D>, boost::noncopyable >("QuadBase4N_Mech2D", no_init)
  .add_property("rho", &XC::FourNodeQuad::getRho,&XC::FourNodeQuad::setRho, "Get/set the mass density of the element.")
  .add_property("thickness", &XC::FourNodeQuad::getThickness,&XC::FourNodeQuad::setThickness, "Get/set the thickness of the element.")
   ;

class_<XC::SolidMech4N, bases<QuadBase4N_Mech2D>, boost::noncopyable >("SolidMech4N", no_init)
  .def("getMeanInternalForce",&XC::SolidMech4N::getMeanInternalForce, "Return the average value of the internal force that corresponds to the given symbol")
  .def("getMeanInternalDeformation",&XC::SolidMech4N::getMeanInternalDeformation, "Return the average value of the internal deformation that corresponds to the given symbol")
  .def("getLocalCoordinatesOfNode", &XC::SolidMech4N::getLocalCoordinatesOfNode, "Return the local coordinates of the i-th node.")
  ;

#include "fourNodeQuad/python_interface.tcc"

XC::SolidMech2D &(ElemWithMaterial3N_Mech2D::*getMech2D3NPhysicalProp)(void)= &ElemWithMaterial3N_Mech2D::getPhysicalProperties;
void (ElemWithMaterial3N_Mech2D::*ewm3n_mech2d_set_material_ref)(const XC::Material &)= &ElemWithMaterial3N_Mech2D::setMaterial;
void (ElemWithMaterial3N_Mech2D::*ewm3n_mech2d_set_material_name)(const std::string &)= &ElemWithMaterial3N_Mech2D::setMaterial;
class_<ElemWithMaterial3N_Mech2D, bases<XC::ElementBase<3> >, boost::noncopyable >("ElemWithMaterial3N_Mech2D", no_init)
  .add_property("physicalProperties", make_function(getMech2D3NPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
  .add_property("getPhysicalProperties", make_function(getMech2D3NPhysicalProp, return_internal_reference<>() ),"TO DEPRECATE: use physicalProperties. Returns materials at integration points (gauss points).")
  .def("setMaterial", ewm3n_mech2d_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", ewm3n_mech2d_set_material_name, "Assigns the material with the given name to the element.")
   ;

class_<PlaneElement3N_Mech2D, bases<ElemWithMaterial3N_Mech2D>, boost::noncopyable >("PlaneElement3N_Mech2D", no_init)
  .def("getPerimeter", &PlaneElement3N_Mech2D::getPerimeter, "Returns element's perimeter.")
  .def("getSide", &PlaneElement3N_Mech2D::getSide, "getSide(i, initialGeometry): the i-th side of the element.")
  .def("getMaximumCornerAngle", &PlaneElement3N_Mech2D::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
  .def("orientation", &PlaneElement3N_Mech2D::orientation, "orientation(initialGeometry) return the orientation of the element.")
   ;

class_<TriBase3N_Mech2D, bases<PlaneElement3N_Mech2D>, boost::noncopyable >("TriBase3N_Mech2D", no_init)
   ;

#include "triangle/python_interface.tcc"

XC::SectionFDPhysicalProperties &(ElemWithMaterial4N_SFD::*getSectionFDPhysicalProp)(void)= &ElemWithMaterial4N_SFD::getPhysicalProperties;
void (ElemWithMaterial4N_SFD::*ewm4n_sfd_set_material_ref)(const XC::Material &)= &ElemWithMaterial4N_SFD::setMaterial;
void (ElemWithMaterial4N_SFD::*ewm4n_sfd_set_material_name)(const std::string &)= &ElemWithMaterial4N_SFD::setMaterial;
class_<ElemWithMaterial4N_SFD, bases<ElementBase4N >, boost::noncopyable >("ElemWithMaterial4N_SFD", no_init)
  .add_property("physicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
  .add_property("getPhysicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"TO DEPRECATE: use physicalProperties. Returns materials at integration points (gauss points).")
  .def("setMaterial", ewm4n_sfd_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", ewm4n_sfd_set_material_name, "Assigns the material with the given name to the element.")
  .def("copyMaterialFrom", &ElemWithMaterial4N_SFD::copyMaterialFrom, "Copy the material from the given element.")
  .add_property("extrapolationMatrix",make_function(&ElemWithMaterial4N_SFD::getExtrapolationMatrix,return_internal_reference<>() ),"Returns the element extrapolation matrix.")
  .def("getExtrapolatedValues", &ElemWithMaterial4N_SFD::getExtrapolatedValues,"Return the values at nodes from the values at the Gauss points.")
  ;

class_<PlaneElement4N_SFD, bases<ElemWithMaterial4N_SFD>, boost::noncopyable >("PlaneElement4N_SFD", no_init)
  .def("getPerimeter", &PlaneElement4N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getSide", &PlaneElement4N_SFD::getSide, "getSide(i, initialGeometry): the i-th side of the element.")
  .def("getMaximumCornerAngle", &PlaneElement4N_SFD::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
  .def("orientation", &PlaneElement4N_SFD::orientation, "orientation(initialGeometry) return the orientation of the element.")
   ;

void (ElemWithMaterial9N_SFD::*ewm9n_sfd_set_material_ref)(const XC::Material &)= &ElemWithMaterial9N_SFD::setMaterial;
void (ElemWithMaterial9N_SFD::*ewm9n_sfd_set_material_name)(const std::string &)= &ElemWithMaterial9N_SFD::setMaterial;
class_<ElemWithMaterial9N_SFD, bases<ElementBase9N >, boost::noncopyable >("ElemWithMaterial9N_SFD", no_init)
  .add_property("physicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
  .add_property("getPhysicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"TO DEPRECATE: use physicalProperties. Returns materials at integration points (gauss points).")
  .def("setMaterial", ewm9n_sfd_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", ewm9n_sfd_set_material_name, "Assigns the material with the given name to the element.")
   ;

class_<PlaneElement9N_SFD, bases<ElemWithMaterial9N_SFD>, boost::noncopyable >("PlaneElement9N_SFD", no_init)
  .def("getPerimeter", &PlaneElement9N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getSide", &PlaneElement9N_SFD::getSide, "getSide(i, initialGeometry): the i-th side of the element.")
  .def("getMaximumCornerAngle", &PlaneElement9N_SFD::getMaximumCornerAngle, "getMaximumCornerAngle(initialGeometry): return the maximum corner angle quality parameter. If intialGeometry is True the returned value corresponds to its undeformed geometry.")
  .def("orientation", &PlaneElement9N_SFD::orientation, "orientation(initialGeometry) return the orientation of the element.")
   ;

// 9 nodes solid mechanics plane element.
XC::SolidMech2D &(ElemWithMaterial9N_Mech2D::*getMech2D9NPhysicalProp)(void)= &ElemWithMaterial9N_Mech2D::getPhysicalProperties;
void (ElemWithMaterial9N_Mech2D::*ewm9n_mech2d_set_material_ref)(const XC::Material &)= &ElemWithMaterial9N_Mech2D::setMaterial;
void (ElemWithMaterial9N_Mech2D::*ewm9n_mech2d_set_material_name)(const std::string &)= &ElemWithMaterial9N_Mech2D::setMaterial;
class_<ElemWithMaterial9N_Mech2D, bases<ElementBase9N >, boost::noncopyable >("ElemWithMaterial9N_Mech2D", no_init)
  .add_property("physicalProperties", make_function(getMech2D9NPhysicalProp, return_internal_reference<>() ),"Returns materials at integration points (gauss points).")
//  .add_property("extrapolationMatrix",make_function(&ElemWithMaterial9N_Mech2D::getExtrapolationMatrix,return_internal_reference<>() ),"Returns the element extrapolation matrix.")
//  .def("getExtrapolatedValues", &ElemWithMaterial9N_Mech2D::getExtrapolatedValues,"Return the values at nodes from the values at the Gauss points.")
  .def("setMaterial", ewm9n_mech2d_set_material_ref, "Assigns the given material to the element.")
  .def("setMaterial", ewm9n_mech2d_set_material_name, "Assigns the material with the given name to the element.")
   ;

#include "UP-ucsd/python_interface.tcc"
//Shell
#include "shell/python_interface.tcc"

// Surface pressures.
#include "surface_pressures/python_interface.tcc"

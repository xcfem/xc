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


typedef XC::QuadBase4N<XC::SectionFDPhysicalProperties> QuadBase4N_SFD;
class_<QuadBase4N_SFD, bases<PlaneElement4N_SFD>, boost::noncopyable >("QuadBase4N_SFD", no_init)
  .def("getPerimeter", &QuadBase4N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &QuadBase4N_SFD::getArea, "Returns element's area.")
   ;

XC::ShellCrdTransf3dBase *(XC::Shell4NBase::*getCoordTransfRef)(void)= &XC::ShellMITC4::getCoordTransf;
XC::Vector (XC::Shell4NBase::*getParticlePos3dInterpolationFactors)(const XC::ParticlePos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::Shell4NBase::*getPos3dInterpolationFactors)(const Pos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::Shell4NBase::*getParticlePos3dInterpolatedDisplacements)(const XC::ParticlePos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::Shell4NBase::*getPos3dInterpolatedDisplacements)(const Pos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
class_<XC::Shell4NBase, bases<QuadBase4N_SFD>, boost::noncopyable >("Shell4NBase", no_init)
    .def("computeBasis", &XC::ShellMITC4Base::computeBasis,"Compute local coordinates and basis")
    .add_property("getCoordTransf", make_function( getCoordTransfRef, return_internal_reference<>() ))
    .def("getLocalCoordinatesOfNode", &XC::ShellMITC4Base::getLocalCoordinatesOfNode,"Returns local coordinates of node i.")
    .def("getNaturalCoordinates", &XC::ShellMITC4Base::getNaturalCoordinates,"Returns natural coordinates of point.")
    .def("getMeanInternalForce",&XC::ShellMITC4Base::getMeanInternalForce)
    .def("getMeanInternalDeformation",&XC::ShellMITC4Base::getMeanInternalDeformation)
    .def("getPos3dInterpolationFactors",getPos3dInterpolationFactors)
    .def("getParticlePos3dInterpolationFactors",getParticlePos3dInterpolationFactors)
    .def("getParticlePos3dInterpolatedDisplacements",getParticlePos3dInterpolatedDisplacements)
    .def("getPos3dInterpolatedDisplacements",getPos3dInterpolatedDisplacements)
    .def("vector3dUniformLoadLocal",make_function(&XC::ShellMITC4Base::vector3dUniformLoadLocal, return_internal_reference<>() ))
    .def("vector3dUniformLoadGlobal",make_function(&XC::ShellMITC4Base::vector3dUniformLoadGlobal, return_internal_reference<>() ))
    .def("strainLoad",&XC::ShellMITC4Base::strainLoad)
   ;

class_<XC::ShellMITC4Base, bases<XC::Shell4NBase>, boost::noncopyable >("ShellMITC4Base", no_init)
   ;

class_<XC::ShellMITC4, bases<XC::ShellMITC4Base>, boost::noncopyable >("ShellMITC4", no_init)
   ;

typedef XC::QuadBase9N<XC::SectionFDPhysicalProperties> QuadBase9N_SFD;
class_<QuadBase9N_SFD, bases<PlaneElement9N_SFD>, boost::noncopyable >("QuadBase9N_SFD", no_init)
  .def("getPerimeter", &QuadBase9N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &QuadBase9N_SFD::getArea, "Returns element's area.")
   ;

XC::ShellCrdTransf3dBase *(XC::ShellMITC9::*getMITC9CoordTransfRef)(void)= &XC::ShellMITC9::getCoordTransf;
class_<XC::ShellMITC9, bases<QuadBase9N_SFD>, boost::noncopyable >("ShellMITC9", no_init)
    .add_property("getCoordTransf", make_function( getMITC9CoordTransfRef, return_internal_reference<>() ))
   ;

class_<XC::ShellNLDKGQ, bases<XC::Shell4NBase>, boost::noncopyable >("ShellNLDKGQ", no_init)
   ;

class_<XC::ASDShellQ4, bases<QuadBase4N_SFD>, boost::noncopyable >("ASDShellQ4", no_init)
   ;

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

const XC::Vector &(XC::ShellCrdTransf3dBase::*getVGlobalFromLocal)(const XC::Vector &) const= &XC::ShellCrdTransf3dBase::getVectorGlobalCoordFromLocal;
const XC::Vector &(XC::ShellCrdTransf3dBase::*getVLocalFromGlobal)(const XC::Vector &) const= &XC::ShellCrdTransf3dBase::getVectorLocalCoordFromGlobal;
class_<XC::ShellCrdTransf3dBase, bases<CommandEntity>, boost::noncopyable >("ShellCrdTransf3dBase", no_init)
  .add_property("getTrfMatrix", &XC::ShellCrdTransf3dBase::getTrfMatrix,"Returns the transformation matrix.")
  .add_property("getLocalAxes", &XC::ShellCrdTransf3dBase::getTrfMatrix,"Returns transformation local axes as matrix rows: [[x1,y1,z1],[x2,y2,z2],...·] .")
  .add_property("getG1Vector", make_function(&XC::ShellCrdTransf3dBase::G1, return_internal_reference<>() ))
  .add_property("getG2Vector", make_function(&XC::ShellCrdTransf3dBase::G2, return_internal_reference<>() ))
  .add_property("getG3Vector", make_function(&XC::ShellCrdTransf3dBase::G3, return_internal_reference<>() ))
  .add_property("getVPosCentroide", make_function(&XC::ShellCrdTransf3dBase::getVPosCentroide, return_internal_reference<>() ))
  .add_property("getPlane", &XC::ShellCrdTransf3dBase::getPlane)
  .def("update", &XC::ShellCrdTransf3dBase::update)
  .def("getBasicTrialDisp", &XC::ShellCrdTransf3dBase::getBasicTrialDisp)
  .def("getBasicTrialVel", &XC::ShellCrdTransf3dBase::getBasicTrialVel)
  .def("getBasicTrialAccel", &XC::ShellCrdTransf3dBase::getBasicTrialAccel)
  .def("getProj", &XC::ShellCrdTransf3dBase::getProj)
  .add_property("getLocalReference", &XC::ShellCrdTransf3dBase::getLocalReference)
  .def("getLocalCoordinates", &XC::ShellCrdTransf3dBase::getLocalCoordinates,"Returns local coordinates of a point.")
  .def("getNaturalCoordinates", &XC::ShellCrdTransf3dBase::getNaturalCoordinates,"Returns local coordinates of a point.")
  .def("getVectorGlobalCoordFromLocal", getVGlobalFromLocal, return_value_policy<copy_const_reference>(),"Returns global coordinates of a vector.")
  .def("getVectorLocalCoordFromGlobal", getVLocalFromGlobal,  return_value_policy<copy_const_reference>(),"Returns local coordinates of a vector.")
   ;

class_<XC::ShellLinearCrdTransf3d, bases<XC::ShellCrdTransf3dBase>, boost::noncopyable >("ShellLinearCrdTransf3d", no_init)
   ;

typedef XC::QuadBase4N<XC::SectionFDPhysicalProperties> QuadBase4N_SFD;
class_<QuadBase4N_SFD, bases<PlaneElement4N_SFD>, boost::noncopyable >("QuadBase4N_SFD", no_init)
  .def("getPerimeter", &QuadBase4N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &QuadBase4N_SFD::getArea, "Returns element's area.")
   ;


XC::ShellCrdTransf3dBase *(XC::ShellMITC4Base::*getCoordTransfRef)(void)= &XC::ShellMITC4::getCoordTransf;
XC::Vector (XC::ShellMITC4Base::*getParticlePos3dInterpolationFactors)(const XC::ParticlePos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::ShellMITC4Base::*getPos3dInterpolationFactors)(const Pos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::ShellMITC4Base::*getParticlePos3dInterpolatedDisplacements)(const XC::ParticlePos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::ShellMITC4Base::*getPos3dInterpolatedDisplacements)(const Pos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
class_<XC::ShellMITC4Base, bases<QuadBase4N_SFD>, boost::noncopyable >("ShellMITC4Base", no_init)
    .add_property("getCoordTransf", make_function( getCoordTransfRef, return_internal_reference<>() ))
    .def("computeBasis", &XC::ShellMITC4Base::computeBasis,"Compute local coordinates and basis")
    .def("getLocalCoordinatesOfNode", &XC::ShellMITC4Base::getLocalCoordinatesOfNode,"Returns local coordinates of node i.")
    .def("getNaturalCoordinates", &XC::ShellMITC4Base::getNaturalCoordinates,"Returns natural coordinates of point.")
    .def("getMeanInternalForce",&XC::ShellMITC4Base::getMeanInternalForce)
    .def("getMeanInternalDeformation",&XC::ShellMITC4Base::getMeanInternalDeformation)
    .def("vector3dUniformLoadLocal",make_function(&XC::ShellMITC4Base::vector3dUniformLoadLocal, return_internal_reference<>() ))
    .def("vector3dUniformLoadGlobal",make_function(&XC::ShellMITC4Base::vector3dUniformLoadGlobal, return_internal_reference<>() ))
    .def("strainLoad",&XC::ShellMITC4Base::strainLoad)
    .def("getParticlePos3dInterpolationFactors",getParticlePos3dInterpolationFactors)
    .def("getPos3dInterpolationFactors",getPos3dInterpolationFactors)
    .def("getParticlePos3dInterpolatedDisplacements",getParticlePos3dInterpolatedDisplacements)
    .def("getPos3dInterpolatedDisplacements",getPos3dInterpolatedDisplacements)
   ;

class_<XC::ShellMITC4, bases<XC::ShellMITC4Base>, boost::noncopyable >("ShellMITC4", no_init)
   ;

typedef XC::QuadBase9N<XC::SectionFDPhysicalProperties> QuadBase9N_SFD;
class_<QuadBase9N_SFD, bases<PlaneElement9N_SFD>, boost::noncopyable >("QuadBase9N_SFD", no_init)
  .def("getPerimeter", &QuadBase9N_SFD::getPerimeter, "Returns element's perimeter.")
  .def("getArea", &QuadBase9N_SFD::getArea, "Returns element's area.")
   ;

class_<XC::ShellMITC9, bases<QuadBase9N_SFD>, boost::noncopyable >("ShellMITC9", no_init)
   ;


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

class_<XC::BidimLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("BidimLoad", no_init)
  .add_property("category", &XC::BidimLoad::Category)
  ;

typedef XC::BidimMecLoad<3> bidim_mec_load_3;
class_<bidim_mec_load_3, bases<XC::BidimLoad>, boost::noncopyable >(" bidim_mec_load_3", no_init)
  .def("getResultant", &bidim_mec_load_3::getResultant,"Returns load resultant (force and moment integration over the elements).")
  .add_property("getForceVectorDimension", &bidim_mec_load_3::getForceVectorDimension)
  .def("getLocalForce",&bidim_mec_load_3::getLocalForce,"Returns local coordinates or force vector.")
  .def("getVector3dLocalForce",&bidim_mec_load_3::getVector3dLocalForce,"Returns a Vector3d with the local coordinates of the force vector.")
  .def("getLocalForces",make_function(&bidim_mec_load_3::getLocalForces, return_internal_reference<>() ))
  .def("getGlobalVectors",make_function(&bidim_mec_load_3::getGlobalVectors, return_internal_reference<>() ))
  .def("getGlobalForces",make_function(&bidim_mec_load_3::getGlobalForces, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  ;

class_<XC::ShellMecLoad, bases<bidim_mec_load_3>, boost::noncopyable >("ShellMecLoad", no_init)
  .add_property("getMomentVectorDimension", &XC::ShellMecLoad::getMomentVectorDimension)
  .def("getLocalMoment",&XC::ShellMecLoad::getLocalMoment,"Returns local coordinates of bending moment vector.")
  .def("getVector3dLocalMoment",&XC::ShellMecLoad::getVector3dLocalMoment,"Returns a Vector3d with the local coordinates of bending moment vector.")
  .def("getLocalMoments",make_function(&XC::ShellMecLoad::getLocalMoments, return_internal_reference<>() ))
  .def("getGlobalMoments",make_function(&XC::ShellMecLoad::getGlobalMoments, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  ;

class_<XC::ShellUniformLoad, bases<XC::ShellMecLoad>, boost::noncopyable >("ShellUniformLoad", no_init)
  .add_property("axial1Component", &XC::ShellUniformLoad::getAxial1Component, &XC::ShellUniformLoad::setAxial1Component)
  .add_property("axial2Component", &XC::ShellUniformLoad::getAxial2Component, &XC::ShellUniformLoad::setAxial2Component)
  .add_property("transComponent", &XC::ShellUniformLoad::getTransComponent, &XC::ShellUniformLoad::setTransComponent)
  .add_property("Wx",make_function(&XC::ShellUniformLoad::Wx, return_value_policy <copy_const_reference> ()))
  .add_property("Wy",make_function(&XC::ShellUniformLoad::Wy, return_value_policy <copy_const_reference> ()))
  .add_property("Wz",make_function(&XC::ShellUniformLoad::Wz, return_value_policy <copy_const_reference> ()))
  .def("getLocalPressures",make_function(&XC::ShellUniformLoad::getLocalPressures, return_internal_reference<>() ),"Returns pressure vectors (one for each element) expressed in element local coordinates.")
  .def("getDistributedLocalMoments",make_function(&XC::ShellUniformLoad::getDistributedLocalMoments, return_internal_reference<>() ),"Returns distributed moment vectors (one for each element) expressed in element local coordinates.")
  .def("getGlobalPressures",make_function(&XC::ShellUniformLoad::getGlobalPressures, return_internal_reference<>() ),"Returns pressure vectors (one for each element) expressed in global coordinates.")
  .def("getDistributedGlobalMoments",make_function(&XC::ShellUniformLoad::getDistributedGlobalMoments, return_internal_reference<>() ),"Returns distributed moment vectors (one for each element) expressed in global coordinates.")
  .def("getResultant",&XC::ShellUniformLoad::getResultant,"Returns the load resultant (integration of the pressures over the elements).")
  ;

class_<XC::ShellRawLoad, bases<XC::ShellMecLoad>, boost::noncopyable >("ShellRawLoad", no_init)
  ;


typedef XC::BidimMecLoad<2> bidim_mec_load_2;
class_<bidim_mec_load_2, bases<XC::BidimLoad>, boost::noncopyable >(" bidim_mec_load_2", no_init)
  .def("getResultant", &bidim_mec_load_2::getResultant,"Returns load resultant (force and moment integration over the elements).")
  .add_property("getForceVectorDimension", &bidim_mec_load_2::getForceVectorDimension)
  .def("getLocalForce",&bidim_mec_load_2::getLocalForce,"Returns local coordinates or force vector.")
  .def("getVector3dLocalForce",&bidim_mec_load_2::getVector3dLocalForce,"Returns a Vector3d with the local coordinates of the force vector.")
  .def("getLocalForces",make_function(&bidim_mec_load_2::getLocalForces, return_internal_reference<>() ))
  .def("getGlobalVectors",make_function(&bidim_mec_load_2::getGlobalVectors, return_internal_reference<>() ))
  .def("getGlobalForces",make_function(&bidim_mec_load_2::getGlobalForces, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  ;

class_<XC::QuadMecLoad, bases<bidim_mec_load_2>, boost::noncopyable >("QuadMecLoad", no_init)
  ;

class_<XC::QuadRawLoad, bases<XC::QuadMecLoad>, boost::noncopyable >("QuadRawLoad", no_init)
  ;

class_<XC::BidimStrainLoad, bases<XC::BidimLoad>, boost::noncopyable >("BidimStrainLoad", no_init)
  .def("getStrain",make_function(&XC::BidimStrainLoad::getStrain, return_internal_reference<>() ), "getStrain(gaussPointIndex) return the strain at the given gauss point.")
  .def("getStrains", &XC::BidimStrainLoad::getStrainsPy, "Return the values of the strains for each gauss point.")
  .def("setStrainComp",&XC::BidimStrainLoad::setStrainComp, "setStrainComp(gaussPointIndex, j, strainValue), set the j-th component of the strain at the given gauss point")
  .def("getElementStrainsMatrix", &XC::BidimStrainLoad::getElementStrainsMatrix, "Return a matrix containing the element gauss points strain tensors as rows.") 
  ;

class_<XC::QuadStrainLoad, bases<XC::BidimStrainLoad>, boost::noncopyable >("QuadStrainLoad", no_init)
  ;

class_<XC::ShellStrainLoad, bases<XC::BidimStrainLoad>, boost::noncopyable >("ShellStrainLoad", no_init)
  ;




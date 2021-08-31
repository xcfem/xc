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

class_<XC::ElasticBeam2dBase, bases<XC::ProtoBeam2d>, boost::noncopyable >("ElasticBeam2dBase", no_init)
  ;

class_<XC::ElasticBeam2d, bases<XC::ElasticBeam2dBase>, boost::noncopyable >("ElasticBeam2d", no_init)
  .add_property("rho", &XC::ElasticBeam2d::getRho,&XC::ElasticBeam2d::setRho, "element density.")
  .add_property("h", &XC::ElasticBeam2d::getDepth,&XC::ElasticBeam2d::setDepth, "element section depth.")
  .add_property("initialStrain", make_function(&XC::ElasticBeam2d::getInitialStrain, return_value_policy<copy_const_reference>()),&XC::ElasticBeam2d::setInitialStrain, "initial strain")
  .add_property("release", &XC::ElasticBeam2d::getReleaseCode,&XC::ElasticBeam2d::setReleaseCode, "element moment release code 0: no release, 1: node I, 2: node J, 2: both nodes.")
  .def("getV", &XC::ElasticBeam2d::getV, "Average internal shear force.")
  .add_property("getV1", &XC::ElasticBeam2d::getV1, "Internal shear force at back end.")
  .add_property("getV2", &XC::ElasticBeam2d::getV2, "Internal shear force at front end.")
  .add_property("getVy1", &XC::ElasticBeam2d::getV1, "Internal shear force at back end.")
  .add_property("getVy2", &XC::ElasticBeam2d::getV2, "Internal shear force at front end.") 
  .add_property("getN1", &XC::ElasticBeam2d::getN1, "Internal axial force at front end.")
  .add_property("getN2", &XC::ElasticBeam2d::getN2, "Internal axial force at back end.")
  .def("getN", &XC::ElasticBeam2d::getN,"Average axial force (call 'calc_resisting_force' before).")
  .add_property("getM1", &XC::ElasticBeam2d::getM1, "Internal bending moment at back end.")
  .add_property("getM2", &XC::ElasticBeam2d::getM2, "Internal bending moment at front end.")
  .add_property("getMz1", &XC::ElasticBeam2d::getM1, "Internal bending moment at back end.")
  .add_property("getMz2", &XC::ElasticBeam2d::getM2, "Internal bending moment at front end.")
  .def("getVDirStrongAxisGlobalCoord",make_function(&XC::ElasticBeam2d::getVDirStrongAxisGlobalCoord, return_value_policy<copy_const_reference>()),"Returns the direction vector of element strong axis expressed in the global coordinate system.")
  .def("getVDirWeakAxisGlobalCoord",make_function(&XC::ElasticBeam2d::getVDirWeakAxisGlobalCoord, return_value_policy<copy_const_reference>()),"Returns the direction vector of element weak axis expressed in the global coordinate system.")
   ;

class_<XC::ElasticTimoshenkoBeam2d, bases<XC::ElasticBeam2dBase>, boost::noncopyable >("ElasticTimoshenkoBeam2d", no_init)
  ;

class_<XC::ElasticBeam3d, bases<XC::ProtoBeam3d>, boost::noncopyable >("ElasticBeam3d", no_init)
  .add_property("rho", &XC::ElasticBeam3d::getRho,&XC::ElasticBeam3d::setRho, "element material density")
  .add_property("initialStrain", make_function(&XC::ElasticBeam3d::getInitialStrain, return_value_policy<copy_const_reference>()),&XC::ElasticBeam3d::setInitialStrain,"initial strain")
  .add_property("getAN2", &XC::ElasticBeam3d::getAN2,"Axial force which acts over the front end of the element (call 'calc_resisting_force' before).")
  .add_property("getN1", &XC::ElasticBeam3d::getN1,"Axial force in the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getN2", &XC::ElasticBeam3d::getN2,"Axial force in the front end of the element (call 'calc_resisting_force' before).")
  .def("getN", &XC::ElasticBeam3d::getN,"Average axial force (call 'calc_resisting_force' before).")
  .add_property("getAMz1", &XC::ElasticBeam3d::getAMz1,"Z bending moment which acts over the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getAMz2", &XC::ElasticBeam3d::getAMz2,"Z bending moment which acts over the front end of the element (call 'calc_resisting_force' before).")
  .add_property("getMz1", &XC::ElasticBeam3d::getMz1,"Z bending moment at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getMz2", &XC::ElasticBeam3d::getMz2,"Z bending moment at the front end of the element (call 'calc_resisting_force' before).")
  .def("getVy", &XC::ElasticBeam3d::getVy, "Average Y shear force (call 'calc_resisting_force' before).")
  .add_property("getAVy1", &XC::ElasticBeam3d::getAVy1, "Y shear force which acts at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getAVy2", &XC::ElasticBeam3d::getAVy2, "Y shear force which acts at the front end of the element (call 'calc_resisting_force' before).")
  .add_property("getVy1", &XC::ElasticBeam3d::getVy1, "Y shear force at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getVy2", &XC::ElasticBeam3d::getVy2, "Y shear force at the front end of the element (call 'calc_resisting_force' before).")
  .def("getVz", &XC::ElasticBeam3d::getVz, "Average Z shear force (call 'calc_resisting_force' before).")
  .add_property("getAVz1", &XC::ElasticBeam3d::getAVz1, "Z shear force which acts at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getAVz2", &XC::ElasticBeam3d::getAVz2, "Z shear force which acts at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getVz1", &XC::ElasticBeam3d::getVz1, "Z shear force at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getVz2", &XC::ElasticBeam3d::getVz2, "Z shear force at the front end of the element (call 'calc_resisting_force' before).")
  .add_property("getMy1", &XC::ElasticBeam3d::getMy1,"Y bending moment at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getMy2", &XC::ElasticBeam3d::getMy2,"Y bending moment at the front end of the element (call 'calc_resisting_force' before).")
  .def("getT", &XC::ElasticBeam3d::getT,"Average torque at the element (call 'calc_resisting_force' before).")
  .add_property("getT1", &XC::ElasticBeam3d::getT1,"Torque at the back end of the element (call 'calc_resisting_force' before).")
  .add_property("getT2", &XC::ElasticBeam3d::getT2,"Torque at the front end of the element (call 'calc_resisting_force' before).")

  .def("getVDirStrongAxisGlobalCoord",make_function(&XC::ElasticBeam3d::getVDirStrongAxisGlobalCoord, return_value_policy<copy_const_reference>()),"Returns the direction vector of element strong axis expressed in the global coordinate system.")
  .def("getVDirWeakAxisGlobalCoord",make_function(&XC::ElasticBeam3d::getVDirWeakAxisGlobalCoord, return_value_policy<copy_const_reference>()),"Returns the direction vector of element weak axis expressed in the global coordinate system.")
   ;

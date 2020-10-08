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

class_<XC::NDMaterial, XC::NDMaterial *, bases<XC::Material>, boost::noncopyable >("NDMaterial", no_init)
    .add_property("getRho", &XC::NDMaterial::getRho,"Return the material density.")
    .add_property("getE", &XC::NDMaterial::getE)
    .add_property("getnu", &XC::NDMaterial::getnu)
    .add_property("getpsi", &XC::NDMaterial::getpsi)
    .def("getTangent",make_function(&XC::NDMaterial::getTangent,return_internal_reference<>()), "Return material stiffness matrix.")
    .def("getInitialTangent",make_function(&XC::NDMaterial::getInitialTangent,return_internal_reference<>()), "Return material initial stiffness matrix.")
    .add_property("getVonMisesStress", &XC::NDMaterial::getVonMisesStress)
       ;

class_<XC::ElasticIsotropicMaterial, bases<XC::NDMaterial>, boost::noncopyable >("ElasticIsotropicMaterial", no_init)
    .add_property("rho", &XC::ElasticIsotropicMaterial::getRho, &XC::ElasticIsotropicMaterial::setRho)
    .add_property("E", &XC::ElasticIsotropicMaterial::getE, &XC::ElasticIsotropicMaterial::setE)
    .add_property("nu", &XC::ElasticIsotropicMaterial::getnu, &XC::ElasticIsotropicMaterial::setnu)
       ;
#include "elastic_isotropic/python_interface.tcc"

//class_<XC::FeapMaterial , bases<XC::NDMaterial>, boost::noncopyable >("FeapMaterial", no_init);
#include "feap/python_interface.tcc"

//#include "FiniteDeformation/python_interface.tcc"

#include "j2_plasticity/python_interface.tcc"

#include "nd_adaptor/python_interface.tcc"

#include "soil/python_interface.tcc"

typedef std::vector<XC::NDMaterial *> vectorNDMaterial;
class_<vectorNDMaterial,boost::noncopyable>("vectorNDMaterial")
  .def(vector_indexing_suite<vectorNDMaterial>() )
  ;

typedef XC::MaterialVector<XC::NDMaterial> material_vector_NDMat;
class_<material_vector_NDMat, bases<vectorNDMaterial, CommandEntity>, boost::noncopyable>("MaterialVectorNDMat", no_init)
  .def("commitState", &material_vector_NDMat::commitState,"Commits materials state.")
  .def("revertToLastCommit", &material_vector_NDMat::revertToLastCommit,"Returns the material to its last committed state.")
  .def("revertToStart", &material_vector_NDMat::revertToStart,"Returns the material to its initial state.")
  .add_property("generalizedStresses", &material_vector_NDMat::getGeneralizedStresses, "Return generalized stresses.")
  .add_property("generalizedStrains", &material_vector_NDMat::getGeneralizedStrains, "Return generalized strains.")
  .def("getNames",&material_vector_NDMat::getNamesPy,"Returns the names of the materials.")
  .def("getValues",&material_vector_NDMat::getValues,"Ask the materials for the values corresponding to the argument code (stress, strain, ...).")
  ;


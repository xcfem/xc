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

#include "analysis/python_interface.tcc"
#include "handler/python_interface.tcc"
#include "numberer/python_interface.tcc"
#include "algorithm/python_interface.tcc"
#include "integrator/python_interface.tcc"
#include "convergenceTest/python_interface.tcc"

XC::DOF_Numberer *(XC::ModelWrapper::*model_wrapper_get_dof_numberer_ptr)(void)= &XC::ModelWrapper::getDOF_NumbererPtr;
class_<XC::ModelWrapper, bases<CommandEntity>, boost::noncopyable >("ModelWrapper","\n" "Wrapper for the finite element model 'seen' from the solver. \n" "The model wrapper is a container for: \n""- Domain of the finite element model. \n""- Analysis model. \n""- Constraint handler. \n""- DOF numberer. \n",no_init)
// DOF numberer.
  .def("newNumberer", &XC::ModelWrapper::newNumberer,return_internal_reference<>(),"\n""newNumberer(nmb)\n""Create a new DOF numberer\n""Parameters: \n""nmb: name of the type of numberer. Available types of numberers: 'default_numberer', 'plain_numberer', 'parallel_numberer'. \n")
.def("getNumberer", make_function(model_wrapper_get_dof_numberer_ptr, return_internal_reference<>()), "Return the DOF numberer.")
// Constraint handler.
  .def("newConstraintHandler", &XC::ModelWrapper::newConstraintHandler,return_internal_reference<>(),"\n""newConstraintHandler(nmb)\n""Create a new constraint handler. \n""Parameters: \n"" nmb: name of the type of handler. Available types of constraint handlers: 'lagrange_constraint_handler', 'penalty_constraint_handler', 'plain_handler', 'transformation_constraint_handler'. \n") 
  .add_property("name",&XC::ModelWrapper::getName,"Return the name of the model wrapper.")
    ;

XC::ModelWrapper *(XC::MapModelWrapper::*get_model_wrapper_ptr)(const std::string &)= &XC::MapModelWrapper::getModelWrapper;
class_<XC::MapModelWrapper, bases<CommandEntity>, boost::noncopyable >("MapModelWrapper", "Finite element model wrappers container.",no_init)
  .add_property("existeModelWrapper", &XC::MapModelWrapper::existeModelWrapper,"\n""existeModelWrapper(cod) \n""Return TRUE if the model wrapper has been created \n""Parameters: \n""cod: name of the model wrapper")
  .add_property("modelWrapperexists", &XC::MapModelWrapper::existeModelWrapper,"\n""modelWrapperExists(cod) \n""Return TRUE if the model wrapper has been created \n""Parameters: \n""cod: name of the model wrapper")
  .def("newModelWrapper", &XC::MapModelWrapper::creaModelWrapper,return_internal_reference<>(),"Create a new container of ModelWrappers.")
  .def("getModelWrapper", make_function(get_model_wrapper_ptr, return_internal_reference<>()), "Return the model wrapper with the given name (if exists).")
  ;

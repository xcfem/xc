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

#include "analysis/python_interface.tcc"
#include "handler/python_interface.tcc"
#include "numberer/python_interface.tcc"
#include "algorithm/python_interface.tcc"
#include "integrator/python_interface.tcc"
#include "convergenceTest/python_interface.tcc"

class_<XC::ModelWrapper, bases<EntCmd>, boost::noncopyable >("ModelWrapper", "Wrapper for the finite element model 'seen' from the solver. \n" "The model wrapper is a container for: \n""- Domain of the finite element model. \n""- Analysis model. \n""- Constraint handler. \n""- DOF numberer. \n",no_init)
    .def("newNumberer", &XC::ModelWrapper::newNumberer,return_internal_reference<>(),"newNumberer(nmb)\n""Create a new DOF numberer of the type whose name is passed as parameter.\n" "Available names for the type of numberer: 'default_numberer', 'plain_numberer', 'parallel_numberer' \n")
    .def("newConstraintHandler", &XC::ModelWrapper::newConstraintHandler,return_internal_reference<>(),"newConstraintHandler(nmb)\n""Create a new constraints handler of the type whose name is passed as parameter.\n""Available names for the type of constraints handler: 'lagrange_constraint_handler', 'penalty_constraint_handler', 'plain_handler', 'transformation_constraint_handler' \n") 
    ;

class_<XC::MapModelWrapper, bases<EntCmd>, boost::noncopyable >("MapModelWrapper", no_init)
    .add_property("existeModelWrapper", &XC::MapModelWrapper::existeModelWrapper)
    .def("newModelWrapper", &XC::MapModelWrapper::creaModelWrapper,return_internal_reference<>(),"Creates a new contenedor de ModelWrappers.")
  ;

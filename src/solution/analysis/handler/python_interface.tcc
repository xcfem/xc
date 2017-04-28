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

class_<XC::ConstraintHandler, bases<XC::MovableObject,EntCmd>, boost::noncopyable >("ConstraintHandler", "Constraint handlers enforce the single and multi freedom constraints that exist in the domain by creating the appropriate FE_Element and DOF_Group objects.",no_init)
    ;

class_<XC::FactorsConstraintHandler, bases<XC::ConstraintHandler>, boost::noncopyable >("FactorsConstraintHandler", "Base class for penalty and Lagrange constraints handlers.",no_init)
    .add_property("alphaSP", &XC::FactorsConstraintHandler::getAlphaSP, &XC::FactorsConstraintHandler::setAlphaSP,"Factor applied with single-freedom constraints.")
    .add_property("alphaMP", &XC::FactorsConstraintHandler::getAlphaMP, &XC::FactorsConstraintHandler::setAlphaMP,"Factor applied with multi-freedom constraints.")
    ;

class_<XC::PenaltyConstraintHandler, bases<XC::FactorsConstraintHandler>, boost::noncopyable >("PenaltyConstraintHandler", "Handle single and multi point constraints by using the penalty method.\n" "This is done by, in addition to creating a DOF_Group object for each Node and an FE_Element for each Element in the Domain, creating either a PenaltySFreedom_FE or a PenaltyMP_FE object for each constraint in the Domain. It is these objects that enforce the constraints by modifying the tangent matrix and residual vector.\n", no_init);

class_<XC::LagrangeConstraintHandler , bases<XC::FactorsConstraintHandler>, boost::noncopyable >("LagrangeConstraintHandler", "Handle single and multi point constraints by using the Lagrange multipliers method.\n" "This is done by, in addition to creating a DOF_Group object for each Node and an FE_Element for each Element in the Domain, creating a LagrangeDOF_Group object and either a LagrangeSFreedom_FE or a LagrangeMP_FE object for each constraint in the Domain. It is these objects that enforce the constraints by modifying the tangent matrix and residual vector.",no_init);

class_<XC::PlainHandler , bases<XC::ConstraintHandler>, boost::noncopyable >("PlainHandler", "Handle homogeneous single point constraints.\n" "Create regular FE_Element and DOF_Group objects and enforce the constraints by specifying that degrees-of-freedom which are constrained are not assigned an equation number.",no_init);

class_<XC::TransformationConstraintHandler , bases<XC::ConstraintHandler>, boost::noncopyable >("TransformationConstraintHandler", "Handle single and multi point constraints by using the Lagrange multipliers method.\n" "This is done by, in addition to creating a DOF_Group object for each Node and an FE_Element for each Element in the Domain, creating either a TransformationSFreedom_FE or a TransformationMP_FE object for each constraint in the Domain. It is these objects that enforce the constraints by modifying the tangent matrix and residual vector.",no_init);


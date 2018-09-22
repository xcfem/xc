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

class_<XC::DOF_Numberer, bases<XC::MovableObject,CommandEntity>, boost::noncopyable >("DOFNumberer", "A DOF numberer is responsible for assigning the equation numbers to the individual DOFs in each of the DOF groups in the analysis model.",no_init)
    .def("useAlgorithm", &XC::DOF_Numberer::useAlgorithm,return_internal_reference<>(),"\n""useAlgorithm(nmb)""Set the algorithm to be used for numerating the graph \n" "Parameters: \n""nmb: name of the algorithm, 'rcm' for Reverse Cuthill-Macgee or 'simple' for simple algorithm.")
    ;

// class_<XC::ParallelNumberer, bases<XC::DOF_Numberer>, boost::noncopyable >("ParallelNumberer", no_init);

class_<XC::PlainNumberer, bases<XC::DOF_Numberer>, boost::noncopyable >("PlainNumberer", "\n""The plain numberer assigns equation numbers to DOFs on a first come first serve basis, that is, it gets the DOF_GrpIter and assigns the equation numbers to de DOFs as it iterrates through the iter.",no_init);

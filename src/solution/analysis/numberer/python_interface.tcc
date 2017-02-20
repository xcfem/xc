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

class_<XC::DOF_Numberer, bases<XC::MovableObject,EntCmd>, boost::noncopyable >("DOFNumberer", no_init)
    .def("useAlgorithm", &XC::DOF_Numberer::useAlgorithm,return_internal_reference<>(),"Sets the algorithm to use to numerate the graph; «Reverse Cuthill-Macgee» or simple.")
    ;

// class_<XC::ParallelNumberer, bases<XC::DOF_Numberer>, boost::noncopyable >("ParallelNumberer", no_init);

class_<XC::PlainNumberer, bases<XC::DOF_Numberer>, boost::noncopyable >("PlainNumberer", no_init);

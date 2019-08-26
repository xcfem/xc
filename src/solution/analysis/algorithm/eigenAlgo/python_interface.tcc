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

class_<XC::EigenAlgorithm, bases<XC::SolutionAlgorithm>, boost::noncopyable >("EigenAlgorithm", "Solution algorithm for eigenproblems.",no_init);

class_<XC::FrequencyAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("FrequencyAlgo","Solution algorithm for obtaining the natural frequencies of the model", no_init);

class_<XC::LinearBucklingAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("LinearBucklingAlgo", "Solution algorithm for linear buckling analysis", no_init);

class_<XC::KEigenAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("KEigenAlgo", "Solution algorithm for ill-conditioning analysis", no_init)
  .add_property("ns", &XC::KEigenAlgo::getNs,&XC::KEigenAlgo::setNs," number of smallest eigenpairs")
  .add_property("nl", &XC::KEigenAlgo::getNl,&XC::KEigenAlgo::setNl," number of largest eigenpairs")
  .add_property("condNumberThreshold",&XC::KEigenAlgo::getConditionNumberThreshold,&XC::KEigenAlgo::setConditionNumberThreshold,"condition number threshold for triggering analysis")
  ;

class_<XC::StandardEigenAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("StandardEigenAlgo", "Solution algorithm for standard eigenvalue problems", no_init);


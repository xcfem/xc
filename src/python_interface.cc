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
//python_interface.cc


#include "FEProblem.h"
#include "python_interface.h"

void export_utility(void);
void export_material_base(void);
void export_material_uniaxial(void);
void export_material_nD(void);
void export_material_section(void);
void export_material_fiber_section(void);
void export_material_damage(void);
void export_domain(void);
void export_domain_mesh(void);
void export_preprocessor_prep_handlers(void);
void export_preprocessor_build_model(void);
void export_preprocessor_sets(void);
void export_preprocessor_main(void);
void export_solution(void);

BOOST_PYTHON_MODULE(xc)
  {
    // Import the "geom" module.
    boost::python::object geom= boost::python::import("geom");
    using namespace boost::python;
    docstring_options doc_options;

    XC::Domain *(XC::PreprocessorContainer::*getContainerDomain)(void)= &XC::PreprocessorContainer::getDomain;
    XC::Preprocessor *(XC::PreprocessorContainer::*getContainerPreprocessor)(void)= &XC::PreprocessorContainer::getPreprocessor;
    class_<XC::PreprocessorContainer, bases<CommandEntity>, boost::noncopyable >("PreprocessorContainer", no_init)
      .add_property("getPreprocessor", make_function( getContainerPreprocessor, return_internal_reference<>() ))
      .add_property("getDomain", make_function( getContainerDomain, return_internal_reference<>() ),"Return a reference to the domain.")
      ;

    export_utility();


//Expose domain components.
#include "domain/component/python_interface.tcc"



    class_<XC::MeshComponent, bases<XC::ContinuaReprComponent>, boost::noncopyable >("MeshComponent", no_init)
       ;
    

    export_material_base(); // Materials exposition.
    export_material_uniaxial(); // Materials exposition.
    export_material_nD(); // Materials exposition.
    export_material_section(); // Materials exposition.
    export_material_fiber_section(); // Materials exposition.
    export_material_damage(); // Damage models  exposition.
    export_domain_mesh(); // Mesh exposition.
    export_domain(); // Domain exposition.
    export_preprocessor_prep_handlers();// Preprocessor exposition.
    export_preprocessor_build_model();
    export_preprocessor_sets();
    export_preprocessor_main();
    export_solution(); // Solution routines exposition.

    XC::Domain *(XC::FEProblem::*getDomainRef)(void)= &XC::FEProblem::getDomain;
    XC::Preprocessor &(XC::FEProblem::*getPreprocessorRef)(void)= &XC::FEProblem::getPreprocessor;
    XC::SolutionProcedure &(XC::FEProblem::*getSoluProcRef)(void)= &XC::FEProblem::getSoluProc;
    class_<XC::FEProblem, bases<CommandEntity>, boost::noncopyable>("FEProblem")
      .def("getXCVersion",  make_function(&XC::FEProblem::getXCVersion,return_value_policy<copy_const_reference>()),"Return XC program version string.").staticmethod("getXCVersion")
      .def("getXCVersionShort",  make_function(&XC::FEProblem::getXCVersionShort,return_value_policy<copy_const_reference>()),"Return XC program (short) version string.").staticmethod("getXCVersionShort")
      .add_property("getDomain", make_function( getDomainRef, return_internal_reference<>() ),"Return a reference to the domain.")
      .add_property("getPreprocessor", make_function( getPreprocessorRef, return_internal_reference<>() ),"Return a reference to the preprocessor.")
      .add_property("getSoluProc", make_function( getSoluProcRef, return_internal_reference<>() ),"Return a reference to the solver")
      .add_property("getDatabase", make_function( &XC::FEProblem::getDataBase, return_internal_reference<>() ),"Return a reference to the data base")
      .def("newDatabase", make_function( &XC::FEProblem::defineDatabase, return_internal_reference<>() ),"Create a data base")
      .def("clearAll",&XC::FEProblem::clearAll,"Delete all entities in the FE problem.")
   ;
    def("getXCVersion",XC::getXCVersion);
    def("getXCVersionShort",XC::getXCVersionShort);
  }


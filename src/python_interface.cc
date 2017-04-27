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
//python_interface.cc


#include "ProblemaEF.h"
#include "python_interface.h"

void export_utility(void);
void export_material_base(void);
void export_material_uniaxial(void);
void export_material_nD(void);
void export_material_section(void);
void export_material_fiber_section(void);
void export_domain(void);
void export_domain_mesh(void);
void export_preprocessor_loaders(void);
void export_preprocessor_build_model(void);
void export_preprocessor_sets(void);
void export_preprocessor_main(void);
void export_solution(void);

BOOST_PYTHON_MODULE(xc)
  {
    using namespace boost::python;
    docstring_options doc_options;

    export_utility();
//Expose domain components.
#include "domain/component/python_interface.tcc"



    class_<XC::MeshComponent, bases<XC::ContinuaReprComponent>, boost::noncopyable >("MeshComponent", no_init)
       ;

    export_material_base(); //Materials exposition.
    export_material_uniaxial(); //Materials exposition.
    export_material_nD(); //Materials exposition.
    export_material_section(); //Materials exposition.
    export_material_fiber_section(); //Materials exposition.
    export_domain_mesh(); //Mesh exposition.
    export_domain(); //Domain exposition.
    export_preprocessor_loaders();//Preprocessor exposition.
    export_preprocessor_build_model();
    export_preprocessor_sets();
    export_preprocessor_main();
    export_solution(); //Solution routines exposition.

#include "post_process/python_interface.tcc"

    XC::Domain *(XC::ProblemaEF::*getDomainRef)(void)= &XC::ProblemaEF::getDomain;
    XC::Preprocessor &(XC::ProblemaEF::*getPreprocessorRef)(void)= &XC::ProblemaEF::getPreprocessor;
    XC::ProcSolu &(XC::ProblemaEF::*getSoluProcRef)(void)= &XC::ProblemaEF::getSoluProc;
    class_<XC::ProblemaEF, bases<EntCmd>, boost::noncopyable>("ProblemaEF")
      .def("getXCVersion",  make_function(&XC::ProblemaEF::getXCVersion,return_value_policy<copy_const_reference>()),"Return XC program version string.").staticmethod("getXCVersion")
      .def("getXCVersionShort",  make_function(&XC::ProblemaEF::getXCVersionShort,return_value_policy<copy_const_reference>()),"Return XC program (short) version string.").staticmethod("getXCVersionShort")
      .add_property("getDomain", make_function( getDomainRef, return_internal_reference<>() ),"Return a reference to the domain.")
      .add_property("getPreprocessor", make_function( getPreprocessorRef, return_internal_reference<>() ),"Return a reference to the preprocessor.")
      .add_property("getSoluProc", make_function( getSoluProcRef, return_internal_reference<>() ),"Return a reference to the solver")
      .add_property("getDatabase", make_function( &XC::ProblemaEF::getDataBase, return_internal_reference<>() ),"Return a reference to the data base")
      .def("newDatabase", make_function( &XC::ProblemaEF::defineDatabase, return_internal_reference<>() ),"Create a data base")
      .add_property("getFields", make_function( &XC::ProblemaEF::getFields, return_internal_reference<>() ),"Return fields definition (export).")
      .def("clearAll",&XC::ProblemaEF::clearAll,"Delete all entities in the FE problem.")
   ;
    def("getXCVersion",XC::getXCVersion);
    def("getXCVersionShort",XC::getXCVersionShort);
  }


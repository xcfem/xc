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
//export_preprocessor_build_model.cxx

#include "python_interface.h"

void export_preprocessor_build_model(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

XC::Preprocessor *(XC::EntMdlrBase::*getPreprocessorRef)(void)= &XC::EntMdlrBase::getPreprocessor;
class_<XC::EntMdlrBase, bases<NamedEntity>, boost::noncopyable >("EntMdlrBase", no_init)
  .add_property("tag", &XC::EntMdlrBase::getTag, "Return the object identifier.")
  .add_property("getPreprocessor", make_function( getPreprocessorRef, return_internal_reference<>() ),"Return the preprocessor that built this object.")
  .def("__eq__", &XC::EntMdlrBase::operator==)
  //.def("__ne__", &XC::EntMdlrBase::operator!=)
   ;
#include "preprocessor/set_mgmt/python_interface_set_base.tcc"


#include "preprocessor/multi_block_topology/python_interface.tcc"

  }


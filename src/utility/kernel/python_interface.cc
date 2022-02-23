//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.cxx

#include "python_utils.h"
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include "utility/kernel/CommandEntity.h"
#include "utility/kernel/NamedEntity.h"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "utility/matrices/ProtoMatrix.h"

BOOST_PYTHON_MODULE(xc_base)
  {
    using namespace boost::python;
    docstring_options doc_options;

    typedef std::vector<int> std_vector_int;
    class_<std_vector_int>("std_vector_int")
      .def(vector_indexing_suite<std_vector_int>() );

    typedef std::vector<double> std_vector_double;
    class_<std_vector_double>("std_vector_double")
      .def(vector_indexing_suite<std_vector_double>() );
    
    typedef std::vector<std::string> std_vector_string;
    class_<std_vector_string>("std_vector_string")
      .def(vector_indexing_suite<std_vector_string>() );

    def("vector_int_to_py_list",vector_int_to_py_list);
    def("vector_double_to_py_list",vector_double_to_py_list);
    def("vector_string_from_py_list",vector_string_from_py_list);

    def("mseImages",mseImages);
    def("psnrImagenes",psnrImagenes);
    def("compare_images",compare_images);

    typedef std::set<int> set_ints;
    class_<set_ints>("setInts")
      .def("__iter__", iterator<set_ints>())
      .add_property("size", &set_ints::size)
      .def("__len__", &set_ints::size)
      .def("empty", &set_ints::empty)
      // .def("__contains__", &set_ints::contains)
      // .def("count", &set_ints::count)
       ;

    class_<StandardStreamWrapper, boost::noncopyable  >("StandardStreamWrapper", no_init)
      .def("reset", &StandardStreamWrapper::reset,"Reset the stream to its default buffer.")
      ;
    
    StandardStreamWrapper &(StandardOutputWrapper::*getErrorStreamWrapper)(void)= &StandardOutputWrapper::getErrorStreamWrapper;
    StandardStreamWrapper &(StandardOutputWrapper::*getLogStreamWrapper)(void)= &StandardOutputWrapper::getLogStreamWrapper;
    StandardStreamWrapper &(StandardOutputWrapper::*getOutputStreamWrapper)(void)= &StandardOutputWrapper::getOutputStreamWrapper;
    class_<StandardOutputWrapper, boost::noncopyable  >("StandardOutputWrapper", no_init)
      .def("reset", &StandardOutputWrapper::reset,"Reset standard output streams to its default buffers.")
      .add_property("errorStreamWrapper", make_function( getErrorStreamWrapper, return_internal_reference<>() ), "returns error stream wrapper.")
      .add_property("errorStreamWrapper", make_function( getErrorStreamWrapper, return_internal_reference<>() ), "returns error stream wrapper.")
      .add_property("logStreamWrapper", make_function( getLogStreamWrapper, return_internal_reference<>() ), "returns log stream wrapper.")
      .add_property("logStreamWrapper", make_function( getLogStreamWrapper, return_internal_reference<>() ), "returns log stream wrapper.")
      .add_property("outStreamWrapper", make_function( getOutputStreamWrapper, return_internal_reference<>() ), "returns regular output stream wrapper.")
      .add_property("outStreamWrapper", make_function( getOutputStreamWrapper, return_internal_reference<>() ), "returns regular output stream wrapper.")
      ;

const EntityWithOwner *(EntityWithOwner::*getOwner)(void) const= &EntityWithOwner::Owner;
    class_<EntityWithOwner,EntityWithOwner *, boost::noncopyable  >("EntityWithOwner", no_init)
      .add_property("owner", make_function( getOwner, return_internal_reference<>() ), "returns object's owner (container).")
      .def("setVerbosityLevel", &EntityWithOwner::setVerbosityLevel,"Set verbosity level.")
        .staticmethod("setVerbosityLevel")
      .def("getVerbosityLevel", &EntityWithOwner::getVerbosityLevel,"Get verbosity level.")
      .staticmethod("getVerbosityLevel")
      .def("tipo", &EntityWithOwner::getClassName,"DEPRECATED Return the class name.")
      .def("type", &EntityWithOwner::getClassName,"Returns class name.")
      ;
    
    class_<EntityWithProperties, bases<EntityWithOwner> >("EntityWithProperties")
      .def("hasProp", &EntityWithProperties::hasPyProp,"True if property exists.")
      .def("getProp", &EntityWithProperties::getPyProp,"Return a user defined property.")
      .def("setProp", &EntityWithProperties::setPyProp,"Sets the value of a user defined property.")
      .def("getPropNames", &EntityWithProperties::getPropNames,"Return the names of the object properties.")
      .def("getDict", &EntityWithProperties::getPyDict,"Return a Python dictionary containing the object members values.")
      .def("setFromDict", &EntityWithProperties::setPyDict,"Set the values of the object members from a Python dictionary.")
  ;

    StandardOutputWrapper &(CommandEntity::*getStandardOutputWrapper)(void)= &CommandEntity::getStandardOutputWrapper;
    class_<CommandEntity, bases<EntityWithProperties> >("CommandEntity")
      .add_property("standardOutputWrapper", make_function( getStandardOutputWrapper, return_internal_reference<>() ), "returns standard output wrapper.")
      .add_property("logFileName", make_function( &CommandEntity::getLogFileName, return_internal_reference<>() ), &CommandEntity::setLogFileName)
      .add_property("errFileName", make_function( &CommandEntity::getErrFileName, return_internal_reference<>() ), &CommandEntity::setErrFileName)
      .add_property("outputFileName", make_function( &CommandEntity::getOutputFileName, return_internal_reference<>() ), &CommandEntity::setOutputFileName)
      .def("evalPy", &CommandEntity_eval,"Evaluates expresion.")
      .def("execPy", &CommandEntity_exec,"Executes code block.")
      .def("execFilePy", &CommandEntity_exec_file,"Executes code block.")
  ;

    class_<NamedEntity, bases<CommandEntity> >("NamedEntity")
      //.add_property("nombr e", make_function( &NamedEntity::getName, return_value_policy<copy_const_reference>()), &NamedEntity::setName,"DEPRECATED returns object name.")
      .add_property("name", make_function( &NamedEntity::getName, return_value_policy<copy_const_reference>()), &NamedEntity::setName,"returns object name.")
      ;

    class_<ProtoMatrix, boost::noncopyable>("ProtoMatrix", no_init)
      .add_property("nRow",&ProtoMatrix::getNumberOfRows,"returns number of rows.")
      .add_property("nCol",&ProtoMatrix::getNumberOfColumns,"returns number of columns.")
      ;
    def("resetStandardOutput",resetStandardOutput);
  }


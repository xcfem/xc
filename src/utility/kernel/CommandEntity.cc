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
//CommandEntity.cxx

#include "CommandEntity.h"
#include <boost/tokenizer.hpp>
#include "utility/matrices/RangoIndice.h"
#include <typeinfo>
#include <stack>
#include <map>
#include <cmath>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <limits>
#include <cstdlib>

//Variables estáticas de CommandEntity.
StandardOutputWrapper CommandEntity::standardOutput; //!< Standard output streams.

//! @brief Compile onceboost::python::object, get back a boost::python::object wrapping the code object.
//! @param code: string containing the code to compile.
//! @param name: name (or name of the file) identifying the script.
boost::python::object compile_script(const std::string& code,
				     const std::string& name = "<script>")
  {
    PyObject* co = Py_CompileString(code.c_str(), name.c_str(), Py_file_input);
    if (!co)
      { boost::python::throw_error_already_set(); }
    // handle<> takes ownership of the new reference
    return boost::python::object(boost::python::handle<>(co));
  }

//!@ Execute a previously-compiled code object against given globals/locals.
//!@ param code: compiled code to execute.
//!@ param globals: global variables available to the code.
//!@ param globals: local variables available to the code.
boost::python::object exec_compiled(const boost::python::object& code,
				    boost::python::object globals,
				    boost::python::object locals)
  {
    PyObject* result= PyEval_EvalCode(code.ptr(), globals.ptr(), locals.ptr());
    if (!result)
      {
        boost::python::throw_error_already_set();
      }
    return boost::python::object(boost::python::handle<>(result));
  }

//! @brief Default constructor.
CommandEntity::CommandEntity(CommandEntity *owr)
  : EntityWithProperties(owr) {}

//! @brief Return a pointer to the object owner.
CommandEntity *CommandEntity::Owner(void)
  {
    CommandEntity *retval= nullptr;
    EntityWithOwner *tmp= EntityWithProperties::Owner();
    if(tmp)
      retval= dynamic_cast<CommandEntity *>(tmp);
    return retval;
  }

//! @brief Return un puntero al objeto propietario de ESTE.
const CommandEntity *CommandEntity::Owner(void) const
  {
    const CommandEntity *retval= nullptr;
    const EntityWithOwner *tmp= EntityWithProperties::Owner();
    if(tmp)
      retval= dynamic_cast<const CommandEntity *>(tmp);
    return retval;
  }

//! @brief Return the regular output stream wrapper.
const StandardOutputWrapper &CommandEntity::getStandardOutputWrapper(void) const
  { return standardOutput; }

//! @brief Return the regular output stream wrapper.
StandardOutputWrapper &CommandEntity::getStandardOutputWrapper(void)
  { return standardOutput; }

//! @brief Returns log file name.
const std::string &CommandEntity::getLogFileName(void) const
  { return standardOutput.getLogFileName(); }

//! @brief Sets log file name.
void CommandEntity::setLogFileName(const std::string &fname)
  {
    if(fname != "")
      standardOutput.setLogFileName(fname);
  }

//! @brief Returns err file name.
const std::string &CommandEntity::getErrFileName(void) const
  { return standardOutput.getErrFileName(); }

//! @brief Sets error file name.
void CommandEntity::setErrFileName(const std::string &fname)
  {
    if(fname != "")
      standardOutput.setErrFileName(fname);
  }

//! @brief Returns regular output file name.
const std::string &CommandEntity::getOutputFileName(void) const
  { return standardOutput.getOutputFileName(); }

//! @brief Sets regular output file name.
void CommandEntity::setOutputFileName(const std::string &fname)
  {
    if(fname != "")
      standardOutput.setOutputFileName(fname);
  }

//! @brief Reset the standard output streams to its defaults buffers.
void CommandEntity::resetStandardOutput(void)
  { standardOutput.reset(); }

//! @brief Compile once the given code block, get back a boost::python::object
//! wrapping the code object.
//! @param block: string containing the code to compile.
//! @param name: name (or name of the file) identifying the script.
boost::python::object CommandEntity::compile_code_block(const std::string &block,
							const std::string& name = "<script>")
  {
    boost::python::object retval;
    compiled_code_const_iterator iter= compiled_code.find(block);
    if(iter != compiled_code.end())
      {
        retval= (iter->second);
      }
    else
      {
        retval= compile_script(block, name);
	compiled_code[block]= retval;
      }
    return retval;
  }

//! @brief Return the Python object that results from evaluating the argument.
boost::python::object CommandEntity::evalPy(boost::python::object dict,const std::string &str)
   {
     boost::python::object main_module = boost::python::import("__main__");
     boost::python::object main_namespace = main_module.attr("__dict__");
     return boost::python::eval(str.c_str(),main_namespace,dict);
   }


//! @brief Return the Python objects that results from executing the code in the string argument.
boost::python::object CommandEntity::execPy(boost::python::object dict,const std::string &block)
   {
     boost::python::object main_module = boost::python::import("__main__");
     boost::python::object main_namespace = main_module.attr("__dict__");
     boost::python::object code= this->compile_code_block(block, "<script>");
     //return boost::python::exec(block.c_str(),main_namespace,dict);
     return exec_compiled(code, main_namespace, dict);
   }

//! @brief Return the Python object that results from executing the code in the file.
boost::python::object CommandEntity::execFilePy(boost::python::object dict,const std::string &fName)
   {
     boost::python::object main_module = boost::python::import("__main__");
     boost::python::object main_namespace = main_module.attr("__dict__");
     return boost::python::exec_file(fName.c_str(),main_namespace,dict);
   }

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
     return boost::python::exec(block.c_str(),main_namespace,dict);
   }

//! @brief Return the Python object that results from executing the code in the file.
boost::python::object CommandEntity::execFilePy(boost::python::object dict,const std::string &fName)
   {
     boost::python::object main_module = boost::python::import("__main__");
     boost::python::object main_namespace = main_module.attr("__dict__");
     return boost::python::exec_file(fName.c_str(),main_namespace,dict);
   }

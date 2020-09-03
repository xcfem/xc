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
//utils_python_interface.cxx

#include "FEProblem.h"
#include "python_interface.h"

void export_utility(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

#include "matrix/python_interface.tcc"
    class_<XC::MovableObject, boost::noncopyable >("MovableObject", no_init)
       ;

    class_<XC::TaggedObject, bases<CommandEntity>, boost::noncopyable >("TaggedObject", no_init)
        .add_property("tag", &XC::TaggedObject::getTag, &XC::TaggedObject::assignTag)
       ;

#include "actor/channel/python_interface.tcc"
#include "database/python_interface.tcc"
#include "recorder/python_interface.tcc"
#include "paving/python_interface.tcc"

  }


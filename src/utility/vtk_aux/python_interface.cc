//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include "vtkDoubleHeadedArrowSource.h"

BOOST_PYTHON_MODULE(vtk_aux)
  {
    using namespace boost::python;
    //docstring_options doc_options;

    //This doesn't work 10.01.2015 because the protection of destructor
    //I'll try to inherit directly in Python.
    //    class_<vtkDoubleHeadedArrowSource, bases<vtkPolyDataAlgorithm> >("vtkDoubleHeadedArrowSource")
      ;
  }


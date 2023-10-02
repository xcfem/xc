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
//broke_property.cc

#include "broke_property.h"
#include <iostream>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Vector2d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"


//! @brief Tries to instantiate the adequate XC object from the data contained
//! in the given Python dictionary.
boost::python::object broke_property_from_python_dict(const boost::python::dict &d)
  {
    static std::vector<std::string> geomClasses= {"Vector2d", "Vector3d", "Pos2d", "Pos3d", "SlidingVecto2d", "SlidingVector3d"};
    // Import the "xc_base" module.
    boost::python::object xc_base = boost::python::import("xc_base");
    // Import the "geom" module.
    boost::python::object geom= boost::python::import("geom");
    boost::python::object retval;
    if(d.has_key("className"))
      {
	const std::string className= boost::python::extract<std::string>(d["className"]);
	if(std::find(geomClasses.begin(), geomClasses.end(), className)!=geomClasses.end())
	  {
	    boost::python::object foo= geom.attr(className.c_str())();
	    boost::python::object method= foo.attr("setFromDict");
	    method(d);
	    retval= foo;
	  }
	else
	  std::cerr << "class name: '" << className
		    << "' unknown." << std::endl;
      }
    else
      std::cerr << "broke_property_from_python_dict; no class name in the given dictionary. Returning none." << std::endl; 
    

    return retval; 
  }


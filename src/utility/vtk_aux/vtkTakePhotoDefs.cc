//----------------------------------------------------------------------------
//  biblioteca vtk_aux; utilidades construidas sobre VTK (<http://www.vtk.org>)
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
//vtkTakePhotoDefs.cc

#include "vtkTakePhotoDefs.h"

//! @brief Return el formato del archivo a partir del entero que
//! se pasa como parámetro
sepFileFormat int_to_file_format(int f)
  {
    sepFileFormat retval= POSTSCRIPT;
    switch(f)
      {
      case 1:
        retval= POSTSCRIPT;
        break;
      case 2:
        retval= TIFF;
        break;
      case 3:
        retval= CGM;
        break;
      case 4: 
        retval= BMP;
        break;
      case 5:
        retval= VRML;
        break;
      default:
	std::cerr << f << " Unknow output format type\n";
      }
    return retval;
  }

//! @brief Return el formato del archivo a partir del entero que
//! se pasa como parámetro
sepFileFormat string_to_file_format(const std::string &fmt_name)
  {
    sepFileFormat retval= POSTSCRIPT;
    if(fmt_name == "postscript")
      retval= POSTSCRIPT;
    else if(fmt_name == "tiff")
      retval=  TIFF;
    else if(fmt_name == "cgm")
      retval=  CGM;
    else if(fmt_name == "bmp")
      retval=  BMP;
    else if(fmt_name == "vrml")
      retval=  VRML;
    else
      std::cerr << fmt_name << " Unknow output format type\n";
    return retval;
  }

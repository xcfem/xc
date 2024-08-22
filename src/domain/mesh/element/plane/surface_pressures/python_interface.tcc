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
//python_interface.tcc

typedef XC::SurfaceLoadBase<2> surface_load_base_2n;
class_<surface_load_base_2n, bases<ElementBase2N>, boost::noncopyable >("surface_load_base_2n", no_init);

class_<XC::QuadSurfaceLoad, bases<surface_load_base_2n >, boost::noncopyable >("QuadSurfaceLoad", no_init)
   .add_property("pressure",&XC::QuadSurfaceLoad::getPressure, &XC::QuadSurfaceLoad::setPressure,"Get/set the pressure load.")
   .def("getLength",&XC::QuadSurfaceLoad::getLength, "Returns element length.")
   ;

typedef XC::SurfaceLoadBase<4> surface_load_base_4n;
class_<surface_load_base_4n, bases<ElementBase4N>, boost::noncopyable >("surface_load_base_4n", no_init);

XC::ShellLinearCrdTransf3d *(XC::BrickSurfaceLoad::*getBrickSurfaceLoadCoordTransf)(void)= &XC::BrickSurfaceLoad::getCoordTransf;
class_<XC::BrickSurfaceLoad, bases<surface_load_base_4n > >("BrickSurfaceLoad")
   .add_property("pressure",&XC::BrickSurfaceLoad::getPressure, &XC::BrickSurfaceLoad::setPressure,"Get/set the pressure load.")
   .add_property("getCoordTransf", make_function( getBrickSurfaceLoadCoordTransf, return_internal_reference<>() ))
   ;


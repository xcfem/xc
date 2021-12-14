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
//utils_python_interface.cxx

class_<XC::Response, boost::noncopyable >("Response", no_init);

class_<XC::ElementResponse , bases<XC::Response>, boost::noncopyable >("ElementResponse", no_init);

// class_<XC::FiberResponse , bases<XC::Response>, boost::noncopyable >("FiberResponse", no_init);

// class_<XC::MaterialResponse , bases<XC::Response>, boost::noncopyable >("MaterialResponse", no_init);


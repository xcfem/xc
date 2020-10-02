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
//python_interface.tcc

XC::SolidMech2D &(XC::FourNodeQuad::*getFourNodeQuadPhysicalPropertiesPtr)(void) = &XC::FourNodeQuad::getPhysicalProperties;
class_<XC::FourNodeQuad, bases<QuadBase4N_Mech2D>, boost::noncopyable >("FourNodeQuad", no_init)
  .def("detJ", &XC::FourNodeQuad::detJ)
  .add_property("physicalProperties",make_function(getFourNodeQuadPhysicalPropertiesPtr,return_internal_reference<>() ),"Returns element physical properties (material).")
   ;

XC::SolidMech2D &(XC::EnhancedQuad::*getEnhancedQuadPhysicalPropertiesPtr)(void) = &XC::EnhancedQuad::getPhysicalProperties;
class_<XC::EnhancedQuad, bases<QuadBase4N_Mech2D>, boost::noncopyable >("EnhancedQuad", no_init)
  .add_property("physicalProperties",make_function(getEnhancedQuadPhysicalPropertiesPtr,return_internal_reference<>() ),"Returns element physical properties (material).")
   ;

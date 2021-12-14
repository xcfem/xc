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

class_<XC::BeamWithHinges2d, bases<XC::BeamColumnWithSectionFDTrf2d>, boost::noncopyable >("BeamWithHinges2d", no_init)
  .add_property("rho", &XC::BeamWithHinges2d::getRho,&XC::BeamWithHinges2d::setRho)
  .add_property("sectionProperties", &XC::BeamWithHinges2d::getSectionProperties, &XC::BeamWithHinges2d::setSectionProperties)
   ;

class_<XC::BeamWithHinges3d, bases<XC::BeamColumnWithSectionFDTrf3d>, boost::noncopyable >("BeamWithHinges3d", no_init)
  .add_property("rho", &XC::BeamWithHinges3d::getRho,&XC::BeamWithHinges3d::setRho)
  .add_property("sectionProperties", &XC::BeamWithHinges3d::getSectionProperties, &XC::BeamWithHinges3d::setSectionProperties)
   ;

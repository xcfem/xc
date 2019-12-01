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

class_<XC::BbarBrick , bases<XC::BrickBase>, boost::noncopyable >("BbarBrick", no_init);

XC::Vector (XC::Brick::*getAvgStressPtr)(void) const= &XC::Brick::getAvgStress;
double (XC::Brick::*getAvgStressIJPtr)(const size_t &,const size_t &) const= &XC::Brick::getAvgStress;
XC::Vector (XC::Brick::*getAvgStrainPtr)(void) const= &XC::Brick::getAvgStrain;
double (XC::Brick::*getAvgStrainIJPtr)(const size_t &,const size_t &) const= &XC::Brick::getAvgStrain;
class_<XC::Brick , bases<XC::BrickBase>, boost::noncopyable >("Brick", no_init)
  .add_property("gaussPointsPositions",&XC::Brick::getGaussPointsPositions,"Return the local coordinates of the Gauss points.")
  .def("getAvgStress", getAvgStressPtr,"Return the average stress on the element.")
  .def("getAvgStressIJ", getAvgStressIJPtr,"Return (i,j) component of the average stress on the element.")
  .def("getAvgStrain", getAvgStrainPtr,"Return the average strain on the element.")
  .def("getAvgStrainIJ", getAvgStrainIJPtr,"Return (i,j) component of the average strain on the element.")

  ;

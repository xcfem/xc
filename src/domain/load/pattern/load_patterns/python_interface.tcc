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

class_<XC::EQBasePattern, bases<XC::LoadPattern>, boost::noncopyable >("EQBasePattern", no_init);

class_<XC::EarthquakePattern , bases<XC::EQBasePattern>, boost::noncopyable >("EarthquakePattern", no_init);

class_<XC::UniformExcitation, bases<XC::EarthquakePattern>, boost::noncopyable >("UniformExcitation", no_init)
  .add_property("motionRecord", make_function( &XC::UniformExcitation::getGroundMotionRecord, return_internal_reference<>()),"Ground motion data.")
  ;

class_<XC::MultiSupportPattern, bases<XC::EQBasePattern>, boost::noncopyable >("MultiSupportPattern", no_init);

class_<XC::PBowlLoading , bases<XC::LoadPattern>, boost::noncopyable >("PBowlLoading", no_init);



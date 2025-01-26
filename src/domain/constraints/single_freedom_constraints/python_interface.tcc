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

class_<XC::ImposedMotionBase , bases<XC::SFreedom_Constraint>, boost::noncopyable >("ImposedMotionBase", no_init);

class_<XC::ImposedMotionSP , bases<XC::ImposedMotionBase>, boost::noncopyable >("ImposedMotionSP", no_init);

class_<XC::ImposedMotionSP1 , bases<XC::ImposedMotionBase>, boost::noncopyable >("ImposedMotionSP1", no_init);

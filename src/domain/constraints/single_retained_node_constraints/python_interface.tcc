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

class_<XC::EqualDOF, bases<XC::MFreedom_Constraint>, boost::noncopyable >("EqualDOF", no_init);

class_<XC::MFreedom_Joint , bases<XC::MFreedom_Constraint>, boost::noncopyable >("MFreedom_Joint2D", no_init);

class_<XC::MFreedom_Joint2D , bases<XC::MFreedom_Joint>, boost::noncopyable >("MFreedom_Joint2D", no_init);

class_<XC::MFreedom_Joint3D, bases<XC::MFreedom_Joint>, boost::noncopyable >("MFreedom_Joint3D", no_init);

class_<XC::RigidDiaphragm, bases<XC::MFreedom_Constraint>, boost::noncopyable >("RigidDiaphragm", no_init);

class_<XC::RigidBase, bases<XC::MFreedom_Constraint>, boost::noncopyable >("RigidBase", no_init);

class_<XC::RigidBeam, bases<XC::RigidBase>, boost::noncopyable >("RigidBeam", no_init);

class_<XC::RigidRod, bases<XC::RigidBase>, boost::noncopyable >("RigidRod", no_init);

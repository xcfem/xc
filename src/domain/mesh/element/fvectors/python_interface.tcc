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

class_<XC::FVector, boost::noncopyable >("FVector", no_init);

typedef XC::FVectorData<3> FVectorData3;
class_<FVectorData3, bases<XC::FVector>, boost::noncopyable >("FVectorData3", no_init);

class_<XC::FVectorBeamColumn2d, bases<FVectorData3>, boost::noncopyable >("FVectorBeamColumn2d", no_init);

typedef XC::FVectorData<5> FVectorData5;
class_<FVectorData5, bases<XC::FVector>, boost::noncopyable >("FVectorData5", no_init);

class_<XC::FVectorBeamColumn3d, bases<FVectorData5>, boost::noncopyable >("FVectorBeamColumn3d", no_init);

typedef XC::FVectorData<24> FVectorData24;
class_<FVectorData24, bases<XC::FVector>, boost::noncopyable >("FVectorData24", no_init);

class_<XC::FVectorShell, bases<FVectorData24>, boost::noncopyable >("FVectorShell", no_init);

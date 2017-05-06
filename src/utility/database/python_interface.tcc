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

class_<XC::FE_Datastore, bases<XC::Channel, XC::PreprocessorContainer>, boost::noncopyable  >("FE_Datastore", no_init)
  .def("save",&XC::FE_Datastore::save,"Save problem data.")
  .def("restore",&XC::FE_Datastore::restore,"Restore problem data.")
  ;


class_<XC::DBDatastore, bases<XC::FE_Datastore>, boost::noncopyable  >("DBDatastore", no_init)
  ;

class_<XC::BerkeleyDbDatastore, bases<XC::DBDatastore>, boost::noncopyable  >("BerkeleyDbDatastore", no_init)
  ;

class_<XC::SQLiteDatastore, bases<XC::DBDatastore>, boost::noncopyable  >("SQLiteDatastore", no_init)
  ;

//class_<XC::OracleDatastore, bases<XC::DBDatastore>, boost::noncopyable  >("OracleDatastore", no_init)
//  ;

class_<XC::MySqlDatastore, bases<XC::DBDatastore>, boost::noncopyable  >("MySqlDatastore", no_init)
  ;

class_<XC::NEESData, bases<XC::FE_Datastore>, boost::noncopyable  >("NEESData", no_init)
  ;

class_<XC::FileDatastore, bases<XC::FE_Datastore>, boost::noncopyable  >("FileDatastore", no_init)
  ;

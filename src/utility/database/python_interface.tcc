//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::FE_Datastore, bases<XC::Channel>, boost::noncopyable  >("FE_Datastore", no_init)
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

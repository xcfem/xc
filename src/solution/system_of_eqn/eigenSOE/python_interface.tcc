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

class_<XC::EigenSOE, bases<XC::SystemOfEqn>, boost::noncopyable >("EigenSOE", no_init)
  .def("newSolver", &XC::EigenSOE::newSolver,return_internal_reference<>(),"Solver to use.")
  ;

class_<XC::ArpackSOE, bases<XC::EigenSOE>, boost::noncopyable >("ArpackSOE", no_init)
  .add_property("shift", make_function(&XC::ArpackSOE::getShift, return_value_policy<copy_const_reference>() ),&XC::ArpackSOE::setShift)
  ;

class_<XC::BandArpackSOE, bases<XC::ArpackSOE>, boost::noncopyable >("BandArpackSOE", no_init)
  ;

class_<XC::BandArpackppSOE, bases<XC::ArpackSOE>, boost::noncopyable >("BandArpackppSOE", no_init)
  ;

class_<XC::SymArpackSOE, bases<XC::ArpackSOE>, boost::noncopyable >("SymArpackSOE", no_init)
  ;

class_<XC::FullGenEigenSOE, bases<XC::EigenSOE>, boost::noncopyable >("FullGenEigenSOE", no_init)
  ;

class_<XC::SymBandEigenSOE, bases<XC::EigenSOE>, boost::noncopyable >("SymBandEigenSOE", no_init)
  ;

class_<XC::EigenSolver, bases<XC::Solver>, boost::noncopyable >("EigenSolver", no_init);

class_<XC::BandArpackppSolver, bases<XC::EigenSolver>, boost::noncopyable >("BandArpackppSolver", no_init)
  ;

class_<XC::SymArpackSolver, bases<XC::EigenSolver>, boost::noncopyable >("SymArpackSolver", no_init)
  ;

class_<XC::BandArpackSolver, bases<XC::EigenSolver>, boost::noncopyable >("BandArpackSolver", no_init)
  ;

class_<XC::FullGenEigenSolver, bases<XC::EigenSolver>, boost::noncopyable >("FullGenEigenSolver", no_init)
  ;

class_<XC::SymBandEigenSolver, bases<XC::EigenSolver>, boost::noncopyable >("SymBandEigenSolver", no_init)
  ;

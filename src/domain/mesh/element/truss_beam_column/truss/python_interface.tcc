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

class_<XC::ProtoTruss, bases<XC::Element1D>, boost::noncopyable >("ProtoTruss", no_init)
  .def("getDim", &XC::ProtoTruss::getNumDIM,"Devuelve la dimensión del espacio en el que se define el elemento (1, 2 ó 3).")
  .def("getMaterial",&XC::ProtoTruss::getMaterialRef,return_internal_reference<>(),"Returns element's material")
  ;

class_<XC::TrussBase, bases<XC::ProtoTruss>, boost::noncopyable >("TrussBase", no_init)
  .def("getL", &XC::TrussBase::getL, return_value_policy<copy_const_reference>(),"Devuelve la longitud del elemento.")
   ;

class_<XC::TrussSection , bases<XC::TrussBase>, boost::noncopyable >("TrussSection", no_init);

class_<XC::Muelle , bases<XC::ProtoTruss>, boost::noncopyable >("Muelle", no_init)
  .def("getN", &XC::Muelle::getAxil,"Returns N internal force.")
  ;


class_<XC::Truss, bases<XC::TrussBase>, boost::noncopyable >("Truss")
  .add_property("area", make_function( &XC::Truss::getArea, return_value_policy<copy_const_reference>() ), &XC::Truss::setArea)
  .def("getN", &XC::Truss::getAxil,"Returns N internal force.")
   ;

class_<XC::CorotTrussBase, bases<XC::ProtoTruss>, boost::noncopyable >("CorotTrussBase", no_init);

class_<XC::CorotTruss, bases<XC::CorotTrussBase>, boost::noncopyable >("CorotTruss", no_init)
  .add_property("area", make_function( &XC::CorotTruss::getArea, return_value_policy<copy_const_reference>() ), &XC::CorotTruss::setArea)
  .def("getN", &XC::CorotTruss::getAxil,"Returns N internal force.")
  ;

class_<XC::CorotTrussSection , bases<XC::CorotTrussBase>, boost::noncopyable >("CorotTrussSection", no_init);




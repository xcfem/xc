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

class_<XC::PlanoDeformacion, bases<Plano3d> >("PlanoDeformacion")
  .def(init<const double&>())
  .def(init<Plano3d>())
  .def(init<Pos2d,double,Pos2d,double,Pos2d,double>())
  .def(init<Pos3d,Pos3d,Pos3d>())
  .def("constantStrain",&XC::PlanoDeformacion::ConstantStrain)
  .def("getDeformation",&XC::PlanoDeformacion::Deformacion)
  ;

class_<XC::CalcPivotes, bases<Ref3d3d>, boost::noncopyable >("CalcPivotes", no_init)
  ;

class_<XC::DefAgotPivotes, bases<EntCmd>, boost::noncopyable >("DefAgotPivotes", no_init)
  .add_property("defAgotPivoteA",make_function(&XC::DefAgotPivotes::getDefAgotPivoteA,return_value_policy<copy_const_reference>()),&XC::DefAgotPivotes::setDefAgotPivoteA)
  .add_property("defAgotPivoteB",make_function(&XC::DefAgotPivotes::getDefAgotPivoteB,return_value_policy<copy_const_reference>()),&XC::DefAgotPivotes::setDefAgotPivoteB)
  .add_property("defAgotPivoteC",make_function(&XC::DefAgotPivotes::getDefAgotPivoteC,return_value_policy<copy_const_reference>()),&XC::DefAgotPivotes::setDefAgotPivoteC)
  ;

class_<XC::DatosDiagInteraccion, bases<EntCmd>>("InteractionDiagramParameters")
  .add_property("umbral",make_function(&XC::DatosDiagInteraccion::getUmbral,return_value_policy<copy_const_reference>()),&XC::DatosDiagInteraccion::setUmbral)
  .add_property("incEps",make_function(&XC::DatosDiagInteraccion::getIncEps,return_value_policy<copy_const_reference>()),&XC::DatosDiagInteraccion::setIncEps)
  .add_property("incTheta",make_function(&XC::DatosDiagInteraccion::getIncTheta,return_value_policy<copy_const_reference>()),&XC::DatosDiagInteraccion::setIncTheta)
  .add_property("defsAgotPivotes",make_function(&XC::DatosDiagInteraccion::getDefsAgotPivotes,return_internal_reference<>()),&XC::DatosDiagInteraccion::setDefsAgotPivotes)
  .add_property("nmbSetHormigon",make_function(&XC::DatosDiagInteraccion::getNmbSetHormigon,return_internal_reference<>()),&XC::DatosDiagInteraccion::setNmbSetHormigon)
  .add_property("tagHormigon",make_function(&XC::DatosDiagInteraccion::getTagHormigon,return_value_policy<copy_const_reference>()),&XC::DatosDiagInteraccion::setTagHormigon)
  .add_property("nmbSetArmadura",make_function(&XC::DatosDiagInteraccion::getNmbSetArmadura,return_internal_reference<>()),&XC::DatosDiagInteraccion::setNmbSetArmadura)
  .add_property("tagArmadura",make_function(&XC::DatosDiagInteraccion::getTagArmadura,return_value_policy<copy_const_reference>()),&XC::DatosDiagInteraccion::setTagArmadura)
  ;


double (XC::DiagInteraccion::*getFactorCapacidad)(const Pos3d &esf_d) const= &XC::DiagInteraccion::FactorCapacidad;
class_<XC::DiagInteraccion, bases<GeomObj3d>, boost::noncopyable >("InteractionDiagram", no_init)
  .def("getMax",&XC::DiagInteraccion::GetMax)
  .def("getMin",&XC::DiagInteraccion::GetMin)
  .def("centroid",&XC::DiagInteraccion::Cdg)
  .def("getLength",&XC::DiagInteraccion::Longitud)
  .def("getArea",&XC::DiagInteraccion::Area)
  .def("getVolume",&XC::DiagInteraccion::Volumen)
  .def("getIx",&XC::DiagInteraccion::Ix)
  .def("getIy",&XC::DiagInteraccion::Iy)
  .def("getIz",&XC::DiagInteraccion::Iz)
  .def("getDimension",&XC::DiagInteraccion::Dimension)
  .def("getNumFacetas",&XC::DiagInteraccion::GetNumFacetas)
  .def("getCapacityFactor",getFactorCapacidad)
  .def("writeTo",&XC::DiagInteraccion::writeTo)
  .def("readFrom",&XC::DiagInteraccion::readFrom)
  ;

double (XC::DiagInteraccion2d::*getFactorCapacidad2d)(const Pos2d &esf_d) const= &XC::DiagInteraccion2d::FactorCapacidad;
class_<XC::DiagInteraccion2d, bases<Poligono2d>, boost::noncopyable >("InteractionDiagram2d", no_init)
  .def("getCapacityFactor",getFactorCapacidad2d)
  .def("simplify",&XC::DiagInteraccion2d::Simplify)
  ;

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

class_<XC::ElasticBeam2d, bases<XC::ProtoBeam2d>, boost::noncopyable >("ElasticBeam2d", no_init)
  .add_property("rho", &XC::ElasticBeam2d::getRho,&XC::ElasticBeam2d::setRho)
  .add_property("h", &XC::ElasticBeam2d::getDepth,&XC::ElasticBeam2d::setDepth)
  .add_property("initialStrain", make_function(&XC::ElasticBeam2d::getInitialStrain, return_value_policy<copy_const_reference>()),&XC::ElasticBeam2d::setInitialStrain)
  .add_property("getV", &XC::ElasticBeam2d::getV)
  .add_property("getV1", &XC::ElasticBeam2d::getV1)
  .add_property("getV2", &XC::ElasticBeam2d::getV2) //Cortante en el extremo frontal.
  .add_property("getVy1", &XC::ElasticBeam2d::getV1)
  .add_property("getVy2", &XC::ElasticBeam2d::getV2) 
  .add_property("getN1", &XC::ElasticBeam2d::getN1) //Axil.
  .add_property("getN2", &XC::ElasticBeam2d::getN2)
  .add_property("getM1", &XC::ElasticBeam2d::getM1)
  .add_property("getM2", &XC::ElasticBeam2d::getM2)
  .add_property("getMz1", &XC::ElasticBeam2d::getM1) //Equivalent.
  .add_property("getMz2", &XC::ElasticBeam2d::getM2)
   ;

class_<XC::ElasticBeam3d, bases<XC::ProtoBeam3d>, boost::noncopyable >("ElasticBeam3d", no_init)
  .add_property("rho", &XC::ElasticBeam3d::getRho,&XC::ElasticBeam3d::setRho)
  .add_property("initialStrain", make_function(&XC::ElasticBeam3d::getInitialStrain, return_value_policy<copy_const_reference>()),&XC::ElasticBeam3d::setInitialStrain)
  .add_property("getAN2", &XC::ElasticBeam3d::getAN2) //fuerza axil que se ejerce sobre la barra en su extremo frontal.
  .add_property("getN1", &XC::ElasticBeam3d::getN1) //axil en el extremo dorsal.
  .add_property("getN2", &XC::ElasticBeam3d::getN2) //axil en el extremo frontal.
  .add_property("getN", &XC::ElasticBeam3d::getN) //axil medio.
  .add_property("getAMz1", &XC::ElasticBeam3d::getAMz1)
  .add_property("getAMz2", &XC::ElasticBeam3d::getAMz2)
  .add_property("getMz1", &XC::ElasticBeam3d::getMz1)
  .add_property("getMz2", &XC::ElasticBeam3d::getMz2)
  .add_property("getVy", &XC::ElasticBeam3d::getVy)
  .add_property("getAVy1", &XC::ElasticBeam3d::getAVy1)
  .add_property("getAVy2", &XC::ElasticBeam3d::getAVy2)
  .add_property("getVy1", &XC::ElasticBeam3d::getVy1)
  .add_property("getVy2", &XC::ElasticBeam3d::getVy2)
  .add_property("getVz", &XC::ElasticBeam3d::getVz)
  .add_property("getAVz1", &XC::ElasticBeam3d::getAVz1)
  .add_property("getAVz2", &XC::ElasticBeam3d::getAVz2)
  .add_property("getVz1", &XC::ElasticBeam3d::getVz1)
  .add_property("getVz2", &XC::ElasticBeam3d::getVz2)
  .add_property("getMy1", &XC::ElasticBeam3d::getMy1)
  .add_property("getMy2", &XC::ElasticBeam3d::getMy2)
  .add_property("getT", &XC::ElasticBeam3d::getT) //Torsor en la barra.
  .add_property("getT1", &XC::ElasticBeam3d::getT1)
  .add_property("getT2", &XC::ElasticBeam3d::getT2)
  .def("getVDirEjeFuerteLocales",&XC::ElasticBeam3d::getVDirEjeFuerteLocales)
  .def("getVDirEjeDebilLocales",&XC::ElasticBeam3d::getVDirEjeDebilLocales)
  .def("getAnguloEjeFuerte",&XC::ElasticBeam3d::getAnguloEjeFuerte)
  .def("getAnguloEjeDebil",&XC::ElasticBeam3d::getAnguloEjeDebil)
  .def("getVDirEjeFuerteGlobales",make_function(&XC::ElasticBeam3d::getVDirEjeFuerteGlobales, return_value_policy<copy_const_reference>()))
  .def("getVDirEjeDebilGlobales",make_function(&XC::ElasticBeam3d::getVDirEjeDebilGlobales, return_value_policy<copy_const_reference>()))
   ;

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

const double &(XC::ConstantesSecc2d::*getE)(void) const= &XC::ConstantesSecc2d::E;
const double &(XC::ConstantesSecc2d::*getG)(void) const= &XC::ConstantesSecc2d::G;
const double &(XC::ConstantesSecc2d::*getA)(void) const= &XC::ConstantesSecc2d::A;
const double &(XC::ConstantesSecc2d::*getAlpha)(void) const= &XC::ConstantesSecc2d::Alpha;
const double &(XC::ConstantesSecc2d::*getI)(void) const= &XC::ConstantesSecc2d::I;
class_<XC::ConstantesSecc2d, bases<EntCmd> >("ConstantesSecc2d")
  .add_property("E", make_function( getE, return_value_policy<return_by_value>() ), &XC::ConstantesSecc2d::setE)
  .add_property("G", make_function( getG, return_value_policy<return_by_value>() ), &XC::ConstantesSecc2d::setG)
  .add_property("A", make_function( getA, return_value_policy<return_by_value>() ), &XC::ConstantesSecc2d::setA)
  .add_property("Alpha", make_function( getAlpha, return_value_policy<return_by_value>() ), &XC::ConstantesSecc2d::setAlpha)
  .add_property("I", make_function( getI, return_value_policy<return_by_value>() ), &XC::ConstantesSecc2d::setI)
  ;

const double &(XC::ConstantesSecc3d::*getIz)(void) const= &XC::ConstantesSecc3d::Iz;
const double &(XC::ConstantesSecc3d::*getIy)(void) const= &XC::ConstantesSecc3d::Iy;
const double &(XC::ConstantesSecc3d::*getIyz)(void) const= &XC::ConstantesSecc3d::Iyz;
const double &(XC::ConstantesSecc3d::*getJ)(void) const= &XC::ConstantesSecc3d::J;
class_<XC::ConstantesSecc3d, bases<XC::ConstantesSecc2d> >("ConstantesSecc3d")
  .add_property("Iz", make_function( getIz, return_value_policy<return_by_value>() ), &XC::ConstantesSecc3d::setIz)
  .add_property("Iy", make_function( getIy, return_value_policy<return_by_value>() ), &XC::ConstantesSecc3d::setIy)
  .add_property("Iyz", make_function( getIyz, return_value_policy<return_by_value>() ), &XC::ConstantesSecc3d::setIyz)
  .add_property("J", make_function( getJ, return_value_policy<return_by_value>() ), &XC::ConstantesSecc3d::setJ)
  .def("gira",&XC::ConstantesSecc3d::gira,"Rotate section (deprecated).")
  .def("getI1",&XC::ConstantesSecc3d::getI1)
  .def("getI2",&XC::ConstantesSecc3d::getI2)
  .def("getVDirEje1",&XC::ConstantesSecc3d::getVDirEje1)
  .def("getVDirEje2",&XC::ConstantesSecc3d::getVDirEje2)
  .def("getVDirEjeFuerte",&XC::ConstantesSecc3d::getVDirEjeFuerte)
  .def("getVDirEjeDebil",&XC::ConstantesSecc3d::getVDirEjeDebil)
  ;

class_<XC::DiscretBase, bases<EntConNmb>, boost::noncopyable >("DiscretBase", no_init)
  .def("getMaxY",&XC::DiscretBase::getMaxY)
  .def("getMaxZ",&XC::DiscretBase::getMaxZ)
  .def("getMinY",&XC::DiscretBase::getMinY)
  .def("getMinZ",&XC::DiscretBase::getMinZ)
  ;

class_<XC::SeccionInerte, bases<EntCmd>, boost::noncopyable >("SeccionInerte", no_init)
  .def("getAreaSeccBruta",&XC::SeccionInerte::getAreaSeccBruta,"Returns region's gross section.")
  .def("getCdgSeccBruta",&XC::SeccionInerte::getCdgSeccBruta,"Returns center of gravity of gross section.")
  .def("getIySeccBruta",&XC::SeccionInerte::getIySeccBruta,"Inertia of the gross section about an axis parallel to y through his center of gravity.")
  .def("getIzSeccBruta",&XC::SeccionInerte::getIzSeccBruta,"Inertia of the gross section about an axis parallel to z through his center of gravity.")
  .def("getPyzSeccBruta",&XC::SeccionInerte::getPyzSeccBruta,"Product of inertia of the gross section about y and z axis through his center of gravity.")

  .def("getIxSeccBruta",&XC::SeccionInerte::getIxSeccBruta,"Inertia of the gross section about an axis parallel to x through his center of gravity.")
  .def("getTheta_pSeccBruta",&XC::SeccionInerte::getTheta_pSeccBruta,"Agle of the principal axis of inertia of the gross section with respect of current axis.")
.def("getDirEjeI_aSeccBruta",&XC::SeccionInerte::getDirEjeI_aSeccBruta,"Direction of a principal axis of inertia of the gross section with respect of current axis.")
  .def("getDirEjeI_bSeccBruta",&XC::SeccionInerte::getDirEjeI_bSeccBruta,"Direction of the other principal axis of inertia of the gross section with respect of current axis.")
  .def("getI1SeccBruta",&XC::SeccionInerte::getI1SeccBruta,"Major principal moment of inertia of the gross section.")
  .def("getI2SeccBruta",&XC::SeccionInerte::getI2SeccBruta,"Minor principal moment of inertia of the gross section.")
  .def("getEjesInerciaSeccBruta",&XC::SeccionInerte::getEjesInerciaSeccBruta,"Principal axis of inertia of the gross section.")
  .def("getVDirEje1SeccBruta",&XC::SeccionInerte::getVDirEje1SeccBruta,"Direction of the major principal axis of inertia of the gross section.")
  .def("getVDirEjeFuerteSeccBruta",&XC::SeccionInerte::getVDirEjeFuerteSeccBruta,"Direction of the major principal axis of inertia of the gross section.")
.def("getVDirEje2SeccBruta",&XC::SeccionInerte::getVDirEje2SeccBruta,"Direction of the minor principal axis of inertia of the gross section.")
  .def("getVDirEjeDebilSeccBruta",&XC::SeccionInerte::getVDirEjeDebilSeccBruta,"Direction of the minor principal axis of inertia of the gross section.")
//.def("getISeccBruta",&XC::SeccionInerte::getISeccBruta)
//.def("getISeccBruta",&XC::SeccionInerte::getISeccBruta)
//.def("getISeccBruta",&XC::SeccionInerte::getISeccBruta)
//.def("getISeccBruta",&XC::SeccionInerte::getISeccBruta)
  .def("getIOSeccBruta",&XC::SeccionInerte::getIOSeccBruta)
//.def("getISeccBruta",&XC::SeccionInerte::getISeccBruta)
//.def("getISeccBruta",&XC::SeccionInerte::getISeccBruta)

  .def("getAreaSeccHomogeneizada",&XC::SeccionInerte::getAreaSeccHomogeneizada,"Returns region's homogeinized section.")
  .def("getCdgSeccHomogeneizada",&XC::SeccionInerte::getCdgSeccHomogeneizada,"Returns center of gravity of homogeinized section.")
  .def("getIySeccHomogeneizada",&XC::SeccionInerte::getIySeccHomogeneizada,"Inertia of the homogeinized section about an axis parallel to y through his center of gravity.")
  .def("getIzSeccHomogeneizada",&XC::SeccionInerte::getIzSeccHomogeneizada,"Inertia of the homogeinized section about an axis parallel to z through his center of gravity.")
  .def("getPyzSeccHomogeneizada",&XC::SeccionInerte::getPyzSeccHomogeneizada,"Product of inertia of the homogeinized section about y and z axis through his center of gravity.")

  .def("getIxSeccHomogeneizada",&XC::SeccionInerte::getIxSeccHomogeneizada,"Inertia of the homogeinized section about an axis parallel to x through his center of gravity.")
  .def("getTheta_pSeccHomogeneizada",&XC::SeccionInerte::getTheta_pSeccHomogeneizada,"Agle of the principal axis of inertia of the homogeinized section with respect of current axis.")
  .def("getDirEjeI_aSeccHomogeneizada",&XC::SeccionInerte::getDirEjeI_aSeccHomogeneizada,"Direction of a principal axis of inertia of the homogeinized section with respect of current axis.")
  .def("getDirEjeI_bSeccHomogeneizada",&XC::SeccionInerte::getDirEjeI_bSeccHomogeneizada,"Direction of the other principal axis of inertia of the homogeinized section with respect of current axis.")
  .def("getI1SeccHomogeneizada",&XC::SeccionInerte::getI1SeccHomogeneizada,"Major principal moment of inertia of the homogeinized section.")
  .def("getI2SeccHomogeneizada",&XC::SeccionInerte::getI2SeccHomogeneizada,"Minor principal moment of inertia of the homogeinized section.")
  .def("getEjesInerciaSeccHomogeneizada",&XC::SeccionInerte::getEjesInerciaSeccHomogeneizada,"Principal axis of inertia of the homogeinized section.")
  .def("getVDirEje1SeccHomogeneizada",&XC::SeccionInerte::getVDirEje1SeccHomogeneizada,"Direction of the major principal axis of inertia of the homogeinized section.")
  .def("getVDirEjeFuerteSeccHomogeneizada",&XC::SeccionInerte::getVDirEjeFuerteSeccHomogeneizada,"Direction of the major principal axis of inertia of the homogeinized section.")
  .def("getVDirEje2SeccHomogeneizada",&XC::SeccionInerte::getVDirEje2SeccHomogeneizada,"Direction of the minor principal axis of inertia of the homogeinized section.")
  .def("getVDirEjeDebilSeccHomogeneizada",&XC::SeccionInerte::getVDirEjeDebilSeccHomogeneizada,"Direction of the minor principal axis of inertia of the homogeinized section.")
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
  .def("getIOSeccHomogeneizada",&XC::SeccionInerte::getIOSeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)

  .def("getConstantesSecc3d",&XC::SeccionInerte::getConstantesSecc3d)
  .def("getConstantesSecc2d",&XC::SeccionInerte::getConstantesSecc2d)
  ;

#include "cell/python_interface.tcc"
#include "geom_section/python_interface.tcc"
#include "section/python_interface.tcc"

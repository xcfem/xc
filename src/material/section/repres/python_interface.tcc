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

const double &(XC::CrossSectionProperties2d::*getE)(void) const= &XC::CrossSectionProperties2d::E;
const double &(XC::CrossSectionProperties2d::*getG)(void) const= &XC::CrossSectionProperties2d::G;
const double &(XC::CrossSectionProperties2d::*getA)(void) const= &XC::CrossSectionProperties2d::A;
const double &(XC::CrossSectionProperties2d::*getAlpha)(void) const= &XC::CrossSectionProperties2d::Alpha;
const double &(XC::CrossSectionProperties2d::*getI)(void) const= &XC::CrossSectionProperties2d::I;
class_<XC::CrossSectionProperties2d, bases<EntCmd> >("CrossSectionProperties2d")
.add_property("E", make_function( getE, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setE,"Elastic modulus.")
  .add_property("G", make_function( getG, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setG,"Shear modulus.")
  .add_property("A", make_function( getA, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setA,"Area.")
  .add_property("Alpha", make_function( getAlpha, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setAlpha," Shear reduction factor.")
  .add_property("I", make_function( getI, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties2d::setI, "Moment of inertia.")
  ;

const double &(XC::CrossSectionProperties3d::*getIz)(void) const= &XC::CrossSectionProperties3d::Iz;
const double &(XC::CrossSectionProperties3d::*getIy)(void) const= &XC::CrossSectionProperties3d::Iy;
const double &(XC::CrossSectionProperties3d::*getIyz)(void) const= &XC::CrossSectionProperties3d::Iyz;
const double &(XC::CrossSectionProperties3d::*getJ)(void) const= &XC::CrossSectionProperties3d::J;
class_<XC::CrossSectionProperties3d, bases<XC::CrossSectionProperties2d> >("CrossSectionProperties3d")
  .add_property("Iz", make_function( getIz, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setIz)
  .add_property("Iy", make_function( getIy, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setIy)
  .add_property("Iyz", make_function( getIyz, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setIyz)
  .add_property("J", make_function( getJ, return_value_policy<return_by_value>() ), &XC::CrossSectionProperties3d::setJ)
  .def("gira",&XC::CrossSectionProperties3d::gira,"Rotate section (deprecated).")
  .def("getI1",&XC::CrossSectionProperties3d::getI1)
  .def("getI2",&XC::CrossSectionProperties3d::getI2)
  .def("getVDirEje1",&XC::CrossSectionProperties3d::getVDirEje1)
  .def("getVDirEje2",&XC::CrossSectionProperties3d::getVDirEje2)
  .def("getVDirEjeFuerte",&XC::CrossSectionProperties3d::getVDirEjeFuerte)
  .def("getVDirEjeDebil",&XC::CrossSectionProperties3d::getVDirEjeDebil)
  ;

class_<XC::DiscretBase, bases<EntConNmb>, boost::noncopyable >("DiscretBase", no_init)
  .def("getMaxY",&XC::DiscretBase::getMaxY,"Returns y coordinate maximum value.")
  .def("getMaxZ",&XC::DiscretBase::getMaxZ,"Returns z coordinate maximum value.")
  .def("getMinY",&XC::DiscretBase::getMinY,"Returns y coordinate minimum value.")
  .def("getMinZ",&XC::DiscretBase::getMinZ,"Returns z coordinate minimum value.")
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

  .def("getAreaSeccHomogeneizada",&XC::SeccionInerte::getAreaSeccHomogeneizada,"Returns region's homogenized section.")
  .def("getCdgSeccHomogeneizada",&XC::SeccionInerte::getCdgSeccHomogeneizada,"Returns center of gravity of homogenized section.")
  .def("getIySeccHomogeneizada",&XC::SeccionInerte::getIySeccHomogeneizada,"Inertia of the homogenized section about an axis parallel to y through his center of gravity.")
  .def("getIzSeccHomogeneizada",&XC::SeccionInerte::getIzSeccHomogeneizada,"Inertia of the homogenized section about an axis parallel to z through his center of gravity.")
  .def("getPyzSeccHomogeneizada",&XC::SeccionInerte::getPyzSeccHomogeneizada,"Product of inertia of the homogenized section about y and z axis through his center of gravity.")

  .def("getIxSeccHomogeneizada",&XC::SeccionInerte::getIxSeccHomogeneizada,"Inertia of the homogenized section about an axis parallel to x through his center of gravity.")
  .def("getTheta_pSeccHomogeneizada",&XC::SeccionInerte::getTheta_pSeccHomogeneizada,"Agle of the principal axis of inertia of the homogenized section with respect of current axis.")
  .def("getDirEjeI_aSeccHomogeneizada",&XC::SeccionInerte::getDirEjeI_aSeccHomogeneizada,"Direction of a principal axis of inertia of the homogenized section with respect of current axis.")
  .def("getDirEjeI_bSeccHomogeneizada",&XC::SeccionInerte::getDirEjeI_bSeccHomogeneizada,"Direction of the other principal axis of inertia of the homogenized section with respect of current axis.")
  .def("getI1SeccHomogeneizada",&XC::SeccionInerte::getI1SeccHomogeneizada,"Major principal moment of inertia of the homogenized section.")
  .def("getI2SeccHomogeneizada",&XC::SeccionInerte::getI2SeccHomogeneizada,"Minor principal moment of inertia of the homogenized section.")
  .def("getEjesInerciaSeccHomogeneizada",&XC::SeccionInerte::getEjesInerciaSeccHomogeneizada,"Principal axis of inertia of the homogenized section.")
  .def("getVDirEje1SeccHomogeneizada",&XC::SeccionInerte::getVDirEje1SeccHomogeneizada,"Direction of the major principal axis of inertia of the homogenized section.")
  .def("getVDirEjeFuerteSeccHomogeneizada",&XC::SeccionInerte::getVDirEjeFuerteSeccHomogeneizada,"Direction of the major principal axis of inertia of the homogenized section.")
  .def("getVDirEje2SeccHomogeneizada",&XC::SeccionInerte::getVDirEje2SeccHomogeneizada,"Direction of the minor principal axis of inertia of the homogenized section.")
  .def("getVDirEjeDebilSeccHomogeneizada",&XC::SeccionInerte::getVDirEjeDebilSeccHomogeneizada,"Direction of the minor principal axis of inertia of the homogenized section.")
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
  .def("getIOSeccHomogeneizada",&XC::SeccionInerte::getIOSeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)
//.def("getISeccHomogeneizada",&XC::SeccionInerte::getISeccHomogeneizada)

  .def("getCrossSectionProperties3d",&XC::SeccionInerte::getCrossSectionProperties3d)
  .def("getCrossSectionProperties2d",&XC::SeccionInerte::getCrossSectionProperties2d)
  ;

#include "cell/python_interface.tcc"
#include "geom_section/python_interface.tcc"
#include "section/python_interface.tcc"

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

//Elasticity.

XC::SolidMech2D &(ElemWithMaterial4N_Mech2D::*getMech2DPhysicalProp)(void)= &ElemWithMaterial4N_Mech2D::getPhysicalProperties;
class_<ElemWithMaterial4N_Mech2D, bases<XC::ElementBase<4> >, boost::noncopyable >("ElemWithMaterial4N_Mech2D", no_init)
  .add_property("getPhysicalProperties", make_function(getMech2DPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<ElemPlano4N_Mech2D, bases<ElemWithMaterial4N_Mech2D>, boost::noncopyable >("ElemPlano4N_Mech2D", no_init)
  .def("getPerimeter", &ElemPlano4N_Mech2D::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &ElemPlano4N_Mech2D::getArea, "Returns element's area.")
   ;

class_<QuadBase4N_Mech2D, bases<ElemPlano4N_Mech2D>, boost::noncopyable >("QuadBase4N_Mech2D", no_init)
   ;

#include "fourNodeQuad/python_interface.tcc"

XC::SolidMech2D &(ElemWithMaterial3N_Mech2D::*getMech2D3NPhysicalProp)(void)= &ElemWithMaterial3N_Mech2D::getPhysicalProperties;
class_<ElemWithMaterial3N_Mech2D, bases<XC::ElementBase<3> >, boost::noncopyable >("ElemWithMaterial3N_Mech2D", no_init)
  .add_property("getPhysicalProperties", make_function(getMech2D3NPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<ElemPlano3N_Mech2D, bases<ElemWithMaterial3N_Mech2D>, boost::noncopyable >("ElemPlano3N_Mech2D", no_init)
  .def("getPerimeter", &ElemPlano3N_Mech2D::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &ElemPlano3N_Mech2D::getArea, "Returns element's area.")
   ;

class_<TriBase3N_Mech2D, bases<ElemPlano3N_Mech2D>, boost::noncopyable >("QuadBase3N_Mech2D", no_init)
   ;

#include "triangle/python_interface.tcc"

XC::SectionFDPhysicalProperties &(ElemWithMaterial4N_SFD::*getSectionFDPhysicalProp)(void)= &ElemWithMaterial4N_SFD::getPhysicalProperties;
class_<ElemWithMaterial4N_SFD, bases<XC::ElementBase<4> >, boost::noncopyable >("ElemWithMaterial4N_SFD", no_init)
  .add_property("getPhysicalProperties", make_function(getSectionFDPhysicalProp, return_internal_reference<>() ),"returns materials at integration points (gauss points).")
   ;

class_<ElemPlano4N_SFD, bases<ElemWithMaterial4N_SFD>, boost::noncopyable >("ElemPlano4N_SFD", no_init)
  .def("getPerimeter", &ElemPlano4N_SFD::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &ElemPlano4N_SFD::getArea, "Returns element's area.")
   ;


//Shell
#include "shell/python_interface.tcc"

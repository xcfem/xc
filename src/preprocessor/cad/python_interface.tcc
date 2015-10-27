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

#include "entidades/python_interface.tcc"
#include "matrices/python_interface.tcc"
#include "trf/python_interface.tcc"

class_<XC::SisRef, bases<XC::EntMdlrBase>, boost::noncopyable >("SisRef", no_init);

class_<XC::SisRefCartesianas3d, bases<XC::SisRef>, boost::noncopyable >("SisRefCartesianas3d", no_init)
.add_property("org",&XC::SisRefCartesianas3d::getOrg,&XC::SisRefCartesianas3d::setOrg)
  ;

class_<XC::MapCadMemberBase, bases<EntCmd>, boost::noncopyable >("MapCadMemberBase", no_init)
  .def("setDefaultTag",&XC::MapCadMemberBase::setTag)
  ;

//class_<XC::MapCadMember, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCadMember", no_init);

typedef XC::MapCadMember<XC::SisRef> MapCadSisRef;
class_<MapCadSisRef, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCadSisRef", no_init);


XC::SisRef *(XC::MapSisRef::*newReferenceSystem)(const std::string &)= &XC::MapSisRef::Nuevo;
class_<XC::MapSisRef, bases<MapCadSisRef>, boost::noncopyable >("MapSisRef", no_init)
.def("newReferenceSystem", make_function(newReferenceSystem,return_internal_reference<>() ))
  ;


XC::MapSisRef &(XC::Cad::*getReferenceSystems)(void)= &XC::Cad::getSistemasReferencia;
XC::MapTrfGeom &(XC::Cad::*getGeometricTransformations)(void)= &XC::Cad::getTransformacionesGeometricas;
XC::MapPuntos &(XC::Cad::*getMapPuntosRef)(void)= &XC::Cad::getPuntos;
XC::MapLineas &(XC::Cad::*getMapLineasRef)(void)= &XC::Cad::getLineas;
XC::MapSuperficies &(XC::Cad::*getMapSuperficiesRef)(void)= &XC::Cad::getSuperficies;
XC::MapEsquemas2d &(XC::Cad::*getEsquemas2dRef)(void)= &XC::Cad::getEsquemas2d;
XC::MapEsquemas3d &(XC::Cad::*getEsquemas3dRef)(void)= &XC::Cad::getEsquemas3d;
XC::MapUniformGrids &(XC::Cad::*getUniformGridsRef)(void)= &XC::Cad::getUniformGrids;
XC::Edge *(XC::Cad::*getLineWithEndPoints)(const XC::MapPuntos::Indice &,const XC::MapPuntos::Indice &) = &XC::Cad::busca_edge_extremos;
class_<XC::Cad, bases<EntCmd>, boost::noncopyable >("Cad", no_init)
  .add_property("getReferenceSystems", make_function( getReferenceSystems, return_internal_reference<>() ))
  .add_property("getGeometricTransformations", make_function( getGeometricTransformations, return_internal_reference<>() ))
  .add_property("getPoints", make_function( getMapPuntosRef, return_internal_reference<>() ))
  .add_property("getLines", make_function( getMapLineasRef, return_internal_reference<>() ))
  .add_property("getSurfaces", make_function( getMapSuperficiesRef, return_internal_reference<>() ))
  .add_property("get2DNets", make_function( getEsquemas2dRef, return_internal_reference<>() ))
  .add_property("get3DNets", make_function( getEsquemas3dRef, return_internal_reference<>() ))
  .add_property("getUniformGrids", make_function( getUniformGridsRef, return_internal_reference<>() ))
  .def("conciliaNDivs", &XC::Cad::conciliaNDivs)
  .def("getLineWithEndPoints",make_function( getLineWithEndPoints, return_internal_reference<>() ))
   ;



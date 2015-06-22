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

#include "region/python_interface.tcc"
#include "reinfBar/python_interface.tcc"
#include "reinfLayer/python_interface.tcc"

class_<XC::EntGeomSection, bases<EntConNmb>, boost::noncopyable >("EntGeomSection", no_init);

class_<XC::SisRefScc, bases<XC::EntGeomSection>, boost::noncopyable >("SisRefScc", no_init);

XC::Spot *(XC::Eje::*getSpot1)(void)= &XC::Eje::P1;
XC::Spot *(XC::Eje::*getSpot2)(void)= &XC::Eje::P2;
class_<XC::Eje, bases<XC::EntGeomSection>, boost::noncopyable >("Eje", no_init)
  .add_property("p1",make_function(getSpot1,return_internal_reference<>()))
  .add_property("p2",make_function(getSpot2,return_internal_reference<>()))
  .add_property("nDiv",&XC::Eje::NDiv,&XC::Eje::SetNDiv)
  .add_property("numVertices",&XC::Eje::NumVertices)
  ;

const Pos2d &(XC::Spot::*getPosSpot)(void) const= &XC::Spot::GetPos;
class_<XC::Spot, bases<XC::EntGeomSection>, boost::noncopyable >("Spot", no_init)
  .add_property("pos",make_function(getPosSpot,return_internal_reference<>()),&XC::Spot::setPos)
  .add_property("numLines",&XC::Spot::nLines)
  ;

class_<XC::Segment, bases<XC::Eje>, boost::noncopyable >("Segment", no_init)
  .def("getLong",&XC::Segment::getLong)
  ;

XC::ListRegiones &(XC::GeomSection::*getRegionContainer)(void)= &XC::GeomSection::getRegiones;
XC::ListReinfLayer &(XC::GeomSection::*getReinfLayerContainer)(void)= &XC::GeomSection::getCapasArmadura;
class_<XC::GeomSection, bases<XC::SeccionInerte>, boost::noncopyable >("GeomSection", no_init)
  .add_property("getRegions",make_function(getRegionContainer,return_internal_reference<>()))
  .add_property("getReinfLayers",make_function(getReinfLayerContainer,return_internal_reference<>()))
  .def("getContornoRegiones",&XC::GeomSection::getContornoRegiones)
  .def("getContornoZonaComprimida",&XC::GeomSection::getContornoZonaComprimida)
  .def("getRecubrimiento",&XC::GeomSection::getRecubrimiento)
  .add_property("tagSpot",&XC::GeomSection::getTagSpot,&XC::GeomSection::setTagSpot)
  .def("newSpot",make_function(&XC::GeomSection::newSpot,return_internal_reference<>()))
  .def("distSpots",&XC::GeomSection::DistSpots)
  .add_property("tagLine",&XC::GeomSection::getTagEje,&XC::GeomSection::setTagEje)
  .def("newSegment",make_function(&XC::GeomSection::newSegment,return_internal_reference<>()))
  ;


class_<XC::SisRefSccCartesianas2d, bases<XC::SisRefScc>, boost::noncopyable >("SisRefSccCartesianas2d", no_init);


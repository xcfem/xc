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

Pos3d (XC::TrfGeom::*getTrfPos)(const Pos3d &) const= &XC::TrfGeom::Transforma;
Vector3d (XC::TrfGeom::*getTrfVector)(const Vector3d &) const= &XC::TrfGeom::Transforma;
class_<XC::TrfGeom, bases<XC::EntMdlrBase>, boost::noncopyable >("TrfGeom", no_init)
  .def("getTrfPos",getTrfPos)
  .def("getTrfVector",getTrfVector)
  ;

class_<XC::Traslacion, bases<XC::TrfGeom>, boost::noncopyable >("Traslacion", no_init)
  .def("setVector",&XC::Traslacion::setVector)
  ;

class_<XC::Escalado, bases<XC::TrfGeom>, boost::noncopyable >("Escalado", no_init)
  .def("setScaleFactor",&XC::Escalado::setScaleFactor)
  ;

class_<XC::Reflexion, bases<XC::TrfGeom>, boost::noncopyable >("Reflexion", no_init)
  .def("setPlane",&XC::Reflexion::setPlane)
  ;

class_<XC::Rotacion, bases<XC::TrfGeom>, boost::noncopyable >("Rotacion", no_init)
  .def("setAxisAng",&XC::Rotacion::setAxisAng)
  ;

typedef XC::MapCadMember<XC::TrfGeom> MapCadTrfGeom;
class_<MapCadTrfGeom, bases<XC::MapCadMemberBase>, boost::noncopyable >("MapCadTraGeom", no_init);

class_<XC::MapTrfGeom, bases<MapCadTrfGeom>, boost::noncopyable >("MapTrfGeom", no_init)
.def("newTransformation",&XC::MapTrfGeom::newTransformation, return_internal_reference<>(),"Defines a new transformation.")
  ;





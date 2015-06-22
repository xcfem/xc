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

class_<XC::Load, bases<XC::DomainComponent>, boost::noncopyable >("Load", no_init);

class_<XC::NodalLoad, bases<XC::Load>, boost::noncopyable >("NodalLoad", no_init);

class_<XC::NodalLoadIter, boost::noncopyable >("NodalLoadIter", no_init)
  .def("next", &XC::NodalLoadIter::operator(), return_internal_reference<>(),"Returns next load.")
   ;

class_<XC::ElementalLoad, bases<XC::Load>, boost::noncopyable >("ElementalLoad", no_init)
  .add_property("numElements", &XC::ElementalLoad::numElements, "Returns the number of loaded elements.")
  .add_property("elementTags", make_function(&XC::ElementalLoad::getElementTags,return_internal_reference<>()),&XC::ElementalLoad::setElementTags)
  ;

class_<XC::ElementalLoadIter, boost::noncopyable >("ElementalLoadIter", no_init)
  .def("next", &XC::ElementalLoadIter::operator(), return_internal_reference<>(),"Returns next load.")
   ;

class_<XC::ElementBodyLoad, bases<XC::ElementalLoad>, boost::noncopyable >("ElementBodyLoad", no_init);

#include "beam_loads/python_interface.tcc"
#include "plane/python_interface.tcc"
#include "volumen/python_interface.tcc"

class_<XC::ForceReprComponent, bases<XC::DomainComponent>, boost::noncopyable >("ForceReprComponent", no_init);

class_<XC::LoadLoaderMember, bases<EntCmd,XC::MovableObject>, boost::noncopyable >("LoadLoaderMember", no_init);

#include "pattern/python_interface.tcc"
#include "groundMotion/python_interface.tcc"

